#------------------------
# .bashrc, for Mac OS X configuration
# author: gerryyang
# update time: 2014-05-03
#------------------------

#------------------------
# set colorful terminal
#------------------------
export CLICOLOR=1
# \h:\W \u\$
export PS1='\[\033[01;33m\]\u@\h\[\033[01;31m\]:\W\$\[\033[00m\]'
#export PS1='\u@\h \W\$'
#export PS1='\[\e[0;31m\]\u@\h\[\e[0;33m\]:\[\e[1;34m\]\w\[\e[1;37m\]$\[\e[m\]'

# use Coreutils
if brew list | grep coreutils > /dev/null ; then
  PATH="$(brew --prefix coreutils)/libexec/gnubin:$PATH"
  alias ls='ls -F --show-control-chars --color=auto'
  eval `gdircolors -b $HOME/.dir_colors`
fi

#------------------------
# set shortcut key
#------------------------
# 搜索关键字高亮
alias grep='grep --color'
alias egrep='egrep --color'
alias fgrep='fgrep --color'

# 使用MacVim
alias vi='vim'
alias vim='mvim'
alias mvim='/Users/gerryyang/Downloads/tools/editor/MacVim-snapshot-66/mvim'

# 使用GCC4.8.1
alias g++481='/usr/local/gcc-4.8.1/bin/g++'
alias gcc490='/Users/gerryyang/LAMP/gcc/install/gcc-4.9.0/bin/gcc'
alias g++490='/Users/gerryyang/LAMP/gcc/install/gcc-4.9.0/bin/g++'

# 使用Sublime Text 2
alias subl='/Applications/Sublime\ Text\ 2.app/Contents/SharedSupport/bin/subl'

# Golang enviroment variable
export GOROOT=/usr/local/go
export PATH=$PATH:$GOROOT/bin
export GOPATH=/Users/gerryyang/code/go/mygo

# MySQL
export MYSQL_PATH=/Users/gerryyang/LAMP/MySQL/mysql-5.6.16-osx10.7-x86
export PATH=$PATH:$MYSQL_PATH/bin

# OpenSSL
export OPENSSL_PATH=/Users/gerryyang/LAMP/OpenSSL/install/openssl-1.0.1g
export PATH=$PATH:$OPENSSL_PATH/bin

