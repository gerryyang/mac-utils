---
layout: post
title:  "Go in Action"
date:   2019-04-14 10:00:00 +0800
categories: [GoLang]
---

* Do not remove this line (it will not be displayed)
{:toc}

本文主要是基于[The Go Programming Language]的读书摘要。之前写过几篇关于[Go的文章](https://blog.csdn.net/delphiwcdj/column/info/b-1-goinaction)和
一些实践[goinaction](https://github.com/gerryyang/goinaction)。

# 起源

![go_stem](/assets/images/201904/go_stem.jpg)

* Go形成构想是在2007年9月，并于`2009年11月`发布。其发明人是Robert Griesemer, Rob Pike和Ken Thompson，这几位都任职于Google。
* Go是个开源项目，所以其编译器，库和工具的源代码都可免费获取。
* Go的运行环境包括，`类UNIX系统` (Linux, FreeBSD, OpenBSD和Mac OS X)，还有`Plan 9`和`Microsoft Windows`。即，**一份程序可以跨平台运行**。
* Go是`编译型`的语言，其`工具链`将程序的`源文件`转变成机器相关的原生`二进制指令`，这些工具可以通过单一的`go命令`配合其`子命令`进行使用。
	- `go run helloworld.go` 将一个或多个`.go`为后缀的源文件进行编译，链接，并运行生成的可执行文件。
	- `go build helloworld.go` 编译，链接，生成二进制程序。
* Go原生支持`Unicode`，所以它可以处理所有国家的语言。
* Go平衡了`表达力`和`安全性`。Go程序通常比动态语言程序运行更快，同时遭遇意料之外的类型错误而导致的崩溃更少。
* Go语言从`祖先语言`中汲取的优点。(*混血儿*)
	- Go从**C语言**中继承了，`表达式语法`，`控制流语句`，`基本数据类型`，`按值调用的形参传递`，`指针`。但比这些更重要的是，继承了C所强调的`程序要编译成高效的机器码`，并自然地与所处的操作系统提供的抽象机制相配合。
	- Go从**Pascal**为发端的一个语言支流中
		+ Modula-2启发了`包`的概念。
		+ Oberon消除了模块接口文件和模块实现文件的差异。
		+ Oberon-2影响了包，导入和声明的语法，并提供了方法声明的语法。
	- Go借鉴了一些基于**CSP (Communicating Sequential Process, 通信顺序进程)**实现的语言
	- Go还有一些特性并非来自祖先的基因。例如，`iota`多多少少有点APL的影子；嵌套函数的词法作用域则来自Scheme。
	- Go还有自己一些**全新的特性**。例如，创新性的`切片`为动态数组提供了高效的随机访问的同时，也允许旧式链表的复杂共享机制；`defer`语句也是新引入的。

* **简单性才是好软件的不二法门，Go保持极端简单性的行为文化**。
	- 在高级语言中，Go出现的较晚，因而有一定后发优势。它的基础部分实现不错。
		+ 垃圾收集
		+ 包系统
		+ 一等函数
		+ 词法作用域
		+ 系统调用接口
		+ 默认用UTF-8编码的字符串
	- 但相对来说，语言特性不多，而且不会增加新特性了。(**Go成熟稳定，并且保证兼容更早版本**)
		+ 没有`隐式数值类型强制转换`
		+ 没有`构造或析构函数`
		+ 没有`运算符重载`
		+ 没有`形参默认值`
		+ 没有`继承`
		+ 没有`泛型`
		+ 没有`异常`
		+ 没有`宏`
		+ 没有`函数注记`
		+ 没有`线程局部存储`
	- Go的`类型系统`，足可以使程序员避免在动态语言中会无意犯下的绝大多数错误，但相对而言，它在带类型的语言中，又算是类型系统比较简单的。Go能为程序员提供，具备相当强类型的系统才能实现的安全性和运行时性能，而不让程序员承担这种系统的复杂性。
	- Go提倡充分利用当代计算机系统设计，尤其强调`局部性的重要`。
		+ 由于现代计算机都是并行工作的，Go有着基于CSP的并行特性。并提供了`变长栈`来运行其轻量级线程(`goroutine`)，这个栈初始化时非常小，所以创建一个goroutine成本极低，创建`100万`个也完全可以接受。
	- Go`标准库`常常被称为`语言自带电池`，它提供了清晰的构件，以及用于`I/O`，`文本处理`，`图像`，`加解密`，`网络`，`分布式应用`的API，而且对许多`标准文件格式和协议`都提供了支持。


# 测试环境

根据[The Go Programming Language Example Programs](https://github.com/adonovan/gopl.io/)，构建测试环境。

## 安装

三种安装方式：

* 源码安装。
* 标准包安装。
* 第三方工具安装。例如，Ubuntu的`apt-get`，Mac的`homebrew`。

```
$go version
go version go1.17.6 linux/amd64
```

## 环境变量

编辑 `~/.bashrc` 添加如下内容，之后执行 `source ~/.bashrc`。

``` bash
# go的安装路径
export GOROOT=/usr/local/go
export PATH=$GOROOT/bin:$PATH

# go的目标文件安装目录
export GOBIN=$HOME/go

# @refer https://learnku.com/go/t/39086#0b3da8
export GO111MODULE=on
```

refer: [Compile and install the application](https://go.dev/doc/tutorial/compile-install)

### GOPATH

* `GOPATH`允许多个目录(Linux下用冒号分割)，当`GOPATH`指定了**多个目录时**，默认将`go get`的内容放在**第一个目录**。
* `GOPATH`目录约定有3个子目录：
	- `src` (源代码，例如，`.go`, `.c`, `.h`, `.s`等)
	- `pkg` (编译后生成的文件，例如，.a)
	- `bin` (编译后生成的可执行文件)

[从 GOPATH 到 GO111MODULE](https://learnku.com/go/t/39086#0b3da8)

> 关于 GOPATH

当 Go 在 2009 年首次推出时，它并没有随包管理器一起提供。取而代之的是 `go get`，通过使用它们的导入路径来获取所有源并将其存储在 `$GOPATH/src` 中，没有版本控制并且 `master` 分支表示该软件包的稳定版本。

> 关于 Go Modules

Go 1.11 引入了 Go 模块。 `Go Modules` 不使用 `GOPATH` 存储每个软件包的单个 `git checkout`，而是存储带有 `go.mod` 的标记版本，并跟踪每个软件包的版本。

> 关于 GO111MODULE 环境变量

`GO111MODULE` 是一个环境变量，更改 Go 导入包的方式时进行设置。**根据 Go 版本，其语义会发生变化**。

由于 `GO111MODULE=on` 允许你选择一个行为。如果不使用 `Go Modules`, `go get` 将会从模块代码的 `master` 分支拉取，而若使用 `Go Modules` 则你可以利用 `Git Tag` 手动选择一个特定版本的模块代码。




## 编译和执行

通过`go get`命令获取源码，构建和安装。

```
$go get gopl.io/ch1/helloworld
```

注意：从 Go 1.17 版本开始，go get 已废弃，不建议再使用。

> Starting in Go 1.17, installing executables with go get is deprecated. go install may be used instead. In Go 1.18, go get will no longer build packages; it will only be used to add, update, or remove dependencies in go.mod. Specifically, go get will always act as if the -d flag were enabled.

> go help get
> The -d flag instructs get not to build or install packages. get will only update go.mod and download source code needed to build packages.

```
go get: installing executables with 'go get' in module mode is deprecated.
        Use 'go install pkg@version' instead.
        For more information, see https://golang.org/doc/go-get-install-deprecation
        or run 'go help get' or 'go help install'.
```


源码在`$(GOBIN)/src/gopl.io/ch1/helloworld/main.go`。


``` go
// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 1.

// Helloworld is our first Go program.
//!+
package main

import "fmt"

func main() {
        fmt.Println("Hello, 世界")
}

//!-
```

编译执行：

```
$go build .
$ls
helloworld main.go
$./helloworld
Hello, 世界
```

# Go入门示例

* Go代码是使用`包`来组织的，包类似其他语言中的库和模块。一个包由一个或多个`.go`源文件组成，放在`一个文件夹`中，该`文件夹的名字`描述了包的作用。
* 每一个源文件的开始都用`package`声明。例如，`package main`，指明了这个文件属于哪个包。后面跟着它导入的`其他包的列表`，然后是存储在文件中的`程序声明`。
* Go的`标准库`中有100多个包用来完成输入，输出，排序，文本处理等常见任务。
* 名为`main`的包用来定义一个独立的可执行程序，而不是库。
* 必须精确地导入需要的包，在缺失导入，或存在不需要的包的情况下，编译会失败。(这种严格的要求可以防止程序演化中引用不需要的包)
	- `goimports`工具可以按需管理导入声明的插入和移除。`go get golang.org/x/tools/cmd/goimports`
	- 包的匿名导入，`import ( _ "fmt")` 对于没有显式用到的 package 通过匿名导入的方式忽略检查。
* `import声明`必须跟在`package声明`之后。import导入声明后面，是组成程序的函数，变量，常量，类型声明。
* Go不需要在语句或声明后使用`分号结尾`，除非有多个语句或声明出现在同一行。事实上，跟在特定符号后面的换行符被转换为分号。在什么地方进行换行会影响对Go代码的解析。例如，`{`符号必须和关键字`func`在同一行，不能独自成行。
* Go对代码的`格式化`要求非常严格。`gofmt`工具将代码以标准格式重写。go工具的fmt子命令会用gofmt工具来格式化指定包里的所有文件，或者当前文件夹中的文件(默认情况下)。许多文本编辑器可以配置为在每次保存文件时自动运行gofmt，因此源文件总可以保持正确的形式。

## 命令行参数

* `os包`提供了一些函数和变量，以平台无关的方式和操作系统打交道。命令行参数以os包中的`Args`名字的变量供程序访问。变量`os.Args`是一个字符串`slice`。

``` go
// go get gopl.io/ch1/echo1
package main

import (
	"fmt"
	"os"
)

func main() {
	var s, sep string
	for i := 1; i < len(os.Args); i++ {
		s += sep + os.Args[i]
		sep = " "
	}
	fmt.Println(s)
}
```
* 注释以`\\`开头。习惯上，在一个包声明前，使用注释对其进行描述。
* `var`关键字声明了两个`string`类型的变量。变量可以在声明的时候初始化，如果变量没有明确地初始化，它将隐式地初始化为这个类型的空值。例如，对于数字初始化结果是0，对于字符串是空字符串。
* 对于数字，Go提供常规的算术和逻辑操作符；对于字符串，`+`操作符表示追加操作。
* `:=`符号用于`短变量声明`，这种语句声明一个或多个变量，并且根据初始化的值给予合适的类型。
* 递增语句`i++`对`i`进行加1，它等价于`i += 1`，又等价于`i = i + 1`。**注意，这些是语句，而不像其他C族语言一样是表达式，所以`j = i ++`是不合法的。并且，仅支持后缀，`++i`也不合法。**
* `for`是Go里面的`唯一循环语句`，它有几种形式。
	- for循环的三个组成部分两边不用`小括号`，但`大括号`是必需的，左大括号必须和`post`语句在同一行。
	- 可选的`initialization`语句在循环开始之前执行。如果存在，它必须是一个简单的语句。
	- `condition`是一个布尔表达式，在循环的每一次迭代开始前推演。
	- 三部分都可以省略。

``` go
for initialization; condition; post {
	// ...
}

// while循环
for condition {
	// ...
}

// 无限循环
for {
	// ...
	// break
	// return
}
```

* 另一种形式的**for循环**在`字符串`或`slice数据`上迭代。

``` go
// Echo2 prints its command-line arguments.
package main

import (
	"fmt"
	"os"
)

func main() {
	s, sep := "", ""
	for _, arg := range os.Args[1:] {
		s += sep + arg
		sep = " "
	}
	fmt.Println(s)
}
```

* 每一次迭代，`range`产生一对值：`索引`和`这个索引处元素的值`。
* 在这个例子中，不需要索引，但是语法上range循环需要处理，因此也必须处理索引。一个主意是将索引赋予一个`临时变量`然后忽略它。但是，**Go不允许存在无用的临时变量，不然会出现编译错误**。解决方案是使用空标识符`_`，**空标识符可以用在任何语法需要变量名但是程序逻辑不需要的地方**。例如，丢弃每次迭代产生的无用的索引。
* 这个版本使用`短的变量声明`来声明和初始化。原则：使用`显式的初始化`来说明初始化变量的重要性，使用`隐式的初始化`来表明初始化变量不重要。

``` go
// 以下几种声明字符串变量的方式是等价的
s := ""              // 此方式，更加简洁，通常在一个函数内部使用，不适合包级别的变量 (推荐)
var s string         // 默认初始化为空字符串 (推荐)
var s = ""           // 很少用
var s string = ""    // 显式的变量类型，在类型一致的情况下是冗余的信息，在类型不一致时是必需的
```

* 上面程序的问题：每次循环，字符串`s`有了新的内容，`+=`语句通过追加旧的字符串，空格字符，和下一个参数，生成一个新的字符串，然后把新字符串赋给`s`。旧的内容不再需要使用，会被`例行垃圾回收`。如果有大量的数据需要处理，这样的代价会比较大。(**TODO: 测试确认**)

* 一个高效的方式是使用`strings`包中的`Join`函数。

``` go
// Echo3 prints its command-line arguments.
package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	fmt.Println(strings.Join(os.Args[1:], " "))
}
```
* 如果关心格式只是想看下值或调试，那么使用`Println`格式化结果就可以了。

``` go
// 任何slice都能以这样的方式输出
fmt.Println(os.Args[1:])
```

## 找出重复行

* 用于文件复制，打印，检索，排序，统计的程序，通常有一个相似的结构：在输入接口上循环读取，然后对每一个元素进行一些计算。

``` go
// Dup1 prints the text of each line that appears more than
// once in the standard input, preceded by its count.
package main

import (
	"bufio"
	"fmt"
	"os"
)

func main() {
	counts := make(map[string]int)
	input := bufio.NewScanner(os.Stdin)
	for input.Scan() {
		counts[input.Text()]++
	}
	// NOTE: ignoring potential errors from input.Err()
	for line, n := range counts {
		if n > 1 {
			fmt.Printf("%d\t%s\n", n, line)
		}
	}
}
```
* 像`for`一样，`if`语句中的条件部分也不放在圆括号里，但是程序体中需要用到大括号。
* `map`存储一个`键/值对集合`，并且提供常量时间的操作来存储，获取，或测试集合中某个元素。内置的函数`make`可以用来新建map。
	- `键`可以是其值能够进行相等比较的任意类型。比如，字符串。
	- `值`可以是任意类型。
* 每次从输入读取一行内容，这一行就作为map中的键，对应的值递增1。
``` go
counts[input.Text()]++
// 等价于
line := input.Text()
counts[line] = counts[line] + 1
```
* `键`在map中不存在时也是没有问题的。当一个新的行第一次出现时，右边的表达式`counts[line]`根据值类型被推演为`零值`，int的零值是0。
* 为了输出结果，使用基于`range`的for循环，这次在map类型的counts变量上遍历。每次迭代输出两个结果，map里面一个元素对应的键和值。
* map里面的键的迭代顺序不是固定的，通常是随机地。每次运行都不一致，这是有意设计的，以防止程序依赖某种特定的序列，此处不对排序做任何保证。
* `bufio`包，可以简便和高效地处理输入和输出。其中一个最有用的特性是称为扫描器(Scanner)的类型，它可以读取输入，以`行`或者`单词`为单位断开，这是处理以行为单位的输入内容的最简单方式。扫描器从程序的标准输入进行读取。每一次调用`input.Scan()`读取下一行，并且将结尾的`换行符`去掉。通过`input.Text()`来获取读到的内容。`Scan()`函数在读到新行时候返回`true`，在没有更多内容的时候返回`false`。
* 函数`fmt.Printf`从一个`表达式列表`生成`格式化的输出`。`Printf`函数有超过10个这样的转义字符，Go称为`verb`。例如：

| verb | 描述
| -- | -- |
| %d | 十进制整数
| %x | 十六进制
| %o | 八进制
| %b | 二进制
| %f, %g, %e | 浮点数: 如，3.141593, 3.141592653589793, 3.141593e+00
| %t | 布尔型: true或false
| %c | 字符（Unicode码点）
| %s | 字符串
| %q | 带引号字符串或字符
| %v | 内置格式的任何值
| %T | 任何值的类型
| %% | 百分号本身

* 除了从`标准输入`读取，也可以从具体的`文件`读取。使用`os.Open`函数来逐个打开。

``` go
// Dup2 prints the count and text of lines that appear more than once
// in the input.  It reads from stdin or from a list of named files.
package main

import (
	"bufio"
	"fmt"
	"os"
)

func main() {
	counts := make(map[string]int)
	files := os.Args[1:]
	if len(files) == 0 {
		countLines(os.Stdin, counts)
	} else {
		for _, arg := range files {
			f, err := os.Open(arg)
			if err != nil {
				fmt.Fprintf(os.Stderr, "dup2: %v\n", err)
				continue
			}
			countLines(f, counts)
			f.Close()
		}
	}
	for line, n := range counts {
		if n > 1 {
			fmt.Printf("%d\t%s\n", n, line)
		}
	}
}

func countLines(f *os.File, counts map[string]int) {
	input := bufio.NewScanner(f)
	for input.Scan() {
		counts[input.Text()]++
	}
	// NOTE: ignoring potential errors from input.Err()
}
```
* 函数`os.Open`返回两个值，第一个是打开的文件`*os.File`；第二个是一个内置的`error`类型的值。
	- 如果`error`等于特殊的内置`nil`值，表示文件成功打开。文件在被读到结尾的时候，`Close`函数关闭文件，然后释放相应的资源。
	- 如果`error`不是`nil`，表示出错了。这时`error`的值描述错误原因。简单的错误处理是使用`Fprintf`和`%v`在标准错误流上输出一条消息，`%v`可以使用默认格式显示任意类型的值。错误处理后，开始处理下一个文件。`continue`语句让循环进入下一个迭代。

* `map`是一个使用`make`创建的数据结构的`引用`。当一个map传递给一个函数时，函数接收到这个引用的副本，所以，被调用函数中对于map数据结构中的改变，对函数调用者使用的map引用也是`可见的`。
* 第三种方式，是一次读取整个输入到大块内存中，一次性地分割所有行，然后处理这些行。使用`ReadFile`函数读取整个命名文件的内容，返回一个可以转化成字符串的字节`slice`，再用`string.Split`函数将一个字符串分割为一个由子串组成的`slice`。

``` go
// Dup3 prints the count and text of lines that
// appear more than once in the named input files.
package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"strings"
)

func main() {
	counts := make(map[string]int)
	for _, filename := range os.Args[1:] {
		data, err := ioutil.ReadFile(filename)
		if err != nil {
			fmt.Fprintf(os.Stderr, "dup3: %v\n", err)
			continue
		}
		for _, line := range strings.Split(string(data), "\n") {
			counts[line]++
		}
	}
	for line, n := range counts {
		if n > 1 {
			fmt.Printf("%d\t%s\n", n, line)
		}
	}
}
```
* 实际上，`bufio.Scanner`，`ioutil.ReadFile`以及`ioutil.WriteFile`使用`*os.File`中的`Read`和`Write`方法。但是大多数情况很少需要直接访问底层的函数，而是像`bufio`和`io/ioutil`包中的**上层的方法**更易使用。

## GIF动画

* Go标准的图像包的使用，来创建一系列的位图图像，然后将位图序列编码为GIF动画。
* 在导入由多段路径，如`image/color`组成的包之后，使用路径最后的一段来引用这个包。所以，`color.White`属于`image/color`包。
* `const`声明用来给常量命名。const声明可以出现在包级别，或在一个函数内。常量必须是数字，字符串或布尔值。
* 表达式`[]color.Color{...}`是`复合字面量`，即用一系列元素的值初始化Go的复合类型的紧凑表达方式。这里，第一个是`slice`，第二个是`结构体`。
* `gif.GIF`是一个结构体类型。结构体由一组称为`字段`的值组成，字段通常有不同的数据类型，它们一起组成单个对象，作为一个单位被对待。`anim`变量是`gif.GIF`结构体类型，这个结构体字面量创建一个结构体`LoopCount`，其值设置为`nframes`，其他字段的值是对应类型的**零值**。结构体的每个字段可以通过**点记法**来访问。

``` go
// Lissajous generates GIF animations of random Lissajous figures.
package main

import (
	"image"
	"image/color"
	"image/gif"
	"io"
	"math"
	"math/rand"
	"os"
)

//!-main
// Packages not needed by version in book.
import (
	"log"
	"net/http"
	"time"
)

//!+main

var palette = []color.Color{color.White, color.Black}

const (
	whiteIndex = 0 // first color in palette
	blackIndex = 1 // next color in palette
)

func main() {
	//!-main
	// The sequence of images is deterministic unless we seed
	// the pseudo-random number generator using the current time.
	// Thanks to Randall McPherson for pointing out the omission.
	rand.Seed(time.Now().UTC().UnixNano())

	if len(os.Args) > 1 && os.Args[1] == "web" {
		//!+http
		handler := func(w http.ResponseWriter, r *http.Request) {
			lissajous(w)
		}
		http.HandleFunc("/", handler)
		//!-http
		log.Fatal(http.ListenAndServe("localhost:8000", nil))
		return
	}
	//!+main
	lissajous(os.Stdout)
}

func lissajous(out io.Writer) {
	const (
		cycles  = 5     // number of complete x oscillator revolutions
		res     = 0.001 // angular resolution
		size    = 100   // image canvas covers [-size..+size]
		nframes = 64    // number of animation frames
		delay   = 8     // delay between frames in 10ms units
	)
	freq := rand.Float64() * 3.0 // relative frequency of y oscillator
	anim := gif.GIF{LoopCount: nframes}
	phase := 0.0 // phase difference
	for i := 0; i < nframes; i++ {
		rect := image.Rect(0, 0, 2*size+1, 2*size+1)
		img := image.NewPaletted(rect, palette)
		for t := 0.0; t < cycles*2*math.Pi; t += res {
			x := math.Sin(t)
			y := math.Sin(t*freq + phase)
			img.SetColorIndex(size+int(x*size+0.5), size+int(y*size+0.5),
				blackIndex)
		}
		phase += 0.1
		anim.Delay = append(anim.Delay, delay)
		anim.Image = append(anim.Image, img)
	}
	gif.EncodeAll(out, &anim) // NOTE: ignoring encoding errors
}
```

## 获取一个URL

* 从互联网获取信息。它获取每个指定URL的内容，然后不加解析的输出。类似`curl`这个工具。
* 使用两个包：`net/http`和`io/ioutil`。
* `http.Get`产生一个`HTTP`请求。如果没有错，返回结果存在响应结构`resp`里面。其中，`resp.Body`包含服务器端响应的一个可读取数据流，随后通过`ioutil.ReadAll`读取整个响应结果并存入`b`。

``` go
// Fetch prints the content found at each specified URL.
package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
)

func main() {
	for _, url := range os.Args[1:] {
		resp, err := http.Get(url)
		if err != nil {
			fmt.Fprintf(os.Stderr, "fetch: %v\n", err)
			os.Exit(1)
		}
		b, err := ioutil.ReadAll(resp.Body)
		resp.Body.Close()
		if err != nil {
			fmt.Fprintf(os.Stderr, "fetch: reading %s: %v\n", url, err)
			os.Exit(1)
		}
		fmt.Printf("%s", b)
	}
}
```

测试输出：

```
$./fetch http://gerryyang.com
<!DOCTYPE html>
<html lang="en"><head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1"><!-- Begin Jekyll SEO tag v2.7.1 -->
<title>Gerry’s blog | 他山之石，可以攻玉</title>

...
```

## 获取多个URL

* Go最大的特点是支持并发编程，本例并发获取多个URL内容，于是这个进程使用的时间不超过耗时最长时间的获取任务，而不是所有获取任务总的时间。这个版本的实现丢弃响应的内容，但是报告每一个响应的大小和花费的时间。
* `goroutine`是一个并发执行的函数。
* **通道是一种允许某一例程向另一个例程传递指定类型的值的通信机制**。
* `main`函数在一个`goroutine`中执行，然后`go`语句创建额外的`goroutine`。
* `main`函数使用`make`创建一个字符串通道，对于每一个命令行参数，go语句在第一轮循环中启动一个新的`goroutine`，它异步调用`fetch`来使用`http.Get`获取URL内容。
* `io.Copy`函数读取响应的内容，然后通过写入`ioutil.Discard`输出流进行丢弃。`Copy`返回字节数以及出现的任何错误。每一个结果返回时，`fetch`发送一行汇总信息到通道`ch`。
* `main`中的第二轮循环接收并输出那些汇总行。

> 注意：当一个`goroutine`试图在一个通道上进行发送或接收操作时，它会阻塞，直到另一个`goroutine`试图进行接收或发送操作，才传递值，并开始处理两个`goroutine`。在示例中，每一个`fetch`在通道`ch`上发送一个值（`ch <- expression`），main函数接收它们（`<-ch`），由main来处理所有的输出确保来每个`goroutine`作为一个整体单元处理，这样就避免了两个`goroutine`同时完成造成输出交织带来的风险。

``` go
// Fetchall fetches URLs in parallel and reports their times and sizes.
package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"net/http"
	"os"
	"time"
)

func main() {
	start := time.Now()
	ch := make(chan string)
	for _, url := range os.Args[1:] {
		go fetch(url, ch) // start a goroutine
	}
	for range os.Args[1:] {
		fmt.Println(<-ch) // receive from channel ch
	}
	fmt.Printf("%.2fs elapsed\n", time.Since(start).Seconds())
}

func fetch(url string, ch chan<- string) {
	start := time.Now()
	resp, err := http.Get(url)
	if err != nil {
		ch <- fmt.Sprint(err) // send to channel ch
		return
	}

	nbytes, err := io.Copy(ioutil.Discard, resp.Body)
	resp.Body.Close() // don't leak resources
	if err != nil {
		ch <- fmt.Sprintf("while reading %s: %v", url, err)
		return
	}
	secs := time.Since(start).Seconds()
	ch <- fmt.Sprintf("%.2fs  %7d  %s", secs, nbytes, url)
}
```

测试输出：

```
$./fetchall http://gerryyang.com https://godoc.org http://baidu.com
1.13s       81  http://baidu.com
1.29s    33353  http://gerryyang.com
2.40s    17406  https://godoc.org
2.40s elapsed
```

## 一个Web服务器

* 使用Go的库非常容易实现一个Web服务器，本例实现一个简单的Web服务，返回访问服务器的URL的路径部分。例如，如果请求的URL是`http://localhost:8000/hello`，则响应是`URL.Path = "/hello"`。
* `main`函数将一个处理函数和以`/`开头的URL链接在一起，代表所有的URL使用这个函数处理，然后启动服务器监听进入8000端口处的请求。
* 一个请求由一个`http.Request`类型的结构体表示，它包含很多关联的域，其中一个是所请求的URL。
* 当一个请求到达时，它被转交给处理函数，并从请求的URL中提取路径部分（`/hello`），使用`fmt.Printf`格式化，然后作为响应发送回去。


``` go
// Server1 is a minimal "echo" server.
package main

import (
	"fmt"
	"log"
	"net/http"
)

func main() {
	http.HandleFunc("/", handler) // each request calls handler
	log.Fatal(http.ListenAndServe("localhost:8000", nil))
}

// handler echoes the Path component of the requested URL.
func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "URL.Path = %q\n", r.URL.Path)
}
```

对上述功能进行扩展：

* 这个服务器有两个处理函数，通过请求的URL来决定哪一个被调用。请求`/count`调用`counter`，其他的调用`handler`。以`\`结尾的处理模式匹配所有含有这个前缀的URL。
* 对于传入的请求，服务器在不同的`goroutine`中运行该处理函数，这样它可以同时处理多个请求。然而，如果两个并发的请求试图同时更新计数值`count`，它可能会不一致地增加，程序会产生一个**严重的竞态bug**。为避免该问题，必现确保最多只有一个`goroutine`在同一时间访问变量，这正是`mu.Lock()`和`mu.Unlock()`语句的作用。


``` go
// Server2 is a minimal "echo" and counter server.
package main

import (
	"fmt"
	"log"
	"net/http"
	"sync"
)

var mu sync.Mutex
var count int

func main() {
	http.HandleFunc("/", handler)
	http.HandleFunc("/count", counter)
	log.Fatal(http.ListenAndServe("localhost:8000", nil))
}

// handler echoes the Path component of the requested URL.
func handler(w http.ResponseWriter, r *http.Request) {
	mu.Lock()
	count++
	mu.Unlock()
	fmt.Fprintf(w, "URL.Path = %q\n", r.URL.Path)
}

// counter echoes the number of calls so far.
func counter(w http.ResponseWriter, r *http.Request) {
	mu.Lock()
	fmt.Fprintf(w, "Count %d\n", count)
	mu.Unlock()
}
```

作为更完整的例子，处理函数可以报告它接收到的消息头和表单数据，这样可以方便服务器审查和调试请求：

* 注意，Go允许一个简单的语句（如一个局部变量声明）跟在`if`条件的前面，这在错误处理的时候特别有用，合并的语句更短而且可以缩小`err`变量的作用域，这是一个好实践。


``` go
// Server3 is an "echo" server that displays request parameters.
package main

import (
	"fmt"
	"log"
	"net/http"
)

func main() {
	http.HandleFunc("/", handler)
	log.Fatal(http.ListenAndServe("localhost:8000", nil))
}

//!+handler
// handler echoes the HTTP request.
func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "%s %s %s\n", r.Method, r.URL, r.Proto)
	for k, v := range r.Header {
		fmt.Fprintf(w, "Header[%q] = %q\n", k, v)
	}
	fmt.Fprintf(w, "Host = %q\n", r.Host)
	fmt.Fprintf(w, "RemoteAddr = %q\n", r.RemoteAddr)
	if err := r.ParseForm(); err != nil {
		log.Print(err)
	}
	for k, v := range r.Form {
		fmt.Fprintf(w, "Form[%q] = %q\n", k, v)
	}
}
```

## 其他内容


* 控制流。除了两个基础的控制语句`if`和`for`，还有一个支持多路分支的`switch`语句。
* 命名类型。`type`声明给已有类型命名。因为结构体类型通常很长，所以她们基本上都独立命名。

``` go
type Point struct {
	X, Y int
}
var p Point
```

* 指针。Go提供了指针，它的值是变量的地址。

> 区别：在C语言中，指针基本上是没有约束的。Go做了一个折中，指针显式可见。使用`&`操作符可以获取一个变量的地址，使用`*`操作符可以获取指针引用的变量值。但是指针不支持算术运算。

* 方法和接口。
	+ 一个关联了命名类型的函数称为**方法**。Go里面的方法可以关联到几乎所有的命名类型。
	+ **接口**可以用相同的方式处理不同的具体类型的抽象类型，它基于这些类型所包含的方法，而不是类型的描述或实现。

* 包。Go自带一个可扩展并且实用的标准库，Go社区创建和共享了更多的库。编程时，更多使用现有的包，而不是自己写所有的源码。
	+ 标准库包的索引：https://golang.org/pkg
	+ 社区贡献的包：https://godoc.org
	+ 使用`go doc`工具可以方便地通过命令行访问这些文档

``` go
$go doc http.ListenAndServe
go: finding golang.org/x/sys v0.0.0-20210423082822-04245dca01da
go: finding golang.org/x/term v0.0.0-20201126162022-7de9c90e9dd1
go: finding golang.org/x/tools v0.0.0-20180917221912-90fa682c2a6e
package http // import "net/http"

func ListenAndServe(addr string, handler Handler) error
    ListenAndServe listens on the TCP network address addr and then calls Serve
    with handler to handle requests on incoming connections. Accepted
    connections are configured to enable TCP keep-alives.

    The handler is typically nil, in which case the DefaultServeMux is used.

    ListenAndServe always returns a non-nil error.
```

* 注释。在声明任何函数前，写一段注释来说明它的行为是一个好的风格，这个约定很重要，因为它们可以被`go doc`和`godoc`工具定位和作为文档显示。对于跨越多行的注视，可以使用类似其他语言中的`/* .... */`注释。这样可以避免在文件的开始有一大块说明文本时每一行都有`//`。在注释内部，`//`和`/*`没有特殊的含义，所以注释不能嵌套。


# Go程序组成

声明，变量，新类型，包和文件，以及作用域。

数值，布尔量，字符串，常量，Unicode

组合类型 (数组，键值对，结构体，切片)

函数，错误处理，崩溃和恢复，以及defer语句

方法，接口，并发，包，测试，反射

面向对象设计 (Go没有类继承，没有类，较复杂的对象行为是通过较简单的对象组合，而非继承完成的)

并发处理，基于CSP思想，采用goroutine和信道实现，共享变量

# 复合数据类型

## JSON

* 把 Go 的数据结构（比如 movies）转换为 JSON 称为`marshal`（通过`json.Marshal`来实现的），Marshal 生成了一个字节 slice，其中包含一个不带有任何多余空白字符的很长的字符串。为了方便阅读，可以使用`json.MarshalIndent`输出整齐格式化的结果，这个函数有两个参数，一个是定义每行输出的前缀字符串，另外一个是定义缩进的字符串。
* `marshal`使用 Go 结构体成员的名称作为 JSON 对象里面字段的名称（通过反射的方式），只有可导出的成员（首字母大写）可以转换为 JSON 字段。
* 结构体成员 `Year` 对应地转换为 `released`，`Color`转换为`color`，这个是通过成员标签定义（`field tag`）实现的。成员标签定义是结构体成员在编译期间关联的一些元信息。标签值的第一部分指定了 Go 结构体成员对应 JSON 中字段的名字，`Color`标签还有一个额外的选项`omitempty`，它表示如果这个成员的值是零值或者为空，则不输出这个成员到 JSON 中。
* 将 JSON 字符串解码为 Go 数据结构，这个过程叫做`unmarshal`，这个是由`json.Unmarshal`实现的。通过合理地定义 Go 的数据结构，可以选择将哪部分 JSON 数据解码到结构对象中，哪些数据可以丢弃。例如，当函数`Unmarshal`调用完成后，它将填充结构体`slice`中`Title`的值，JSON 中其他的字段就丢弃了。

``` go
// Movie prints Movies as JSON.
package main

import (
	"encoding/json"
	"fmt"
	"log"
)

//!+
type Movie struct {
	Title  string
	Year   int  `json:"released"`
	Color  bool `json:"color,omitempty"`
	Actors []string
}

var movies = []Movie{
	{Title: "Casablanca", Year: 1942, Color: false,
		Actors: []string{"Humphrey Bogart", "Ingrid Bergman"}},
	{Title: "Cool Hand Luke", Year: 1967, Color: true,
		Actors: []string{"Paul Newman"}},
	{Title: "Bullitt", Year: 1968, Color: true,
		Actors: []string{"Steve McQueen", "Jacqueline Bisset"}},
	// ...
}

//!-

func main() {
	{
		//!+Marshal
		data, err := json.Marshal(movies)
		if err != nil {
			log.Fatalf("JSON marshaling failed: %s", err)
		}
		fmt.Printf("%s\n", data)
		//!-Marshal
	}

	{
		//!+MarshalIndent
		data, err := json.MarshalIndent(movies, "", "    ")
		if err != nil {
			log.Fatalf("JSON marshaling failed: %s", err)
		}
		fmt.Printf("%s\n", data)
		//!-MarshalIndent

		//!+Unmarshal
		var titles []struct{ Title string }
		if err := json.Unmarshal(data, &titles); err != nil {
			log.Fatalf("JSON unmarshaling failed: %s", err)
		}
		fmt.Println(titles) // "[{Casablanca} {Cool Hand Luke} {Bullitt}]"
		//!-Unmarshal
	}
}
```

输出：

```
$./movie
[{"Title":"Casablanca","released":1942,"Actors":["Humphrey Bogart","Ingrid Bergman"]},{"Title":"Cool Hand Luke","released":1967,"color":true,"Actors":["Paul Newman"]},{"Title":"Bullitt","released":1968,"color":true,"Actors":["Steve McQueen","Jacqueline Bisset"]}]
[
    {
        "Title": "Casablanca",
        "released": 1942,
        "Actors": [
            "Humphrey Bogart",
            "Ingrid Bergman"
        ]
    },
    {
        "Title": "Cool Hand Luke",
        "released": 1967,
        "color": true,
        "Actors": [
            "Paul Newman"
        ]
    },
    {
        "Title": "Bullitt",
        "released": 1968,
        "color": true,
        "Actors": [
            "Steve McQueen",
            "Jacqueline Bisset"
        ]
    }
]
[{Casablanca} {Cool Hand Luke} {Bullitt}]
```

## Github

* 由于用户的查询请求参数中可能存在一些字符，这些字符在 URL 中是特殊字符，比如`?`或者`&`，因此使用`url.QueryEscape`函数来确保它们拥有正确的含义。
* 这里使用流式解码器`json.Decoder`来依次从字节流里面解码出多个JSON实体。对应有一个`json.Encoder`的流式编码器。

```go
// github.go

// Package github provides a Go API for the GitHub issue tracker.
// See https://developer.github.com/v3/search/#search-issues.
package github

import "time"

const IssuesURL = "https://api.github.com/search/issues"

type IssuesSearchResult struct {
	TotalCount int `json:"total_count"`
	Items      []*Issue
}

type Issue struct {
	Number    int
	HTMLURL   string `json:"html_url"`
	Title     string
	State     string
	User      *User
	CreatedAt time.Time `json:"created_at"`
	Body      string    // in Markdown format
}

type User struct {
	Login   string
	HTMLURL string `json:"html_url"`
}
```


``` go
// search.go

package github

import (
	"encoding/json"
	"fmt"
	"net/http"
	"net/url"
	"strings"
)

// SearchIssues queries the GitHub issue tracker.
func SearchIssues(terms []string) (*IssuesSearchResult, error) {
	q := url.QueryEscape(strings.Join(terms, " "))
	resp, err := http.Get(IssuesURL + "?q=" + q)
	if err != nil {
		return nil, err
	}
	//!-
	// For long-term stability, instead of http.Get, use the
	// variant below which adds an HTTP request header indicating
	// that only version 3 of the GitHub API is acceptable.
	//
	//   req, err := http.NewRequest("GET", IssuesURL+"?q="+q, nil)
	//   if err != nil {
	//       return nil, err
	//   }
	//   req.Header.Set(
	//       "Accept", "application/vnd.github.v3.text-match+json")
	//   resp, err := http.DefaultClient.Do(req)
	//!+

	// We must close resp.Body on all execution paths.
	// (Chapter 5 presents 'defer', which makes this simpler.)
	if resp.StatusCode != http.StatusOK {
		resp.Body.Close()
		return nil, fmt.Errorf("search query failed: %s", resp.Status)
	}

	var result IssuesSearchResult
	if err := json.NewDecoder(resp.Body).Decode(&result); err != nil {
		resp.Body.Close()
		return nil, err
	}
	resp.Body.Close()
	return &result, nil
}
```

主程序：

``` go
// main.go

// Issues prints a table of GitHub issues matching the search terms.
package main

import (
	"fmt"
	"log"
	"os"

	"gopl.io/ch4/github"
)

//!+
func main() {
	result, err := github.SearchIssues(os.Args[1:])
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("%d issues:\n", result.TotalCount)
	for _, item := range result.Items {
		fmt.Printf("#%-5d %9.9s %.55s\n",
			item.Number, item.User.Login, item.Title)
	}
}

/*
$ go build gopl.io/ch4/issues
$ ./issues repo:golang/go is:open json decoder
13 issues:
#5680    eaigner encoding/json: set key converter on en/decoder
#6050  gopherbot encoding/json: provide tokenizer
#8658  gopherbot encoding/json: use bufio
#8462  kortschak encoding/json: UnmarshalText confuses json.Unmarshal
#5901        rsc encoding/json: allow override type marshaling
#9812  klauspost encoding/json: string tag not symmetric
#7872  extempora encoding/json: Encoder internally buffers full output
#9650    cespare encoding/json: Decoding gives errPhase when unmarshalin
#6716  gopherbot encoding/json: include field name in unmarshal error me
#6901  lukescott encoding/json, encoding/xml: option to treat unknown fi
#6384    joeshaw encoding/json: encode precise floating point integers u
#6647    btracey x/tools/cmd/godoc: display type kind of each named type
#4237  gjemiller encoding/base64: URLEncoding padding is optional
*/
```

## HTML模版

* 当要求格式和代码彻底分离，这个额可以通过`text/template`和`html/template`里面的方法实现。这两个包提供了一种机制，可以将程序变量的值代入到文本或者HTML模版中。
* 模版，是一个字符串或文件，它包含一个或者多个，两边用双大括号包围的单元{% raw %}`{{...}}`{% endraw %}，这个称为**操作**。每个操作在模版语言里都对应一个表达式，提供的功能包括：输出值，选择结构体成员，调用函数和方法，描述控制逻辑，实例化其他的模版等。
* 通过模版输出结果需要两个步骤
	+ 需要解析模版并转换为内部的表示方法（解析模版只需要执行一次）
	+ 然后在指定的输入上面执行
* 创建并解析定义的文本模版`templ`，注意方法的链式调用：`template.New`创建并返回一个新的模版，`Funcs`添加`daysAgo`到模版内部可以访问的函数列表中，然后返回这个模版对象；最后调用`Parse`方法。
* 更多：`go doc text/template` `go doc html/template`


{% raw %}
``` go
// Issuesreport prints a report of issues matching the search terms.
package main

import (
	"log"
	"os"
	"text/template"
	"time"

	"gopl.io/ch4/github"
)

//!+template
const templ = `{{.TotalCount}} issues:
{{range .Items}}----------------------------------------
Number: {{.Number}}
User:   {{.User.Login}}
Title:  {{.Title | printf "%.64s"}}
Age:    {{.CreatedAt | daysAgo}} days
{{end}}`

//!-template

//!+daysAgo
func daysAgo(t time.Time) int {
	return int(time.Since(t).Hours() / 24)
}

//!-daysAgo

//!+exec
var report = template.Must(template.New("issuelist").
	Funcs(template.FuncMap{"daysAgo": daysAgo}).
	Parse(templ))

func main() {
	result, err := github.SearchIssues(os.Args[1:])
	if err != nil {
		log.Fatal(err)
	}
	if err := report.Execute(os.Stdout, result); err != nil {
		log.Fatal(err)
	}
}

//!-exec

func noMust() {
	//!+parse
	report, err := template.New("report").
		Funcs(template.FuncMap{"daysAgo": daysAgo}).
		Parse(templ)
	if err != nil {
		log.Fatal(err)
	}
	//!-parse
	result, err := github.SearchIssues(os.Args[1:])
	if err != nil {
		log.Fatal(err)
	}
	if err := report.Execute(os.Stdout, result); err != nil {
		log.Fatal(err)
	}
}
/*
//!+output
$ go build gopl.io/ch4/issuesreport
$ ./issuesreport repo:golang/go is:open json decoder
13 issues:
----------------------------------------
Number: 5680
User:   eaigner
Title:  encoding/json: set key converter on en/decoder
Age:    750 days
----------------------------------------
Number: 6050
User:   gopherbot
Title:  encoding/json: provide tokenizer
Age:    695 days
----------------------------------------
...
//!-output
*/
```
{% endraw %}


`html/template`包和`text/template`包里面使用一样的API和表达式语句。

{% raw %}
``` go
// Issueshtml prints an HTML table of issues matching the search terms.
package main

import (
	"log"
	"os"

	"gopl.io/ch4/github"
)

//!+template
import "html/template"

var issueList = template.Must(template.New("issuelist").Parse(`
<h1>{{.TotalCount}} issues</h1>
<table>
<tr style='text-align: left'>
  <th>#</th>
  <th>State</th>
  <th>User</th>
  <th>Title</th>
</tr>
{{range .Items}}
<tr>
  <td><a href='{{.HTMLURL}}'>{{.Number}}</a></td>
  <td>{{.State}}</td>
  <td><a href='{{.User.HTMLURL}}'>{{.User.Login}}</a></td>
  <td><a href='{{.HTMLURL}}'>{{.Title}}</a></td>
</tr>
{{end}}
</table>
`))

//!-template

//!+
func main() {
	result, err := github.SearchIssues(os.Args[1:])
	if err != nil {
		log.Fatal(err)
	}
	if err := issueList.Execute(os.Stdout, result); err != nil {
		log.Fatal(err)
	}
}
```
{% endraw %}

./issueshtml repo:golang/go commenter:gopherbot json encoder > issues.html

``` html
<h1>65 issues</h1>
<table>
<tr style='text-align: left'>
  <th>#</th>
  <th>State</th>
  <th>User</th>
  <th>Title</th>
</tr>

<tr>
  <td><a href='https://github.com/golang/go/issues/7872'>7872</a></td>
  <td>open</td>
  <td><a href='https://github.com/extemporalgenome'>extemporalgenome</a></td>
  <td><a href='https://github.com/golang/go/issues/7872'>encoding/json: Encoder internally buffers full output</a></td>
</tr>

<tr>
  <td><a href='https://github.com/golang/go/issues/5901'>5901</a></td>
  <td>open</td>
  <td><a href='https://github.com/rsc'>rsc</a></td>
  <td><a href='https://github.com/golang/go/issues/5901'>encoding/json: allow per-Encoder/per-Decoder registration of marshal/unmarshal functions</a></td>
</tr>
```

![issueshtml](/assets/images/202202/issueshtml.png)

注意：`html/template`包自动将 HTML 元字符转义，这样显示才能正常。

{% raw %}
``` go
// Autoescape demonstrates automatic HTML escaping in html/template.
package main

import (
	"html/template"
	"log"
	"os"
)

//!+
func main() {
	const templ = `<p>A: {{.A}}</p><p>B: {{.B}}</p>`
	t := template.Must(template.New("escape").Parse(templ))
	var data struct {
		A string        // untrusted plain text
		B template.HTML // trusted HTML
	}
	data.A = "<b>Hello!</b>"
	data.B = "<b>Hello!</b>"
	if err := t.Execute(os.Stdout, data); err != nil {
		log.Fatal(err)
	}
}
```
{% endraw %}

输出：

```
$./autoescape
<p>A: &lt;b&gt;Hello!&lt;/b&gt;</p><p>B: <b>Hello!</b></p>
```


# 惯例用法

## Three Dots

[3 dots in 4 places](https://yourbasic.org/golang/three-dots-ellipsis/)

### Variadic function parameters

You can pass a slice `s` directly to **a variadic function** if you unpack it with the `s...` notation. In this case no new slice is created. In this example, we pass a slice to the Sum function.

``` go
primes := []int{2, 3, 5, 7}
fmt.Println(Sum(primes...)) // 17
```

### Array literals

In an array literal, the `...` notation specifies a length equal to the number of elements in the literal.

``` go
stooges := [...]string{"Moe", "Larry", "Curly"} // len(stooges) == 3
```

### The go command

Three dots are used by the go command as a **wildcard** when describing package lists. This command tests all packages in the current directory and its subdirectories.

``` bash
$ go test ./...
```

# 模版生成代码（go generate / ast）

通过`go generate`命令解析go的源代码，并生成新的代码文件。

例子：

* https://pkg.go.dev/golang.org/x/tools/cmd/stringer
* https://github.com/mohuishou/gen-const-msg

refer:

* https://lailin.xyz/post/41140.html?f=tt
* https://pkg.go.dev/go/ast




# Golang Runtime

TODO

# 各种函数的用法

https://blog.csdn.net/delphiwcdj/article/details/17611699

常用的几种函数用法，主要包括：

* 首先main是一个没有返回值的函数
* 普通函数
* 函数返回多个值
* 不定参函数
* 闭包函数
* 递归函数
* 类型方法, 类似C++中类的成员函数
* 接口和多态
* Defer 接口
* 错误处理, Panic/Recover

``` go
package main

import (
	"fmt"
	"math"
	"errors"
	"os"
	"io"
)


///
func max(a int, b int) int {
	if a > b {
		return a
	}
	return b
}

func multi_ret(key string) (int, bool) {
	m := map[string]int{"one":1, "two":2, "three":3}
	var err bool
	var val int
	val, err = m[key]
	return val, err
}

func sum(nums ...int) {
	fmt.Print(nums, " ")
	total := 0
	for _, num := range nums {
		total += num
	}
	fmt.Println(total)
}

func nextNum() func() int {
	i, j := 0, 1
	return func() int {
		var tmp = i + j
		i, j = j, tmp
		return tmp
	}
}

func fact(n int) int {
	if n == 0 {
		return 1
	}
	return n * fact(n-1)
}

// 长方形
type rect struct {
	width, height float64
}

func (r *rect) area() float64 {
	return r.width * r.height
}

func (r *rect) perimeter() float64 {
	return 2 * (r.width + r.height)
}

// 圆形
type circle struct {
	radius float64
}

func (c *circle) area() float64 {
	return math.Pi * c.radius * c.radius
}

func (c *circle) perimeter() float64 {
	return 2 * math.Pi * c.radius
}

// 接口
type shape interface {
	area() float64
	perimeter() float64
}

func interface_test() {
	r := rect{width: 2, height: 4}
	c := circle{radius: 4.3}

	// 通过指针实现
	s := []shape{&r, &c}

	for _, sh := range s {
		fmt.Println(sh)
		fmt.Println(sh.area())
		fmt.Println(sh.perimeter())
	}
}

type myError struct {
	arg int
	errMsg string
}

// 实现error的Error()接口
func (e *myError) Error() string {
	return fmt.Sprintf("%d - %s", e.arg, e.errMsg)
}

func error_test(arg int) (int, error) {
	if arg < 0 {
		return -1, errors.New("Bad Arguments, negtive")
	} else if arg > 256 {
		return -1, &myError{arg, "Bad Arguments, too large"}
	}
	return arg * arg, nil
}

func CopyFile(dstName, srcName string) (written int64, err error) {
	src, err := os.Open(srcName)
	if err != nil {
		fmt.Println("Open failed")
		return
	}
	defer src.Close()

	dst, err := os.Create(dstName)
	if err != nil {
		fmt.Println("Create failed")
		return
	}
	defer dst.Close()

	// 注意dst在前面
	return io.Copy(dst, src)
}


//
func main() {

	// [0] 首先main是一个没有返回值的函数

	// [1] 普通函数
	fmt.Println(max(1, 100))

	// [2] 函数返回多个值
	v, e := multi_ret("one")
	fmt.Println(v, e)
	v, e = multi_ret("four")
	fmt.Println(v, e)
	// 典型的判断方法
	if v, e = multi_ret("five"); e {
		fmt.Println("OK")
	} else {
		fmt.Println("Error")
	}

	// [3] 不定参函数
	sum(1, 2)
	sum(2, 4, 5)
	nums := []int{1, 2, 3, 4, 5}
	sum(nums...)

	// [4] 闭包函数
	nextNumFunc := nextNum()
	for i := 0; i < 10; i++ {
		fmt.Println(nextNumFunc())
	}

	// [5] 递归函数
	fmt.Println(fact(4))

	// [6] 类型方法, 类似C++中类的成员函数
	r := rect{width: 10, height: 15}
	fmt.Println("area: ", r.area())
	fmt.Println("perimeter: ", r.perimeter())

	rp := &r
	fmt.Println("area: ", rp.area())
	fmt.Println("perimeter: ", rp.perimeter())

	// [7] 接口和多态
	interface_test()

	// [8] 错误处理, Error接口
	for _, val := range []int{-1, 4, 1000} {
		if r, e := error_test(val); e != nil {
			fmt.Printf("failed: %d:%s\n", r, e)
		} else {
			fmt.Println("success: ", r, e)
		}
	}

	// [9] 错误处理, Defer接口
	if w, err := CopyFile("/data/home/gerryyang/dst_data.tmp", "/data/home/gerryyang/src_data.tmp"); err != nil {
		fmt.Println("CopyFile failed: ", e)
	} else {
		fmt.Println("CopyFile success: ", w)
	}

	// 你猜下面会打印什么内容
	fmt.Println("beg ------------")
	for i := 0; i < 5; i++ {
		defer fmt.Printf("%d ", i)
	}
	fmt.Println("end ------------")

	// [10] 错误处理, Panic/Recover
	// 可参考相关资料, 此处省略

}
/*
output:
100
1 true
0 false
Error
[1 2] 3
[2 4 5] 11
[1 2 3 4 5] 15
1
2
3
5
8
13
21
34
55
89
24
area:  150
perimeter:  50
area:  150
perimeter:  50
&{2 4}
8
12
&{4.3}
58.088048164875275
27.01769682087222
failed: -1:Bad Arguments, negtive
success:  16 <nil>
failed: -1:1000 - Bad Arguments, too large
CopyFile success:  8
------------
------------
4 3 2 1 0
*/
```


# Tips

## [go -ldflags 信息注入](https://ms2008.github.io/2018/10/08/golang-build-version/)

## 构建时指定 -mod=vendor 的作用

在使用 Go 1.11 及以上版本的时候，Go 引入了 Go Modules 的特性，用于管理项目的依赖关系。在使用 Go Modules 的时候，可以通过在项目根目录下创建 go.mod 文件来指定项目的依赖关系。

在使用 Go Modules 的时候，可以通过 go build 命令来构建项目。如果项目依赖的包已经被下载到本地缓存中，go build 命令会自动使用本地缓存中的包。如果本地缓存中没有需要的包，go build 命令会从远程仓库中下载需要的包。

在使用 Go Modules 的时候，可以通过 -mod 参数来指定包的下载方式。其中，`-mod=vendor` **表示优先使用项目根目录下的 vendor 目录中的包**，如果 vendor 目录中没有需要的包，则从远程仓库中下载需要的包。

使用 `-mod=vendor` 的好处是可以将项目依赖的包保存在项目根目录下的 vendor 目录中，避免了依赖包的版本冲突和不稳定性。同时，也可以避免在构建项目时从远程仓库中下载依赖包，提高了构建的速度和稳定性。

需要注意的是，使用 `-mod=vendor` 的时候，需要在项目根目录下创建 vendor 目录，并将依赖的包复制到 vendor 目录中。可以使用 `go mod vendor` 命令来自动将依赖的包复制到 vendor 目录中。


## [Format errors in Go - %s %v or %w](https://stackoverflow.com/questions/61283248/format-errors-in-go-s-v-or-w)

**TL;DR;** Use `%w` in 99.99% of cases. In the other 0.001% of cases, `%v` and `%s` probably "should" behave the same, except when the error value is `nil`, but there are no guarantees.

Use `%v` for an error val.

``` go
if err != nil {
    return fmt.Errorf("pack %v: %v", name, err)
}
```

But, In `Go 1.13`, the `fmt.Errorf` function supports a new `%w` verb. When this verb is present, the error returned by `fmt.Errorf` will have an **Unwrap** method returning the argument of `%w`, which must be an error. In all other ways, `%w` is identical to `%v`.

``` go
if err != nil {
    // Return an error which unwraps to err.
    return fmt.Errorf("pack %v: %w", name, err)
}
```

Places where you need to differentiate between `%w` and `%v`:

Read comments in the codeblock

``` go
f, err := os.Open(filename)
if err != nil {
    // The *os.PathError returned by os.Open is an internal detail.
    // To avoid exposing it to the caller, repackage it as a new
    // error with the same text.
    //
    //
    // We use the %v formatting verb, since
    // %w would permit the caller to unwrap the original *os.PathError.
    return fmt.Errorf("%v", err)
}
```

Read: [For an error, when should I switch to w](https://github.com/golang/go/wiki/ErrorValueFAQ#i-am-already-using-fmterrorf-with-v-or-s-to-provide-context-for-an-error-when-should-i-switch-to-w)

Also, the built-in error interface allows Go programmers to add whatever information they desire. All it requires is a type that implements an `Error` method

Example:

``` go
type QueryError struct {
    Query string
    Err   error
}

func (e *QueryError) Error() string { return e.Query + ": " + e.Err.Error() }
```

So, mostly most examples have a similar type of implementation where `err` has an `Error` method which returns `string` for which you can use `%s`



# 标准库

## [exec](https://pkg.go.dev/os/exec)

Package `exec` runs external commands. It wraps os.StartProcess to make it easier to remap stdin and stdout, connect I/O with pipes, and do other adjustments.

`exec` 包是 Go 语言中用于运行外部命令的标准库。它封装了 `os.StartProcess` 函数，使得重定向标准输入输出、使用管道连接 I/O 等操作更加方便。

与其他语言中的 "system" 库调用不同，`os/exec` 包有意地不调用系统 shell，并且不会扩展任何通配符模式或处理其他扩展、管道或重定向，这通常是 shell 所做的。该包的行为更像 C 语言中的 "exec" 函数族。要扩展通配符模式，请直接调用 shell，注意转义任何危险的输入，或使用 `path/filepath` 包的 `Glob` 函数。要扩展环境变量，请使用 `os` 包的 `ExpandEnv` 函数。

https://pkg.go.dev/os/exec#Cmd.Start

``` golang
package main

import (
	"log"
	"os/exec"
)

func main() {
	cmd := exec.Command("sleep", "5")
	err := cmd.Start()
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("Waiting for command to finish...")
	err = cmd.Wait()
	log.Printf("Command finished with error: %v", err)
}
```


``` golang
package main

import (
	"context"
	"fmt"
	"os/exec"
	"time"
)

func main() {
	// 创建一个上下文对象，设置超时时间为 5 秒
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	// 创建一个命令对象
	cmd := exec.CommandContext(ctx, "ls", "-l")

	// 执行命令，并获取输出
	output, err := cmd.Output()

	// 判断命令是否执行成功
	if err != nil {
		fmt.Println("Command failed:", err)
		return
	}

	// 打印命令的输出
	fmt.Println(string(output))
}
```

## time.Duration

在 Go 语言中，`time.Duration` 类型表示两个时间点之间的时间差，单位为**纳秒**。`time.Duration` 的初始化可以通过以下几种方式完成：

``` go
// 直接使用整数，单位为纳秒
var duration time.Duration = 1000 // 1000纳秒


// 使用 time 包中的常量进行初始化
var duration time.Duration = 5 * time.Second // 5秒

// 使用 time.ParseDuration 函数解析一个时间段字符串
duration, err := time.ParseDuration("1h30m") // 1小时30分钟
if err != nil {
    log.Fatal(err)
}
```


# 代码检查

## golangci-lint

[golangci-lint](https://golangci-lint.run/) is a Go linters aggregator.

```
# binary will be $(go env GOPATH)/bin/golangci-lint
curl -sSfL https://raw.githubusercontent.com/golangci/golangci-lint/master/install.sh | sh -s -- -b $(go env GOPATH)/bin v1.52.2

golangci-lint --version
```

## [静态代码分析 Staticcheck](https://staticcheck.io/docs/)

Staticcheck is a state of the art linter for the Go programming language. Using static analysis, it finds bugs and performance issues, offers simplifications, and enforces style rules.

Each of the [150+](https://staticcheck.io/docs/checks/) checks has been designed to be fast, precise and useful. When Staticcheck flags code, you can be sure that it isn't wasting your time with unactionable warnings. Unlike many other linters, Staticcheck focuses on checks that produce few to no false positives. It's the ideal candidate for running in CI without risking spurious failures.

Staticcheck aims to be trivial to adopt. It behaves just like the official `go` tool and requires no learning to get started with. Just run `staticcheck ./...` on your code in addition to `go vet ./...` .

While checks have been designed to be useful out of the box, they still provide [configuration](https://staticcheck.io/docs/configuration/) where necessary, to fine-tune to your needs, without overwhelming you with hundreds of options.

Staticcheck can be used from the command line, in CI, and even [directly from your editor](https://github.com/golang/tools/blob/master/gopls/doc/settings.md#staticcheck-bool).


[Ignoring problems with linter directives](https://staticcheck.io/docs/configuration/#ignoring-problems)

* https://stackoverflow.com/questions/70208440/how-to-disable-golang-unused-function-error

``` go
//lint:ignore U1000 Ignore unused function temporarily for debugging
```


## 数据竞争检查 (go build -race)

go build -race 命令是 Go 语言工具链中的一个选项，用于启用数据竞争检测器。数据竞争是指两个或多个并发执行的线程访问同一个内存位置，其中至少一个线程执行写操作，而这些线程的执行顺序是不确定的。数据竞争可能导致程序行为不稳定和不可预测。

-race 选项在编译和链接阶段启用数据竞争检测器，它会在运行时检测数据竞争。当你使用 -race 选项构建 Go 程序时，程序会在运行时检测潜在的数据竞争问题。如果检测到数据竞争，程序会报告竞争条件并退出，同时返回非零退出状态。

> 请注意，启用数据竞争检测器会增加程序的运行时开销。因此，通常在开发和测试阶段使用 -race 选项来识别和修复数据竞争问题，而在生产环境中，不建议使用 -race 选项。

测试代码：

``` go
package main

import (
    "fmt"
    "sync"
)

var counter int

func main() {
    var wg sync.WaitGroup

    for i := 0; i < 1000; i++ {
        wg.Add(1)
        go func() {
            counter++
            wg.Done()
        }()
    }

    wg.Wait()
    fmt.Println("Counter:", counter)
}
```

go build -race main.go

测试输出：

```
./main
==================
WARNING: DATA RACE
Read at 0x000001216848 by goroutine 9:
  main.main.func1()
      /Users/gerry/Proj/github/goinaction/src/race_check/main.go:16 +0x32

Previous write at 0x000001216848 by goroutine 6:
  main.main.func1()
      /Users/gerry/Proj/github/goinaction/src/race_check/main.go:16 +0x4a

Goroutine 9 (running) created at:
  main.main()
      /Users/gerry/Proj/github/goinaction/src/race_check/main.go:15 +0x64

Goroutine 6 (running) created at:
  main.main()
      /Users/gerry/Proj/github/goinaction/src/race_check/main.go:15 +0x64
==================
Counter: 990
Found 1 data race(s)
```


# 问题调试 (delve)

生成 coredump 文件后，可以使用 [delve](https://github.com/derekparker/delve) 来调试 coredump 文件，delve 是一款专门为 Go 语言开发的调试器，它能够提供丰富的调试功能。源码地址 https://github.com/go-delve/delve/tree/master/Documentation/installation

服务启动前加上 `GOTRACEBACK=crash`，可以生成 corefile，和 `gdb` 类似，可以用 `dlv` 进行调试。

``` bash
ulimit -c unlimited
export GOTRACEBACK=crash
```

安装：

``` bash
go install github.com/go-delve/delve/cmd/dlv@latest
```

`GOTRACEBACK` 是一个环境变量，用于控制当 Go 程序崩溃时，运行时系统生成的调试信息的详细程度。这些调试信息通常包括堆栈跟踪（stack trace），帮助开发者定位问题。

GOTRACEBACK 的可选参数如下：

* none：不产生任何调试信息。
* single：只显示当前 goroutine的堆栈跟踪，如果没有设置 GOTRACEBACK 环境变量，将默认使用此选项。
* all：显示所有 goroutine 的堆栈跟踪。
* system：显示所有 goroutine 的堆栈跟踪，包括运行时系统的 goroutine。
* crash：与 system 类似，但在生成堆栈跟踪后，程序会通过调用操作系统的 crash 功能来终止，这对于生成核心转储文件（core dump）以便进一步分析非常有用。

调试 coredump 文件执行指令：

``` bash
dlv core your_program your_corefile --check-go-version=false
```

`--check-go-version=false` 是忽略 go 版本和 dlv 版本的区别，不然会报错。

* 执行 `goroutines` 或 `grs` 指令，查看执行的协程
* 使用命令 `goroutine $协程ID` 对当前的 goroutine 进行切换，然后 `bt` 查看堆栈信息
* 如果希望查看更多的堆栈帧，可以使用 `bt -full` ，它将显示完整的堆栈跟踪，包括函数参数和局部变量
* 如果只想查看特定堆栈帧的详细信息，可以使用 `frame` 命令，后跟堆栈帧的编号
* 使用 `grs` 或 `bt` 命令时如果需要翻页，可以使用 `les` 进行翻页操作


![dlv_debug](/assets/images/202403/dlv_debug.png)



# Tools

## golang 百科全书

https://awesome-go.com/

## golang developer roadmap

https://github.com/Alikhll/golang-developer-roadmap

## sql2go 工具

http://stming.cn/tool/sql2go.html

## toml2go 工具

https://xuri.me/toml-to-go/

## curl2go 工具

https://mholt.github.io/curl-to-go/

## json2go 工具

https://mholt.github.io/json-to-go/

## 泛型工具

https://github.com/cheekybits/genny

## QR Code encoder (二维码生成工具)

* 通过第三方服务生成。qrserver 提供的二维码生成服务，参考[API 文档](https://goqr.me/api/doc/create-qr-code/)

* 通过 https://github.com/skip2/go-qrcode 或 https://github.com/yeqown/go-qrcode 方案自己生成。

``` go
package main

import (
	"github.com/skip2/go-qrcode"
)

func main() {
	var url = "http://gerryyang.com"
	err := qrcode.WriteFile(url, qrcode.Medium, 256, "qr.png")
	if err != nil {
		panic(err)
	}
}
```

## Protocol Buffers go_package

* https://developers.google.com/protocol-buffers/docs/reference/go-generated#package
* [Correct format of protoc go_package?](https://stackoverflow.com/questions/61666805/correct-format-of-protoc-go-package)


# [Go Wiki: Go Code Review Comments](https://go.dev/wiki/CodeReviewComments)

This page collects common comments made during reviews of Go code, so that a single detailed explanation can be referred to by shorthands. This is a laundry list of common style issues, not a comprehensive style guide.

## Gofmt

Run [gofmt](https://pkg.go.dev/cmd/gofmt/) on your code to automatically fix the majority of mechanical style issues. Almost all Go code in the wild uses gofmt. The rest of this document addresses non-mechanical style points.

An alternative is to use [goimports](https://pkg.go.dev/golang.org/x/tools/cmd/goimports), a superset of gofmt which additionally adds (and removes) import lines as necessary.

## Comment Sentences

See https://go.dev/doc/effective_go#commentary. Comments documenting declarations should be full sentences, even if that seems a little redundant. This approach makes them format well when extracted into godoc documentation. Comments should begin with the name of the thing being described and end in a period:

``` golang
// Request represents a request to run a command.
type Request struct { ...

// Encode writes the JSON encoding of req to w.
func Encode(w io.Writer, req *Request) { ...
```

and so on.

## Contexts

Values of the context.Context type carry security credentials, tracing information, deadlines, and cancellation signals across API and process boundaries. Go programs pass Contexts explicitly along the entire function call chain from incoming RPCs and HTTP requests to outgoing requests.

Most functions that use a Context should accept it as their first parameter:

``` golang
func F(ctx context.Context, /* other arguments */) {}
```

A function that is never request-specific may use context.Background(), but err on the side of passing a Context even if you think you don’t need to. The default case is to pass a Context; only use context.Background() directly if you have a good reason why the alternative is a mistake.

Don’t add a Context member to a struct type; instead add a ctx parameter to each method on that type that needs to pass it along. The one exception is for methods whose signature must match an interface in the standard library or in a third party library.

Don’t create custom Context types or use interfaces other than Context in function signatures.

If you have application data to pass around, put it in a parameter, in the receiver, in globals, or, if it truly belongs there, in a Context value.

Contexts are immutable, so it’s fine to pass the same ctx to multiple calls that share the same deadline, cancellation signal, credentials, parent trace, etc.

## Copying

To avoid unexpected aliasing, be careful when copying a struct from another package. For example, the bytes.Buffer type contains a []byte slice. If you copy a Buffer, the slice in the copy may alias the array in the original, causing subsequent method calls to have surprising effects.

In general, do not copy a value of type T if its methods are associated with the pointer type, *T.

## Declaring Empty Slices

When declaring an empty slice, prefer

``` golang
var t []string
```

over

``` golang
t := []string{}
```

**The former declares a nil slice value, while the latter is non-nil but zero-length**. They are functionally equivalent—their len and cap are both zero—but the nil slice is the preferred style.

Note that there are limited circumstances where a non-nil but zero-length slice is preferred, such as when encoding JSON objects (a nil slice encodes to null, while []string{} encodes to the JSON array []).

When designing interfaces, avoid making a distinction between a nil slice and a non-nil, zero-length slice, as this can lead to subtle programming errors.

For more discussion about nil in Go see Francesc Campoy’s talk [Understanding Nil](https://www.youtube.com/watch?v=ynoY2xz-F8s).

-------


TODO





# Go Conference

https://github.com/gopherchina/conference

# Manual

* [Go语言圣经 《The Go Programming Language》 中文版本](https://books.studygolang.com/gopl-zh/)
* [Go 语言设计与实现](https://draveness.me/golang/)
* [Dave Cheney的golang博客]

# 开源代码

## https://github.com/urfave/cli

cli is a simple, fast, and fun package for building command line apps in Go. The goal is to enable developers to write fast and distributable command line applications in an expressive way.

## https://github.com/gammazero/deque

Fast ring-buffer deque ([double-ended queue](https://en.wikipedia.org/wiki/Double-ended_queue)) implementation. For a pictorial description, see the [Deque diagram](https://github.com/gammazero/deque/wiki)



# Q&A

* [DigitalOcean的How to code in go系列](https://www.digitalocean.com/community/tutorial_series/how-to-code-in-go)
* [Concurrency limiting goroutine pool](https://github.com/gammazero/workerpool)
* [High performance, minimalist Go web framework](https://github.com/labstack/echo)
* [Running periodic background tasks in Golang](https://medium.com/@mkfeuhrer/running-periodic-background-tasks-in-golang-8baa1af9a1f6)
* [Go: Goroutine, OS Thread and CPU Management](https://medium.com/a-journey-with-go/go-goroutine-os-thread-and-cpu-management-2f5a5eaf518a)
* [Beating C with 70 Lines of Go](https://ajeetdsouza.github.io/blog/posts/beating-c-with-70-lines-of-go/)
* [The Value in Go's Simplicity](https://benjamincongdon.me/blog/2019/11/11/The-Value-in-Gos-Simplicity/)
* [Go 号称几行代码开启一个 HTTP Server，底层都做了什么？](https://mp.weixin.qq.com/s/n7mSUB6pxoYmr5u575Nqqg)
* [Interrupt handling in Go](https://embeddedgo.github.io/2019/11/29/interrupt_handling_in_go.html)
* [Go Composition vs Inheritance](http://jim-mcbeath.blogspot.com/2019/11/go-composition-vs-inheritance.html)
* [fasthttp-协程池](https://github.com/valyala/fasthttp/blob/master/workerpool.go)


# Refer

* [Go官网]
* [Go博客]
* [Go在线测试]
* [Go在线交互式课程]
* [The Go Programming Language]
* [High Performance Go Workshop - Dave Cheney]
* [go-web-framework-benchmark]
* [fasthttp]
* [fasthttp-router]
* [awesome-go]
* [Jaeger]
* [go-chassis]
* [learn-go-with-tests]
* [How To Install and Set Up a Local Programming Environment for Go](https://www.digitalocean.com/community/tutorial_series/how-to-install-and-set-up-a-local-programming-environment-for-go)
* [How To Write Packages in Go](https://www.digitalocean.com/community/tutorials/how-to-write-packages-in-go)
* [Go 语言简介（上）— 语法](http://coolshell.cn/articles/8460.html)
* [Go 语言简介（下）— 特性](http://coolshell.cn/articles/8489.html)
* [Go Wiki: Go Code Review Comments](https://go.dev/wiki/CodeReviewComments)


[Go官网]: https://golang.org
[Go博客]: https://blog.golang.org
[Go在线测试]: https://play.golang.org
[Go在线交互式课程]: https://tour.golang.org
[The Go Programming Language]: http://www.gopl.io/
[The Go Programming Language Example Programs]: https://github.com/adonovan/gopl.io/
[Dave Cheney的golang博客]: dave.cheney.net
[High Performance Go Workshop - Dave Cheney]: https://dave.cheney.net/high-performance-go-workshop/gopherchina-2019.html?from=singlemessage&isappinstalled=0

[go-web-framework-benchmark]: https://github.com/smallnest/go-web-framework-benchmark
[fasthttp]: https://github.com/valyala/fasthttp
[fasthttp-router]: https://github.com/fasthttp/router
[awesome-go]: https://github.com/avelino/awesome-go
[Jaeger]: https://github.com/jaegertracing/jaeger
[go-chassis]: https://github.com/go-chassis/go-chassis
[learn-go-with-tests]: https://github.com/quii/learn-go-with-tests
