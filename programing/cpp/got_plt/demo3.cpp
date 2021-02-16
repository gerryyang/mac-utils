#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <dlfcn.h>

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <limits.h>
#include <elf.h>
#include <link.h>

#include "testa.h"
#include "testa_hook.h"

//#include "plthook.h"

typedef struct plthook plthook_t;

#define PLTHOOK_SUCCESS              0
#define PLTHOOK_FILE_NOT_FOUND       1
#define PLTHOOK_INVALID_FILE_FORMAT  2
#define PLTHOOK_FUNCTION_NOT_FOUND   3
#define PLTHOOK_INVALID_ARGUMENT     4
#define PLTHOOK_OUT_OF_MEMORY        5
#define PLTHOOK_INTERNAL_ERROR       6
#define PLTHOOK_NOT_IMPLEMENTED      7

#if defined __x86_64__ || defined __x86_64
#define R_JUMP_SLOT   R_X86_64_JUMP_SLOT
#define R_GLOBAL_DATA R_X86_64_GLOB_DAT
#else
#error unsupported OS, X86_64 required.
#endif

#if !defined __linux__
#error unsupported OS, linux required.
#endif


#define Elf_Plt_Rel   Elf_Rela
#define PLT_DT_REL    DT_RELA
#define PLT_DT_RELSZ  DT_RELASZ
#define PLT_DT_RELENT DT_RELAENT

#if defined __LP64__
#ifndef ELF_CLASS
#define ELF_CLASS     ELFCLASS64
#endif

#define SIZE_T_FMT "lu"
#define ELF_WORD_FMT "u"
#define ELF_XWORD_FMT "lu"
#define ELF_SXWORD_FMT "ld"
#define Elf_Half Elf64_Half
#define Elf_Xword Elf64_Xword
#define Elf_Sxword Elf64_Sxword
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Sym  Elf64_Sym
#define Elf_Dyn  Elf64_Dyn
#define Elf_Rel  Elf64_Rel
#define Elf_Rela Elf64_Rela

#ifndef ELF_R_SYM
#define ELF_R_SYM ELF64_R_SYM
#endif

#ifndef ELF_R_TYPE
#define ELF_R_TYPE ELF64_R_TYPE
#endif

#endif/*__LP64__*/

struct plthook {
	const Elf_Sym *dynsym;
	const char *dynstr;
	size_t dynstr_size;
	const char *plt_addr_base;
	const Elf_Plt_Rel *rela_plt;
	size_t rela_plt_cnt;
#ifdef R_GLOBAL_DATA
	const Elf_Plt_Rel *rela_dyn;
	size_t rela_dyn_cnt;
#endif
};

static size_t page_size;
#define ALIGN_ADDR(addr) ((void*)((size_t)(addr) & ~(page_size - 1)))


//static void set_errmsg(const char *fmt, ...) __attribute__((__format__ (__printf__, 1, 2)));

static char errmsg[512];
static void set_errmsg(const char *fmt, ...)
{ 
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(errmsg, sizeof(errmsg) - 1, fmt, ap);
	va_end(ap);
} 
const char *plthook_error(void)
{
	return errmsg;
}

static const Elf_Dyn *find_dyn_by_tag(const Elf_Dyn *dyn, Elf_Sxword tag)
{
	while (dyn->d_tag != DT_NULL) {
		if (dyn->d_tag == tag) {
			return dyn;
		}
		dyn++;
	}
	return NULL;
}

#if 0
struct link_map
  {
    /* These first few members are part of the protocol with the debugger.
       This is the same format used in SVR4.  */

    ElfW(Addr) l_addr;   /* Difference between the address in the ELF
                            file and the addresses in memory.  */
    char *l_name;        /* Absolute file name object was found in.  */
    ElfW(Dyn) *l_ld;     /* Dynamic section of the shared object.  */
    struct link_map *l_next, *l_prev; /* Chain of loaded objects.  */
  }; 
#endif
static int plthook_open_real(plthook_t **plthook_out, struct link_map *lmap)
{
	plthook_t plthook = {NULL,};
	const Elf_Dyn *dyn;
	const char *dyn_addr_base = NULL;

	if (page_size == 0) {
		page_size = sysconf(_SC_PAGESIZE);
		printf("page_size(%" SIZE_T_FMT ")\n", page_size);
	}

	plthook.plt_addr_base = (char*)lmap->l_addr;

	/* get .dynsym section */
	dyn = find_dyn_by_tag(lmap->l_ld, DT_SYMTAB);
	if (dyn == NULL) {
		set_errmsg("failed to find DT_SYMTAB");
		return PLTHOOK_INTERNAL_ERROR;
	}
	plthook.dynsym = (const Elf_Sym*)(dyn_addr_base + dyn->d_un.d_ptr);

	/* Check sizeof(Elf_Sym) */
	dyn = find_dyn_by_tag(lmap->l_ld, DT_SYMENT);
	if (dyn == NULL) {
		set_errmsg("failed to find DT_SYMTAB");
		return PLTHOOK_INTERNAL_ERROR;
	}
	if (dyn->d_un.d_val != sizeof(Elf_Sym)) {
		set_errmsg("DT_SYMENT size %" ELF_XWORD_FMT " != %" SIZE_T_FMT, dyn->d_un.d_val, sizeof(Elf_Sym));
		return PLTHOOK_INTERNAL_ERROR;
	}

	/* get .dynstr section */
	dyn = find_dyn_by_tag(lmap->l_ld, DT_STRTAB);
	if (dyn == NULL) {
		set_errmsg("failed to find DT_STRTAB");
		return PLTHOOK_INTERNAL_ERROR;
	}
	plthook.dynstr = dyn_addr_base + dyn->d_un.d_ptr;

	/* get .dynstr size */
	dyn = find_dyn_by_tag(lmap->l_ld, DT_STRSZ);
	if (dyn == NULL) {
		set_errmsg("failed to find DT_STRSZ");
		return PLTHOOK_INTERNAL_ERROR;
	}
	plthook.dynstr_size = dyn->d_un.d_val;

	/* get .rela.plt or .rel.plt section */
	dyn = find_dyn_by_tag(lmap->l_ld, DT_JMPREL);
	if (dyn != NULL) {
		plthook.rela_plt = (const Elf_Plt_Rel *)(dyn_addr_base + dyn->d_un.d_ptr);
		dyn = find_dyn_by_tag(lmap->l_ld, DT_PLTRELSZ);
		if (dyn == NULL) {
			set_errmsg("failed to find DT_PLTRELSZ");
			return PLTHOOK_INTERNAL_ERROR;
		}
		plthook.rela_plt_cnt = dyn->d_un.d_val / sizeof(Elf_Plt_Rel);
	}
#ifdef R_GLOBAL_DATA
	/* get .rela.dyn or .rel.dyn section */
	dyn = find_dyn_by_tag(lmap->l_ld, PLT_DT_REL);
	if (dyn != NULL) {
		size_t total_size, elem_size;

		plthook.rela_dyn = (const Elf_Plt_Rel *)(dyn_addr_base + dyn->d_un.d_ptr);
		dyn = find_dyn_by_tag(lmap->l_ld, PLT_DT_RELSZ);
		if (dyn == NULL) {
			set_errmsg("failed to find PLT_DT_RELSZ");
			return PLTHOOK_INTERNAL_ERROR;
		}
		total_size = dyn->d_un.d_ptr;

		dyn = find_dyn_by_tag(lmap->l_ld, PLT_DT_RELENT);
		if (dyn == NULL) {
			set_errmsg("failed to find PLT_DT_RELENT");
			return PLTHOOK_INTERNAL_ERROR;
		}
		elem_size = dyn->d_un.d_ptr;
		plthook.rela_dyn_cnt = total_size / elem_size;
	}
#endif

#ifdef R_GLOBAL_DATA
	if (plthook.rela_plt == NULL && plthook.rela_dyn == NULL) {
		set_errmsg("failed to find either of DT_JMPREL and DT_REL");
		return PLTHOOK_INTERNAL_ERROR;
	}
#else
	if (plthook.rela_plt == NULL) {
		set_errmsg("failed to find DT_JMPREL");
		return PLTHOOK_INTERNAL_ERROR;
	}
#endif

	*plthook_out = (plthook_t *)malloc(sizeof(plthook_t));
    if (*plthook_out == NULL) {
        set_errmsg("failed to allocate memory: %" SIZE_T_FMT " bytes", sizeof(plthook_t));
        return PLTHOOK_OUT_OF_MEMORY;
    }
    **plthook_out = plthook;

	return 0;
}

#if 0
/* Rendezvous structure used by the run-time dynamic linker to communicate
   details of shared object loading to the debugger.  If the executable's
   dynamic section has a DT_DEBUG element, the run-time linker sets that
   element's value to the address where this structure can be found.  */

struct r_debug
  {
    int r_version;              /* Version number for this protocol.  */

    struct link_map *r_map;     /* Head of the chain of loaded objects.  */

    /* This is the address of a function internal to the run-time linker,
       that will always be called when the linker begins to map in a
       library or unmap it, and again when the mapping change is complete.
       The debugger can set a breakpoint at this address if it wants to
       notice shared object mapping changes.  */
    ElfW(Addr) r_brk;
    enum
      {
        /* This state value describes the mapping change taking place when
           the `r_brk' address is called.  */
        RT_CONSISTENT,          /* Mapping change is complete.  */
        RT_ADD,                 /* Beginning to add a new object.  */
        RT_DELETE               /* Beginning to remove an object mapping.  */
      } r_state;

    ElfW(Addr) r_ldbase;        /* Base address the linker is loaded at.  */
  };

/* This is the instance of that structure used by the dynamic linker.  */
extern struct r_debug _r_debug;
#endif
static int plthook_open_executable(plthook_t **plthook_out)
{
    // _r_debug.r_map: Head of the chain of loaded objects
	return plthook_open_real(plthook_out, _r_debug.r_map);
}

static int plthook_open_shared_library(plthook_t **plthook_out, const char *filename)
{
    void *hndl = dlopen(filename, RTLD_LAZY | RTLD_NOLOAD);

    struct link_map *lmap = NULL;

    if (hndl == NULL) {
        set_errmsg("dlopen error: %s", dlerror());
        return PLTHOOK_FILE_NOT_FOUND;
    }

    if (dlinfo(hndl, RTLD_DI_LINKMAP, &lmap) != 0) {
        set_errmsg("dlinfo error");
        dlclose(hndl);
        return PLTHOOK_FILE_NOT_FOUND;
    }
    dlclose(hndl);
    
    return plthook_open_real(plthook_out, lmap);
}


int plthook_open(plthook_t **plthook_out, const char *filename)
{
	*plthook_out = NULL;
	if (filename == NULL) {
		return plthook_open_executable(plthook_out);
	} else {
		return plthook_open_shared_library(plthook_out, filename);
	}
	return 0;
}

void plthook_close(plthook_t *plthook)
{
    if (plthook != NULL) {
        free(plthook);
    }
}

static int get_memory_permission(void *address)
{
    unsigned long addr = (unsigned long)address;
    FILE *fp;
    char buf[PATH_MAX];
    char perms[5];
    int bol = 1;

    fp = fopen("/proc/self/maps", "r");
    if (fp == NULL) {
        set_errmsg("failed to open /proc/self/maps");
        return 0;
    }
    while (fgets(buf, PATH_MAX, fp) != NULL) {
        unsigned long start, end;
        int eol = (strchr(buf, '\n') != NULL);
        if (bol) {
            /* The fgets reads from the beginning of a line. */
            if (!eol) {
                /* The next fgets reads from the middle of the same line. */
                bol = 0;
            }
        } else {
            /* The fgets reads from the middle of a line. */
            if (eol) {
                /* The next fgets reads from the beginnig of a line. */
                bol = 1;
            }
            continue;
        }

        if (sscanf(buf, "%lx-%lx %4s", &start, &end, perms) != 3) {
            continue;
        }
        if (start <= addr && addr < end) {
            int prot = 0;
            if (perms[0] == 'r') {
                prot |= PROT_READ;
            } else if (perms[0] != '-') {
                goto unknown_perms;
            }
            if (perms[1] == 'w') {
                prot |= PROT_WRITE;
            } else if (perms[1] != '-') {
                goto unknown_perms;
            }
            if (perms[2] == 'x') {
                prot |= PROT_EXEC;
            } else if (perms[2] != '-') {
                goto unknown_perms;
            }
            if (perms[3] != 'p') {
                goto unknown_perms;
            }
            if (perms[4] != '\0') {
                perms[4] = '\0';
                goto unknown_perms;
            }
            fclose(fp);
            return prot;
        }
    }
    fclose(fp);
    set_errmsg("Could not find memory region containing %p", (void*)addr);
    return 0;
unknown_perms:
    fclose(fp);
    set_errmsg("Unexcepted memory permission %s at %p", perms, (void*)addr);
    return 0;
}

static int check_rel(const plthook_t *plthook, const Elf_Plt_Rel *plt, Elf_Xword r_type, const char **name_out, void ***addr_out)
{
    if (ELF_R_TYPE(plt->r_info) == r_type) {
        size_t idx = ELF_R_SYM(plt->r_info);
        idx = plthook->dynsym[idx].st_name;
        if (idx + 1 > plthook->dynstr_size) {
            set_errmsg("too big section header string table index: %" SIZE_T_FMT, idx);
            return PLTHOOK_INVALID_FILE_FORMAT;
        }
        *name_out = plthook->dynstr + idx;
        *addr_out = (void**)(plthook->plt_addr_base + plt->r_offset);
        return 0;
    }
    return -1;
}

int plthook_enum(plthook_t *plthook, unsigned int *pos, const char **name_out, void ***addr_out)
{
    while (*pos < plthook->rela_plt_cnt) {
        const Elf_Plt_Rel *plt = plthook->rela_plt + *pos;
        int rv = check_rel(plthook, plt, R_JUMP_SLOT, name_out, addr_out);
        (*pos)++;
        if (rv >= 0) {
            return rv;
        }
    }
#ifdef R_GLOBAL_DATA
    while (*pos < plthook->rela_plt_cnt + plthook->rela_dyn_cnt) {
        const Elf_Plt_Rel *plt = plthook->rela_dyn + (*pos - plthook->rela_plt_cnt);
        int rv = check_rel(plthook, plt, R_GLOBAL_DATA, name_out, addr_out);
        (*pos)++;
        if (rv >= 0) {
            return rv;
        }
    }
#endif
    *name_out = NULL;
    *addr_out = NULL;
    return EOF;
}

int plthook_replace(plthook_t *plthook, const char *funcname, void *funcaddr, void **oldfunc)
{
    size_t funcnamelen = strlen(funcname);
    unsigned int pos = 0;
    const char *name;
    void **addr;
    int rv;

    if (plthook == NULL) {
        set_errmsg("invalid argument: The first argument is null.");
        return PLTHOOK_INVALID_ARGUMENT;
    }

    while ((rv = plthook_enum(plthook, &pos, &name, &addr)) == 0) {
		printf("name(%s)\n", name);

        if (strncmp(name, funcname, funcnamelen) == 0) {
            if (name[funcnamelen] == '\0' || name[funcnamelen] == '@') {
                int prot = get_memory_permission(addr);
                if (prot == 0) {
                    return PLTHOOK_INTERNAL_ERROR;
                }
                if (!(prot & PROT_WRITE)) {
                    if (mprotect(ALIGN_ADDR(addr), page_size, PROT_READ | PROT_WRITE) != 0) {
                        set_errmsg("Could not change the process memory permission at %p: %s",
                                   ALIGN_ADDR(addr), strerror(errno));
                        return PLTHOOK_INTERNAL_ERROR;
                    }
                }
                if (oldfunc) {
                    *oldfunc = *addr;
                }
                *addr = funcaddr;
                if (!(prot & PROT_WRITE)) {
                    mprotect(ALIGN_ADDR(addr), page_size, prot);
                }
                return 0;
            }
        }
    }
    if (rv == EOF) {
        set_errmsg("no such function: %s", funcname);
        rv = PLTHOOK_FUNCTION_NOT_FOUND;
    }
    return rv;
}

int install_hook_function()
{
	plthook_t *plthook;

	if (plthook_open(&plthook, NULL) != 0) {
		printf("plthook_open error: %s\n", plthook_error());
		return -1;
	}
	if (plthook_replace(plthook, "say_hello", (void*)say_hello_hook, NULL) != 0) {
		printf("plthook_replace error: %s\n", plthook_error());
		plthook_close(plthook);
		return -1;
	}
	plthook_close(plthook);
	return 0;
}

int main()
{
	install_hook_function();

	say_hello();

	return 0;
}
