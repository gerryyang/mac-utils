
Abstract
===
*What is Markdown?*

[Markdown](http://daringfireball.net/projects/markdown/) is a way to style text on the web. You control the display of the document; formatting words as bold or italic, adding images, and creating lists are just a few of the things we can do with Markdown. Mostly, Markdown is just regular text with a few non-alphabetic characters thrown in, like `#` or `*`.


* [Gists](https://gist.github.com/)
* Comments in Issues and Pull Requests
* Files with the .md or .markdown extension

*特点*

* Lightweight
* Plain Text
* Easy-to-read, write
* Optionally render to HTML
* Popular format in GitHub, StackExchange


Tips
===

**(1) Paragraphs**

在Markdown中，Paragraphs之间需要使用一个或多个blank lines分隔。

**(2) Headings**

You can create a heading by adding one or more # symbols before your heading text. The number of # you use will determine the size of the heading. For example,

**(3) Blockquotes**

You can indicate blockquotes with a >

**(4) Styling text**

You can make text **bold** or *italic*

> *This text will be italic*

> **This text will be bold**

Both **bold** and *italic* can use either a * or an _ around the text for styling. This allows you to combine both bold and italic if needed.

> **Everyone _must_ attend the meeting at 5 o'clock today.**


**(5) Lists**

Unorderde lists

You can make an unordered list by preceding list items with either a * or a -

* Item
* Item
* Item

Ordered lists

You can make an ordered list by preceding list items with a number

1. Item 1
2. Item 2
3. Item 3

`Nested lists`

You can create nested lists by indenting list items by two spaces.


**(6) Links**

You can create an inline link, by wrapping link text in brackets([]), and then wrapping the link in parenthesis(()).

For example, to create a hyperlink to www.github.com, with a link text that says, Visit GitHub!, you'd write this in Markdown: [Visit GitHub!](www.github.com).

**(7) Images**

![GitHub Logo](/images/logo.png)

Format: ![Alt Text](url)


**(8) Inline formats**

Use single backticks (`) to format text in a special monospace format. Everything within the backticks appear as-is, with no other special formatting.

Here's an idea: why don't we take `SuperiorProject` and turn it into `**Reasonable**Project`.

**(9) Multiple lines**

You can use triple backticks (```) to format text as its own distinct block.

Check out this neat program I wrote:

```
x = 0
x = 2 + 2
what is x
```

**(10) Tables**

You can create tables by assembling a list of words and dividing them with hyphens `-` (for the first row), and then separating each column with a pipe `|`:

First Header | Second Header
------------ | -------------:
Content from cell 1 | Content from cell 2
Content in the first column | Content in the second column

表格的分隔线的冒号，是对齐方式。冒号在左边是左对齐，右边右对齐，两边居中。


---

**Reference**

[1] [https://help.github.com/articles/markdown-basics](https://help.github.com/articles/markdown-basics)

[2] [https://guides.github.com/features/mastering-markdown/](https://guides.github.com/features/mastering-markdown/)

[3] [http://daringfireball.net/projects/markdown/syntax](http://daringfireball.net/projects/markdown/syntax)

[4] [http://www.appinn.com/markdown/#philosophy](http://www.appinn.com/markdown/#philosophy)

[5] [GitHub Flavored Markdown](https://github.com/guodongxiaren/README)

**Editors**

* Offline

	* Sublime Text with MarkdownEditing & Markdown Preview (install via Package Control)

* Online
	* [https://gist.github.com/](https://gist.github.com/) 
	* [https://stackedit.io/](https://stackedit.io/)(can sync with google doc)
	* [https://www.zybuluo.com/mdeditor](https://www.zybuluo.com/mdeditor)(Cmd Markdown, support LaTeX formula)
	* [https://raysnote.com/](https://raysnote.com/)(support LaTeX formula)
	* [http//maxiang.info/](http//maxiang.info/)(马克飞象，can sync with EverNote)

[1] [10款流行的Markdown编辑器，总有一款适合你](http://code.csdn.net/news/2819623)

[2] [Online Markdown](http://daringfireball.net/projects/markdown/dingus)

[3] [马克飞象-支持流程图，适合印象笔记用户](https://maxiang.io/)

[4] [Mou](http://25.io/mou/)

[5] [Cmd Markdown](https://www.zybuluo.com/mdeditor)

[6] [Markdown Plus-支持流程图](http://mdp.tylingsoft.com/)


**Other**

[1] [Markdown+Pandoc 最佳写作拍档](http://www.tuicool.com/articles/zQrQbaU)




