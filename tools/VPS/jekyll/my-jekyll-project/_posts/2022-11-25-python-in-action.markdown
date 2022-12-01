---
layout: post
title:  "Python in Action"
date:   2022-11-25 12:00:00 +0800
categories: Lua
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
```

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















