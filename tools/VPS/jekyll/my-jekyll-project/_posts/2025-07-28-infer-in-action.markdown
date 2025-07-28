---
layout: post
title:  "Infer in Action"
date:   2025-07-28 12:00:00 +0800
categories: Tools
---

* Do not remove this line (it will not be displayed)
{:toc}


[Infer](http://fbinfer.com/) is a static analysis tool for Java, C++, Objective-C, and C. Infer is written in [OCaml](https://ocaml.org/).

> `Infer` is a static analysis tool - if you give Infer some Java or C/C++/Objective-C code it produces a list of potential bugs. Anyone can use Infer to intercept critical bugs before they have shipped to users, and help prevent crashes or poor performance.

Start with the [Getting Started](https://fbinfer.com/docs/getting-started) guide and our other [docs](https://fbinfer.com/docs/getting-started) to download and try `Infer` yourself. `Infer` is still evolving, and we want to continue to develop it in the open. We hope it will be useful for other projects, so please try it out or contribute to it, join the community and give us feedback!


# Getting started with Infer

## Get Infer

You can use our binary releases, build infer from source, or use our Docker image.

Find our latest [binary release here](https://github.com/facebook/infer/releases/latest). Download the tarball then extract it anywhere on your system to start using infer. For example, this downloads infer in `/opt` on Linux (replace `VERSION` with the latest release, eg `VERSION=1.0.0`):

``` bash
VERSION=0.XX.Y; \
curl -sSL "https://github.com/facebook/infer/releases/download/v$VERSION/infer-linux64-v$VERSION.tar.xz" \
| sudo tar -C /opt -xJ && \
sudo ln -s "/opt/infer-linux64-v$VERSION/bin/infer" /usr/local/bin/infer
```

If the binaries do not work for you, or if you would rather build infer from source, follow the [install from source](https://github.com/facebook/infer/blob/main/INSTALL.md#install-infer-from-source) instructions to install Infer on your system.

Alternatively, use our [Docker images](https://github.com/facebook/infer/tree/main/docker).


## Try Infer in your browser

Try Infer on a small example on [Codeboard](https://codeboard.io/projects/11587?view=2.1-21.0-22.0).


# [Hello, World](https://fbinfer.com/docs/hello-world)

Follow the instructions in this page to try `Infer` on a few small examples. You should be able to see the bugs reported by `Infer`, fix the bugs and run `Infer` again to check that they are not reported anymore. This should give you a first idea of how `Infer` works. See the [User Guide](https://fbinfer.com/docs/infer-workflow) for more details about the use of `Infer`.

All these examples can be found in the [infer/examples](https://github.com/facebook/infer/tree/main/examples) directory distributed with Infer.

## Hello world Java

Here is a simple Java example to illustrate Infer at work.

``` java
// Hello.java
class Hello {
  int test() {
    String s = null;
    return s.length();
  }
}
```

To run `Infer`, type the following in your terminal from the same directory as `Hello.java`.

``` bash
infer run -- javac Hello.java
```

You should see the following error reported by `Infer`.

```
Hello.java:5: error: NULL_DEREFERENCE
  object s last assigned on line 4 could be null and is dereferenced at line 5
```

Now edit the file to add null checks:

``` java
  int test() {
    String s = null;
    return s == null ? 0 : s.length();
  }
```

Run `Infer` again. This time we get no error: `Infer` reports **No issues found**.

## Hello world C

Here is a simple C example to illustrate Infer at work.

``` c
// hello.c
#include <stdlib.h>

void test() {
  int *s = NULL;
  *s = 42;
}
```

To run `Infer`, type the following in your terminal from the same directory as `hello.c`.

``` bash
infer run -- gcc -c hello.c
```

You should see the following error reported by Infer.

![infer1](/assets/images/202507/infer1.png)

Now edit the file to add null checks:

``` c
void test() {
  int *s = NULL;
  if (s != NULL) {
    *s = 42;
  }
}
```

Run Infer again. This time we get no error.

![infer2](/assets/images/202507/infer2.png)

When analyzing C files, `Infer` captures the `gcc` command and runs `clang` instead to parse them. Thus you may get compiler errors and warnings that differ from gcc's. So in particular, the following two commands are equivalent:

``` bash
infer run -- gcc -c hello.c
infer run -- clang -c hello.c
```


# Infer workflow (Version: 1.2.0)

This page documents several ways of running `Infer`, that you can adapt to your own project.

**TL; DR:**

1. **Make sure your project is clean** when you first run Infer on it (with `make clean`, or `gradle clean`, or ...).
2. When running `Infer` several times in a row, either clean your project as in step 1 in-between `Infer` runs, or add `--reactive` to the `infer` command.
3. These steps are not needed if you are not using an incremental build system, for instance if you are analyzing single files with `infer run -- javac Hello.java`.
4. After a successful Infer run, you can explore Infer's reports in more details by running `infer explore` from the same directory.

## The two phases of an Infer run

Regardless of the input language (Java, Objective-C, or C), there are two main phases in an Infer run:

### 1. The capture phase

Compilation commands are captured by `Infer` to translate the files to be analyzed into Infer's own internal intermediate language.

This translation is similar to compilation, so `Infer` takes information from the compilation process to perform its own translation. This is why we call infer with a compilation command: `infer run -- javac File.java` or `infer run -- clang -c file.c`. What happens is that the files get compiled as usual, and they also get translated by `Infer` to be analyzed in the second phase. In particular, if no file gets compiled, also no file will be analyzed.

`Infer` stores the intermediate files in the results directory which by default is created in the folder where the `infer` command is invoked, and is called `infer-out/`. You can change the name of the results directory with the option `-o`, e.g.

``` bash
infer run -o /tmp/out -- javac Test.java
```

You can run just the capture phase using the `capture` subcommand instead of the `run` subcommand:

``` bash
infer capture -- javac Test.java
```


### 2. The analysis phase

In this phase, the files in `infer-out/` are analyzed by `Infer`. Infe`r analyzes each function and method separately. If `Infer` encounters an error when analyzing a method or function, it stops there for that method or function, but will continue the analysis of other methods and functions. So, a possible workflow would be to run `Infer` on your code, fix the errors generated, and run it again to find possibly more errors or to check that all the errors have been fixed.

The errors will be displayed in the standard output and also in a file `infer-out/report.txt`. We filter the bugs and show the ones that are most likely to be real.

## Global (default) and differential workflows

**By default**, running `Infer` will delete the previous `infer-out/` directory if it exists. This leads to a default workflow where the entire project is analyzed every time. Passing `--reactive` (or `-r`) to `Infer` prevents it from deleting `infer-out/`, leading to a differential workflow.

There are exceptions to this. In particular, you can run only one of the phases above. For instance, `infer run -- javac Hello.java` is equivalent to running these two commands:

``` bash
infer capture -- javac Hello.java
infer analyze
```

Notice that the second command does not erase `infer-out/`, as the files it needs to analyze live there!

You can learn more about the subcommands supported by Infer by running `infer --help`, `infer capture --help`, or more generally `infer <subcommand> --help`.

Let us highlight when you may need global and differential workflows.


### Global workflow

The global workflow is well suited to running `Infer` **on all the files in a project**, e.g., for a Gradle-based project that compiles using the `gradle build` command:

``` bash
infer run -- gradle build
```

In general, running Infer on your project is as simple as running `infer run -- <your build command here>` where the build command is the one you would normally use to compile your source code.

To start a fresh analysis and be sure to analyze all the files in your project, you have to clean the build products, for instance with `make clean` for a make-based project, `gradle clean` for Gradle, etc.


### Differential workflow

Software projects such as mobile apps use incremental build systems, where code evolves as a sequence of code changes. For these projects, it can often make sense to analyze only the current changes in the project, instead of analyzing the whole project every time. It is possible to analyze only what's changed using Infer's reactive mode.

## Exploring Infer reports

You can get more information about the reports generated by Infer by running `infer explore` in the same directory. For instance

``` bash
infer run -- gradle build
infer explore
```

This tool allows you to see error traces leading to each bug reported by Infer, which can be helpful in tracking down the precise cause of each bug. See the output of `infer explore --help` for more information.


# Analyzing apps or projects

To analyze files with Infer you can use the compilers `javac` and `clang`. You can also use Infer with `gcc`, however, internally Infer will use `clang` to compile your code. **So, it may not work if your code does not compile with `clang`**.





# Examples

https://github.com/facebook/infer/tree/main/examples


# Q&A

## [any plans for bazel support ? #457](https://github.com/facebook/infer/issues/457)

No current plans. If you have experience building Bazel plugins and are interested in contributing one for Infer, that would be awesome!

## [Nothing to compile. Try running make clean first. #1167](https://github.com/facebook/infer/issues/1167)

Infer keys in on a small set of possible compiler commands. Cross compilers and versioned compilers (such as gcc-6.4.1) are not in its list and won't register as a compilation task to intercept and analyze.




# Refer

* https://github.com/facebook/infer


