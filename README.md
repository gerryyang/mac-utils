mac-utils
========

> Some tools about Mac OSX system.

#Tips:

* 把Finder中的文件直接拖拽到terminal可以立即显示文件的路径

* 在终端下通过 `open $dirpath` 可以打开某个文件夹，或者使用 `shift+command+g` 快捷键前往某个路径

* say Mac OS X，通过 `say` 命令进行语音阅读

* Mac上的 `otool -L` 等同于Linux上的 `ldd` 命令

* Mac上的 `otool` 等同于Linux上的 `objdump`，也可以通过brew search/install binutils然后使用 `gobjdump`

	use `otool -tV <executable>` to disassemble

	use `gobjdump -a libcrypto.a` to see mach-o-i386 or something

* Mac上使用 `dtruss` 命令跟踪某个进程，对应Linux上的strace命令

* Mac上使用 `sysctl -a` 查看系统的相关信息

	在Linux上查看cpu的信息，可以 cat /proc/cpuinfo

	在Mac上，则可以 sysctl -a machdep.cpu

* Mac上的 `top` 命令用法和Linux上有些不同

	Linux上使用 **h** 查看帮助信息

	Mac上top使用 **?** 来查看帮助信息

	按CPU排序是ocpu，按内存排序是ovsize

	top -F -R -o cpu (降低top自身暂用的CPU使用率)

	[更多](http://osxdaily.com/2009/10/06/monitoring-cpu-usage-on-your-mac-a-better-top-command/)

	在Linux上使用 `top 1` 可以查看每个cpu的使用率

	在Mac上使用活动监视器的图形工具也可以查看

---



