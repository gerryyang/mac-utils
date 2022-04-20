---
layout: post
title:  "CPP Cpplint in Action"
date:   2022-03-31 12:22:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Cpplint

> cpplint or cpplint.py is an open source lint-like tool developed by Google, designed to ensure that C++ code conforms to [Google's coding style guides](https://google.github.io/styleguide/cppguide.html)

From: [Weinberger, Benjy. "Google C++ Style Guide, section cpplint"](https://google.github.io/styleguide/cppguide.html#cpplint)

Use cpplint.py to detect style errors.

cpplint.py is a tool that reads a source file and identifies many style errors. It is not perfect, and has both false positives and false negatives, but it is still a valuable tool.

Some projects have instructions on how to run cpplint.py from their project tools. If the project you are contributing to does not, you can download [cpplint.py](https://raw.githubusercontent.com/google/styleguide/gh-pages/cpplint/cpplint.py) separately.

Cpplint 是一个 Python 编写的基于 Google 代码规范的检测工具。它只是一个代码风格检测工具，其并不对代码逻辑、语法错误等进行检查。Cpplint 有一套推测项目的根目录的算法，但是如果错误的话会导致对头文件保护宏命名的检查出错，此时可以通过 --repository 或者 --root 指定项目的根目录。Cpplint 也支持项目/目录级别的 `CPPLINT.cfg` 配置文件，这样就不需要每次在命令行传递相同的参数了。



# Refer

* https://en.wikipedia.org/wiki/Cpplint
* https://google.github.io/styleguide/cppguide.html#cpplint
* https://git.woa.com/standards/cpp/tree/master/cpplint
* https://git.woa.com/codecheck-tools/cpplint_scan/blob/master/tool/cpplint.py






  

	
	