
```
$ g++ data_race2.cc -fsanitize=thread -fPIE -pie -g
$ ./a.out 
==================
WARNING: ThreadSanitizer: data race (pid=7279)
  Read of size 4 at 0x55c04c64f01c by thread T2:
    #0 Thread2(void*) /home/ubuntu/github/mac-utils/programing/cpp/data_race/data_race2.cc:12 (a.out+0xb3d)
    #1 <null> <null> (libtsan.so.0+0x296ad)

  Previous write of size 4 at 0x55c04c64f01c by thread T1:
    #0 Thread1(void*) /home/ubuntu/github/mac-utils/programing/cpp/data_race/data_race2.cc:7 (a.out+0xafe)
    #1 <null> <null> (libtsan.so.0+0x296ad)

  Location is global 'Global' of size 4 at 0x55c04c64f01c (a.out+0x00000020201c)

  Thread T2 (tid=7282, running) created by main thread at:
    #0 pthread_create <null> (libtsan.so.0+0x2bcee)
    #1 main /home/ubuntu/github/mac-utils/programing/cpp/data_race/data_race2.cc:19 (a.out+0xbce)

  Thread T1 (tid=7281, finished) created by main thread at:
    #0 pthread_create <null> (libtsan.so.0+0x2bcee)
    #1 main /home/ubuntu/github/mac-utils/programing/cpp/data_race/data_race2.cc:18 (a.out+0xbad)

SUMMARY: ThreadSanitizer: data race /home/ubuntu/github/mac-utils/programing/cpp/data_race/data_race2.cc:12 in Thread2(void*)
==================
ThreadSanitizer: reported 1 warnings
```

refer: https://github.com/google/sanitizers/wiki/ThreadSanitizerCppManual

