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


# 模版配置 .clang-format

业务实际使用的配置可以参考`clang-format -style=google -dump-config`的默认配置，并在其基础上修改或增加新的选项。



# Q&A

## [Why do the PointerAlignment options not work?](https://stackoverflow.com/questions/56537847/why-do-the-pointeralignment-options-not-work)

```
$clang-format -style=google -dump-config
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

`DerivePointerAlignment: true`

The documentation says it

> If true, analyze the formatted file for the most common alignment of & and *. Pointer and reference alignment styles are going to be updated according to the preferences found in the file. **PointerAlignment is then used only as fallback**.

Which means one must explicitly set DerivePointerAlignment: false if one wants to handle it by oneself.





# Refer

* [Clang 3.4 documentation](https://releases.llvm.org/3.4/tools/clang/docs/ClangFormat.html)

* clang doc ver 3: https://releases.llvm.org/3.4/tools/clang/docs/ClangFormatStyleOptions.html
* clang doc ver 12: https://clang.llvm.org/docs/ClangFormatStyleOptions.html




  

	
	