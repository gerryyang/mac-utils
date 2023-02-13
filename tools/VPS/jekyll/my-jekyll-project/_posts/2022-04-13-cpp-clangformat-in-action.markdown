---
layout: post
title:  "CPP ClangFormat in Action"
date:   2022-04-13 10:00:00 +0800
categories: [C/C++]
---

* Do not remove this line (it will not be displayed)
{:toc}


# 格式化工具 ClangFormat

Clang 有着非常模块化的设计，容易被其他工具复用其代码分析功能。LLVM 团队自己也提供一些工具，其中包括[ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)。

ClangFormat describes a set of tools that are built on top of LibFormat. It can support your workflow in a variety of ways including a standalone tool and editor integrations.

## Standalone Tool

clang-format is located in clang/tools/clang-format and can be used to format C/C++/Obj-C code.

```
$ clang-format -help
OVERVIEW: A tool to format C/C++/Obj-C code.
```

When the desired code formatting style is different from the available options, the style can be customized using the `-style="{key: value, ...}"` option or by putting your style configuration in the `.clang-format` or `_clang-format` file in your project’s directory and using `clang-format -style=file`.

An easy way to create the `.clang-format` file is:

```
clang-format -style=llvm -dump-config > .clang-format
```

Available style options are described in [Clang-Format Style Options](https://releases.llvm.org/3.4/tools/clang/docs/ClangFormatStyleOptions.html).


## editor integrations

* [Vim Integration](https://releases.llvm.org/3.4/tools/clang/docs/ClangFormat.html#vim-integration)

There is an integration for vim which lets you run the clang-format standalone tool on your current buffer, optionally selecting regions to reformat. The integration has the form of a python-file which can be found under `clang/tools/clang-format/clang-format.py`.

* [Visual Studio Integration](https://releases.llvm.org/3.4/tools/clang/docs/ClangFormat.html#visual-studio-integration)

Download the latest Visual Studio plugin from the [alpha build site](http://llvm.org/builds/). The default key-binding is Ctrl-R,Ctrl-F.

* [Script for patch reformatting](https://releases.llvm.org/3.4/tools/clang/docs/ClangFormat.html#script-for-patch-reformatting)

The python script `clang/tools/clang-format-diff.py` parses the output of a unified diff and reformats all contained lines with clang-format.

```
usage: clang-format-diff.py [-h] [-p P] [-style STYLE]

Reformat changed lines in diff.

optional arguments:
  -h, --help    show this help message and exit
  -p P          strip the smallest prefix containing P slashes
  -style STYLE  formatting style to apply (LLVM, Google, Chromium, Mozilla,
                WebKit)
```

So to reformat all the lines in the latest git commit, just do:

```
git diff -U0 HEAD^ | clang-format-diff.py -p1
```

The `-U0` will create a diff without context lines (the script would format those as well).


# 模版配置 `.clang-format`

The `.clang-format` file uses `YAML` format:

``` yaml
key1: value1
key2: value2
# A comment.
...
```

An example of a configuration file for multiple languages:

``` yaml
---
# We'll use defaults from the LLVM style, but with 4 columns indentation.
BasedOnStyle: LLVM
IndentWidth: 4
---
Language: Cpp
# Force pointers to the type for C++.
DerivePointerAlignment: false
PointerAlignment: Left
---
Language: JavaScript
# Use 100 columns for JS.
ColumnLimit: 100
---
Language: Proto
# Don't format .proto files.
DisableFormat: true
...
```

执行 clang-format -style=google -dump-config 命令的输出：

``` yaml
---
Language:        Cpp
# BasedOnStyle:  Google
AccessModifierOffset: -1
ConstructorInitializerIndentWidth: 4
AlignEscapedNewlinesLeft: true
AlignTrailingComments: true
AllowAllParametersOfDeclarationOnNextLine: true
AllowShortBlocksOnASingleLine: false
AllowShortIfStatementsOnASingleLine: true
AllowShortLoopsOnASingleLine: true
AllowShortFunctionsOnASingleLine: All
AlwaysBreakTemplateDeclarations: true
AlwaysBreakBeforeMultilineStrings: true
BreakBeforeBinaryOperators: false
BreakBeforeTernaryOperators: true
BreakConstructorInitializersBeforeComma: false
BinPackParameters: true
ColumnLimit:     80
ConstructorInitializerAllOnOneLineOrOnePerLine: true
DerivePointerAlignment: true
ExperimentalAutoDetectBinPacking: false
IndentCaseLabels: true
IndentWrappedFunctionNames: false
IndentFunctionDeclarationAfterType: false
MaxEmptyLinesToKeep: 1
KeepEmptyLinesAtTheStartOfBlocks: false
NamespaceIndentation: None
ObjCSpaceAfterProperty: false
ObjCSpaceBeforeProtocolList: false
PenaltyBreakBeforeFirstCallParameter: 1
PenaltyBreakComment: 300
PenaltyBreakString: 1000
PenaltyBreakFirstLessLess: 120
PenaltyExcessCharacter: 1000000
PenaltyReturnTypeOnItsOwnLine: 200
PointerAlignment: Left
SpacesBeforeTrailingComments: 2
Cpp11BracedListStyle: true
Standard:        Auto
IndentWidth:     2
TabWidth:        8
UseTab:          Never
BreakBeforeBraces: Attach
SpacesInParentheses: false
SpacesInAngles:  false
SpaceInEmptyParentheses: false
SpacesInCStyleCastParentheses: false
SpacesInContainerLiterals: true
SpaceBeforeAssignmentOperators: true
ContinuationIndentWidth: 4
CommentPragmas:  '^ IWYU pragma:'
ForEachMacros:   [ foreach, Q_FOREACH, BOOST_FOREACH ]
SpaceBeforeParens: ControlStatements
DisableFormat:   false
...
```

业务实际使用的配置可以参考`clang-format -style=google -dump-config`的默认配置，并在其基础上修改或增加新的选项。



# Disabling Formatting on a Piece of Code

Clang-format understands also special comments that switch formatting in a delimited range. The code between a comment `// clang-format off` or `/* clang-format off */` up to a comment `// clang-format on` or `/* clang-format on */` will not be formatted. The comments themselves will be formatted (aligned) normally.

``` cpp
int formatted_code;
// clang-format off
    void    unformatted_code  ;
// clang-format on
void formatted_code_again;
```

# Configurable Format Style Options

## Language

Language, this format style is targeted at.

Possible values:

* LK_None (in configuration: `None`) Do not use.
* LK_Cpp (in configuration: `Cpp`) Should be used for C, C++, ObjectiveC, ObjectiveC++.
* LK_Java (in configuration: `Java`) Should be used for Java.
* LK_JavaScript (in configuration: `JavaScript`) Should be used for JavaScript.
* LK_Proto (in configuration: `Proto`) Should be used for Protocol Buffers (https://developers.google.com/protocol-buffers/).


## BasedOnStyle (代码风格)

The style used for all options not specifically set in the configuration.

This option is supported only in the clang-format configuration (both within `-style='{...}'` and the `.clang-format` file).

Possible values:

* `LLVM` A style complying with the [LLVM coding standards](http://llvm.org/docs/CodingStandards.html)
* `Google` A style complying with [Google’s C++ style guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml)
* `Chromium` A style complying with [Chromium’s style guide](http://www.chromium.org/developers/coding-style)
* `Mozilla` A style complying with [Mozilla’s style guide](https://developer.mozilla.org/en-US/docs/Developer_Guide/Coding_Style)
* `WebKit` A style complying with [WebKit’s style guide](http://www.webkit.org/coding/coding-style.html)

## DisableFormat (启用开关)

Disables formatting completely.

## AlignTrailingComments (对齐注释)

If true, aligns trailing comments.

## BreakBeforeBraces (大括号换行规则)

The brace breaking style to use.

* BS_Attach (in configuration: `Attach`) Always attach braces to surrounding context.
* BS_Linux (in configuration: `Linux`) Like Attach, but break before braces on function, namespace and class definitions.
* BS_Mozilla (in configuration: `Mozilla`) Like Attach, but break before braces on enum, function, and record definitions.
* BS_Stroustrup (in configuration: `Stroustrup`) Like Attach, but break before function definitions, and ‘else’.
* BS_Allman (in configuration: `Allman`) Always break before braces.
* BS_GNU (in configuration: `GNU`) Always break before braces and add an extra level of indentation to braces of control statements, not to those of class, function or other definitions.

## ColumnLimit (每行的字符个数限制)

The column limit.

A column limit of `0` means that there is **no column limit**. In this case, clang-format will respect the input’s line breaking decisions within statements unless they contradict other rules.

## IndentWidth (缩进宽度)

The number of columns to use for indentation.

## TabWidth

The number of columns used for tab stops.

## UseTab (UseTabStyle)

The way to use tab characters in the resulting file.

Possible values:

* UT_Never (in configuration: `Never`) Never use tab.
* UT_ForIndentation (in configuration: `ForIndentation`) Use tabs only for indentation.
* UT_Always (in configuration: `Always`) Use tabs whenever we need to fill whitespace that spans at least from one tab stop to the next one.





# Q&A

## [Why do the PointerAlignment options not work?](https://stackoverflow.com/questions/56537847/why-do-the-pointeralignment-options-not-work)

通过 clang-format -style=google -dump-config 命令，可以看到 `DerivePointerAlignment: true`

The documentation says it

> If true, analyze the formatted file for the most common alignment of & and *. Pointer and reference alignment styles are going to be updated according to the preferences found in the file. **PointerAlignment is then used only as fallback**.

Which means one must explicitly set DerivePointerAlignment: false if one wants to handle it by oneself.





# Refer

* [Clang 3.4 documentation](https://releases.llvm.org/3.4/tools/clang/docs/ClangFormat.html)

* clang doc ver 3: https://releases.llvm.org/3.4/tools/clang/docs/ClangFormatStyleOptions.html
* clang doc ver 12: https://clang.llvm.org/docs/ClangFormatStyleOptions.html







