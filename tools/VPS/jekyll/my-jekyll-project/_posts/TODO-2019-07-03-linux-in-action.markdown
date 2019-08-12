---
layout: post
title:  "Linux in Action"
date:   2019-05-06 17:00:00 +0800
categories: cpp
---

* Do not remove this line (it will not be displayed)
{:toc}


## 开发相关

### strip & objcopy (对可执行文件瘦身)

```
strip - Discard symbols from object files
```

```
objcopy - copy and translate object file

-g
--strip-debug
Do not copy debugging symbols or sections from the source file.

--only-keep-debug
Strip a file, removing contents of any sections that would not be stripped by --strip-debug and leaving the debugging sections intact. In ELF files, this preserves all note sections in the output.

Note - the section headers of the stripped sections are preserved, including their sizes, but the contents of the section are discarded. The section headers are preserved so that other tools can match up the debuginfo file with the real executable, even if that executable has been relocated to a different address space.

The intention is that this option will be used in conjunction with --add-gnu-debuglink to create a two part executable. One a stripped binary which will occupy less space in RAM and in a distribution and the second a debugging information file which is only needed if debugging abilities are required. The suggested procedure to create these files is as follows:

Link the executable as normal. Assuming that it is called foo then...
Run objcopy --only-keep-debug foo foo.dbg to create a file containing the debugging info.
Run objcopy --strip-debug foo to create a stripped executable.
Run objcopy --add-gnu-debuglink=foo.dbg foo to add a link to the debugging info into the stripped executable.
Note—the choice of .dbg as an extension for the debug info file is arbitrary. Also the --only-keep-debug step is optional. You could instead do this:

Link the executable as normal.
Copy foo to foo.full
Run objcopy --strip-debug foo
Run objcopy --add-gnu-debuglink=foo.full foo
i.e., the file pointed to by the --add-gnu-debuglink can be the full executable. It does not have to be a file created by the --only-keep-debug switch.

Note—this switch is only intended for use on fully linked files. It does not make sense to use it on object files where the debugging information may be incomplete. Besides the gnu_debuglink feature currently only supports the presence of one filename containing debugging information, not multiple filenames on a one-per-object-file basis.

--add-gnu-debuglink=path-to-file
Creates a .gnu_debuglink section which contains a reference to path-to-file and adds it to the output file. Note: the file at path-to-file must exist. Part of the process of adding the .gnu_debuglink section involves embedding a checksum of the contents of the debug info file into the section.

If the debug info file is built in one location but it is going to be installed at a later time into a different location then do not use the path to the installed location. The --add-gnu-debuglink option will fail because the installed file does not exist yet. Instead put the debug info file in the current directory and use the --add-gnu-debuglink option without any directory components, like this:

 objcopy --add-gnu-debuglink=foo.debug
At debug time the debugger will attempt to look for the separate debug info file in a set of known locations. The exact set of these locations varies depending upon the distribution being used, but it typically includes:

* The same directory as the executable.
* A sub-directory of the directory containing the executable
called .debug

* A global debug directory such as /usr/lib/debug.
As long as the debug info file has been installed into one of these locations before the debugger is run everything should work correctly.
```

例如：

```
# 去除目标文件中的符号
$strip objfile
$nm objfile
nm: objfile: no symbols

# 拷贝出一个符号表文件
$objcopy --only-keep-debug mainO3 mainO3.symbol       

# 拷贝出一个不包含调试信息的执行文件
$objcopy --strip-debug mainO3 mainO3.bin


$objcopy --add-gnu-debuglink=mainO3.symbol mainO3
```

[linux中的strip命令简介------给文件脱衣服](https://blog.csdn.net/stpeace/article/details/47090255)






