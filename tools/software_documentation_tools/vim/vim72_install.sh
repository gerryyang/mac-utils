#!/bin/bash

echo "init..."
rm -rf vim72

echo "install..."
tar xvf vim-7.2.tar.bz2 
tar zxvf vim-7.2-lang.tar.gz
unzip vim_color_conf.zip
mv .vim ..

echo "make..."
mkdir -p install/vim72
cd vim72
./configure --prefix=/data/home/gerryyang/vim-7.2/install/vim72 --with-features=huge --disable-darwin --enable-perlinterp --enable-pythoninterp --enable-rubyinterp --enable-cscope --enable-multibyte --enable-hangulinput
make && make install

echo "install ok, and then add PATH to ur .bashrc to use vim72"


