---
layout: post
title:  "Cppcheck in Action"
date:   2025-07-28 12:00:00 +0800
categories: Tools
---

* Do not remove this line (it will not be displayed)
{:toc}

[Cppcheck](https://cppcheck.sourceforge.io/) is a **static analysis tool** for `C/C++` code. It provides [unique code analysis](https://cppcheck.sourceforge.io/#unique) to detect bugs and focuses on detecting undefined behaviour and dangerous coding constructs. **The goal is to have very few false positives**. `Cppcheck` is designed to be able to analyze your `C/C++` code even if it has non-standard syntax (common in embedded projects).

> The original name of this program was "C++check", but it was later changed to "Cppcheck". Despite the name, Cppcheck is designed for both C and C++.


`Cppcheck` is available both as open-source (this page) and as `Cppcheck` Premium with extended functionality and support. Please visit [www.cppcheck.com](https://www.cppcheck.com/?utm_source=sourceforge&utm_medium=opensource&utm_campaign=websitelink) for more information and purchase options for the **commercial version**.




# Compiling

`Cppcheck` requires a C++ compiler with (partial) C++11 support. Minimum required versions are GCC 5.1 / Clang 3.5 / Visual Studio 2015.

When building the command line tool, [PCRE](http://www.pcre.org/) is optional. It is used if you build with rules.

The minimum required Python version is 3.6.

``` bash
# 安装 PCRE 开发库
# RHEL/CentOS
$ sudo yum install pcre-devel

# 确认 PCRE 开发库是否安装
# RHEL/CentOS
$ rpm -qa | grep pcre-devel
pcre-devel-8.42-6.tl3.x86_64

# 定位 pcre-config
$ which pcre-config || find /usr -name pcre-config 2>/dev/null
/usr/bin/pcre-config
```

> GNU make

Simple, unoptimized build (no dependencies):

``` bash
make
```

The recommended `release` build is:

``` bash
make -j36 MATCHCOMPILER=yes DESTDIR=$HOME/tools/cppcheck/cppcheck-2.18.0 FILESDIR=-install HAVE_RULES=yes CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" install
```

Flags:

* `MATCHCOMPILER=yes` Python is used to optimise cppcheck. The Token::Match patterns are converted into C++ code at compile time.
* `DESTDIR=xxx FILESDIR=xxx` Specify folder where cppcheck files are installed (`addons`, `cfg`, `platform`)
* `HAVE_RULES=yes` Enable rules (`PCRE` is required if this is used)
* `CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function"` Enables most compiler optimizations, disables cppcheck-internal debugging code and enables basic compiler warnings.


```
$ ldd cppcheck
        linux-vdso.so.1 (0x00007ffffdfb9000)
        /$LIB/libonion_block.so => /lib64/libonion_block.so (0x00007fcf022ad000)
        /$LIB/libonion.so => /lib64/libonion.so (0x00007fcf022a5000)
        libpcre.so.1 => /lib64/libpcre.so.1 (0x00007fcf02229000)
        libstdc++.so.6 => /lib64/libstdc++.so.6 (0x00007fcf02091000)
        libm.so.6 => /lib64/libm.so.6 (0x00007fcf01f0d000)
        libgcc_s.so.1 => /lib64/libgcc_s.so.1 (0x00007fcf01ef2000)
        libpthread.so.0 => /lib64/libpthread.so.0 (0x00007fcf01ecf000)
        libc.so.6 => /lib64/libc.so.6 (0x00007fcf01d07000)
        libdl.so.2 => /lib64/libdl.so.2 (0x00007fcf01d00000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fcf024b3000)
```

# About static analysis

The kinds of bugs that you can find with static analysis are:

* Undefined behavior
* Using dangerous code patterns
* Coding style

**There are many bugs that you can not find with static analysis**. Static analysis tools do not have human knowledge about what your program is intended to do. If the output from your program is valid but unexpected then in most cases this is not detected by static analysis tools. For instance, if your small program writes “Helo” on the screen instead of “Hello” it is unlikely that any tool will complain about that.

**Static analysis should be used as a complement in your quality assurance**. It does not replace any of:

*  Careful design
*  Testing
*  Dynamic analysis
*  Fuzzing





# Manual

You can read the [manual](https://cppcheck.sourceforge.io/manual.pdf) or download some [articles](http://sourceforge.net/projects/cppcheck/files/Articles/).


# Features

1. Unique code analysis that detect various kinds of bugs in your code.
2. Both command line interface and graphical user interface are available.
3. Cppcheck has a strong focus on detecting undefined behaviour.

## Unique analysis

Using several static analysis tools can be a good idea. There are unique features in each tool. This has been established in many studies.

So what is unique in `Cppcheck`.

**`Cppcheck` uses unsound flow sensitive analysis**. Several other analyzers use path sensitive analysis based on abstract interpretation, that is also great however that has both advantages and disadvantages. In theory by definition, it is better with path sensitive analysis than flow sensitive analysis. **But in practice, it means Cppcheck will detect bugs that the other tools do not detect**.

In `Cppcheck` the data flow analysis is not only "forward" but "bi-directional". Most analyzers will diagnose this:

``` c
void foo(int x)
{
    int buf[10];
    if (x == 1000)
        buf[x] = 0; // <- ERROR
}
```

Most tools can determine that the array index will be `1000` and there will be overflow.

`Cppcheck` will also diagnose this:

``` c
void foo(int x)
{
    int buf[10];
    buf[x] = 0; // <- ERROR
    if (x == 1000) {}
}
```

## Undefined behaviour

* Dead pointers
* Division by zero
* Integer overflows
* Invalid bit shift operands
* Invalid conversions
* Invalid usage of STL
* Memory management
* Null pointer dereferences
* Out of bounds checking
* Uninitialized variables
* Writing const data

## Security

`CVEs` that was found using `Cppcheck`:

[CVE-2017-1000249](https://nvd.nist.gov/vuln/detail/CVE-2017-1000249): file : stack based buffer overflow. This was found by Thomas Jarosch using `Cppcheck`. The cause is a mistake in a condition.

These `CVEs` are shown when you google "cppcheck CVE". Feel free to compare the search results with other static analysis tools.

Security experts recommend that static analysis is used. And using several tools is the best approach from a security perspective.


## Coding standards

| Coding standard | Open Source  | Premium
| -- | -- | --
| Misra C 2012 - original rules | Partial | Yes
| Misra C 2012 - amendment #1 | Partial | Yes
| Misra C 2012 - amendment #2 | Partial | Yes
| Misra C 2012 - amendment #3 | | Yes
| Misra C 2012 - amendment #4 | | Yes
| Misra C 2012 - Compliance report | | Yes
| Misra C 2012 - Rule texts | User provided | Yes
| Misra C 2023  | | | Yes
| Misra C++ 2008 | | | Yes
| Misra C++ 2023 | | | Yes
| Cert C | | | Yes
| Cert C++ | | | Yes
| Autosar | | | [Partial](https://files.cppchecksolutions.com/autosar.html)


# All checks

For a list of all checks in Cppcheck see: http://sourceforge.net/p/cppcheck/wiki/ListOfChecks.



# Download

[Cppcheck 2.18 (open source)](https://github.com/danmar/cppcheck/archive/2.18.0.zip)


# Clients and plugins

`Cppcheck` is integrated with many popular development tools. For instance:


* CLion - [Cppcheck plugin](https://plugins.jetbrains.com/plugin/8143)
* Code::Blocks - integrated
* Eclipse - [Cppcheclipse](https://github.com/cppchecksolutions/cppcheclipse/wiki/Installation)
* Tortoise SVN - [Adding a pre-commit hook script](http://omerez.com/automatic-static-code-analysis/)
* Vim - [Vim Compiler](https://vimhelp.org/quickfix.txt.html#compiler-cppcheck)
* Visual Studio - [Visual Studio plugin](https://github.com/VioletGiraffe/cppcheck-vs-addin/releases/latest)
* VScode - [VScode plugin](https://marketplace.visualstudio.com/items?itemName=NathanJ.cppcheck-plugin)

![cppcheck_vscode_plugin](/assets/images/202507/cppcheck_vscode_plugin.png)

> 注意：Cppcheck must be installed to your system! You'll need to add cppcheck to the "PATH" environment variable in Windows

# Other static analysis tools

Using a battery of tools is better than using one tool. Each tool has unique code analysis and therefore we recommend that you also use other tools.

`Cppcheck` focus on bugs instead of stylistic issues. Therefore a tool that focus on stylistic issues could be a good addition.

`Cppcheck` tries very hard to avoid false positives. Sometimes people want to detect all bugs even if there will be many false warnings, for instance when they are working on a release and want to verify that there are no bugs. A tool that is much more noisy than `Cppcheck` might be a good addition.

Even tools that have the same design goals as `Cppcheck` will probably be good additions. **Static analysis is such a big field, `Cppcheck` only covers a small fraction of it. No tool covers the whole field**. The day when all manual testing will be **obsolete(被淘汰的，过时的)** because of some tool is very far away.


# [News](https://sourceforge.net/p/cppcheck/news/)

[Cppcheck-2.18.0](https://sourceforge.net/p/cppcheck/news/2025/07/cppcheck-2180/)


# Cppcheck Manual


## First test (check single file)

Here is some simple code:

``` c
#include <cstdio>

int main()
{
    char a[10];
    a[10] = 0;
    return 0;
}
```

If you save that into `file1.c` and execute: `cppcheck file1.c`

The output from `Cppcheck` will then be:

```
Checking file1.c...
[file1.c:4]: (error) Array'a[10]' index 10 out of bounds
```

## Checking all files in a folder

Normally a program has many source files. `Cppcheck` can check all source files in a directory: `cppcheck path`

If “path” is a folder, then `Cppcheck` will recursively check all source files in this folder:

```
Checking path/file1.cpp...
1/2 files checked 50% done
Checking path/file2.cpp...
2/2 files checked 100% done
```

## Check files manually or use project file

With `Cppcheck` you can check files manually by specifying files/paths to check and settings. Or you can use a build environment, such as `CMake` or `Visual Studio`.

We don’t know which approach (project file or manual configuration) will give you the best results. It is recommended that you try both. It is possible that you will get different results so that to find the largest amount of bugs you need to use both approaches. Later chapters will describe this in more detail.


## Check files matching a given file filter

With `--file-filter=<str>` you can set a file filter and only those files matching the filter will be checked.

For example: if you want to check only those files and folders starting from a subfolder `src/` that start with “test” you have to type:

``` bash
cppcheck src/ --file-filter=src/test*
```

`Cppcheck` first collects all files in `src/` and will apply the filter after that. So the filter must start with the given start folder.


## Excluding a file or folder from checking

To **exclude a file or folder**, there are two options. The first option is to only provide the paths and files you want to check:

``` bash
cppcheck src/a src/b
```

All files under `src/a` and `src/b` are then checked.

The second option is to use `-i`, which specifies the files/paths to ignore. With this command no files in `src/c` are checked:

``` bash
cppcheck -isrc/c src
```

This option is only valid when supplying an input directory. To ignore multiple directories supply the `-i` flag for each directory individually. The following command ignores both the `src/b` and `src/c` directories:

``` bash
cppcheck -isrc/b -isrc/c
```


## Clang parser (experimental)

By default `Cppcheck` uses an internal C/C++ parser. However there is an experimental option to use the **Clang parser** instead.

Install `clang`. Then use Cppcheck option `--clang`.

Technically, `Cppcheck` will execute clang with its `-ast-dump` option. The `Clang` output is then imported and converted into the normal `Cppcheck` format. And then normal `Cppcheck` analysis is performed on that.

You can also pass a custom `Clang` executable to the option by using for example `--clang=clang-10`. You can also pass it with a path. On Windows it will append the `.exe` extension unless you use a path.


## Severities

The possible severities for messages are:

* **error**

when code is executed there is either undefined behavior or other error, such as a memory leak or resource leak

* **warning**

when code is executed there might be undefined behavior

* **style**

stylistic issues, such as unused functions, redundant code, constness, operator precedence, possible mistakes.

* **performance**

run time performance suggestions based on common knowledge, though it is not certain any measurable speed difference will be achieved by fixing these messages.

* **portability**

portability warnings. Implementation defined behavior. 64-bit portability. Some undefined behavior that probably works “as you want”, etc.

* **information**

configuration problems, which does not relate to the syntactical correctness, but the used Cppcheck configuration could be improved.


## Possible speedup analysis of template code

`Cppcheck` **instantiates the templates** in your code.

If your templates are **recursive** this can lead to slow analysis that uses a lot of memory. `Cppcheck` will write information messages when there are potential problems.

Example code:

``` cpp
template <int i>
void a()
{
    a<i+1>();
}

void foo()
{
    a<0>();
}
```

`Cppcheck` output:

```
test.cpp:4:5: information: TemplateSimplifier: max template recursion (100) reached for template'a<101>'. You might want to limit Cppcheck recursion. [templateRecursion]
    a<i+1>();
    ^
```

As you can see `Cppcheck` has instantiated `a<i+1>` until `a<101>` was reached and then it bails out.

To limit template recursion you can:

* add template specialisation
* configure `Cppcheck`, which can be done in the GUI project file dialog

Example code with template specialisation:

``` cpp
template <int i>
void a()
{
    a<i+1>();
}

void foo()
{
    a<0>();
}

#ifdef __cppcheck__
template<> void a<3>() {}
#endif
```

You can pass `-D__cppcheck__` when checking this code.


## Cppcheck build folder

Using a `Cppcheck` build folder is **not mandatory but it is recommended**. `Cppcheck` save analyzer information in that folder.

**The advantages are:**

* It speeds up the analysis as it makes incremental analysis possible. Only changed files are analyzed when you recheck.
* Whole program analysis also when multiple threads are used.

On the command line you configure that through `--cppcheck-build-dir=path`.

> Rechecking code will be much faster. Cppcheck does not analyse unchanged code. The old warnings are loaded from the build dir and reported again.

> Whole program analysis does not work when multiple threads are used; unless you use a cppcheck build dir. For instance, the `unusedFunction` warnings require whole program analysis.


Example:

``` bash
mkdir b

# All files are analyzed
cppcheck --cppcheck-build-dir=b src

# Faster! Results of unchanged files are reused
cppcheck --cppcheck-build-dir=b src
```


## Importing a project

You can import some project files and build configurations into `Cppcheck`.

> CMake

Generate a compile database:

``` bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .
```

The file `compile_commands.json` is created in the current folder. Now run `Cppcheck` like this:

``` bash
cppcheck --project=compile_commands.json
```

To ignore certain folders you can use `-i`. This will skip analysis of source files in the `foo` folder.

``` bash
cppcheck --project=compile_commands.json -ifoo
```

> Other

If you can generate a compile database, then it is possible to import that in `Cppcheck`.

In **Linux** you can use for instance the `bear` (**build ear**) utility to generate a compile database from arbitrary build tools:

``` bash
bear -- make
```

## Preprocessor Settings

If you use `--project` then `Cppcheck` will automatically use the preprocessor settings in the imported project file and **likely you don’t have to configure anything extra**.

If you don’t use `--project` then a bit of manual preprocessor configuration might be required. However `Cppcheck` has automatic configuration of defines.


### Automatic configuration of preprocessor defines

Cppcheck automatically test different combinations of preprocessor defines to achieve as high coverage in the analysis as possible.

Here is a file that has 3 bugs (when x,y,z are assigned).

``` cpp
#ifdef A
  x=100/0;
  #ifdef B
    y=100/0;
  #endif
#else
  z=100/0;
#endif

#ifndef C
#error C must be defined
#endif
```

The flag `-D` tells `Cppcheck` that **a name is defined**. There will be no `Cppcheck` analysis without this define. The flag `-U` tells `Cppcheck` that **a name is not defined**. There will be no `Cppcheck` analysis with this define.

The flag `--force` and `--max-configs` is used to control how many combinations are checked.

When `-D` is used, `Cppcheck` will only check 1 configuration unless these are used.

Example:

``` bash
# test all configurations
# all bugs are found
cppcheck test.c

# only test configuration "-DA"
# No bug is found (#error)
cppcheck -DA test.c

# only test configuration "-DA -DC"
# The first bug is found
cppcheck -DA -DC test.c

# The configuration "-DC" is tested
# The last bug is found
cppcheck -UA test.c

# All configurations with "-DA" are tested
# The two first bugs are found
cppcheck --force -DA test.c
```

### Include paths

To add an include path, use `-I`, followed by the path.

Cppcheck’s preprocessor basically handles includes like any other preprocessor. However, while other preprocessors stop working when they encounter a missing header, `Cppcheck` will just print an information message and continues parsing the code.

**The purpose of this behaviour is that `Cppcheck` is meant to work without necessarily seeing the entire code**. Actually, it is recommended to not give all include paths. While it is useful for `Cppcheck` to see the declaration of a class when checking the implementation of its members, passing standard library headers is discouraged, because the analysis will not work fully and lead to a longer checking time. For such cases, `.cfg` files are the preferred way to provide information about the implementation of functions and types to `Cppcheck`, see below for more information.


## Platform

You should use a platform configuration that matches your target environment.

By default `Cppcheck` uses native platform configuration that works well if your code is compiled and executed locally.

`Cppcheck` has builtin configurations for `Unix` and `Windows` targets. You can easily use these with the `--platform` command line flag.

You can also create your own custom platform configuration in a `XML` file. Here is an example:

``` xml
<?xml version="1"?>
<platform>
  <char_bit>8</char_bit>
  <default-sign>signed</default-sign>
  <sizeof>
    <short>2</short>
    <int>4</int>
    <long>4</long>
    <long-long>8</long-long>
    <float>4</float>
    <double>8</double>
    <long-double>12</long-double>
    <pointer>4</pointer>
    <size_t>4</size_t>
    <wchar_t>2</wchar_t>
  </sizeof>
</platform>
```


## C/C++ Standard

Use `--std` on the command line to specify a C/C++ standard.

`Cppcheck` assumes that the code is compatible with the latest C/C++ standard, but it is possible to override this.

The available options are:

* c89: C code is C89 compatible
* c99: C code is C99 compatible
* c11: C code is C11 compatible
* c17: C code is C17 compatible
* c23: C code is C23 compatible (default)
* c++03: C++ code is C++03 compatible
* c++11: C++ code is C++11 compatible
* c++14: C++ code is C++14 compatible
* c++17: C++ code is C++17 compatible
* c++20: C++ code is C++20 compatible
* c++23: C++ code is C++23 compatible
* c++26: C++ code is C++26 compatible (default)


## Suppressions

If you want to **filter** out certain errors from being generated, then it is possible to **suppress** these.

If you encounter a **false positive**, then please report it to the `Cppcheck` team so that it can be fixed.

### Plain text suppressions

The format for an error **suppression** is one of:

```
[error id]:[filename]:[line]
[error id]:[filename2]
[error id]
```

The `error id` is the id that you want to **suppress**. The id of a warning is shown in brackets in the normal `cppcheck` text output. The suppression `error id` may contain `*` to match any sequence of tokens.

The filename may include the wildcard characters `*` or `?`, which matches any sequence of characters or any single character respectively. It is recommended to use forward-slash `/` as path separator on all operating systems. The filename must match the filename in the reported warning exactly. For instance, if the warning contains a relative path, then the suppression must match that relative path.

### Command line suppression

The `--suppress=` command line option is used to specify suppressions on the command line. Example:

``` bash
cppcheck --suppress=memleak:src/file1.cpp src/
```

### Suppressions in a file

You can create a suppressions file for example as follows:

```
// suppress memleak and exceptNew errors in the file src/file1.cpp
memleak:src/file1.cpp
exceptNew:src/file1.cpp

uninitvar // suppress all uninitvar errors in all files
```

Note that **you may add empty lines and comments** in the **suppressions file**. Comments must start with `#` or `//` and be at the start of the line, or after the suppression line.

The usage of the suppressions file is as follows:

``` bash
cppcheck --suppressions-list=suppressions.txt src/
```


### XML suppressions

You can specify suppressions in a `XML` file, for example as follows:


``` xml
<?xml version="1.0"?>
<suppressions>
  <suppress>
    <id>uninitvar</id>
    <fileName>src/file1.c</fileName>
    <lineNumber>10</lineNumber>
    <symbolName>var</symbolName>
  </suppress>
</suppressions>
```

The `XML` format is extensible and may be extended with further attributes in the future.

The usage of the suppressions file is as follows:

``` bash
cppcheck --suppress-xml=suppressions.xml src/
```

### Inline suppressions

**Suppressions can also be added directly in the code by adding comments that contain special keywords**. Note that adding comments sacrifices the readability of the code somewhat.

This code will normally generate an error message:

``` cpp
void f() {
    char arr[5];
    arr[10] = 0;
}
```

The output is:

```
cppcheck test.c
[test.c:3]: (error) Array'arr[5]' index 10 out of bounds
```

To activate inline suppressions:

``` bash
cppcheck --inline-suppr test.c
```

> Format

You can suppress a warning `aaaa` with:

``` cpp
// cppcheck-suppress aaaa
```

Suppressing multiple ids in one comment by using `[]`:

``` cpp
// cppcheck-suppress [aaaa, bbbb]
```

Suppressing warnings aaaa on a block of code:

``` cpp
// cppcheck-suppress-begin aaaa

// ...

// cppcheck-suppress-end aaaa
```

Suppressing multiple ids on a block of code:

``` cpp
// cppcheck-suppress-begin [aaaa, bbbb]

// ...

// cppcheck-suppress-end [aaaa, bbbb]
```

Suppressing warnings aaaa for a whole file:

``` cpp
// cppcheck-suppress-file aaaa
```

Suppressing multiple ids for a whole file:

``` cpp
// cppcheck-suppress-file [aaaa, bbbb]
```

Suppressing warnings aaaa where macro is used:

``` cpp
// cppcheck-suppress-macro aaaa
#define MACRO ...

// ...

x = MACRO; // <- aaaa warnings are suppressed here
```

Suppressing multiple ids where macro is used:

``` cpp
// cppcheck-suppress-macro [aaaa, bbbb]
#define MACRO ...

// ...

x = MACRO; // <- aaaa and bbbb warnings are suppressed here
```

### Comment before code or on same line

The comment can be put before the code or at the same line as the code.

**Before the code**:

``` cpp
void f() {
  char arr[5];
  // cppcheck-suppress arrayIndexOutOfBounds
  arr[10] = 0;
}
```

Or **at the same line as the code**:

``` cpp
void f() {
  char arr[5];
  arr[10] = 0; // cppcheck-suppress arrayIndexOutOfBounds
}
```

In this example there are 2 lines with code and 1 suppression comment. The suppression comment only applies to 1 line: `a = b + c;`.

``` cpp
void f() {
  a = b + c; // cppcheck-suppress abc
  d = e + f;
}
```

### Multiple suppressions

For a line of code there might be several warnings you want to suppress.

There are several options;

Using 2 suppression comments before code:


``` cpp
void f() {
  char arr[5];

  // cppcheck-suppress arrayIndexOutOfBounds
  // cppcheck-suppress zerodiv
  arr[10] = arr[10] / 0;
}
```

Using 1 suppression comment before the code:

``` cpp
void f() {
  char arr[5];
  // cppcheck-suppress[arrayIndexOutOfBounds,zerodiv]
  arr[10] = arr[10] / 0;
}
```

### Comment about suppression

You can write comments about a suppression as follows:

``` cpp
// cppcheck-suppress[warningid] some comment
// cppcheck-suppress warningid ; some comment
// cppcheck-suppress warningid // some comment
```


## XML output

`Cppcheck` can generate output in `XML` format. Use `--xml` to enable this format.

A sample command to check a file and output errors in the `XML` format:

``` bash
cppcheck --xml file1.cpp
```

Here is a sample report:

``` xml
<?xml version="1.0" encoding="UTF-8"?>
<results version="2">
    <cppcheck version="1.66"/>
    <errors>
        <error id="someError" severity="error" msg="short error text" verbose="long error text" inconclusive="true" cwe="312">
            <location file0="file.c" file="file.h" line="1"/>
        </error>
    </errors>
</results>
```

Each error is reported in a `<error>` element. Attributes:

* **id**

id of error, and which are valid symbolnames

* **severity**

error/warning/style/performance/portability/information

* **msg**

the error message in short format

* **verbose**

the error message in long format

* **inconclusive** (无定论的)

this attribute is only used when the error message is inconclusive

* **cwe**

CWE ID for the problem; note that this attribute is only used when the CWE ID for the message is known

* **remark**

Optional attribute. The related remark/justification from a remark comment.


All locations related to an error are listed with `<location>` elements. The primary location is listed first.

Attributes:

* **file**

filename, both relative and absolute paths are possible

* **file0**

name of the source file (optional)

* **line**

line number

* **info**

short information for each location (optional)


## Reformatting the text output

If you want to **reformat** the output so that it looks different, then you can use **templates**.

### Predefined output formats

To get `gcc` compatible output you can use `–template=gcc`:

``` bash
cppcheck --template=gcc samples/arrayIndexOutOfBounds/bad.c
```

### User defined output format (single line)

You can write your own pattern. For instance:

``` bash
cppcheck --template="{file}:{line}:{column}: {severity}:{message}" samples/arrayIndexOutOfBounds/bad.c
```

The output will then look like this:

```
Checking samples/arrayIndexOutOfBounds/bad.c ...
samples/arrayIndexOutOfBounds/bad.c:6:6: error: Array 'a[2]' accessed at index 2, which is out of bounds.
```

A comma separated format:

``` bash
cppcheck --template="{file},{line},{severity},{id},{message}" samples/arrayIndexOutOfBounds/bad.c
```

The output will look like this:

```
Checking samples/arrayIndexOutOfBounds/bad.c ...
samples/arrayIndexOutOfBounds/bad.c,6,error,arrayIndexOutOfBounds, Array'a[2]' accessed at index 2, which is out of bounds.
```

### User defined output format (multi line)

Many warnings have multiple locations. Example code:

``` cpp
void f(int *p)
{
    *p = 3; // line 3
}

int main()
{
    int *p = 0; // line 8
    f(p); // line 9
    return 0;
}
```

There is a possible null pointer dereference at line 3. Cppcheck can show how it came to that conclusion by showing extra location information. You need to use both `–template` and `–template-location` at the command line, for example:

``` bash
cppcheck --template="{file}:{line}: {severity}: {message}\n{code}" --template-location="{file}:{line}: note: {info}\n{code}" multiline.c
```

The output from `Cppcheck` is:

1. The first line in the warning is formatted by the `–template` format.
2. The other lines in the warning are formatted by the `–template-location` format.

![cppcheck2](/assets/images/202507/cppcheck2.png)


### Format specifiers for –template

The available specifiers for `–template` are:

* `{file}`

File name

* `{line}`

Line number

* `{column}`

Column number

* `{callstack}`

Write all locations. Each location is written in `[{file}:{line}]` format and the locations are separated by `->`. For instance it might look like: `[multiline.c:8] -> [multiline.c:9] -> [multiline.c:3]`

* `{inconclusive:text}`

If warning is **inconclusive**, then the given text is written. The given text can be any text that does not contain `}`. Example: `{inconclusive:inconclusive,}`

* `{severity}`

error/warning/style/performance/portability/information

* `{message}`

The warning message

* `{id}`

Warning id

* `{remark}`

The remark text if a remark comment has been provided

* `{code}`

The real code

* `\t`

Tab

* `\n`

Newline

* `\r`

Carriage return

### Format specifiers for –template-location

The available specifiers for `--template-location` are:

* `{file}`

File name

* `{line}`

Line number

* `{column}`

Column number

* `{info}`

Information message about the current location

* `{code}`

The real code

* `\t`

Tab

* `\n`

Newline

* `\r`

Carriage return

### Justifications for warnings in the report

You can add **remark comments** in the source code that justify why there is a warning/violation.

Such a remark comment shall:

* start with `REMARK`.
* can either be added above the source code that generates the warning, or after the code on the same line.

Example code:

``` c
void foo() {
    // REMARK Initialize x with 0
    int x = 0;
}
```

In `Cppcheck` text output the remarks are not shown by default, you can use `--template` option `{remark}` to show remarks:

``` bash
cppcheck --enable=style --template="{file}:{line}: {message} [{id}]\\n{remark}" test1.c
```

![cppcheck3](/assets/images/202507/cppcheck3.png)


## Addons

**Addons are scripts** that analyse `Cppcheck` dump files to check compatibility with secure coding standards and to locate issues.

`Cppcheck` is distributed with a few **addons** which are listed below.

* `misra.py`

`misra.py` is used to verify compliance with MISRA C 2012, a proprietary set of guidelines to avoid questionable code, developed for embedded systems.


* `y2038.py`

`y2038.py` checks Linux systems for year 2038 problem safety.

* `threadsafety.py`

`threadsafety.py` analyses Cppcheck dump files to locate thread safety issues like **static local objects** used by **multiple threads**.

> Running Addons

**Addons** could be run through `Cppcheck` command line utility as follows:

``` bash
cppcheck --addon=misra.py somefile.c
```

This will launch all `Cppcheck` checks and additionally calls specific checks provided by selected addon.

Some **addons** need extra arguments. You can configure how you want to execute an addon in a json file. For example put this in `misra.json`:

``` json
{
  "script": "misra.py",
  "args": [
      "--rule-texts=misra.txt"
  ]
}
```

And then the configuration can be executed on the `Cppcheck` command line:

``` bash
cppcheck --addon=misra.json somefile.c
```

**By default** `Cppcheck` would search **addon** at the standard path which was specified during the installation process. You also can set this path directly, for example:

``` bash
cppcheck --addon=/opt/cppcheck/configurations/my_misra.json somefile.c
```

This allows you to create and manage multiple configuration files for different projects.



## Library configuration

When external libraries are used, such as WinAPI, POSIX, gtk, Qt, etc, `Cppcheck` has no information about functions, types, or macros contained in those libraries. `Cppcheck` then fails to detect various problems in the code, or might even abort the analysis. **But this can be fixed by using the appropriate configuration files**.

**`Cppcheck` already contains configurations for several libraries**. They can be loaded as described below. Note that the configuration for the standard libraries of C and C++, `std.cfg`, is always loaded by `cppcheck`. If you create or update a configuration file for a popular library, we would appreciate if you supplied it to the cppcheck project.

### Using a `.cfg` file

To use a `.cfg` file shipped with cppcheck, pass the `--library=<lib>` option. The table below shows the currently existing libraries:

### Creating a custom `.cfg` file

You can create and use your own `.cfg` files for your projects. Use `--check-library` to get hints about what you should configure.

The `.cfg` file format is documented in the Reference: Cppcheck `.cfg` format (https://cppcheck.sourceforge.io/reference-cfg-format.pdf) document.


## HTML Report

You can convert the `XML` output from Cppcheck into a `HTML` report.

You’ll need `Python` and the **pygments module** (http://pygments.org/) for this to work.

In the `Cppcheck` source tree there is a folder `htmlreport` that contains a script that transforms a Cppcheck XML file into HTML output.

This command generates the help screen:

``` bash
htmlreport/cppcheck-htmlreport -h
```

The output screen says:

![cppcheck4](/assets/images/202507/cppcheck4.png)

Example usage:

``` bash
cppcheck gui/test.cpp --xml 2> err.xml
cppcheck-htmlreport --file=err.xml --report-dir=test1 --source-dir=.
```

## Check Level

### Reduced

The “reduced” check level performs a limited data flow analysis. If developers want to run cppcheck directly during development and require faster results than “normal” provides then this reduced checking can be an option.


### Normal

**The “normal” check level is chosen by default**. Our aim is that this checking level will provide an effective checking in “reasonable” time.

The “normal” check level should be useful during active development:

* checking files while you edit them.
* block changes to the repo
* etc



### Exhaustive (详尽无遗的)

When you can wait longer for the results you can enable the “exhaustive” checking, by using the option `--check-level=exhaustive`.

`Exhaustive` checking level should be useful for scenarios where you can wait for results. For instance:

* nightly builds
* etc


## Speeding up analysis

> Limit preprocessor configurations

For performance reasons it might be a good idea to limit preprocessor configurations to check.

> Limit ValueFlow: max if count

The command line option `--performance-valueflow-max-if-count` adjusts the max count for number of if in a function.






# cppcheck --doc

```
$ cppcheck --doc
## 64-bit portability ##
Check if there is 64-bit portability issues:
- assign address to/from int/long
- casting address from/to integer when returning from function

## Assert ##
Warn if there are side effects in assert statements (since this cause different behaviour in debug/release builds).

## Auto Variables ##
A pointer to a variable is only valid as long as the variable is in scope.
Check:
- returning a pointer to auto or temporary variable
- assigning address of an variable to an effective parameter of a function
- returning reference to local/temporary variable
- returning address of function parameter
- suspicious assignment of pointer argument
- useless assignment of function argument

## Boolean ##
Boolean type checks
- using increment on boolean
- comparison of a boolean expression with an integer other than 0 or 1
- comparison of a function returning boolean value using relational operator
- comparison of a boolean value with boolean value using relational operator
- using bool in bitwise expression
- pointer addition in condition (either dereference is forgot or pointer overflow is required to make the condition false)
- Assigning bool value to pointer or float
- Returning an integer other than 0 or 1 from a function with boolean return value

## Bounds checking ##
Out of bounds checking:
- Array index out of bounds
- Pointer arithmetic overflow
- Buffer overflow
- Dangerous usage of strncat()
- Using array index before checking it
- Partial string write that leads to buffer that is not zero terminated.
- Check for large enough arrays being passed to functions
- Allocating memory with a negative size

## Check function usage ##
Check function usage:
- missing 'return' in non-void function
- return value of certain functions not used
- invalid input values for functions
- Warn if a function is called whose usage is discouraged
- memset() third argument is zero
- memset() with a value out of range as the 2nd parameter
- memset() with a float as the 2nd parameter
- copy elision optimization for returning value affected by std::move
- use memcpy()/memset() instead of for loop

## Class ##
Check the code for each class.
- Missing constructors and copy constructors
- Constructors which should be explicit
- Are all variables initialized by the constructors?
- Are all variables assigned by 'operator='?
- Warn if memset, memcpy etc are used on a class
- Warn if memory for classes is allocated with malloc()
- If it's a base class, check that the destructor is virtual
- Are there unused private functions?
- 'operator=' should check for assignment to self
- Constness for member functions
- Order of initializations
- Suggest usage of initialization list
- Initialization of a member with itself
- Suspicious subtraction from 'this'
- Call of pure virtual function in constructor/destructor
- Duplicated inherited data members
- Check that arbitrary usage of public interface does not result in division by zero
- Delete "self pointer" and then access 'this'
- Check that the 'override' keyword is used when overriding virtual functions
- Check that the 'one definition rule' is not violated

## Condition ##
Match conditions with assignments and other conditions:
- Mismatching assignment and comparison => comparison is always true/false
- Mismatching lhs and rhs in comparison => comparison is always true/false
- Detect usage of | where & should be used
- Duplicate condition and assignment
- Detect matching 'if' and 'else if' conditions
- Mismatching bitand (a &= 0xf0; a &= 1; => a = 0)
- Opposite inner condition is always false
- Identical condition after early exit is always false
- Condition that is always true/false
- Mutual exclusion over || always evaluating to true
- Comparisons of modulo results that are always true/false.
- Known variable values => condition is always true/false
- Invalid test for overflow. Some mainstream compilers remove such overflow tests when optimising code.
- Suspicious assignment of container/iterator in condition => condition is always true.

## Exception Safety ##
Checking exception safety
- Throwing exceptions in destructors
- Throwing exception during invalid state
- Throwing a copy of a caught exception instead of rethrowing the original exception
- Exception caught by value instead of by reference
- Throwing exception in noexcept, nothrow(), __attribute__((nothrow)) or __declspec(nothrow) function
- Unhandled exception specification when calling function foo()
- Rethrow without currently handled exception

## IO using format string ##
Check format string input/output operations.
- Bad usage of the function 'sprintf' (overlapping data)
- Missing or wrong width specifiers in 'scanf' format string
- Use a file that has been closed
- File input/output without positioning results in undefined behaviour
- Read to a file that has only been opened for writing (or vice versa)
- Repositioning operation on a file opened in append mode
- The same file can't be open for read and write at the same time on different streams
- Using fflush() on an input stream
- Invalid usage of output stream. For example: 'std::cout << std::cout;'
- Wrong number of arguments given to 'printf' or 'scanf;'

## Leaks (auto variables) ##
Detect when a auto variable is allocated but not deallocated or deallocated twice.

## Memory leaks (address not taken) ##
Not taking the address to allocated memory

## Memory leaks (class variables) ##
If the constructor allocate memory then the destructor must deallocate it.

## Memory leaks (function variables) ##
Is there any allocated memory when a function goes out of scope

## Memory leaks (struct members) ##
Don't forget to deallocate struct members

## Null pointer ##
Null pointers
- null pointer dereferencing
- undefined null pointer arithmetic

## Other ##
Other checks
- division with zero
- scoped object destroyed immediately after construction
- assignment in an assert statement
- free() or delete of an invalid memory location
- bitwise operation with negative right operand
- cast the return values of getc(),fgetc() and getchar() to character and compare it to EOF
- race condition with non-interlocked access after InterlockedDecrement() call
- expression 'x = x++;' depends on order of evaluation of side effects
- overlapping write of union
- either division by zero or useless condition
- access of moved or forwarded variable.
- potentially dangerous C style type cast of pointer/reference to object.
- redundant data copying for const variable
- subsequent assignment or copying to a variable or buffer
- passing parameter by value
- Passing NULL pointer to function with variable number of arguments leads to UB.
- Casting non-zero integer literal in decimal or octal format to pointer.
- C-style pointer cast in C++ code
- casting between incompatible pointer types
- [Incomplete statement](IncompleteStatement)
- [check how signed char variables are used](CharVar)
- variable scope can be limited
- unusual pointer arithmetic. For example: "abc" + 'd'
- redundant assignment, increment, or bitwise operation in a switch statement
- redundant strcpy in a switch statement
- Suspicious case labels in switch()
- assignment of a variable to itself
- Comparison of values leading always to true or false
- Clarify calculation with parentheses
- suspicious comparison of '\0' with a char\* variable
- duplicate break statement
- unreachable code
- testing if unsigned variable is negative/positive
- Suspicious use of ; at the end of 'if/for/while' statement.
- Array filled incompletely using memset/memcpy/memmove.
- NaN (not a number) value used in arithmetic expression.
- comma in return statement (the comma can easily be misread as a semicolon).
- prefer erfc, expm1 or log1p to avoid loss of precision.
- identical code in both branches of if/else or ternary operator.
- redundant pointer operation on pointer like &\*some_ptr.
- find unused 'goto' labels.
- function declaration and definition argument names different.
- function declaration and definition argument order different.
- shadow variable.
- variable can be declared const.
- calculating modulo of one.
- known function argument, suspicious calculation.

## STL usage ##
Check for invalid usage of STL:
- out of bounds errors
- misuse of iterators when iterating through a container
- mismatching containers in calls
- same iterators in calls
- dereferencing an erased iterator
- for vectors: using iterator/pointer after push_back has been used
- optimisation: use empty() instead of size() to guarantee fast code
- suspicious condition when using find
- unnecessary searching in associative containers
- redundant condition
- common mistakes when using string::c_str()
- useless calls of string and STL functions
- dereferencing an invalid iterator
- erasing an iterator that is out of bounds
- reading from empty STL container
- iterating over an empty STL container
- consider using an STL algorithm instead of raw loop
- incorrect locking with mutex

## Sizeof ##
sizeof() usage checks
- sizeof for array given as function argument
- sizeof for numeric given as function argument
- using sizeof(pointer) instead of the size of pointed data
- look for 'sizeof sizeof ..'
- look for calculations inside sizeof()
- look for function calls inside sizeof()
- look for suspicious calculations with sizeof()
- using 'sizeof(void)' which is undefined

## String ##
Detect misusage of C-style strings:
- overlapping buffers passed to sprintf as source and destination
- incorrect length arguments for 'substr' and 'strncmp'
- suspicious condition (runtime comparison of string literals)
- suspicious condition (string/char literals as boolean)
- suspicious comparison of a string literal with a char\* variable
- suspicious comparison of '\0' with a char\* variable
- overlapping strcmp() expression

## Type ##
Type checks
- bitwise shift by too many bits (only enabled when --platform is used)
- signed integer overflow (only enabled when --platform is used)
- dangerous sign conversion, when signed value can be negative
- possible loss of information when assigning int result to long variable
- possible loss of information when returning int result as long return value
- float conversion overflow

## Uninitialized variables ##
Uninitialized variables
- using uninitialized local variables
- using allocated data before it has been initialized

## UnusedVar ##
UnusedVar checks
- unused variable
- allocated but unused variable
- unread variable
- unassigned variable
- unused struct member

## Using postfix operators ##
Warn if using postfix operators ++ or -- rather than prefix operator

## Vaarg ##
Check for misusage of variable argument lists:
- Wrong parameter passed to va_start()
- Reference passed to va_start()
- Missing va_end()
- Using va_list before it is opened
- Subsequent calls to va_start/va_copy()
```

# Usage

``` bash
cppcheck $DIR --check-level=normal --checkers-report=cppcheck-report.txt --clang=/usr/local/bin/clang --enable=all --platform=unix64 --report-progress --showtime=file --std=c++17 --inconclusive --verbose -j8

```

```
$ ./cppcheck --help
Cppcheck - A tool for static C/C++ code analysis

Syntax:
    cppcheck [OPTIONS] [files or paths]

If a directory is given instead of a filename, *.cpp, *.cxx, *.cc, *.c++, *.c, *.ipp,
*.ixx, *.tpp, and *.txx files are checked recursively from the given directory.

Options:
    --addon=<addon>
                         Execute addon. i.e. --addon=misra. If options must be
                         provided a json configuration is needed.
    --addon-python=<python interpreter>
                         You can specify the python interpreter either in the
                         addon json files or through this command line option.
                         If not present, Cppcheck will try "python3" first and
                         then "python".
    --cppcheck-build-dir=<dir>
                         Cppcheck work folder. Advantages:
                          * whole program analysis
                          * faster analysis; Cppcheck will reuse the results if
                            the hash for a file is unchanged.
                          * some useful debug information, i.e. commands used to
                            execute clang/clang-tidy/addons.
    --check-config       Check cppcheck configuration. The normal code
                         analysis is disabled by this flag.
    --check-level=<level>
                         Configure how much valueflow analysis you want:
                          * reduced: Reduce valueflow to finish checking quickly.
                          * normal: Cppcheck uses some compromises in the analysis so
                            the checking will finish in reasonable time.
                          * exhaustive: deeper analysis that you choose when you can
                            wait.
                         The default choice is 'normal'.
    --check-library      Show information messages when library files have
                         incomplete info.
    --checkers-report=<file>
                         Write a report of all the active checkers to the given file.
    --clang=<path>       Experimental: Use Clang parser instead of the builtin Cppcheck
                         parser. Takes the executable as optional parameter and
                         defaults to `clang`. Cppcheck will run the given Clang
                         executable, import the Clang AST and convert it into
                         Cppcheck data. After that the normal Cppcheck analysis is
                         used. You must have the executable in PATH if no path is
                         given.
    --config-exclude=<dir>
                         Path (prefix) to be excluded from configuration
                         checking. Preprocessor configurations defined in
                         headers (but not sources) matching the prefix will not
                         be considered for evaluation.
    --config-excludes-file=<file>
                         A file that contains a list of config-excludes
    --disable=<id>       Disable individual checks.
                         Please refer to the documentation of --enable=<id>
                         for further details.
    --dump               Dump xml data for each translation unit. The dump
                         files have the extension .dump and contain ast,
                         tokenlist, symboldatabase, valueflow.
    -D<ID>               Define preprocessor symbol. Unless --max-configs or
                         --force is used, Cppcheck will only check the given
                         configuration when -D is used.
                         Example: '-DDEBUG=1 -D__cplusplus'.
    -E                   Print preprocessor output on stdout and don't do any
                         further processing.
    --enable=<id>        Enable additional checks. The available ids are:
                          * all
                                  Enable all checks. It is recommended to only
                                  use --enable=all when the whole program is
                                  scanned, because this enables unusedFunction.
                          * warning
                                  Enable warning messages
                          * style
                                  Enable all coding style checks. All messages
                                  with the severities 'style', 'warning',
                                  'performance' and 'portability' are enabled.
                          * performance
                                  Enable performance messages
                          * portability
                                  Enable portability messages
                          * information
                                  Enable information messages
                          * unusedFunction
                                  Check for unused functions. It is recommended
                                  to only enable this when the whole program is
                                  scanned.
                          * missingInclude
                                  Warn if there are missing includes.
                         Several ids can be given if you separate them with
                         commas. See also --std
    --error-exitcode=<n> If errors are found, integer [n] is returned instead of
                         the default '0'. '1' is returned
                         if arguments are not valid or if no input files are
                         provided. Note that your operating system can modify
                         this value, e.g. '256' can become '0'.
    --errorlist          Print a list of all the error messages in XML format.
    --exitcode-suppressions=<file>
                         Used when certain messages should be displayed but
                         should not cause a non-zero exitcode.
    --file-filter=<str>  Analyze only those files matching the given filter str
                         Can be used multiple times
                         Example: --file-filter=*bar.cpp analyzes only files
                                  that end with bar.cpp.
    --file-list=<file>   Specify the files to check in a text file. Add one
                         filename per line. When file is '-,' the file list will
                         be read from standard input.
    -f, --force          Force checking of all configurations in files. If used
                         together with '--max-configs=', the last option is the
                         one that is effective.
    --fsigned-char       Treat char type as signed.
    --funsigned-char     Treat char type as unsigned.
    -h, --help           Print this help.
    -I <dir>             Give path to search for include files. Give several -I
                         parameters to give several paths. First given path is
                         searched for contained header files first. If paths are
                         relative to source files, this is not needed.
    --includes-file=<file>
                         Specify directory paths to search for included header
                         files in a text file. Add one include path per line.
                         First given path is searched for contained header
                         files first. If paths are relative to source files,
                         this is not needed.
    --include=<file>
                         Force inclusion of a file before the checked file.
    -i <str>             Exclude source files or directories matching str from
                         the check. This applies only to source files so header
                         files included by source files are not matched.
    --inconclusive       Allow that Cppcheck reports even though the analysis is
                         inconclusive.
                         There are false positives with this option. Each result
                         must be carefully investigated before you know if it is
                         good or bad.
    --inline-suppr       Enable inline suppressions. Use them by placing one or
                         more comments, like: '// cppcheck-suppress warningId'
                         on the lines before the warning to suppress.
    -j <jobs>            Start <jobs> threads to do the checking simultaneously.
    -l <load>            Specifies that no new threads should be started if
                         there are other threads running and the load average is
                         at least <load>.
    --language=<language>, -x <language>
                         Forces cppcheck to check all files as the given
                         language. Valid values are: c, c++
    --library=<cfg>      Load file <cfg> that contains information about types
                         and functions. With such information Cppcheck
                         understands your code better and therefore you
                         get better results. The std.cfg file that is
                         distributed with Cppcheck is loaded automatically.
                         For more information about library files, read the
                         manual.
    --max-configs=<limit>
                         Maximum number of configurations to check in a file
                         before skipping it. Default is '12'. If used together
                         with '--force', the last option is the one that is
                         effective.
    --max-ctu-depth=N    Max depth in whole program analysis. The default value
                         is 2. A larger value will mean more errors can be found
                         but also means the analysis will be slower.
    --output-file=<file> Write results to file, rather than standard error.
    --output-format=<format>
                        Specify the output format. The available formats are:
                          * text
                          * sarif
                          * xml
    --platform=<type>, --platform=<file>
                         Specifies platform specific types and sizes. The
                         available builtin platforms are:
                          * unix32
                                 32 bit unix variant
                          * unix64
                                 64 bit unix variant
                          * win32A
                                 32 bit Windows ASCII character encoding
                          * win32W
                                 32 bit Windows UNICODE character encoding
                          * win64
                                 64 bit Windows
                          * avr8
                                 8 bit AVR microcontrollers
                          * elbrus-e1cp
                                 Elbrus e1c+ architecture
                          * pic8
                                 8 bit PIC microcontrollers
                                 Baseline and mid-range architectures
                          * pic8-enhanced
                                 8 bit PIC microcontrollers
                                 Enhanced mid-range and high end (PIC18) architectures
                          * pic16
                                 16 bit PIC microcontrollers
                          * mips32
                                 32 bit MIPS microcontrollers
                          * native
                                 Type sizes of host system are assumed, but no
                                 further assumptions.
                          * unspecified
                                 Unknown type sizes
    --plist-output=<path>
                         Generate Clang-plist output files in folder.
    --project=<file>     Run Cppcheck on project. The <file> can be a Visual
                         Studio Solution (*.sln), Visual Studio Project
                         (*.vcxproj), compile database (compile_commands.json),
                         or Borland C++ Builder 6 (*.bpr). The files to analyse,
                         include paths, defines, platform and undefines in
                         the specified file will be used.
    --project-configuration=<config>
                         If used together with a Visual Studio Solution (*.sln)
                         or Visual Studio Project (*.vcxproj) you can limit
                         the configuration cppcheck should check.
                         For example: '--project-configuration=Release|Win32'
    -q, --quiet          Do not show progress reports.
                         Note that this option is not mutually exclusive with --verbose.
    -rp=<paths>, --relative-paths=<paths>
                         Use relative paths in output. When given, <paths> are
                         used as base. You can separate multiple paths by ';'.
                         Otherwise path where source files are searched is used.
                         We use string comparison to create relative paths, so
                         using e.g. ~ for home folder does not work. It is
                         currently only possible to apply the base paths to
                         files that are on a lower level in the directory tree.
    --report-progress    Report progress messages while checking a file (single job only).
    --report-type=<type> Add guideline and classification fields for specified coding standard.
                         The available report types are:
                          * normal           Default, only show cppcheck error ID and severity
                          * autosar          Autosar
                          * cert-c-2016      Cert C 2016
                          * cert-cpp-2016    Cert C++ 2016
                          * misra-c-2012     Misra C 2012
                          * misra-c-2023     Misra C 2023
                          * misra-c-2025     Misra C 2025
                          * misra-cpp-2008   Misra C++ 2008
                          * misra-cpp-2023   Misra C++ 2023
    --rule=<rule>        Match regular expression.
    --rule-file=<file>   Use given rule file. For more information, see:
                         http://sourceforge.net/projects/cppcheck/files/Articles/
    --safety             Enable safety-certified checking mode: display checker summary, enforce
                         stricter checks for critical errors, and return a non-zero exit code
                         if such errors occur.
    --showtime=<mode>    Show timing information.
                         The available modes are:
                          * none
                                 Show nothing (default)
                          * file
                                 Show for each processed file
                          * file-total
                                 Show total time only for each processed file
                          * summary
                                 Show a summary at the end
                          * top5_file
                                 Show the top 5 for each processed file
                          * top5_summary
                                 Show the top 5 summary at the end
    --std=<id>           Set standard.
                         The available options are:
                          * c89
                                 C code is C89 compatible
                          * c99
                                 C code is C99 compatible
                          * c11
                                 C code is C11 compatible (default)
                          * c++03
                                 C++ code is C++03 compatible
                          * c++11
                                 C++ code is C++11 compatible
                          * c++14
                                 C++ code is C++14 compatible
                          * c++17
                                 C++ code is C++17 compatible
                          * c++20
                                 C++ code is C++20 compatible (default)
    --suppress=<spec>    Suppress warnings that match <spec>. The format of
                         <spec> is:
                         [error id]:[filename]:[line]
                         The [filename] and [line] are optional. If [error id]
                         is a wildcard '*', all error ids match.
    --suppressions-list=<file>
                         Suppress warnings listed in the file. Each suppression
                         is in the same format as <spec> above.
    --suppress-xml=<file>
                         Suppress warnings listed in a xml file. XML file should
                         follow the manual.pdf format specified in section.
                         `6.4 XML suppressions` .
    --template='<text>'  Format the error messages. Available fields:
                           {file}              file name
                           {line}              line number
                           {column}            column number
                           {callstack}         show a callstack. Example:
                                                 [file.c:1] -> [file.c:100]
                           {inconclusive:text} if warning is inconclusive, text
                                               is written
                           {severity}          severity
                           {message}           warning message
                           {id}                warning id
                           {cwe}               CWE id (Common Weakness Enumeration)
                           {code}              show the real code
                           \t                 insert tab
                           \n                 insert newline
                           \r                 insert carriage return
                         Example formats:
                         '{file}:{line},{severity},{id},{message}' or
                         '{file}({line}):({severity}) {message}' or
                         '{callstack} {message}'
                         Pre-defined templates: gcc (default), cppcheck1 (old default), vs, edit.
    --template-location='<text>'
                         Format error message location. If this is not provided
                         then no extra location info is shown.
                         Available fields:
                           {file}      file name
                           {line}      line number
                           {column}    column number
                           {info}      location info
                           {code}      show the real code
                           \t         insert tab
                           \n         insert newline
                           \r         insert carriage return
                         Example format (gcc-like):
                         '{file}:{line}:{column}: note: {info}\n{code}'
    -U<ID>               Undefine preprocessor symbol. Use -U to explicitly
                         hide certain #ifdef <ID> code paths from checking.
                         Example: '-UDEBUG'
    -v, --verbose        Output more detailed error information.
                         Note that this option is not mutually exclusive with --quiet.
    --version            Print out version number.
    --xml                Write results in xml format to error stream (stderr).

Example usage:
  # Recursively check the current folder. Print the progress on the screen and
  # write errors to a file:
  cppcheck . 2> err.txt

  # Recursively check ../myproject/ and don't print progress:
  cppcheck --quiet ../myproject/

  # Check test.cpp, enable all checks:
  cppcheck --enable=all --inconclusive --library=posix test.cpp

  # Check f.cpp and search include files from inc1/ and inc2/:
  cppcheck -I inc1/ -I inc2/ f.cpp

For more information:
    https://files.cppchecksolutions.com/manual.pdf

Many thanks to the 3rd party libraries we use:
 * tinyxml2 -- loading project/library/ctu files.
 * picojson -- loading compile database.
 * pcre -- rules.
 * qt -- used in GUI
```

# Tips

``` bash
# get list of checks
cppcheck --doc

# get list of error messages
cppcheck --errorlist
```

# 参考脚本

## run_cppcheck_by_dir.sh

``` bash
#!/bin/bash
# Cppcheck 2.18.0
# https://github.com/danmar/cppcheck/releases/tag/2.18.0
#
# Usage: run_cppcheck_by_dir.sh [<directory>]
# If no directory is specified, it defaults to the current directory.
# Example: ./run_cppcheck_by_dir.sh /path/to/directory

set -euo pipefail

# Configuration parameters
DIR="${1:-.}"  # Default to current directory if no argument is provided
WORKDIR="$HOME/jlib_proj/JLib/tools/cppcheck/workdir"
OUTPUT_FILE="cppcheck.output"
REPORT_FILE="cppcheck.report"
ERROR_FILE="cppcheck.error"
# Number of threads to use for parallel processing
# Default to number of CPU cores if not set
THREADS="${THREADS:-$(nproc)}"

# Check if cppcheck is installed
if ! command -v cppcheck &> /dev/null; then
    echo "cppcheck command not found. Please install cppcheck."
    exit 1
fi

# Clean workdir cache
mkdir -p "$WORKDIR"
rm $WORKDIR/* || true

# Run cppcheck
# --template="{file}:{line}:{column}: {severity}:{message}" \
cppcheck "$DIR" \
    --cppcheck-build-dir="$WORKDIR" \
    -j "$THREADS" \
    --enable=all \
    --platform=unix64 \
    --std=c++17 \
    --inconclusive \
    --force \
    --output-format=text \
    --output-file="$OUTPUT_FILE" \
    --checkers-report="$REPORT_FILE" \
    --inline-suppr \
    --verbose \
    -D__cppcheck__ \
    --template="{file}:{line}:{column}: {severity}:{message}" \
    2> >(tee "$ERROR_FILE" >&2)

echo "Check completed. Output saved to: $OUTPUT_FILE"
echo "Detailed error log: $ERROR_FILE"
```

## run_cppcheck_by_compile_database.sh

``` bash
#!/bin/bash
# Cppcheck 2.18.0
# https://github.com/danmar/cppcheck/releases/tag/2.18.0
#
# Usage: run_jlib_cppcheck_by_compile_database.sh </path/to/compile_commands.json>
# Example: ./run_jlib_cppcheck_by_compile_database.sh /path/to/compile_commands.json

set -euo pipefail

# Configuration parameters
COMPILE_COMMANDS_FILE="${1:-$HOME/jlib_proj/JLib/compile_commands.json}"  # Path to compile_commands.json
WORKDIR="$HOME/jlib_proj/JLib/tools/cppcheck/workdir"
OUTPUT_FILE="cppcheck.output"
REPORT_FILE="cppcheck.report"
ERROR_FILE="cppcheck.error"
CLANG_PATH="${CLANG_PATH:-/usr/local/bin/clang}"  # Clang path (if needed)
THREADS="${THREADS:-$(nproc)}"  # Automatically get CPU core count

# Check if cppcheck is installed
if ! command -v cppcheck &> /dev/null; then
    echo "cppcheck command not found. Please install cppcheck."
    exit 1
fi

# Clean workdir cache
mkdir -p "$WORKDIR"
rm $WORKDIR/* || true

# Run cppcheck
cppcheck -i$HOME/jlib_proj/JLib/third_party \
    -j "$THREADS" \
    --enable=all \
    --cppcheck-build-dir=$WORKDIR \
    --project=$COMPILE_COMMANDS_FILE \
    --platform=unix64 \
    --std=c++17 \
    --inconclusive \
    --force \
    --output-format=text \
    --output-file="$OUTPUT_FILE" \
    --checkers-report="$REPORT_FILE" \
    --suppress=missingInclude \
    --suppress=missingIncludeSystem \
    --suppress=cstyleCast \
    --suppress=unusedFunction \
    --inline-suppr \
    2> >(tee "$ERROR_FILE" >&2)

echo "Check completed. Output saved to: $OUTPUT_FILE"
echo "Detailed error log: $ERROR_FILE"
```




# Refer

* https://cppcheck.sourceforge.io/
* https://github.com/danmar/cppcheck


