---
layout: post
title:  "AddressSanitizer: A Fast Address Sanity Checker (Paper 2012)"
date:   2022-11-30 20:30:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Abstract

Memory access bugs, including buffer overflows and uses of freed heap memory, remain a serious problem for programming languages like C and C++. Many memory error detectors exist, but most of them are either **slow** or **detect a limited set of bugs**, or both.

This paper presents **AddressSanitizer**, a new memory error detector. Our tool finds **out-of-bounds** accesses to heap, stack, and global objects, as well as **use-after-free** bugs. **It employs a specialized memory allocator and code instrumentation** that is simple enough to be implemented in any compiler, binary translation system, or even in hardware.

**AddressSanitizer** achieves **efficiency** without sacrificing comprehensiveness. **Its average slowdown is just 73% yet it accurately detects bugs at the point of occurrence**. It has found over 300 previously unknown bugs in the Chromium browser and many bugs in other software.

# Introduction

Dozens of memory error detection tools are available on the market [3, 8, 11, 13, 15, 21, 23, 24]. These tools differ in **speed**, **memory consumption**, **types of detectable bugs**, **probability of detecting a bug**, **supported platforms**, and other characteristics. Many tools succeed in detecting a wide range of bugs but **incur high overhead**, or **incur low overhead but detect fewer bugs**.

We present **AddressSanitizer**, a new tool that **combines performance and coverage**. AddressSanitizer finds **out-of-bounds accesses** (for heap, stack, and global objects) and **uses of freed heap memory** at the relatively low **cost of 73% slowdown and 3.4x increased memory usage**, making it a good choice for testing a wide range of C/C++ applications.

**AddressSanitizer** consists of two parts: **an instrumentation module** and **a run-time library**.

* The instrumentation module modifies the code to check the shadow state for each memory access and creates poisoned redzones around stack and global objects to detect overflows and underflows. The current implementation is based on the **LLVM** [4] compiler infrastructure.

* The run-time library replaces `malloc`, `free` and related functions, creates poisoned redzones around allocated heap regions, delays the reuse of freed heap regions, and does error reporting.

## Contributions

In this paper we:

* show that a memory error detector can leverage **the comprehensiveness of shadow memory** with much lower overhead than the conventional wisdom;

* present a novel **shadow state encoding that enables compact shadow memory** – as much as a 128-to-1 mapping – for detecting out-of-bounds and use- after-free bugs;

* describe a **specialized memory allocator** targeting our shadow encoding;

* evaluate a new publicly **available tool** that efficiently identifies memory bugs.

## Outline

After summarizing related work in the next section, we describe the **AddressSanitizer algorithm** in Section 3. **Experimental results** with AddressSanitizer are provided in Section 4. We discuss **further improvements** in Section 5 and then **conclude the paper**.

# Related Work

This section explores the range of existing memory detection tools and techniques.

## Shadow Memory

Many different tools **use shadow memory to store metadata corresponding to each piece of application data**. Typically an application address is mapped to a shadow address either by a direct scale and offset, where the full application address space is mapped to a single shadow address space, or by extra levels of translation involving table lookups. Examples of direct mapping include TaintTrace [10] and LIFT [26]. TaintTrace requires a shadow space of equal size to the application address space, which results in difficulties supporting applications that cannot survive with only one-half of their normal address space. The shadow space in LIFT is one-eighth of the application space.

To provide more flexibility in address space layout, some tools use **multi-level translation schemes**. Valgrind [20] and Dr. Memory [8] split their shadow memory into pieces and use a table lookup to obtain the shadow address, requiring an extra memory load. For 64-bit platforms, Valgrind uses an additional table layer for application addresses not in the lower 32GB.

Umbra [30, 31] combines layout flexibility with efficiency, avoiding a table lookup via a non-uniform and dynamically-tuned scale and offset scheme. BoundLess [9] stores some of its metadata in 16 higher bits of 64-bit pointers, but falls back to more traditional shadow memory on the slow path. LBC [12] performs a fast-path check using special values stored in the application memory and relies on two-level shadow memory on the slow path.

## Instrumentation

A large number of memory error detectors are based on binary instrumentation. Among the most popular are Valgrind (Memcheck) [21], Dr. Memory [8], Purify[13], BoundsChecker [17], Intel Parallel Inspector [15] and Discover [23]. These tools find out-of-bounds and use-after-free bugs for heap memory with (typically) no false positives. **To the best of our knowledge none of the tools based on binary instrumentation can find out-of-bounds bugs in the stack (other than beyond the top of the stack) or globals**. These tools additionally find uninitialized reads.

Mudflap [11] uses compile-time instrumentation and hence is capable of detecting out-of-bounds accesses for stack objects. However, it does not insert redzones between different stack objects in one stack frame and will thus not detect all stack buffer overflow bugs. It is also known to have false positive reports in complex C++ code.

CCured [19] combines instrumentation with static analysis (only for C programs) to eliminate redundant checks; their instrumentation is incompatible with uninstrumented libraries.

LBC [12] uses source-to-source transformation and relies on CCured to eliminate redundant checks. LBC is limited to the C language and does not handle use-after-free bugs.

Insure++ [24] relies mainly on compile-time instrumentation but also uses binary instrumentation. Details of its implementation are not publicly available.

## Debug Allocators

Another class of memory error detectors uses a specialized memory allocator and does not change the rest of the execution.

Tools like Electric Fence [25], Duma [3], GuardMalloc [16] and Page Heap [18] **use CPU page protection**. Each allocated region is placed into a dedicated page (or a set of pages). One extra page at the right (and/or at the left) is allocated and marked as inaccessible. A subsequent page fault accessing these pages is then reported as an out-of-bounds error. **These tools incur large memory overheads** and may be very slow on malloc-intensive applications (as each malloc call requires at least one system call). Also, these tools may miss some classes of bugs (e.g., reading the byte at offset 6 from the start of a 5-byte memory region). If a bug is reported, the responsible instruction is provided in the error message.

Some other malloc implementations, including DieHarder [22] (a descendant of DieHard [5] malloc) and Dmalloc [2], find memory bugs on a **probabilistic** and/or delayed basis. **Their modified malloc function adds redzones around memory regions returned to the user and populates the newly allocated memory with special magic values. The free function also writes magic values to the memory region**.

If a magic value is read then the program has accessed an out-of-bounds or uninitialized value. However, there is no immediate detection of this. Through properly selected magic values, there is a chance that the program will behave incorrectly in a way detectable by existing application tests (DieHard [5] has a replicated mode in which it is able to detect such incorrect behavior by comparing the output of several program replicas initialized with different magic values). In other words, the detection of out-of-bounds reads and read-after-free bugs is probabilistic.

If a magic value in a redzone is overwritten, this will later be detected when the redzone is examined on free, but the tool does not know exactly when the out-of-bounds write or write-after-free occurred. For large programs it is often equivalent to reporting “your program has a bug”. Note that the goal of DieHarder is not only to detect bugs, but also to protect from security attacks.

The two debug malloc approaches are often combined. Debug malloc tools do not handle stack variables or globals.

The same magic value technique is often used for buffer overflow protection. StackGuard [29] and ProPolice [14] (the StackGuard reimplementation currently used by GCC) place a canary value between the local variables and the return address in the current stack frame and check for that value’s consistency upon function exit. This helps to prevent stack smashing buffer overflows, but is unable to detect arbitrary out-of-bounds accesses to stack objects.


# AddressSanitizer Algorithm

From a high level, our approach to memory error detection is similar to that of the Valgrind-based tool AddrCheck [27]: **use shadow memory to record whether each byte of application memory is safe to access, and use instrumentation to check the shadow memory on each application load or store**. However, AddressSanitizer uses a more efficient shadow mapping, a more compact shadow encoding, detects errors in stack and global variables in addition to the heap and is an order of magnitude faster than AddrCheck. The following sections describe how AddressSanitizer encodes and maps its shadow memory, inserts its instrumentation, and how its run-time library operates.


## Shadow Memory

The memory addresses returned by the `malloc` function `are typically aligned to at least 8 bytes`. This leads to the observation that any aligned 8-byte sequence of application heap memory is in one of 9 different states: the first k (0 ≤ k ≤ 8) bytes are addressable and the remaining 8 − k bytes are not. This state can be encoded into a single byte of shadow memory.

AddressSanitizer dedicates **one-eighth of the virtual address space to its shadow memory** and uses a direct mapping with a scale and offset to translate an application address to its corresponding shadow address. Given the application memory address Addr, the address of the shadow byte is computed as `(Addr>>3)+Offset`. If `Max-1` is the maximum valid address in the virtual address space, the value of `Offset` should be chosen in such a way that the region from `Offset` to `Offset+Max/8` is not occupied at startup. Unlike in Umbra [31], the `Offset` must be chosen statically for every platform, but we do not see this as a serious limitation.

On a typical `32-bit` Linux or MacOS system, where the virtual address space is `0x00000000-0xffffffff`, we use `Offset = 0x20000000` (`2^29`).

On a `64-bit` system with `47` significant address bits we use `Offset = 0x0000100000000000`(`2^44`). In some cases (e.g., with `-fPIE/-pie` compiler flags on Linux) a zero offset can be used to simplify instrumentation even further.

**Figure 1** shows the address space layout. The application memory is split into two parts (low and high) which map to the corresponding shadow regions. Applying the shadow mapping to addresses in the shadow region gives us addresses in the Bad region, which is marked inaccessible via page protection.

// Figure 1



We use the following encoding for each shadow byte: `0` means that all `8` bytes of the corresponding application memory region are addressable; `k` (`1 ≤ k ≤ 7`) means that the first `k` bytes are addressible; any **negative value** indicates that the entire `8-byte` word is unaddressable. We use different negative values to distinguish between different kinds of unaddressable memory (heap redzones, stack redzones, global redzones, freed memory).

This shadow mapping could be generalized to the form `(Addr>>Scale)+Offset`, where `Scale` is one of `1 . . . 7`. With `Scale=N`, the shadow memory occupies `1/2^N` of the virtual address space and the minimum size of the redzone (and the `malloc` alignment) is `2^N` bytes. Each shadow byte describes the state of `2^N` bytes and encodes `2^N + 1` different values. Larger values of Scale require less shadow memory but greater redzone sizes to satisfy alignment requirements. Values of Scale greater than `3` require more complex instrumentation for `8-byte` accesses (see Section 3.2) but provide more flexibility with applications that may not be able to give up a single contiguous one-eighth of their address space.

## Instrumentation



## Run-time Library



## Stack And Globals


## False Negatives


## False Positives


### Conflict With Load Widening



### Conflict With Clone


### Intentional Wild Dereferences


## Threads


# Evaluation

## Comparison



## AddressSanitizer Deployment


## Tuning Accuracy And Resource Usage


# Future Work

## Compile-time Optimizations


## Handling Libraries


## Hardware Support




# Conclusions

In this paper we presented AddressSanitizer, a fast memory error detector. AddressSanitizer finds out-of-bounds (for heap, stack, and globals) accesses and use-after-free bugs at the cost of 73% slowdown on average; the tool has no false positives.



## Availability

AddressSanitizer is open source and is integrated with the LLVM compiler tool chain [4] starting from version 3.1. The documentation can be found at http://clang.llvm.org/docs/AddressSanitizer.html.



# Appendix: Instrumentation Examples

Here we give two examples of instrumentation on x86 64 (8- and 4- byte stores). C program:

``` cpp
void foo(T *a) {
  *a = 0x1234;
}
```

8-byte store:

clang -O2 -faddress-sanitizer a.c -c -DT=long

``` c
push %rax
mov %rdi,%rax
shr $0x3,%rax
mov $0x100000000000,%rcx
or %rax,%rcx
cmpb $0x0,(%rcx) # Compare Shadow with 0 jne 23 <foo+0x23> # To Error
movq $0x1234,(%rdi) # Original store pop %rax
retq
callq __asan_report_store8 # Error
```

4-byte store:

clang -O2 -faddress-sanitizer a.c -c -DT=int

``` c
push %rax
mov %rdi,%rax
shr $0x3,%rax
mov $0x100000000000,%rcx
or %rax,%rcx
mov (%rcx),%al # Get Shadow
test %al,%al
je 27 <foo+0x27> # To original store mov %edi,%ecx # Slow path
and $0x7,%ecx # Slow path
add $0x3,%ecx # Slow path
cmp %al,%cl
jge 2f <foo+0x2f> # To Error
movl $0x1234,(%rdi) # Original store pop %rax
retq
callq __asan_report_store4  # Error
```



# References

[1] The Chromium project. http://dev.chromium.org
[2] Dmalloc – Debug Malloc Library. http://www.dmalloc.com.
[3] D.U.M.A. – Detect Unintended Memory Access. http://duma.sourceforge.net/.
[4] The LLVM Compiler Infrastructure. http://llvm.org.
[5] Emery D. Berger and Benjamin G. Zorn. DieHard: probabilistic memory safety for unsafe languages. In PLDI 06, pages 158–168. ACM Press, 2006.
[6] Derek Bruening. Efficient, Transparent, and Comprehensive Run-time Code Manipulation. PhD thesis, M.I.T., September 2004.
[7] Derek Bruening, Timothy Garnett, and Saman Amarasinghe. An infrastructure for adaptive dynamic optimization. In Proc. of the International Symposium on Code Generation and Optimization (CGO ’03), pages 265–275, March 2003.
[8] Derek Bruening and Qin Zhao. Practical memory checking with Dr. Memory. In Proc. of the International Symposium on Code Generation and Optimization (CGO ’11), pages 213–223, April 2011.
[9] Marc Bru ̈ nink, Martin Su ̈ ßkraut, and Christof Fetzer. Boundless memory allocations for memory safety and high availability. In Proc. of the 41st Annual IEEE/IFIP International Conference on Dependable Systems and Networks (DSN 2011). IEEE Computer Society, June 2011.
[10] Winnie Cheng, Qin Zhao, Bei Yu, and Scott Hiroshige. Taint-trace: Efficient flow tracing with dynamic binary rewriting. In Proc. of the 11th IEEE Symposium on Computers and Communi- cations (ISCC ’ 06), pages 749–754, 2006.
[11] Frank Ch. Eigler. Mudflap: pointer use checking for C/C++. Red Hat Inc.
[12] Niranjan Hasabnis, Ashish Misra, and R. Sekar. Light-weight bounds checking. In Proc. of the International Symposium on Code Generation and Optimization (CGO ’12), pages 135–144, April 2012.
[13] Reed Hastings and Bob Joyce. Purify: Fast detection of memory leaks and access errors. In Proc. of the Winter USENIX Conference, pages 125–136, January 1992.
[14] IBM Research. GCC extension for protecting applications from stack-smashing attacks. http://researchweb.watson.ibm.com/trl/projects/security/ssp/.
[15] Intel. Intel Parallel Inspector. http://software.intel.com/en-us/intel-parallel-inspector/.
[16] Mac OS X Developer Library. Memory Usage Performance Guidelines: Enabling the Malloc Debugging Features. http://developer.apple.com/library/mac/#documentation/darwin/reference/manpages/man3/libgmalloc.3.html.
[17] Micro Focus. BoundsChecker. http://www.microfocus.com/products/micro-focus-developer/devpartner/visual-c.aspx.
[18] Microsoft Support. How to use Pageheap.exe in Windows XP, Windows 2000, and Windows Server 2003. http://support.microsoft.com/kb/286470.
[19] George C. Necula, Scott McPeak, and Westley Weimer. CCured: Type-safe retrotting of legacy code. In Proc. of the , Principles of Programming Languages, pages 128–139, 2002.
[20] Nicholas Nethercote and Julian Seward. How to shadow every byte of memory used by a program. In Proc. of the 3rd International Conference on Virtual Execution Environments (VEE ’07), pages 65–74, June 2007.
[21] Nicholas Nethercote and Julian Seward. Valgrind: A framework for heavyweight dynamic binary instrumentation. In Proc. of the ACM SIGPLAN Conference on Programming Language Design and Implementation (PLDI ’07), pages 89–100, June 2007.
[22] Gene Novark and Emery D. Berger. DieHarder: securing the heap. In Proc. of the 17th ACM conference on Computer and communications security, CCS ’10, pages 573–584. ACM, 2010.
[23] Oracle. Sun Memory Error Discovery Tool (Discover). http://download.oracle.com/docs/cd/E19205-01/821-1784/6nmoc18gq/index.html.
[24] Parasoft. Insure++. http://www.parasoft.com/jsp/products/insure.jsp?itemId=63.
[25] Bruce Perens. Electric Fence. http://perens.com/FreeSoftware/ElectricFence/.
[26] Feng Qin, Cheng Wang, Zhenmin Li, Ho-seop Kim, Yuanyuan Zhou, and Youfeng Wu. LIFT: A low-overhead practical information flow tracking system for detecting security attacks. In Proc. of the 39th International Symposium on Microarchitecture (MICRO 39), pages 135–148, 2006.
[27] Julian Seward and Nicholas Nethercote. Using Valgrind to detect undefined value errors with bit-precision. In Proc. of the USENIX Annual Technical Conference, pages 2–2, 2005.
[28] Standard Performance Evaluation Corporation. SPEC CPU2006 benchmark suite, 2006. http://www.spec.org/osg/cpu2006/.
[29] Perry Wagle and Crispin Cowan. Stackguard: Simple stack smash protection for gcc. In Proc. of the GCC Developers Summit, pages 243–255, 2003.
[30] Qin Zhao, Derek Bruening, and Saman Amarasinghe. Efficient memory shadowing for 64-bit architectures. In Proc. of the The International Symposium on Memory Management (ISMM ’10), pages 93–102, Jun 2010.
[31] Qin Zhao, Derek Bruening, and Saman Amarasinghe. Umbra: Efficient and scalable memory shadowing. In Proc. of the International Symposium on Code Generation and Optimization (CGO ’10), pages 22–31, April 2010.



