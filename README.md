my-utils
========

some tools about mac

Tips:

(1) 把Finder中的文件直接拖拽到terminal可以立即显示文件的路径

(2) say Mac OS X，通过say命令进行语音阅读

(3) Mac上的otool工具等同于Linux上的objdump，也可以通过brew search/install binutils然后使用gobjdump
use otool -tV <executable> to disassemble
use gobjdump -a libcrypto.a to see mach-o-i386 or something

(4) Mac上使用dtruss命令跟踪某个进程，对应Linux上的strace命令

(5) Mac上使用 sysctl -a 查看系统的相关信息
在Linux上查看cpu的信息，可以 cat /proc/cpuinfo
在Mac上，则可以 sysctl -a machdep.cpu



