
int main(int argc, char **argv)
{
    int *array = new int[100];
    delete[] array;
    return array[argc];  // BOOM
}

/*
$ ./test
=================================================================
==190161==ERROR: AddressSanitizer: heap-use-after-free on address 0x614000000044 at pc 0x0000004f584f bp 0x7ffc0b451fb0 sp 0x7ffc0b451fa8
READ of size 4 at 0x614000000044 thread T0
    #0 0x4f584e in main /data/home/gerryyang/github/mac-utils/programing/cpp/asan/test.cc:6:12
    #1 0x7f6ec75b1f92 in __libc_start_main (/lib64/libc.so.6+0x26f92)
    #2 0x41f31d in _start (/data/home/gerryyang/github/mac-utils/programing/cpp/asan/test+0x41f31d)

0x614000000044 is located 4 bytes inside of 400-byte region [0x614000000040,0x6140000001d0)
freed by thread T0 here:
    #0 0x4f35c0 in operator delete[](void*) /data/home/gerryyang/tools/clang/llvm-project-11.0.0/compiler-rt/lib/asan/asan_new_delete.cpp:163:3
    #1 0x4f581e in main /data/home/gerryyang/github/mac-utils/programing/cpp/asan/test.cc:5:5
    #2 0x7f6ec75b1f92 in __libc_start_main (/lib64/libc.so.6+0x26f92)

previously allocated by thread T0 here:
    #0 0x4f2c28 in operator new[](unsigned long) /data/home/gerryyang/tools/clang/llvm-project-11.0.0/compiler-rt/lib/asan/asan_new_delete.cpp:102:3
    #1 0x4f5813 in main /data/home/gerryyang/github/mac-utils/programing/cpp/asan/test.cc:4:18
    #2 0x7f6ec75b1f92 in __libc_start_main (/lib64/libc.so.6+0x26f92)

SUMMARY: AddressSanitizer: heap-use-after-free /data/home/gerryyang/github/mac-utils/programing/cpp/asan/test.cc:6:12 in main
Shadow bytes around the buggy address:
  0x0c287fff7fb0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c287fff7fc0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c287fff7fd0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c287fff7fe0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c287fff7ff0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x0c287fff8000: fa fa fa fa fa fa fa fa[fd]fd fd fd fd fd fd fd
  0x0c287fff8010: fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd
  0x0c287fff8020: fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd
  0x0c287fff8030: fd fd fd fd fd fd fd fd fd fd fa fa fa fa fa fa
  0x0c287fff8040: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c287fff8050: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==190161==ABORTING
*/