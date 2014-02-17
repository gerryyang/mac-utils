
"------------------------------
" MacVim configuration settings
" author: gerryyang
" update time: 2014-02-17
"------------------------------

" -------------
" 基本设置
" -------------

" 启用vim高级特性(置于配置最前)
se nocompatible

" 设置文件编码
set encoding=utf-8
set fenc=cp936
set fileencodings=ucs-bom,utf-8,cp936,gb2312,gb18030,big5

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



