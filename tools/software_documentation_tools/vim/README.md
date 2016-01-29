Tips
===

> 记录Vi/Vim的一些用法。推荐一个印度男孩(Swaroop C H)写的《[byte of vim](http://www.swaroopch.com/notes/Vim/)》的manual。之前也总结过一些关于Vi/Vim的用法，可见[这里](http://blog.csdn.net/delphiwcdj/article/category/801838)。

###1 Move

在Vim中完全使用快捷键来实现光标的移动，下面是一些常用的*shortcut keys*：

**[0] 多个tab编辑**

:tabedit filename

**[1] 两个位置的切换**

``

**[2] 多个光标位置切换，Jump around**

You want to jump back to the previous location? Press ctrl-o

to jump forward to the next location? Press ctrl-i

**[3] 行首和行末**

^ key moves to the start of the line

$ key moves to the end of the line

**[4] 跳转到某行、行首、行末**

If you know the absolute line number that you want to jump to, say line 50, press 50G and Vim will jump to the 50th line. If no number is specified, G will take you to the last line of the file. How do you get to the top of the file? Simple, press 1G. （或者gg）

**[5] 上页和下页**

ctrl-b which means move one screen 'b'ackward

ctrl-f which means move one screen 'f'orward

**[6] 窗口内跳转，上、中、下**

What if you wanted to the middle of the text that  is currently being shown in the window?

Press H to jump as 'h'igh as possible (first line of the window)

Press M to jump to the 'm'iddle of the window

Press L to jump as 'l'ow as possible (last line being displayed)

**[7] Word, Sentences, Paragraphs的移动**

w, move to the next 'w'ord

e, move to the 'e'nd of the next word

b, move one word 'b'ackward

), move to the next sentence

(, move to the previous sentence

}, move to the next paragraph

{, move to the previous paragraph

**[8] 设定自己的mark，方便跳转（my favorite）**

Use ma to create a mark named 'a'. Move the cursor to wherever you want. Press 'a (i.e. single quote followed by the name of the mark) , Vim jumps (back) to the line where that mark was located.

You can use any alphabet (a-zA-Z) to name a mark which means you can have up to 26*2=52 named marks for each file. 


###2 plugins

**(1) tabular (代码对齐功能)**

对于代码洁癖的人这个功能是必须的。推荐使用[tabular](https://github.com/godlygeek/tabular)，可以将一些常用的对齐操作设置为快捷键添加在_vimrc配置中。

**(2) pathogen.vim (插件管理)**

[pathogen](https://github.com/tpope/vim-pathogen)

**(3) Molokai Color Scheme for Vim (配色)**

[Molokai](https://github.com/tomasr/molokai), 配色与Sublime的风格类似


*END*


