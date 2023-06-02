---
layout: post
title:  "Go Reflect in Action"
date:   2023-04-27 12:30:00 +0800
categories: [GoLang]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Reflect

> 反射是指计算机程序在运行时（Runtime）可以访问，检测和修改它本身状态或行为的一种能力。

程序编译后，变量被转换为内存地址，而变量名无法被编译器写入可执行部分。在运行程序时，程序无法获取自身的信息。**支持反射的语言，可以在编译器将变量的反射信息如字段名称、类型信息等整合到可执行文件中，并给程序提供接口访问反射信息，这样可以在程序运行期获取类型的反射信息，并修改他们**。

静态语言：

* C/C++ **不支持**反射
* Go，Java，C# **支持**反射

动态语言：

* Lua，JavaScript，可以在运行期访问程序自身的值与类型，故不需要反射特性


Go 提供了一种在运行时更新和检查变量的值、调用变量的方法的机制，但在编译器不知道这些变量的具体类型，这种机制被称为反射。Go 使用 reflect 包访问程序的反射信息。

# 实现原理

## 反射的基础 interface{}

Go 的接口是由两部分组成的，一部分是**类型信息**，另一部分是**数据信息**。

``` golang
var a = 1
var b interface{} = a
```

`b`的类型信息是`int`，数据信息是`1`，这两部分信息都是存储 `b`里面。

`b`的类型实际上是`eface`，它是一个**空接口**，在`src/runtime/runtime2.go`中，它的定义如下：

``` golang
type eface struct {
    _type *_type
    data  unsafe.Pointer
}
```

也就是说，一个 `interface{}` 中实际上既包含了变量的**类型信息**，也包含了类型的**数据**。 正因为如此，才可以通过**反射**来获取到变量的类型信息，以及变量的数据信息。

## 反射对象 reflect.Type & reflect.Value

> 反射，可以将**接口类型变量**转换为**反射类型对象**

* `reflect.TypeOf`: 返回**反射类型**（returns the reflection Type that represents the dynamic type of i）
* `reflect.ValueOf`: 返回**反射值**（returns a new Value initialized to the concrete value）

``` golang
var a = 1
t := reflect.TypeOf(a)  // t = int

var b = "hello"
v := reflect.ValueOf(b)  // v = "hello"
```

看一下 `TypeOf` 和 `ValueOf` 的源码会发现，这两个方法都接收一个 `interface{}` 类型的参数，然后返回一个 `reflect.Type` 和 `reflect.Value` 类型的值。这也就是为什么可以通过 `reflect.TypeOf` 和 `reflect.ValueOf` 来获取到一个变量的类型和值的原因。

`reflect.TypeOf()` 源码：

``` golang
func TypeOf(i any) Type {
    eface := *(*emptyInterface)(unsafe.Pointer(&i))
    return toType(eface.typ)
}

func toType(t *rtype) Type {
    if t == nil {
        return nil
    }
    return t
}
```

`reflect.ValueOf()` 源码：

``` golang
func ValueOf(i any) Value {
    if i == nil {
        return Value{}
    }

    // TODO: Maybe allow contents of a Value to live on the stack.
    // For now we make the contents always escape to the heap. It
    // makes life easier in a few places (see chanrecv/mapassign
    // comment below).
    escapes(i)

    return unpackEface(i)
}

// Dummy annotation marking that the value x escapes,
// for use in cases where the reflect code is so clever that
// the compiler cannot follow.
func escapes(x any) {
    if dummy.b {
        dummy.x = x
    }
}

var dummy struct {
    b bool
    x any
}
```

## 反射定律

在 Go 官方博客中关于反射的文章 [laws-of-reflection](https://go.dev/blog/laws-of-reflection) 中，提到了三条反射定律：

1. 反射可以将 `interface` 类型变量转换成反射对象。
2. 反射可以将反射对象还原成 `interface` 对象。
3. 如果要修改反射对象，那么反射对象必须是可设置的（`CanSet`）。

关于第 2 点：

可以通过 `reflect.Value.Interface` 来获取到反射对象的 `interface` 对象，也就是传递给 `reflect.ValueOf` 的那个变量本身。不过返回值类型是 `interface{}`，所以需要进行类型断言。

``` golang
type Student struct {
    Name string `json:"name1" db:"name2"`
    Age  int    `json:"age1" db:"age2"`
}

func main() {
    var s Student
    v := reflect.ValueOf(&s)

    // 将反射对象还原成interface对象
    i := v.Interface()
    fmt.Println(i.(*Student))
}
```

关于第 3 点：

可以通过 `reflect.Value.CanSet` 来判断一个反射对象是否是可设置的。如果是可设置的，就可以通过 `reflect.Value.Set` 来修改反射对象的值。

``` golang
func main() {
    s := &Student{
        Name: "zhangSan",
        Age:  18,
    }
    v := reflect.ValueOf(s)

    fmt.Println("set ability of v:", v.CanSet())           // false
    fmt.Println("set ability of Elem:", v.Elem().CanSet()) // true
}
```

**可设置要求：**

* 反射对象是一个指针
* 这个指针指向的是一个可设置的变量

**原因：**

如果这个值只是一个普通的变量，这个值实际上被拷贝了一份。如果通过反射修改这个值，那么实际上是修改的这个拷贝的值，而不是原来的值。 所以 go 语言在这里做了一个限制。

**为什么v.CanSet() == false ？**

v 是一个指针，而 v.Elem() 是指针指向的值，对于这个指针本身，修改它是没有意义的，可以设想一下，如果修改了指针变量（也就是修改了指针变量指向的地址），那会发生什么呢？那样指针变量就不是指向 x 了， 而是指向了其他的变量，这样就不符合预期了。所以 `v.CanSet()` 返回的是 `false`。


``` golang
package demo

import (
	"fmt"
	"reflect"
	"testing"
)

type Student3 struct {
	Name string `json:"name1" db:"name2"`
	Age  int    `json:"age1" db:"age2"`
}

func Test3(t *testing.T) {
	s := &Student3{
		Name: "zhangSan",
		Age:  18,
	}
	v := reflect.ValueOf(s)

	fmt.Println("set ability of v:", v.CanSet())           // false
	fmt.Println("set ability of Elem:", v.Elem().CanSet()) // true

	if v.Elem().CanSet() {
		for i := 0; i < v.Elem().NumField(); i++ {
			switch v.Elem().Field(i).Kind() {
			case reflect.String:
				v.Elem().Field(i).Set(reflect.ValueOf("lisi"))
			case reflect.Int:
				v.Elem().Field(i).Set(reflect.ValueOf(20))
			}
		}
	}

	fmt.Println("v: ", v)
	fmt.Println("student: ", v.Interface().(*Student3))
}
```

```
$go test -v demo3_test.go
=== RUN   Test3
set ability of v: false
set ability of Elem: true
v:  &{lisi 20}
student:  &{lisi 20}
--- PASS: Test3 (0.00s)
PASS
ok      command-line-arguments  0.002s
```


# 测试代码

``` golang
package demo1

import (
	"fmt"
	"reflect"
	"testing"
)

type student struct {
	name  string
	age   uint8
	infos interface{}
}

func TestReflect(t *testing.T) {
	s := &student{
		name: "zhangSan",
		age:  18,
		infos: map[string]interface{}{
			"class": "class1",
			"grade": uint8(1),
			"read": func(str string) {
				fmt.Println(str)
			},
		},
	}
	options := s.infos
	fmt.Println("infos type:", reflect.TypeOf(options))
	fmt.Println("infos value:", reflect.ValueOf(options))

	fmt.Println("infos.class type:", reflect.TypeOf(options.(map[string]interface{})["class"]))
	fmt.Println("infos.class value:", reflect.ValueOf(options.(map[string]interface{})["class"]))

	fmt.Println("infos.grade type:", reflect.TypeOf(options.(map[string]interface{})["grade"]))
	fmt.Println("infos.grade value:", reflect.ValueOf(options.(map[string]interface{})["grade"]))

	fmt.Println("infos.read type:", reflect.TypeOf(options.(map[string]interface{})["read"]))
	fmt.Println("infos.read value:", reflect.ValueOf(options.(map[string]interface{})["read"]))

	read := options.(map[string]interface{})["read"]
	if reflect.TypeOf(read).Kind() == reflect.Func {
		read.(func(str string))("I am reading!")
	}
}
```

测试：

```
$go mod init github.com/gerryyang/goinaction/src/reflect
$go mod tidy
```

```
$go test -v demo1_test.go
=== RUN   Test1
infos type: map[string]interface {}
infos value: map[class:class1 grade:1 read:0x4f8e00]
infos.class type: string
infos.class value: class1
infos.grade type: uint8
infos.grade value: 1
infos.read type: func(string)
infos.read value: 0x4f8e00
I am reading!
--- PASS: Test1 (0.00s)
PASS
ok      command-line-arguments  0.003s
```

``` golang
package demo

import (
	"fmt"
	"reflect"
	"testing"
)

type Student struct {
	Name string `json:"name1" db:"name2"`
	Age  int    `json:"age1" db:"age2"`
}

func Test2(t *testing.T) {
	var s Student
	v := reflect.ValueOf(&s)

	// 类型
	ty := v.Type()

	// 获取字段
	for i := 0; i < ty.Elem().NumField(); i++ {
		f := ty.Elem().Field(i)
		fmt.Println(f.Tag.Get("json"))
		fmt.Println(f.Tag.Get("db"))
	}
}
```

```
$go test -v demo2_test.go
=== RUN   Test2
name1
name2
age1
age2
--- PASS: Test2 (0.00s)
PASS
ok      command-line-arguments  0.003s
```

