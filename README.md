my-utils
========

some tools about mac

Tips:

(1) 把Finder中的文件直接拖拽到terminal可以立即显示文件的路径

(2) say Mac OS X，通过say命令进行语音阅读

(3) Mac上的 otool -L 等同于Linux上的ldd命令
gerryyang@mba:mac-utils$otool -L /bin/ls
/bin/ls:
	/usr/lib/libutil.dylib (compatibility version 1.0.0, current version 1.0.0)
	/usr/lib/libncurses.5.4.dylib (compatibility version 5.4.0, current version 5.4.0)
	/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1197.1.1)

(4) Mac上的otool工具等同于Linux上的objdump，也可以通过brew search/install binutils然后使用gobjdump
use otool -tV <executable> to disassemble
use gobjdump -a libcrypto.a to see mach-o-i386 or something

(5) Mac上使用dtruss命令跟踪某个进程，对应Linux上的strace命令

(6) Mac上使用 sysctl -a 查看系统的相关信息
在Linux上查看cpu的信息，可以 cat /proc/cpuinfo
在Mac上，则可以 sysctl -a machdep.cpu

(7) Mac上的top命令用法和Linux上有些不同
Linux上使用h查看帮助信息
Mac上top使用?来查看帮助信息
按CPU排序是ocpu，按内存排序是ovsize
top -F -R -o cpu (降低top自身暂用的CPU使用率)
http://osxdaily.com/2009/10/06/monitoring-cpu-usage-on-your-mac-a-better-top-command/

在Linux上使用top 1可以查看每个多核cpu每个核的使用率
在Mac上使用活动监视器的图形工具也可以查看



