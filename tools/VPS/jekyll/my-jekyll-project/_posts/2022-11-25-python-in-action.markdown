---
layout: post
title:  "Python in Action"
date:   2022-11-25 12:00:00 +0800
categories: Python
---

* Do not remove this line (it will not be displayed)
{:toc}


# 环境配置

## 源码安装 Python3.8

``` bash
wget https://www.python.org/ftp/python/3.8.12/Python-3.8.12.tgz
tar xvf Python-3.8.12.tgz
cd Python-3.8.12
./configure
make -j 16
make install
```

```
$ python --version
Python 3.8.12
$ which python
/usr/bin/python
```

设置环境变量：

```
export Python_INCLUDE_DIRS="/usr/local/include/python3.8"
export PATH=/usr/local/bin:$PATH
```

安装 pip：

```
/usr/local/bin/python3.8 -m pip install --upgrade pip
```

安装 module 依赖：例如，xxhash

```
/usr/local/bin/pip install xxhash

# or
python3 -m pip install xxhash
```


# Tips

## 指定使用的 Python 版本 (shebang)

在 Python 脚本中指定使用的 Python 版本，可以通过在脚本文件的开头添加 shebang 行来实现。shebang 行是一个以 `#!` 开头的特殊注释行，用于指定脚本文件的解释器。

例如，如果我们想要在 Python 3 中运行脚本，可以在脚本文件的开头添加以下 shebang 行：

``` python
#!/usr/bin/env python3
```

这行代码告诉操作系统，使用 `/usr/bin/env` 命令来查找 Python 3 的解释器，并将该解释器用于执行脚本文件。如果系统中安装了多个 Python 版本，这种方式可以自动选择最新的 Python 3 版本。

如果我们想要在 Python 2 中运行脚本，可以使用以下 shebang 行：

``` python
#!/usr/bin/env python2
```

需要注意的是，不同的操作系统可能支持的 shebang 行格式不同。在 Linux 和 macOS 等类 Unix 系统中，shebang 行的格式为 `#!interpreter [optional-arg]`，其中 `interpreter` 是解释器的路径，`optional-arg` 是传递给解释器的可选参数。在 Windows 系统中，shebang 行的格式为 `#!python`，其中 `python` 是解释器的名称。因此，在编写跨平台的 Python 脚本时，需要注意 shebang 行的格式。



# 错误定位

参考：[How to catch and print the full exception traceback without halting/exiting the program?](https://stackoverflow.com/questions/3702675/how-to-catch-and-print-the-full-exception-traceback-without-halting-exiting-the)

`traceback.format_exc()` or `sys.exc_info()` will yield more info if that's what you want.


``` python
import traceback
import sys

try:
    do_stuff()
except Exception:
    print(traceback.format_exc())
    # or
    print(sys.exc_info()[2])
```

# Q&A

## [Use isinstance to test for Unicode string](https://stackoverflow.com/questions/24514891/use-isinstance-to-test-for-unicode-string)

Test for `str`:

``` python
isinstance(unicode_or_bytestring, str)
```

or, if you must handle bytestrings, test for `bytes` separately:

``` python
isinstance(unicode_or_bytestring, bytes)
```

## [Error: 'dict' object has no attribute 'iteritems'](https://stackoverflow.com/questions/30418481/error-dict-object-has-no-attribute-iteritems)

Take a look at Python 3.0 Wiki [Built-in Changes](https://wiki.python.org/moin/Python3.0#Built-In_Changes) section, where it is stated:

> Removed dict.iteritems(), dict.iterkeys(), and dict.itervalues().
>
> Instead: use dict.items(), dict.keys(), and dict.values() respectively.



# Refer

* [how to succesfully compile python 3.x](https://stackoverflow.com/questions/58048079/how-to-succesfully-compile-python-3-x)














