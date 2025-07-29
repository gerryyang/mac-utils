---
layout: post
title:  "VS Code in Action"
date:   2021-04-09 08:00:00 +0800
categories: [VS Code,]
---

* Do not remove this line (it will not be displayed)
{:toc}

> Tips: Whenever you want to do something in VS Code and don't know how, it's a good idea to bring up the command palette with `CTRL+SHIFT+P` on Windows(`CMD+SHIFT+P` on mac), and try typing in a keyword for you want. Oftentimes the command will show up there so you don't have to go searching the net for how to do something.


VS Code [下载地址](https://code.visualstudio.com/)，当前版本：1.55 版本，2021-04

# Using C++ on Linux in VS Code

In this tutorial, you will configure Visual Studio Code to use the `GCC C++ compiler (g++)` and `GDB` debugger on Linux. GCC stands for GNU Compiler Collection; GDB is the GNU debugger.

* https://code.visualstudio.com/docs
* https://code.visualstudio.com/docs/cpp/config-linux
* https://gourav.io/blog/setup-vscode-to-run-debug-c-cpp-code

# Debugging

For debugging languages and runtimes (including `PHP`, `Ruby`, `Go`, `C#`, `Python`, `C++`, `PowerShell` and many others), look for **Debuggers extensions** in the VS Code Marketplace.

To bring up the Run view, select the Run icon in the Activity Bar on the side of VS Code. You can also use the keyboard shortcut `⇧⌘D`. The Run view displays all information related to running and debugging and has a top bar with debugging commands and configuration settings.

> 调试包含的主要部分：

* Start debugging (Launch Program, Attach Program)
* Debug actions
    + Continue / Pause `F5`
    + Step Over `F10`
    + Step Into `F11`
    + Step Out `⇧F11`
    + Restart `⇧⌘F5`
    + Stop `⇧F5`

* Debug side bar
* Debug console panel

If running and debugging is not yet configured (no `launch.json` has been created), VS Code shows the Run start view.

VS Code keeps debugging configuration information in a `launch.json` file located in a `.vscode` folder in your workspace (project root folder) or in your user settings or workspace settings.

To create a `launch.json` file, click the create a launch.json file link in the Run start view.

If you go back to the File Explorer view (`⇧⌘E`), you'll see that VS Code has created a `.vscode` folder and added the `launch.json` file to your workspace.

> Launch versus Attach:

In VS Code, there are two core debugging modes, `Launch` and `Attach`, which handle two different workflows and segments of developers.

VS Code debuggers typically support launching a program in debug mode or attaching to an already running program in debug mode. Depending on the request (attach or launch), different attributes are required, and VS Code's `launch.json` validation and suggestions should help with that.

To add a new configuration to an existing `launch.json`, use one of the following techniques:

* Use IntelliSense if your cursor is located inside the configurations array.
* Press the `Add Configuration` button to invoke snippet IntelliSense at the start of the array.
* Choose Add Configuration option in the Run menu.

In order to start a debug session, first select the configuration named **Launch Program** using the **Configuration dropdown** in the Run view. Once you have your launch configuration set, start your debug session with F5.

Alternatively you can run your configuration through the **Command Palette** (`⇧⌘P`), by filtering on **Debug: Select and Start Debugging** or typing `debug`, and selecting the configuration you want to debug.

As soon as a debugging session starts, the **DEBUG CONSOLE** panel is displayed and shows debugging output, and the Status Bar changes color (**orange** for default color themes):

> Breakpoints:

Breakpoints can be toggled by clicking on the **editor margin** or using **F9** on the current line. Finer breakpoint control (enable/disable/reapply) can be done in the Run view's BREAKPOINTS section.

## Debug C++ in Visual Studio Code

To configure debug configuration, Two files are required `launch.json` and `tasks.json` inside `.vscode` folder. VSCode can create and auto-configure these files if we try to debug for the first time.

### launch.json

```
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [

        {
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/a.out",
            "args": [],
            "stopAtEntry": true,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

More: https://code.visualstudio.com/docs/editor/debugging#_launchjson-attributes

### tasks.json

```
{
	"version": "2.0.0",
	"tasks": [
		{
			"type": "cppbuild",
			"label": "C/C++: g++ build active file",
			"command": "/usr/bin/g++",
			"args": [
				"-g",
				"${file}",
				"-o",
				"${fileDirname}/${fileBasenameNoExtension}"
			],
			"options": {
				"cwd": "${workspaceFolder}"
			},
			"problemMatcher": [
				"$gcc"
			],
			"group": "build",
			"detail": "compiler: /usr/bin/g++"
		}
	]
}
```


### follow-fork-mode

``` cpp
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    printf("pid(%d)\n", getpid());
    int pid = fork();
    if (0 != pid) {
        printf("pid(%d)\n", pid);
    }
    wait(NULL);
}
```

```
// launch.json
"setupCommands": [
    {"text": "-gdb-set follow-fork-mode child"}
]
```

* https://github.com/microsoft/vscode-cpptools/issues/511
* https://github.com/microsoft/vscode-cpptools/issues/1211



* https://code.visualstudio.com/docs/cpp/cpp-debug
* https://code.visualstudio.com/docs/editor/debugging

# VS Code Remote 配置

在VS Code的应用商店搜索`Remote Development`插件并安装。这是一个插件包，会同时安装`Remote Container`，`Remote SSH`，`Remote-WSL`几个组件。安装后VS Code侧边栏会多出一个`Remote Development`的图标。

## Remote-SSH

* 在本地`~/.ssh/`目录执行`ssh-keygen -t rsa -f vscode_id_rsa`命令生成公私钥。
* 将`公钥`复制到Remote机器的`~/.ssh/authorized_keys`文件中。
* VS Code和VS Code Server的通信走SSH通道，需要开启SSH端口转发。打开Remote机器上的`/etc/ssh/sshd_config`文件 ，将`AllowTcpForwarding`选项改成`yes`，然后重启sshd服务`service sshd restart`。
* 在本地`~/.ssh/`目录下添加或修改`config`配置。
```
# VS Code Remote Development
Host $ip_ssh_config
  User $user_name
  HostName $remote_ip
  Port $ssh_port
  IdentityFile ~/.ssh/vscode_id_rsa
```

* 执行`Remote-SSH`->`Remote-SSH: Connect to Host…`->`配置的Remote Host`，第一次连接默认会在Remote机器`~/.vscode-server`目录下安装`VS Code Server`。

```
# Remote Server
~/.vscode-server$du -csh *
109M    bin
56K     data
4.0K    extensions
109M    总用量
```

* 链接成功后，可以执行`Open folder...`打开Remote机器的Project查看和编辑代码，同时，可以打开`Remote-SSH`集成的`Terminal`查看和执行Remote服务器的相关操作。

# Shortcuts
## 通用快捷键（macOS）

* `shift + command + E`: 文件浏览（`⇧⌘E`）
* `shift + command + F`: 全局查找（`⇧⌘F`）
* `command + F`: 查找（在当前文件）
* `option + command + F`: 替换（在当前文件）
* `shift + command + P`: 运行命令（`⇧⌘P`）
* `shift + command + B`:  运行任务（`⇧⌘B`）
* `shift + command + X`: 在Marketplace搜索扩展（`⇧⌘X`）
* `control + ~`: 打开terminal
* `option`: 多行编辑
* `shift + command + D`: 打开Debugging的Run view（`⇧⌘D`）


## 代码快捷键

* `command + tick`:  查找跳转
* `control + -`/`shift + control + -`: 返回/前进
* `command + P`:
	+ 查找文件，显示最近打开的文件
	+ 当前文件行数（`:行号`）或函数（`@函数名`）跳转。其中，函数跳转等价于：`shift + command + o`
* `shift + u / l`: 大小写转化
* mac下，`command + k + 0`  折叠所有代码块；`command + k + j` 展开所有代码块。Windows下，`ctrl + k + 0` 折叠所有代码块；`ctrl + k + j` 展开所有代码块。0是代码折叠级别，同理可以换用1，2，3等
* `command + option + [` 折叠光标所在代码块；`command + option + ]` 展开光标所在代码块

More: https://code.visualstudio.com/docs/editor/codebasics

# 常用配置

## 标题显示完整的文件路径名

``` json
"window.title": "${dirty}${activeEditorLong}${separator}${rootName}${separator}${appName}"
```

## 自动删除文件行尾空格

``` json
"files.trimTrailingWhitespace": true
```

## 在explorer中设置不关心的文件

``` json
"files.exclude": {
    "*.log": true,
}
```

## 显示空格和tab符号

* 打开 setting，在搜索框中输入 renderControlCharacters，选中勾选框，即可显示tab
* 在搜索框中输入 renderWhitespace，选择 all，即可显示空格

# 常用插件

## clangd (C++)

比 C/C++ 扩展执行效率更高，但是 clangd 需要使用 GLIBC_2.18 版本。

```
"clangd.arguments": [
    "--clang-tidy",                 // 开启clang-tidy
    "--all-scopes-completion",      // 全代码库补全
    "--completion-style=detailed",  // 详细补全
    "--header-insertion=iwyu",
    "--pch-storage=disk",           // 如果内存够大可以关闭这个选项
    "--log=error",
    "--j=5",                        // 后台线程数，可根据机器配置自行调整
    "--background-index"
  ],
"clangd.path": "xxx", // 安装的clangd地址
"[cpp]": {
    "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd"
},
```


* https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd
* https://github.com/llvm/llvm-project/tree/main/clang-tools-extra/clangd#building-and-testing-clangd
* https://github.com/clangd/clangd/releases/tag/15.0.1
* https://clangd.llvm.org/installation.html

## Error Lens

Improve highlighting of errors, warnings and other language diagnostics.

Error Lens 插件用于将错误提示直接展示在代码后。

## ccls (C++)

C/C++/ObjC language server supporting cross references, hierarchies, completion and semantic highlight

## Vim

* 提供Vim快捷键操作

## Terminal

* 在编辑器中提供终端操作

## TabNine

* 智能的代码自动补全工具

## Kite Autocomplete

* Kite works for all major programming languages: Python, Java, Go, PHP, C/C#/C++, Javascript, HTML/CSS, Typescript, React, Ruby, Scala, Kotlin, Bash, Vue and React.

## Bracket Pair Colorizer2

* 括号通过不同颜色区分，多层级括号便于区分

> 注意：发现安装此插件后，输入汉字会卡顿，禁用后可恢复

##  Indenticator

* 代码层级对齐

##  GitLens — Git supercharged

* 分支管理，代码对比

## Git Graph

* View a Git Graph of your repository, and perform Git actions from the graph.

## Git (Submodule) Assistant

* VS Code extension that detects common git (and submodule) issues and helps to solve them.

## Git History

* View git log, file history, compare branches or commits.

## One Dark Pro

* VS Code主题，方便阅读代码
* 用法：`Code->Preferences->Color Theme`

## Peacock

* 设置每个VS Code窗口边框的颜色，打开多个VS Code时，便于快速找到想要的哪个窗口
* 用法：`>peacock …`

## Markdown All in One

* 提供Markdown文档的一些功能，比如自动生成/更新目录
* 用法：`>markdown …`

## Markdown Extended

* Extended syntaxes to built-in markdown & What you see is what you get exporter.
* Export to Self Contained HTML / PDF / PNG / JPEG

## Edit csv

* 阅读/编辑 csv 文件
* 用法：`>edit as csv`

## shellman

* shell script snippet，提供bash常用模版

## vscode-proto3

Protobuf 3 support for Visual Studio Code

## Clang-Format

Clang-Format is a tool to format C/C++/Java/JavaScript/Objective-C/Objective-C++/Protobuf code. It can be configured with a config file within the working folder or a parent folder. Configuration see: http://clang.llvm.org/docs/ClangFormatStyleOptions.html

## Format Files (批量格式化代码)

Visual Studio code format all files

Use the extension called Format Files.

Download the extension called ”Format Files” on VSCode.
Select and open the folder with files to format on VSCode.
Press Ctrl+Shift+P to open command palette.
Enter "Start Format Files: Workspace" and select this option.

## Switcher

Switch between related files (same name, different extensions) using keybindings, context menu or command palette.

`option+ o`: 实现`.h`和`.cc`之间的跳转。

## Mermaid Preview

The plugin enables live editing and visualization of [mermaid](https://mermaidjs.github.io/) supported diagrams.

## XML Tools

XML Formatting, XQuery, and XPath Tools for Visual Studio Code.

## Bash Beautify

Format / Beautify bash and shell scripts.


## CMake

CMake langage support for Visual Studio Code

## protobuf

这是有一个解析 google protobuf 提供提示 和 跳转的工具

## Kubernetes

Develop, deploy and debug Kubernetes applications

## Excalidraw (画图，素材丰富)

Draw schemas in VS Code using Excalidraw

This extension integrates Excalidraw into VS Code. To use it, create an empty file with a .excalidraw, .excalidraw.json, .excalidraw.svg or excalidraw.png extension and open it in Visual Studio Code.

Try the web version at : https://excalidraw.com/

Import Public Library

Check out the available libraries at libraries.excalidraw.com, and don't hesitate to contribute your own !

## Bazel

* Bazel
* bazel-stack-vscode


## Continue

The leading open-source AI code assistant


## Cppcheck Plug-in

A plug-in for Cppcheck, capable of checking folders or editor tabs, shows output in the output channel, severity options available.


# 有趣的扩展

## LeetCode

Solve LeetCode problems in VS Code.

## 韭菜盒子

查看股市。

## 小霸王

小霸王是一款基于vscode的nes游戏插件，能让你在紧张的开发之余在vscode里发松身心。通过劳逸结合，提升开发效率。

## Rainbow Fart

一个在你编程时疯狂语音夸你写得牛逼的扩展。

# Q&A

## Paste Not Working

解决方法：VIM插件问题，可尝试重新安装。

* [In Visual Studio Code Ctrl+V is not working](https://stackoverflow.com/questions/51521004/in-visual-studio-code-ctrlv-is-not-working)
* [Copy Paste Not Working #55303](https://github.com/microsoft/vscode/issues/55303)

## Tab to blank

* [How can I customize the tab-to-space conversion factor?](https://stackoverflow.com/questions/29972396/how-can-i-customize-the-tab-to-space-conversion-factor)

## [Can I run VS Code Server on older Linux distributions?](https://code.visualstudio.com/docs/remote/faq#_can-i-run-vs-code-server-on-older-linux-distributions)

Starting with VS Code release 1.86.1 (January 2024), the minimum requirements for the build toolchain of the remote server were raised. The prebuilt servers distributed by VS Code are compatible with Linux distributions based on glibc 2.28 or later, for example, Debian 10, RHEL 8, or Ubuntu 20.04. VS Code will still allow users to connect to an OS that is not supported by VS Code (OS that does not provide glibc >= 2.28 and libstdc++ >= 3.4.25) until February 2025. This allows time for you and your companies to migrate to newer Linux distributions. VS Code will show a dialog and banner message when you connect to an OS version that is not supported by VS Code.




# Refer

* https://code.visualstudio.com/docs/editor/codebasics





