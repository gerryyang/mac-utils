/* Code sample: using ptrace for simple tracing of a child process.
**
** Note: this was originally developed for a 32-bit x86 Linux system; some
** changes may be required to port to x86-64.
**
** Eli Bendersky (https://eli.thegreenplace.net)
** This code is in the public domain.
*/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>

/* Print a message to stdout, prefixed by the process ID
 */
void procmsg(const char* format, ...)
{
    va_list ap;
    fprintf(stdout, "[%d] ", getpid());
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}

void run_target(const char* programname)
{
    procmsg("target started. will run '%s'\n", programname);

    /* Allow tracing of this process */
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
    {
        perror("ptrace");
        return;
    }

    /* Replace this process's image with the given program */
    execl(programname, programname, 0);
}

void run_debugger(pid_t child_pid)
{
    int wait_status;
    unsigned icounter = 0;
    procmsg("debugger started\n");

    /* Wait for child to stop on its first instruction */
    wait(&wait_status);

    while (WIFSTOPPED(wait_status))
    {
        icounter++;
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
        unsigned instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);

        procmsg("icounter = %u.  RIP = 0x%08x.  instr = 0x%08x\n", icounter, regs.rip, instr);

        /* Make the child execute another instruction */
        if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0)
        {
            perror("ptrace");
            return;
        }

        /* Wait for child to stop on its next instruction */
        wait(&wait_status);
    }

    procmsg("the child executed %u instructions\n", icounter);
}

int main(int argc, char** argv)
{
    pid_t child_pid;

    if (argc < 2)
    {
        fprintf(stderr, "Expected a program name as argument\n");
        return -1;
    }

    child_pid = fork();
    if (child_pid == 0)
        run_target(argv[1]);
    else if (child_pid > 0)
        run_debugger(child_pid);
    else
    {
        perror("fork");
        return -1;
    }

    return 0;
}
/*
构建环境：x86-64
gcc -g test.c

[3136473] debugger started
[3136473] icounter = 1.  EIP = 0x9a413090.  instr = 0xe8e78948
[3136473] icounter = 2.  EIP = 0x9a413093.  instr = 0x000d58e8
[3136473] icounter = 3.  EIP = 0x9a413df0.  instr = 0xfa1e0ff3
[3136473] icounter = 4.  EIP = 0x9a413df4.  instr = 0xe5894855
[3136473] icounter = 5.  EIP = 0x9a413df5.  instr = 0x41e58948
[3136473] icounter = 6.  EIP = 0x9a413df8.  instr = 0x56415741
[3136473] icounter = 7.  EIP = 0x9a413dfa.  instr = 0x55415641
[3136473] icounter = 8.  EIP = 0x9a413dfc.  instr = 0x54415541
[3136473] icounter = 9.  EIP = 0x9a413dfe.  instr = 0x89495441
...
[3136473] icounter = 195941.  EIP = 0x9a100af0.  instr = 0xfa1e0ff3
[3136473] icounter = 195942.  EIP = 0x9a100af4.  instr = 0xb841fa89
[3136473] icounter = 195943.  EIP = 0x9a100af6.  instr = 0x00e7b841
[3136473] icounter = 195944.  EIP = 0x9a100afc.  instr = 0x00003cbe
[3136473] icounter = 195945.  EIP = 0x9a100b01.  instr = 0x780d8b4c
[3136473] icounter = 195946.  EIP = 0x9a100b08.  instr = 0x0f6615eb
[3136473] icounter = 195947.  EIP = 0x9a100b1f.  instr = 0x8944d789
[3136473] icounter = 195948.  EIP = 0x9a100b21.  instr = 0x0fc08944
[3136473] icounter = 195949.  EIP = 0x9a100b24.  instr = 0x3d48050f
[3136473] the child executed 195949 instructions
*/