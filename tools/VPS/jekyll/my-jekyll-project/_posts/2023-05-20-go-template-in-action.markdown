---
layout: post
title:  "Go Template in Action"
date:   2023-05-20 12:00:00 +0800
categories: [GoLang]
---

* Do not remove this line (it will not be displayed)
{:toc}



# [go template](https://pkg.go.dev/text/template)

Package `template` implements data-driven templates for generating textual output.

To generate HTML output, see package `html/template`, which has the same interface as this package but automatically secures HTML output against certain attacks.

Templates are executed by applying them to a **data structure**. Annotations in the template refer to elements of the data structure (typically a field of a struct or a key in a map) to control execution and derive values to be displayed. Execution of the template walks the structure and sets the cursor, represented by a period '.' and called "dot", to the value at the current location in the structure as execution proceeds.


{% raw %}

The input text for a template is UTF-8-encoded text in any format. "Actions"--data evaluations or control structures--are delimited by `{{` and `}}`; all text outside actions is copied to the output unchanged.

{% endraw %}

Once parsed, a template may be executed safely in parallel, although if parallel executions share a Writer the output may be interleaved.

Here is a trivial example that prints "17 items are made of wool".


{% raw %}
``` golang
type Inventory struct {
	Material string
	Count    uint
}
sweaters := Inventory{"wool", 17}
tmpl, err := template.New("test").Parse("{{.Count}} items are made of {{.Material}}")
if err != nil { panic(err) }
err = tmpl.Execute(os.Stdout, sweaters)
if err != nil { panic(err) }
```
{% endraw %}

另一个示例：

{% raw %}
``` golang
package main

import (
    "os"
    "text/template"
)

type Person struct {
    Name string
    Age  int
}

func main() {
    // Define a template
    tmpl := "Name: {{.Name}}, Age: {{.Age}}\n"

    // Create a new template and parse the template string
    t := template.Must(template.New("person").Parse(tmpl))

    // Create a new person object
    p := Person{Name: "Alice", Age: 30}

    // Execute the template with the person object as input
    err := t.Execute(os.Stdout, p)
    if err != nil {
        panic(err)
    }
}
```
{% endraw %}


```
Name: Alice, Age: 30
```

{% raw %}
模板内容可以是 UTF-8 编码的任何内容。其中用`{{`和`}}`包围的部分称为动作，`{{}}`外的其它文本在输出保持不变。模板需要应用到数据，模板中的动作会根据数据生成相应的内容来替换。
{% endraw %}

首先调用`template.New`创建一个模板，参数为模板名。然后调用 Template 类型的`Parse`方法，解析模板字符串，生成模板主体。最后，调用模板对象的`Execute`方法，传入数据。`Execute`执行模板中的动作，将结果输出到`os.Stdout`。





## Text and spaces

{% raw %}

By default, all text between actions is copied verbatim (一字不差地) when the template is executed. For example, the string " items are made of " in the example above appears on standard output when the program is run.


However, to aid in formatting template source code, if an action's left delimiter (by default `{{`) is followed immediately by a **minus sign and white space**, all trailing white space is trimmed from the immediately preceding text. Similarly, if the right delimiter (`}}`) is preceded by **white space and a minus sign**, all leading white space is trimmed from the immediately following text. In these trim markers, the white space must be present: `{{- 3}}` is like `{{3}}` but trims the immediately preceding text, while `{{-3}}` parses as an action containing the number `-3`.

For instance, when executing the template whose source is

```
"{{23 -}} < {{- 45}}"
```

the generated output would be

```
"23<45"
```

For this trimming, the definition of white space characters is the same as in Go: space, horizontal tab, carriage return, and newline.

{% endraw %}


## Actions (动作)

Go 模板中的动作就是一些嵌入在模板里面的**命令**。动作大体上可以分为以下几种类型：点动作；条件动作；迭代动作；设置动作；包含动作。

### 点动作

{% raw %}

点动作（`{{ . }}`）。它其实代表是传递给模板的数据，其他动作或函数基本上都是对这个数据进行处理，以此来达到格式化和内容展示的目的。

在 template 中，点 "." 代表当前作用域的当前对象。例如，`{{.}}`，这个点是**顶级作用域**范围内的，所有数据都可以通过顶级作用域来访问。

但是并非只有一个顶级作用域，range、with、if 等内置 action 都有自己的**本地作用域**。例如 `{{with .allocatesvr | .SvrFE }}` ，`with .allocatesvr` 会将当前 action 的作用域改为`.allocatesvr`对象，所以可以直接用`.SvrFE`访问到`.allocatesvr.SvrFE`的值。

{% endraw %}

### 条件动作

{% raw %}

```
{{ if pipeline }} T1 {{ end }}
{{ if pipeline }} T1 {{ else }} T2 {{ end }}
{{ if pipeline1 }} T1 {{ else if pipeline2 }} T2 {{ else }} T3 {{ end }}
```

pipeline 表示管道，可以将它理解为一个值。 T1/T2 等形式表示语句块，里面可以嵌套其它类型的动作。最简单的语句块就是不包含任何动作的字符串。

以 `{{ if pipeline }} T1 {{ end }}` 为例，如果管道计算出来的值不为空，执行 `T1`。否则，不生成输出。


例如：

```
{{ if .CmdService.Port -}} CMDSVR_PORT = {{ .CmdService.Port -}} {{ end }}
```

`-` 是 Go 语言提供的针对空白符的处理，方便规范文本输出格式。如果一个动作以 `{{- ` （注意有一个空格），那么该动作与它前面相邻的非空文本或动作间的空白符将会被全部删除。类似地，如果一个动作以 ` -}}` 结尾，那么该动作与它后面相邻的非空文本或动作间的空白符将会被全部删除。

{% endraw %}

### 迭代动作

{% raw %}

```
{{ range pipeline }} T1 {{ end }}
{{ range pipeline }} T1 {{ else }} T2 {{ end }}
```

例如：

```
<ApsSvrCount> {{- len .Apollo.UrlList -}} </ApsSvrCount>
{{- range .Apollo.UrlList }}
<ApsSvrUrls>{{ . }}</ApsSvrUrls>
{{- end }}
```

如果`.Apollo.UrlList`长度不为 0，遍历列表，输出列表值到`<ApsSvrUrls>{{ . }}</ApsSvrUrls>`。range 语句循环体内，`.` 被设置为当前遍历的元素，即 Url 的值。

{% endraw %}

### 设置动作

{% raw %}

```
{{ with pipeline }} T1 {{ end }}
{{ with pipeline }} T1 {{ else }} T2 {{ end }}
```

设置动作使用 `with` 关键字重定义 `.` 。在 `with` 语句内，`.` 会被定义为指定的值。一般用在结构嵌套很深时，能起到简化代码的作用。

例如：

模版为：

```
{{ with .gamesvr }} {{- .tconnd_lwip.vIP -}} {{ end }}
```

配置为：

```
gamesvr:
  tconnd_lwip:
    vIP: 127.0.0.1
```

在 `with` 语句内，`.` 被替换成了 gamesvr 的值，这样就可以通过 `.tconnd_lwip.vIP` 访问配置。

{% endraw %}

### Examples

Here is the list of actions. "Arguments" and "pipelines" are evaluations of data, defined in detail in the corresponding sections that follow.

{% raw %}

```
{{/* a comment */}}
{{- /* a comment with white space trimmed from preceding and following text */ -}}
	A comment; discarded. May contain newlines.
	Comments do not nest and must start and end at the
	delimiters, as shown here.

{{pipeline}}
	The default textual representation (the same as would be
	printed by fmt.Print) of the value of the pipeline is copied
	to the output.

{{if pipeline}} T1 {{end}}
	If the value of the pipeline is empty, no output is generated;
	otherwise, T1 is executed. The empty values are false, 0, any
	nil pointer or interface value, and any array, slice, map, or
	string of length zero.
	Dot is unaffected.

{{if pipeline}} T1 {{else}} T0 {{end}}
	If the value of the pipeline is empty, T0 is executed;
	otherwise, T1 is executed. Dot is unaffected.

{{if pipeline}} T1 {{else if pipeline}} T0 {{end}}
	To simplify the appearance of if-else chains, the else action
	of an if may include another if directly; the effect is exactly
	the same as writing
		{{if pipeline}} T1 {{else}}{{if pipeline}} T0 {{end}}{{end}}

{{range pipeline}} T1 {{end}}
	The value of the pipeline must be an array, slice, map, or channel.
	If the value of the pipeline has length zero, nothing is output;
	otherwise, dot is set to the successive elements of the array,
	slice, or map and T1 is executed. If the value is a map and the
	keys are of basic type with a defined order, the elements will be
	visited in sorted key order.

{{range pipeline}} T1 {{else}} T0 {{end}}
	The value of the pipeline must be an array, slice, map, or channel.
	If the value of the pipeline has length zero, dot is unaffected and
	T0 is executed; otherwise, dot is set to the successive elements
	of the array, slice, or map and T1 is executed.

{{break}}
	The innermost {{range pipeline}} loop is ended early, stopping the
	current iteration and bypassing all remaining iterations.

{{continue}}
	The current iteration of the innermost {{range pipeline}} loop is
	stopped, and the loop starts the next iteration.

{{template "name"}}
	The template with the specified name is executed with nil data.

{{template "name" pipeline}}
	The template with the specified name is executed with dot set
	to the value of the pipeline.

{{block "name" pipeline}} T1 {{end}}
	A block is shorthand for defining a template
		{{define "name"}} T1 {{end}}
	and then executing it in place
		{{template "name" pipeline}}
	The typical use is to define a set of root templates that are
	then customized by redefining the block templates within.

{{with pipeline}} T1 {{end}}
	If the value of the pipeline is empty, no output is generated;
	otherwise, dot is set to the value of the pipeline and T1 is
	executed.

{{with pipeline}} T1 {{else}} T0 {{end}}
	If the value of the pipeline is empty, dot is unaffected and T0
	is executed; otherwise, dot is set to the value of the pipeline
	and T1 is executed.

```

{% endraw %}

## Arguments

Arguments may evaluate to any type; if they are pointers the implementation automatically indirects to the base type when required. If an evaluation yields a function value, such as a function-valued field of a struct, the function is not invoked automatically, but it can be used as a truth value for an if action and the like. To invoke it, use the call function, defined below.


## Pipelines (管道)

{% raw %}

A pipeline is a possibly chained sequence of "commands". A command is a simple value (argument) or a function or method call, possibly with multiple arguments.

```
Argument
	The result is the value of evaluating the argument.
.Method [Argument...]
	The method can be alone or the last element of a chain but,
	unlike methods in the middle of a chain, it can take arguments.
	The result is the value of calling the method with the
	arguments:
		dot.Method(Argument1, etc.)
functionName [Argument...]
	The result is the value of calling the function associated
	with the name:
		function(Argument1, etc.)
	Functions and function names are described below.
```

A pipeline may be "chained" by separating a sequence of commands with pipeline characters `|`. In a chained pipeline, the result of each command is passed as the last argument of the following command. The output of the final command in the pipeline is the value of the pipeline.

The output of a command will be either one value or two values, the second of which has type error. If that second value is present and evaluates to non-nil, execution terminates and the error is returned to the caller of Execute.

在一个链式管道中，每个命令的结果会作为下一个命令的最后一个参数。最后一个命令的结果作为整个管道的值。

例如：

```
{{ with .Required.GroupName | index $ }} {{- coalesce .tconnd_gcp.ApsAuthInfo.BusinessID $.Apollo.BusinessID -}} {{ end }}
```

因为作用域发生变化，要访问顶级作用域需要加上`$`，如`$.Apollo.BusinessID`。假设 `.Required.GroupName` 得到的结果是 gamesvr，这个 `index $ gamesvr` 就是取 `.gamesvr`

{% endraw %}


## Variables

A pipeline inside an action may initialize a variable to capture the result. The initialization has syntax

```
$variable := pipeline
```

where `$variable` is the name of the variable. An action that declares a variable produces no output.


## Functions (函数)

Go 模板提供了大量的预定义函数，如果有特殊需求也可以实现自定义函数。模板执行时，遇到函数调用，先从模板自定义函数表中查找，而后查找全局函数表。

### 全局函数

**During execution functions are found in two function maps: first in the template, then in the global function map**. By default, no functions are defined in the template but the Funcs method can be used to add them.

Predefined global functions are named as follows.

```
and
	Returns the boolean AND of its arguments by returning the
	first empty argument or the last argument. That is,
	"and x y" behaves as "if x then y else x."
	Evaluation proceeds through the arguments left to right
	and returns when the result is determined.
call
	Returns the result of calling the first argument, which
	must be a function, with the remaining arguments as parameters.
	Thus "call .X.Y 1 2" is, in Go notation, dot.X.Y(1, 2) where
	Y is a func-valued field, map entry, or the like.
	The first argument must be the result of an evaluation
	that yields a value of function type (as distinct from
	a predefined function such as print). The function must
	return either one or two result values, the second of which
	is of type error. If the arguments don't match the function
	or the returned error value is non-nil, execution stops.
html
	Returns the escaped HTML equivalent of the textual
	representation of its arguments. This function is unavailable
	in html/template, with a few exceptions.
index
	Returns the result of indexing its first argument by the
	following arguments. Thus "index x 1 2 3" is, in Go syntax,
	x[1][2][3]. Each indexed item must be a map, slice, or array.
slice
	slice returns the result of slicing its first argument by the
	remaining arguments. Thus "slice x 1 2" is, in Go syntax, x[1:2],
	while "slice x" is x[:], "slice x 1" is x[1:], and "slice x 1 2 3"
	is x[1:2:3]. The first argument must be a string, slice, or array.
js
	Returns the escaped JavaScript equivalent of the textual
	representation of its arguments.
len
	Returns the integer length of its argument.
not
	Returns the boolean negation of its single argument.
or
	Returns the boolean OR of its arguments by returning the
	first non-empty argument or the last argument, that is,
	"or x y" behaves as "if x then x else y".
	Evaluation proceeds through the arguments left to right
	and returns when the result is determined.
print
	An alias for fmt.Sprint
printf
	An alias for fmt.Sprintf
println
	An alias for fmt.Sprintln
urlquery
	Returns the escaped value of the textual representation of
	its arguments in a form suitable for embedding in a URL query.
	This function is unavailable in html/template, with a few
	exceptions.
```

The boolean functions take any zero value to be false and a non-zero value to be true.

There is also a set of binary comparison operators defined as functions:

```
eq
	Returns the boolean truth of arg1 == arg2
ne
	Returns the boolean truth of arg1 != arg2
lt
	Returns the boolean truth of arg1 < arg2
le
	Returns the boolean truth of arg1 <= arg2
gt
	Returns the boolean truth of arg1 > arg2
ge
	Returns the boolean truth of arg1 >= arg2
```

### 自定义函数

默认情况下，模板中无自定义函数，可以使用模板的`Funcs`方法添加。`sprig` 模板函数库，就是通过这种方式使用的。

``` golang
t, err := template.New("test").Funcs(sprig.TxtFuncMap()).Parse(string(tmpStr))
err = t.Execute(os.Stdout, meta)
if err != nil {
	return
}
```

`Parse()` 解析模板字符串之前，先调用 `Funcs(sprig.TxtFuncMap())`，指定使用 `sprig` 的模板函数。




## Examples

Here are some example one-line templates demonstrating pipelines and variables. All produce the quoted word "output":

{% raw %}

```
{{"\"output\""}}
	A string constant.
{{`"output"`}}
	A raw string constant.
{{printf "%q" "output"}}
	A function call.
{{"output" | printf "%q"}}
	A function call whose final argument comes from the previous
	command.
{{printf "%q" (print "out" "put")}}
	A parenthesized argument.
{{"put" | printf "%s%s" "out" | printf "%q"}}
	A more elaborate call.
{{"output" | printf "%s" | printf "%q"}}
	A longer chain.
{{with "output"}}{{printf "%q" .}}{{end}}
	A with action using dot.
{{with $x := "output" | printf "%q"}}{{$x}}{{end}}
	A with action that creates and uses a variable.
{{with $x := "output"}}{{printf "%q" $x}}{{end}}
	A with action that uses the variable in another action.
{{with $x := "output"}}{{$x | printf "%q"}}{{end}}
	The same, but pipelined.

```

{% endraw %}




# [Sprig: Template functions for Go templates](https://github.com/Masterminds/sprig)

The Go language comes with a [built-in template language](http://golang.org/pkg/text/template/), but not very many template functions. Sprig is a library that provides more than 100 commonly used template functions.

It is inspired by the template functions found in [Twig](http://twig.sensiolabs.org/documentation) and in various JavaScript libraries, such as [underscore.js](http://underscorejs.org/).


* [Sprig Function Documentation](https://masterminds.github.io/sprig/)

## Default Functions

Sprig provides tools for setting default values for templates.

### default

To set a simple default value, use `default`:

```
default "foo" .Bar
```

In the above, if `.Bar` evaluates to a non-empty value, it will be used. But if it is empty, `foo` will be returned instead.

The definition of “empty” depends on type:

* Numeric: 0
* String: “”
* Lists: `[]`
* Dicts: `{}`
* Boolean: `false`
* And always `nil` (aka null)

For structs, there is no definition of empty, so a struct will never return the default.

### coalesce

The `coalesce` function takes a list of values and returns the first non-empty one.

```
coalesce 0 1 2
```

The above returns 1.

This function is useful for scanning through multiple variables or values:

```
coalesce .name .parent.name "Matt"
```

The above will first check to see if `.name` is empty. If it is not, it will return that value. If it is empty, coalesce will evaluate `.parent.name` for emptiness. Finally, if both `.name` and `.parent.name` are empty, it will return `Matt`.

# 使用示例

## 示例1

yaml 配置：

``` yaml
UpdateMgr:
  EpollMgr: {Type: "BY_MICROSECOND", Interval: "1000"}
  LibEventMgr: {Type: "BY_MICROSECOND", Interval: "1000"}
```

go template 配置：

{% raw %}

``` xml
<?xml version="1.0" encoding="UTF-8" ?>
<UpdateMgr>
{{- range $name,$attr := $.UpdateMgr }}
    <Update Name="{{ $name }}" {{ range $key,$val := $attr }} {{ $key }}="{{ $val }}" {{- end }} />
{{- end }}
</UpdateMgr>
```

{% endraw %}

生成的格式：

``` xml
<?xml version="1.0" encoding="UTF-8" ?>
<UpdateMgr>
    <Update Name="EpollMgr" Type="BY_MICROSECOND" Interval="1000" />
    <Update Name="LibEventMgr" Type="BY_MICROSECOND" Interval="1000" />
</UpdateMgr>
```


# Refer

* [go template](https://pkg.go.dev/text/template)
* [Sprig Function Documentation](https://masterminds.github.io/sprig/)





