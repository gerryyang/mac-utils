#------------------------
# .bashrc, for Mac OS X configuration
# author: gerryyang
# update time: 2014-08-10
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
alias grep='grep --color'
alias egrep='egrep --color'
alias fgrep='fgrep --color'

# MacVim
alias vi='vim'
alias vim='mvim'
alias mvim='/Users/gerryyang/Downloads/tools/editor/MacVim-snapshot-72/mvim'

# GCC4.8.1
alias g++481='/usr/local/gcc-4.8.1/bin/g++'
alias gcc490='/Users/gerryyang/LAMP/gcc/install/gcc-4.9.0/bin/gcc'
alias g++490='/Users/gerryyang/LAMP/gcc/install/gcc-4.9.0/bin/g++'

# Sublime Text 2
alias subl='/Applications/Sublime\ Text\ 2.app/Contents/SharedSupport/bin/subl'

# Golang enviroment variable
export GOROOT=/usr/local/go
export GOBIN=$GOROOT/bin
export PATH=$PATH:$GOBIN
#export GOPATH=/Users/gerryyang/code/go/mygo
export GOPATH=/Users/gerryyang/github_project/goinaction/programming_in_go

# MySQL
export MYSQL_PATH=/Users/gerryyang/LAMP/MySQL/mysql-5.6.16-osx10.7-x86
export PATH=$PATH:$MYSQL_PATH/bin

# OpenSSL
export OPENSSL_PATH=/Users/gerryyang/LAMP/OpenSSL/install/openssl-1.0.1g
export PATH=$PATH:$OPENSSL_PATH/bin

# Mou Markdown
export MOU_PATH=/Applications/Mou.app/Contents/MacOS
export PATH=$PATH:$MOU_PATH

# Protobuf
export PROTOBUF_PATH=/Users/gerryyang/LAMP/protobuf/install/protobuf-2.4.1
export PATH=$PATH:$PROTOBUF_PATH/bin

# pkg-config
export PKGCONFIG_PATH=/Users/gerryyang/LAMP/pkg-config/install/pkg-config-0.28
export PATH=$PATH:$PKGCONFIG_PATH/bin
export PKG_CONFIG_PATH=/Users/gerryyang/LAMP/protobuf/protobuf-2.4.1

# docker
export DOCKER_HOST=tcp://192.168.59.103:2376
export DOCKER_CERT_PATH=/Users/gerryyang/.boot2docker/certs/boot2docker-vm
export DOCKER_TLS_VERIFY=1




