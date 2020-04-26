

# Max OS X

## 系统设置

通过Command + `,`，打开默认配置。

例如：

```
{
	"font_size": 11,

	"ignored_packages":  // 使用VIM模式
	[
	],

	"word_wrap": true    // 文本自动换行显示
}

```

## Package Control

* 首先需要[安装Package Control](https://packagecontrol.io/installation)
* 使用 Package Control 安装 Package 的方法
	+ 按下 `Shift + Command + P` 调出命令面板（Package Control）
	+ 输入 `Install`，按下回车，然后输入插件名称，进行安装
	+ 输入 `Remove`，所有显示的插件都可以删除
	+ 详细的Package列表可以在 https://sublime.wbond.net/ 查询


## 常用快捷键

| 功能 | 快捷键 | 作用
| -- | -- | --
| 编辑功能 | 
| | Command + D | 反复按下实现将所有相同的关键字加入选择
| | Control + Command + G | 选择所有相同的关键字，并同时进行编辑
| | Command + 鼠标点击不同的位置 | 对不同位置进行选择和编辑
| | Command + ⇧ + Space | 基于范围的选择，重复使用会扩大范围
| | Control + Command + ↑ or ↓ | 将当前行向上移动或向下移动
| | Command + ⇧ + D | 对当前行或所选择的内容，进行复制
| | Command + [ or ] | 向前缩进，或向后缩进
| | Control + ⇧ + W | 对所选内容自动生成配对的tag
| | Command + / | 对当前行或所选内容注释
| | Command + K then U | 转为大写
| | Command + K then L | 转为小写
| UI界面显示功能 | 
| | Command + K; Command + B | 实现对侧边栏文件列表的打开和关闭显示
| | Control + Command + F | 全屏模式
| | Command + `=` 或者 Command + `-` | 字体大小动态调整
| | Command + Option + 数字 | 实现多个tab并行编辑和对比
| | Control + \` | 显示和隐藏控制台


## 常用插件

| 插件名称 | 用途 | 获取地址
| -- | -- | --
| MarkdownPreview | Markdown文档预览 | https://packagecontrol.io/packages/MarkdownPreview https://facelessuser.github.io/MarkdownPreview/usage/
| AlignTab | 对齐，The most flexible alignment plugin for Sublime Text 3. This plugin is inspired by the excellent VIM plugin, [tabular](https://github.com/godlygeek/tabular) | https://github.com/randy3k/AlignTab
| Sublime​AStyle​Formatter | C/C++/C#/Java code formatter/beautifier with AStyle, For OSX, control + option + f to format current file | https://packagecontrol.io/packages/SublimeAStyleFormatter
| GoSublime (x) | GoLang配置 | https://packagecontrol.io/packages/GoSublime
| ConvertToUTF8 | 字符编码转换 | https://packagecontrol.io/packages/ConvertToUTF8
| SyncedSideBar | | https://packagecontrol.io/packages/SyncedSideBar
| URLEncode | url编解码 | https://packagecontrol.io/packages/URLEncode


* AlignTab 使用说明

安装完AlignTab后，对选择的内容在右键菜单中可以进行常见的格式化，或者根据自己的需求将常用的格式化命令配置为快捷键。(Custom User Bindings for using AlignTab with ='s, =>'s, and :'s in Sublime Text
)

"Preferences" -> "Key Bindings"，配置以下快捷键，之后即可通过`super+ctrl+=`来格式化`=`的语句。

```
[
  {
    "keys": ["super+ctrl+;"], "command": "align_tab",
    "args" : {
      "user_input" : ":/f"
    }
  },
  {
    "keys": ["super+ctrl+="], "command": "align_tab",
    "args" : {
      "user_input" : "=/f"
    }
  },
  {
    "keys": ["super+ctrl+."], "command": "align_tab",
    "args" : {
      "user_input" : "=>/f"
    }
  }
]
```


# Windows

| 功能 | 快捷键 | 作用
| -- | -- | --
| 编辑功能 |
| | Ctrl + D | 反复按下实现将所有相同的关键字加入选择
| | Alt + F3 | 选择所有相同的关键字，并同时进行编辑
| | Ctrl + 鼠标点击不同的位置 | 对不同位置进行选择和编辑
| | ctrl + K, ctrl + 1 | 折叠所有函数
| | ctrl + K, ctrl + J | 展开所有函数
| UI界面显示功能 |
| | Ctrl + K; Ctrl + B | 实现对侧边栏文件列表的打开和关闭显示
| | F11 | 全屏模式
| | Ctrl + `=` 或者 Ctrl + `-` | 字体大小动态调整
| | Alt + Shift + 数字 | 实现多个tab并行编辑和对比



# Refer

1. [GoLang及Sublime Text 2之Mac OS X 10.8.4开发环境安装](http://blog.csdn.net/delphiwcdj/article/details/11873023)
2. [快乐的sublime编辑器](http://www.imooc.com/video/6488)
3. [My Overused Sublime Text Keyboard Shortcuts](http://viget.com/extend/my-overused-sublime-text-keyboard-shortcuts)




