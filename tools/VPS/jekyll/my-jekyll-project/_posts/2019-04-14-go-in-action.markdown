---
layout: post
title:  "Go in Action"
date:   2019-04-14 10:00:00 +0800
categories: [GoLang, 编程语言]
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
		+ Oberon消除了模块接口文件和模块实现文件的区隔。
		+ Object Oberon提供了方法声明语法。
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

| 类型语言 | 复杂性 | 性能和安全性 | 
| -- | -- | --
| 强 | 高 | 高
| 弱 | 低 | 低        

# 测试环境

根据[The Go Programming Language Example Programs](https://github.com/adonovan/gopl.io/)，构建测试环境。

## 安装

三种安装方式：

* 源码安装。
* 标准包安装。
* 第三方工具安装。例如，Ubuntu的`apt-get`，Mac的`homebrew`。

```
root@gerryyang:~/workspace/go# go version
go version go1.9.2 linux/amd64
```

## 环境变量

``` bash
# go的安装路径
export GOROOT=/root/LAMP/golang/go_1_9_2
export PATH=$PATH:$GOROOT/bin
# go的工作目录
export GOPATH=/root/workspace/go
```
* `GOPATH`允许多个目录(Linux下用冒号分割)，当GOPATH指定了**多个目录时**，默认将`go get`的内容放在**第一个目录**。
* GOPATH目录约定有3个子目录：
	- `src` (源代码，例如，.go, .c, .h, .s等)
	- `pkg` (编译后生成的文件，例如，.a)
	- `bin` (编译后生成的可执行文件)

## 编译和执行

通过`go get`命令获取源码，构建和安装。

```
root@gerryyang:~/workspace/go#go get gopl.io/ch1/helloworld      
root@gerryyang:~/workspace/go# ls
bin  src
root@gerryyang:~/workspace/go# ls bin
helloworld
root@gerryyang:~/workspace/go# ./bin/helloworld 
Hello, 世界
root@gerryyang:~/workspace/go# ls src/gopl.io/
ch1  ch10  ch11  ch12  ch13  ch2  ch3  ch4  ch5  ch6  ch7  ch8  ch9  README.md
```

源码在`$(GOPATH)/src/gopl.io/ch1/helloworld/main.go`。


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


# Go入门示例

* Go代码是使用`包`来组织的，包类似其他语言中的库和模块。一个包由一个或多个`.go`源文件组成，放在`一个文件夹`中，该`文件夹的名字`描述了包的作用。
* 每一个源文件的开始都用`package`声明。例如，`package main`，指明了这个文件属于哪个包。后面跟着它导入的`其他包的列表`，然后是存储在文件中的`程序声明`。
* Go的`标准库`中有100多个包用来完成输入，输出，排序，文本处理等常见任务。
* 名为`main`的包用来定义一个独立的可执行程序，而不是库。
* 必须精确地导入需要的包，在缺失导入，或存在不需要的包的情况下，编译会失败。(这种严格的要求可以防止程序演化中引用不需要的包)
	- `goimports`工具可以按需管理导入声明的插入和移除。`go get golang.org/x/tools/cmd/goimports`
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
* 递增语句`i++`对i进行加1，它等价于`i += 1`，又等价于`i = i + 1`。**注意，这些是语句，而不像其他C族语言一样是表达式，所以`j = i ++`是不合法的。并且，仅支持后缀，`++i`也不合法。**
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
	// break;
	// return;
}
```

* 另一种形式的for循环在字符串或slice数据上迭代。

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
* 在这个例子中，不需要索引，但是语法上range循环需要处理，因此也必须处理索引。一个主意是将索引赋予一个`临时变量`然后忽略它。但是，Go不允许存在无用的临时变量，不然会出现编译错误。解决方案是使用空标识符`_`，空标识符可以用在任何语法需要变量名但是程序逻辑不需要的地方。例如，丢弃每次迭代产生的无用的索引。
* 这个版本使用`短的变量声明`来声明和初始化。原则：使用`显式的初始化`来说明初始化变量的重要性，使用`隐式的初始化`来表明初始化变量不重要。

``` go
// 以下几种声明字符串变量的方式是等价的
s := ""              // 此方式，更加简洁，通常在一个函数内部使用，不适合包级别的变量 (推荐)
var s string         // 默认初始化为空字符串 (推荐)
var s = ""           // 很少用
var s string = ""    // 显式的变量类型，在类型一致的情况下是冗余的信息，在类型不一致时是必需的
```

* 上面程序的问题：每次循环，字符串`s`有了新的内容，`+=`语句通过追加旧的字符串，空格字符，和下一个参数，生成一个新的字符串，然后把新字符串赋给`s`。旧的内容不再需要使用，会被`例行垃圾回收`。如果有大量的数据需要处理，这样的代价会比较大。

* 一个高效的方式是使用strings包中的`Join`函数。

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
* 像for一样，`if`语句中的条件部分也不放在圆括号里，但是程序体中需要用到大括号。
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
* `键`在map中不存在时也是没有问题的。当一个新的行第一次出现时，右边的表达式counts[line]根据值类型被推演为`零值`，int的零值是0。
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

* `map`是一个使用`make`创建的数据结构的`引用`。当一个map传递个一个函数时，函数接收到这个引用的副本，所以，被调用函数中对于map数据结构中的改变，对函数调用者使用的map引用也是`可见的`。
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
* 'http.Get'产生一个HTTP请求。如果没有错，返回结果存在响应结构`resp`里面。其中，`resp.Body`包含服务器端响应的一个可读取数据流，随后通过`ioutil.ReadAll`读取整个响应结果并存入`b`。

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

## 获取多个URL

TODO




# Go程序组成

声明，变量，新类型，包和文件，以及作用域。

数值，布尔量，字符串，常量，Unicode

组合类型 (数组，键值对，结构体，切片)

函数，错误处理，崩溃和恢复，以及defer语句

方法，接口，并发，包，测试，反射

面向对象设计 (Go没有类继承，没有类，较复杂的对象行为是通过较简单的对象组合，而非继承完成的)

并发处理，基于CSP思想，采用goroutine和信道实现，共享变量

# Others

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

1. [Go官网]
2. [Go博客]
3. [Go在线测试]
4. [Go在线交互式课程]
5. [The Go Programming Language]
6. [Dave Cheney的golang博客]
7. [High Performance Go Workshop - Dave Cheney]
8. [go-web-framework-benchmark]
9. [fasthttp]
10. [fasthttp-router]
11. [awesome-go]
12. [Jaeger]
13. [go-chassis]
14. [learn-go-with-tests]

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