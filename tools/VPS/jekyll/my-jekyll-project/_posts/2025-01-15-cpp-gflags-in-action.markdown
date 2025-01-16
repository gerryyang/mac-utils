---
layout: post
title:  "CPP gflags in Action"
date:   2025-01-15 20:30:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Example

https://github.com/gflags/example

``` cpp
#include <cstdio>
#include <gflags/gflags.h>

DEFINE_bool(verbose, false, "Display agent's version information");
DEFINE_string(svrname, "agent", "Set agent's name");

int main(int argc, char **argv)
{
    std::string strUsage("This program is an example.\n");
    strUsage += "\nUsage:\n";
    strUsage += "  ";
    strUsage += argv[0];
    strUsage += " [flags]\n";
    strUsage += "    Start an agent server.";

    gflags::SetUsageMessage(strUsage);
    google::SetVersionString(JLib::kVersion);

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    std::string strSvrName = FLAGS_svrname;
    ServerApp::SetServerName(strSvrName.c_str());

    if (FLAGS_verbose)
    {
        printf("%s verbose information:\n\n", gflags::ProgramInvocationShortName());
        ServerApp::ShowServerVersion();
        exit(0);
    }

    ServerApp app;
    app.Run();

    return 0;
}
```

输出示例：

```
$ ./agent -help
agent: This program is an example.

Usage:
  ./agent [flags]
    Start an agent server.

  Flags from agent/agent.cpp:
    -svrname (Set agent's name) type: string default: "agent"
    -verbose (Display agent's version information) type: bool default: false

...
```

# Introduction, and Comparison to Other Commandline Flags Libraries

**Commandline flags** are flags that users specify on the command line when they run an executable. In the command

``` bash
fgrep -l -f /var/tmp/foo johannes brahms
```

`-l` and `-f /var/tmp/foo` are the **two commandline flags**. (`johannes` and `brahms`, which don't start with a **dash**, are **commandline arguments**.)

Typically, an application lists what flags the user is allowed to pass in, and what arguments they take `--` in this example, `-l` takes no argument, and `-f` takes a string (in particular, a filename) as an argument. Users can use a library to help parse the commandline and store the flags in some data structure.

`Gflags`, **the commandline flags library used within Google**, differs from other libraries, such as `getopt()`, in that flag definitions can be scattered around the source code, and not just listed in one place such as `main()`. In practice, this means that a single source-code file will define and use flags that are meaningful to that file. Any application that links in that file will get the flags, and the gflags library will automatically handle that flag appropriately.

There's significant gain in flexibility, and ease of code reuse, due to this technique. **However, there is a danger that two files will define the same flag, and then give an error when they're linked together**.

The rest of this document describes how to use the commandlineflag library. It's a C++ library, so examples are in C++.

# Download and Installation

The `gflags` library can be downloaded from [GitHub](https://github.com/gflags/gflags). You can clone the project using the command:

```
git clone https://github.com/gflags/gflags.git
```

Build and installation instructions are provided in the [INSTALL](https://github.com/gflags/gflags/blob/master/INSTALL.md) file. The installation of the gflags package includes configuration files for popular build systems such as [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/), [CMake](https://gflags.github.io/gflags/#cmake), and [Bazel](https://gflags.github.io/gflags/#bazel).


# Declare dependency on gflags with CMake

Using gflags within a project which uses [CMake](http://www.cmake.org/) for its build system is easy. You can either require an external installation of the gflags package and find it using CMake's `find_package` command, or include the gflags project as subtree or submodule within your project's source tree and add the directory using CMake's `add_subdirectory` command.



# Declare dependency on gflags with Bazel

To use `gflags` within a project which uses [Bazel](https://bazel.build/) as build tool, add the following lines to your `WORKSPACE` file (see also Bazel documentation of [git_repository](https://www.bazel.io/versions/master/docs/be/workspace.html#git_repository)):

```
git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/gflags/gflags.git",
    tag = "v2.2.2"
)
```

You can then add `@com_github_gflags_gflags//:gflags` to the deps section of a `cc_binary` or `cc_library` rule, and `#include "gflags/gflags.h"` to include it in your source code. This uses the shared gflags library with multi-threading enabled. In order to use the single-threaded shared gflags library, use the dependency `@com_github_gflags_gflags//:gflags_nothreads` instead.

For example, see the following `BUILD` rule of the `gflags/example` project:

```
cc_binary(
    name = "foo",
    srcs = ["main.cc"],
    deps = ["@com_github_gflags_gflags//:gflags"],
)
```

# DEFINE: Defining Flags In Program

Defining a flag is easy: just use the appropriate macro for the type you want the flag to be, as defined at the bottom of `gflags/gflags.h`. Here's an example file, `foo.cc`:

``` cpp
// foo.cc

#include <gflags/gflags.h>

DEFINE_bool(big_menu, true, "Include 'advanced' options in the menu listing");
DEFINE_string(languages, "english,french,german",
                "comma-separated list of languages to offer in the 'lang' menu");
```

`DEFINE_bool` defines a boolean flag. Here are the types supported:

* `DEFINE_bool`: boolean
* `DEFINE_int32`: 32-bit integer
* `DEFINE_int64`: 64-bit integer
* `DEFINE_uint64`: unsigned 64-bit integer
* `DEFINE_double`: double
* `DEFINE_string`: C++ string

Note that there are no 'complex' types like lists: the "languages" flag in our example is a list of strings, but is defined of type "string", not "list_of_string" or similar. This is by design. We'd rather use only simple types for the flags, and allow for complex, arbitrary parsing routines to parse them, than to try to put the logic inside the flags library proper.

**All DEFINE macros take the same three arguments**: **the name of the flag**, **its default value**, and **a 'help' string that describes its use**. The 'help' string is displayed when the user runs the application with the `--help` flag.

You can define a flag in any source-code file in your executable. Only define a flag once! If you want to access a flag in more than one source file, DEFINE it in one file, and [DECLARE](https://gflags.github.io/gflags/#declare) it in the others. Even better, DEFINE it in `foo.cc` and DECLARE it in `foo.h`; then everyone who `#includes foo.h` can use the flag.

Defining flags in libraries rather than in main() is powerful, but does have some costs. One is that a library might not have a good default value for its flags, for example if the flag holds a filename that might not exist in some environments. **To mitigate such problems, you can use flag validators to ensure prompt notification (in the form of a crash) of an invalid flag value**.

Note that while most functions in this library are defined in the `google` namespace, `DEFINE_foo` (and `DECLARE_foo`, [below](https://gflags.github.io/gflags/#declare)), should always be in the global namespace.


# Accessing the Flag

All defined flags are available to the program as just a normal variable, with the prefix `FLAGS_` prepended. In the above example, the macros define two variables, `FLAGS_big_menu` (a bool), and `FLAGS_languages` (a C++ string).

You can read and write to the flag just like any other variable:

``` cpp
if (FLAGS_consider_made_up_languages)
    FLAGS_languages += ",klingon";   // implied by --consider_made_up_languages

if (FLAGS_languages.find("finnish") != string::npos)
    HandleFinnish();
```

You can also get and set flag values via special functions in `gflags.h`. That's a rarer use case, though.


# DECLARE: Using the Flag in a Different File

TODO


# RegisterFlagValidator: Sanity-checking Flag Values

After DEFINE-ing a flag, you may optionally register a validator function with the flag. If you do this, after the flag is parsed from the commandline, and whenever its value is changed via a call to `SetCommandLineOption()`, the validator function is called with the new value as an argument. The validator function should return 'true' if the flag value is valid, and false otherwise. If the function returns false for the new setting of the flag, the flag will retain its current value. If it returns false for the default value, `ParseCommandLineFlags` will die.

Here is an example use of this functionality:

``` cpp
static bool ValidatePort(const char* flagname, int32 value) {
   if (value > 0 && value < 32768)   // value is ok
     return true;
   printf("Invalid value for --%s: %d\n", flagname, (int)value);
   return false;
}
DEFINE_int32(port, 0, "What port to listen on");
DEFINE_validator(port, &ValidatePort);
```

By doing the registration at global initialization time (right after the DEFINE_int32), we ensure that the registration happens before the commandline is parsed at the beginning of `main()`.

The above used `DEFINE_validator` macro calls the `RegisterFlagValidator()` function which returns true if the registration is successful. It returns false if the registration fails because

a. the first argument does not refer to a commandline flag, or
b. a different validator has already been registered for this flag.

The return value is available as global static boolean variable named `<flag>_validator_registered`.

# Putting It Together: How to Set Up Flags

The final piece is the one that tells the executable to process the commandline flags, and set the `FLAGS_*` variables to the appropriate, non-default value based on what is seen on the commandline. This is equivalent to the `getopt()` call in the getopt library, but has much less overhead to use. In fact, it's just a single function call:

``` cpp
gflags::ParseCommandLineFlags(&argc, &argv, true);
```

Usually, this code is at the beginning of `main()`. `argc` and `argv` are exactly as passed in to `main()`. This routine might modify them, which is why pointers to them are passed in.

The last argument is called "remove_flags". If **true**, then `ParseCommandLineFlags` removes the flags and their arguments from `argv`, and modifies `argc` appropriately. In this case, after the function call, `argv` will hold only commandline arguments, and not commandline flags.

If, on the other hand, remove_flags is **false**, then `ParseCommandLineFlags` will leave argc unchanged, but will rearrange the arguments in argv so that the flags are all at the beginning. For example, if the input is "/bin/foo" "arg1" "-q" "arg2" (which is legal but weird), the function will rearrange argv so it reads "/bin/foo", "-q", "arg1", "arg2". In this case, `ParseCommandLineFlags` returns the index into argv that holds the first commandline argument: that is, the index past the last flag. (In this example, it would return 2, since `argv[2]` points to arg1.)

In either case, the `FLAGS_*` variables are modified based on what was [passed in on the commandline](https://gflags.github.io/gflags/#commandline).

> gflags::ParseCommandLineFlags 函数的第三个参数是一个布尔值，决定了是否从 argv 中移除已经解析的标志。
>
> 如果你设置这个参数为 true，那么 ParseCommandLineFlags 会从 argv 中移除已经解析的标志，并且会修改 argc 的值。这样，当函数返回时，argv 只包含那些没有被解析的参数，argc 是这些参数的数量。这对于你的程序来说可能会更加方便，因为你可以直接处理剩下的参数，而不需要关心已经被解析的标志。
>
> 如果你设置这个参数为 false，那么 ParseCommandLineFlags 不会修改 argv 和 argc。这意味着你需要自己处理已经被解析的标志。




# Setting Flags on the Command Line

The reason you make something a flag instead of a compile-time constant, is so users can specify a non-default value on the commandline. Here's how they might do it for an application that links in `foo.cc`:

``` bash
app_containing_foo --nobig_menu -languages="chinese,japanese,korean" ...
```

This sets `FLAGS_big_menu = false`; and `FLAGS_languages = "chinese,japanese,korean"`, when `ParseCommandLineFlags` is run.

Note the **atypical syntax** for setting a boolean flag to **false**: putting "no" in front of its name. There's a fair bit of flexibility to how flags may be specified. Here's an example of all the ways to specify the "languages" flag:

```
app_containing_foo --languages="chinese,japanese,korean"
app_containing_foo -languages="chinese,japanese,korean"
app_containing_foo --languages "chinese,japanese,korean"
app_containing_foo -languages "chinese,japanese,korean"
```

For boolean flags, the possibilities are slightly different:

```
app_containing_foo --big_menu
app_containing_foo --nobig_menu
app_containing_foo --big_menu=true
app_containing_foo --big_menu=false
```

(as well as the single-dash variant on all of these).

Despite this flexibility, **we recommend using only a single form: `--variable=value` for non-boolean flags, and `--variable/--novariable` for boolean flags.** This consistency will make your code more readable, and is also the format required for certain special-use cases like [flagfiles](https://gflags.github.io/gflags/#flagfiles).

It is a fatal error to specify a flag on the commandline that has not been DEFINED somewhere in the executable. If you need that functionality for some reason -- say you want to use the same set of flags for several executables, but not all of them DEFINE every flag in your list -- you can specify `--undefok` to **suppress the error**.

As in `getopt()`, `--` by itself will terminate flags processing. So in `foo -f1 1 -- -f2 2`, `f1` is considered a flag, but `-f2` is not.

> `--` 有一个特殊的含义：它表示后面的参数不应被视为标志（flags）或选项（options），而应被视为普通的参数。foo 是一个命令，-f1 是一个标志，1 是 -f1 的参数。`--` 表示后面的参数不应被视为标志或选项。因此，-f2 不会被视为一个标志，而是被视为一个普通的参数。

**If a flag is specified more than once, only the last specification is used**; the others are ignored. (**若有重复的选项，以最后一个为准**)

Note that flags do not have single-letter synonyms, like they do in the `getopt` library, nor do we allow "combining" flags behind a single dash, as in `ls -la`.

> 在 getopt 库中，标志（flags）可以有单字母的简写形式，例如 -v 可能是 --verbose 的简写。然而，在这个 gflags 库中，标志没有单字母的简写形式。此外，getopt 库允许在一个短横线后面组合多个标志。例如，在 ls -la 命令中，-la 实际上是 -l 和 -a 两个标志的组合。然而，在这个库中，不允许这种组合标志的形式。


# Changing the Default Flag Value

Sometimes a flag is defined in a library, and you want to change its default value in one application but not others. It's simple to do this: just assign a new value to the flag in `main()`, before calling `ParseCommandLineFlags()`:

``` cpp
DECLARE_bool(lib_verbose);   // mylib has a lib_verbose flag, default is false

int main(int argc, char** argv) {
    FLAGS_lib_verbose = true;  // in my app, I want a verbose lib by default
    ParseCommandLineFlags(...);
}
```

For this application, users can still set the flag value on the commandline, but if they do not, the flag's value will default to true.


# Special Flags

There are a few flags defined by the commandlineflags module itself, and are available to all applications that use commandlineflags. These fall into three categories.

**First are the 'reporting' flags** that, when found, cause the application to print some information about itself and exit.

```
--help	shows all flags from all files, sorted by file and then by name; shows the flagname, its default value, and its help string
--helpfull	same as -help, but unambiguously asks for all flags (in case -help changes in the future)
--helpshort	shows only flags for the file with the same name as the executable (usually the one containing main())
--helpxml	like --help, but output is in xml for easier parsing
--helpon=FILE  	shows only flags defined in FILE.*
--helpmatch=S	shows only flags defined in *S*.*
--helppackage	shows flags defined in files in same directory as main()
--version	prints version info for the executable
```

**Second are the flags that affect how other flags are parsed**.

```
--undefok=flagname,flagname,...

for those names listed as the argument to --undefok, suppress the normal error-exit that occurs when --name is seen on the commandline, but name has not been DEFINED anywhere in the application
```

**Third are the 'recursive' flags**, that cause other flag values to be set: --fromenv, --tryfromenv, --flagfile. These are described below in more detail.

> --fromenv

`--fromenv=foo,bar` says to read the values for the `foo` and `bar` flags from the **environment**. In concert with this flag, you must actually set the values in the environment, via a line like one of the two below:

``` bash
export FLAGS_foo=xxx; export FLAGS_bar=yyy   # sh

setenv FLAGS_foo xxx; setenv FLAGS_bar yyy   # tcsh
```

This is equivalent to specifying `--foo=xxx`, `--bar=yyy` on the commandline.

Note it is a fatal error to say `--fromenv=foo` if foo is not DEFINED somewhere in the application. (Though you can suppress this error via `--undefok=foo`, just like for any other flag.)

It is also a fatal error to say `--fromenv=foo` if `FLAGS_foo` is not actually defined in the environment.

> --tryfromenv

`--tryfromenv` is exactly like --fromenv, except it is not a fatal error to say `--tryfromenv=foo` if `FLAGS_foo` is not actually defined in the environment. Instead, in such cases, FLAGS_foo just keeps its default value as specified in the application.

Note it is still an error to say `--tryfromenv=foo` if foo is not DEFINED somewhere in the application.

> --flagfile

`--flagfile=f `tells the commandlineflags module to read the file `f`, and to run all the flag-assignments found in that file as if these flags had been specified on the commandline.

In its simplest form, `f` should just be a list of flag assignments, one per line. Unlike on the commandline, the equals sign separating a flagname from its argument is required for flagfiles. An example flagfile, `/tmp/myflags`:

```
--nobig_menus
--languages=english,french
```

With this flagfile, the following two lines are equivalent:

``` bash
./myapp --foo --nobig_menus --languages=english,french --bar
./myapp --foo --flagfile=/tmp/myflags --bar
```

# The API

In addition to accessing `FLAGS_foo` directly, it is possible to access the flags programmatically, through an API. It is also possible to access information about a flag, such as its default value and help-string. A `FlagSaver` makes it easy to modify flags and then automatically undo the modifications later. Finally, there are somewhat unrelated, but useful, routines to easily access parts of `argv` outside main, including the program name (`argv[0]`).

For more information about these routines, and other useful helper methods such as `gflags::SetUsageMessage()` and `gflags::SetVersionString`, see `gflags.h`.

# Miscellaneous Notes

If your application has code like this:

``` cpp
#define STRIP_FLAG_HELP 1    // this must go before the #include!
#include <gflags/gflags.h>
```

we will remove the help messages from the compiled source. This can reduce the size of the resulting binary somewhat, and may also be useful for security reasons.


# Issues and Feature Requests

Please report any issues or ideas for additional features on [GitHub](https://github.com/gflags/gflags/issues). We would also like to encourage [pull requests](https://github.com/gflags/gflags/pulls) for bug fixes and implementations of new features.


# 对比方案

## getopt

https://www.man7.org/linux/man-pages/man3/getopt.3.html

The following trivial example program uses `getopt()` to handle two program options: `-n`, with no associated value; and `-t val`, which expects an associated value.

``` cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int flags, opt;
    int nsecs, tfnd;

    nsecs = 0;
    tfnd = 0;
    flags = 0;
    while ((opt = getopt(argc, argv, "nt:")) != -1) {
        switch (opt) {
        case 'n':
            flags = 1;
            break;
        case 't':
            nsecs = atoi(optarg);
            tfnd = 1;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    printf("flags=%d; tfnd=%d; nsecs=%d; optind=%d\n",
            flags, tfnd, nsecs, optind);

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    printf("name argument = %s\n", argv[optind]);

    /* Other code omitted */

    exit(EXIT_SUCCESS);
}
```



## Boost.Program_options

传统上 C 代码里处理命令行参数会使用 `getopt`。比如在下面的代码中：

[https://github.com/adah1972/breaktext/blob/master/breaktext.c](https://github.com/adah1972/breaktext/blob/master/breaktext.c)

这种方式有不少缺陷：

* 一个选项通常要在三个地方重复：说明文本里，getopt 的参数里，以及对 getopt 的返回结果进行处理时。
* 对选项的附加参数需要手工写代码处理，因而常常不够严格（C 的类型转换不够方便，尤其是检查错误）。

`Program_options` 正是解决这个问题的。这个代码有点老了，不过还挺实用；懒得去找特别的处理库时，至少这个伸手可用。使用这个库需要链接 `boost_program_options` 库。

``` cpp
#include <iostream>
#include <string>
#include <stdlib.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;

string locale;
string lang;
int width = 72;
bool keep_indent = false;
bool verbose = false;

int main(int argc, char* argv[])
{
  po::options_description desc(
    "Usage: breaktext [OPTION]... "
    "<Input File> [Output File]\n"
    "\n"
    "Available options");

  desc.add_options()
    ("locale,L",
     po::value<string>(&locale),
     "Locale of the console (system locale by default)")
    ("lang,l",
     po::value<string>(&lang),
     "Language of input (asssume no language by default)")
    ("width,w",
     po::value<int>(&width),
     "Width of output text (72 by default)")
    ("help,h", "Show this help message and exit")
    (",i",
     po::bool_switch(&keep_indent),
     "Keep space indentation")
    (",v",
     po::bool_switch(&verbose),
     "Be verbose");

  po::variables_map vm;
  try {
    po::store(
      po::parse_command_line(
        argc, argv, desc),
      vm);
  }
  catch (po::error& e) {
    cout << e.what() << endl;
    exit(1);
  }
  vm.notify();

  if (vm.count("help")) {
    cout << desc << "\n";
    exit(1);
  }
}
```

* `options_description` 是基本的选项描述对象的类型，构造时我们给出对选项的基本描述。
* `options_description` 对象的 `add_options` 成员函数会返回一个函数对象，然后我们直接用括号就可以添加一系列的选项。
* 每个选项初始化时可以有两个或三个参数，**第一项是选项的形式，使用长短选项用逗号隔开的字符串（可以只提供一种**），最后一项是选项的文字描述，中间如果还有一项的话，就是选项的值描述。
* 选项的值描述可以用 `value`，`bool_switch` 等方法，参数是**输出变量的指针**。
* `variables_map`，**变量映射表**，用来存储对命令行的扫描结果；它继承了标准的 `std::map`。
* `notify` 成员函数**用来把变量映射表的内容实际传送到选项值描述里提供的那些变量里去**。
* `count` 成员函数继承自 std::map，只能得到 0 或 1 的结果。

这样，程序就能处理上面的那些选项了。如果运行时在命令行加上 `-h` 或 `--help` 选项，程序就会输出跟原来类似的帮助输出——额外的好处是选项的描述信息较长时还能自动帮你折行，不需要手工排版了。



# Refer

* [How To Use gflags (formerly Google Commandline Flags)](https://gflags.github.io/gflags/)
* https://github.com/gflags