
"------------------------------
" MacVim configuration settings
" author: gerryyang
" update time: 2019-07-08
"------------------------------

" -------------
" Vundle
" https://github.com/gmarik/Vundle.vim
" -------------

set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
"set rtp+=~/.vim/bundle/Vundle.vim
"call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
"Plugin 'gmarik/Vundle.vim'

" The following are examples of different formats supported.
" Keep Plugin commands between vundle#begin/end.
" plugin on GitHub repo
""Plugin 'tpope/vim-fugitive'
" plugin from http://vim-scripts.org/vim/scripts.html
""Plugin 'L9'
" Git plugin not hosted on GitHub
""Plugin 'git://git.wincent.com/command-t.git'
" git repos on your local machine (i.e. when working on your own plugin)
""Plugin 'file:///home/gmarik/path/to/plugin'
" The sparkup vim script is in a subdirectory of this repo called vim.
" Pass the path to set the runtimepath properly.
""Plugin 'rstacruz/sparkup', {'rtp': 'vim/'}
" Avoid a name conflict with L9
""Plugin 'user/L9', {'name': 'newL9'}

" Install Vim-go
"Plugin 'fatih/vim-go'

" All of your Plugins must be added before the following line
"call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line

" -------------
" 基本设置
" -------------

" 启用vim高级特性(置于配置最前)
se nocompatible

" 设置文件编码
set encoding=utf-8
" for Linux
set fenc=utf-8
" for Windows
" set fenc=cp936
set fileencodings=ucs-bom,utf-8,cp936,gb2312,gb18030,big5

" 设置粘贴格式
set clipboard+=unnamed  " use the clipboards of vim and win
set paste               " Paste from a windows or from vim
set go+=a               " Visual selection automatically copied to the clipboard

" 显示行号
se nu

" 设置默认字体
se guifont=Monaco:h14

" 设置默认窗口大小
se lines=35 columns=99

" 设置当前行高亮, cursorline
se cul

" 将当前光标下的列高亮, cursorcolumn
" 对于Pythoner比较有用
"se cuc

" 显示光标位置
se ruler

" 记录上次打开的位置
if has("autocmd")
autocmd BufRead *.txt set tw=78
autocmd BufReadPost *
\ if line("'\"") > 0 && line ("'\"") <= line("$") |
\   exe "normal g'\"" |
\ endif
endif


" -------------
" 搜索设置
" -------------

" enable magic matching
set magic

" show matching bracets
set showmatch

" highlight search things
set hlsearch

" ignore case when searching
set smartcase
set ignorecase

" incremental match when searching
set incsearch

" -------------
" 代码设置
" -------------

" 设置语法高亮
syntax enable
syntax on

" 显示缩进tab线
se list lcs=tab:\|\ 

" 设置代码折叠
set foldenable
set foldnestmax=1
set foldmethod=syntax

" -------------
" 缩进设置
" -------------

" auto indent
set autoindent

" c indent
set cindent

" smart indet
set smartindent

" use hard tabs
set tabstop=4
" 将空格转换为tab, :%retab
set noexpandtab
set shiftwidth=4

" 自动换行
" break long lines
set textwidth=1000
" 设置某列高亮
"se cc=+1

" config C-indenting
set cinoptions=:0,l1,t0,g0

" enable filetype plugin
filetype plugin indent on
 
" use soft tabs for python
autocmd Filetype python set et sta ts=4 sw=4

" -------------
" 快捷键设置
" -------------

" format codes
map <F8> ggVG= <CR>

" open multiple files in tabs
map <F0> :tabedit <CR>

" ------ Tabular Plugin [beg] ------
" Tabular codes, '='
map <F1> :Tab/= <CR>
" Tabular codes, ','
map <F2> :Tab/,\zs <CR>
" Tabular codes, '<<'
map <F3> :Tab/<< <CR>

" it will call the :Tabularize command each time you insert a | character.
inoremap <silent> <Bar> <Bar><Esc>:call <SID>align()<CR>a
function! s:align()
        let p = '^\s*|\s.*\s|\s*$'
        "if exists(':Tabularize') && getline('.') =~# '^\s*|' && (getline(line('.')-1) =~# p || getline(line('.')+1) =~# p)
                let column = strlen(substitute(getline('.')[0:col('.')],'[^|]','','g'))
                let position = strlen(matchstr(getline('.')[0:col('.')],'.*|\s*\zs.*'))
                Tabularize/|/l1
                normal! 0
                call search(repeat('[^|]*|',column).'\s\{-\}'.repeat('.',position),'ce',line('.'))
        "endif
endfunction
" ------ Tabular Plugin [end] ------

" To insert timestamp, press F4
" 命令模式
nmap <F4> a<C-R>=strftime("%Y-%m-%d %a %I:%M %p")<CR><Esc>
" 插入模式
imap <F4> <C-R>=strftime("%Y-%m-%d %a %I:%M %p")<CR>

" To save, press ctrl-s.
nmap <c-s> :w<CR>
imap <c-s> <Esc>:w<CR>a



