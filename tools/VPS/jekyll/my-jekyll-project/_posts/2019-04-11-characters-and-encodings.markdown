---
layout: post
title:  "Characters and Encodings"
date:   2019-04-11 08:00:00 +0800
categories: 基础理论
---

* Do not remove this line (it will not be displayed)
{:toc}

字符和编码是计算机技术的基石。

* `Character` is an overloaded term that can mean many things.

* `Code Point(码点)` is the atomic unit of information. Text is a sequence of code points. Each code point is a number which is given meaning by **the Unicode standard**.

* `Code Unit` is the unit of storage of a part of an encoded code point. In UTF-8 this means 8-bits, in UTF-16 this means 16-bits. A single code unit may represent a full code point, or part of a code point.

* `Grapheme(字素)` is a sequence of one or more code points that are displayed as a single, graphical unit that a reader recognizes as a single element of the writing system.

* `Glyph(图象字符)` is an image, usually stored in a font (which is a collection of glyphs), used to represent graphemes or parts thereof.


refer:

* [What's the difference between a character, a code point, a glyph and a grapheme?](https://stackoverflow.com/questions/27331819/whats-the-difference-between-a-character-a-code-point-a-glyph-and-a-grapheme)


# ASCII

计算机内部，所有信息最终都是一个`二进制值`。每一个二进制位（`bit`）有`0`和`1`两种状态，因此`八个二进制位`就可以组合出`256`种状态，这被称为一个`字节`（`byte`）。也就是说，**一个字节一共可以用来表示256种不同的状态，每一个状态对应一个符号，就是`256`个符号，从`00000000`到`11111111`**。上个世纪60年代，美国制定了一套字符编码，对英语字符与二进制位之间的关系，做了统一规定。这被称为**ASCII码**，一直沿用至今。**ASCII码**一共规定了`128`个字符的编码，比如`空格SPACE`是`32`（二进制00100000），`大写的字母A`是`65`（二进制01000001）。这`128`个符号（包括`32`个不能打印出来的控制符号），只占用了一个字节的后面`7`位，最前面的一位统一规定为`0`。

> 为什么需要非ASCII编码？

英语用128个符号编码就够了，但是用来表示其他语言，128个符号是不够的。比如，在法语中，字母上方有注音符号，它就无法用ASCII码表示。于是，一些欧洲国家就决定，利用字节中闲置的最高位编入新的符号。比如，法语中的`é`的编码为`130`（二进制10000010）。这样一来，这些欧洲国家使用的编码体系，可以表示最多256个符号。

但是，这里又出现了新的问题。不同的国家有不同的字母，因此，哪怕它们都使用256个符号的编码方式，代表的字母却不一样。比如，`130`在法语编码中代表了`é`，在希伯来语编码中却代表了字母`Gimel (ג)`，在俄语编码中又会代表另一个符号。但是不管怎样，所有这些编码方式中，0--127表示的符号是一样的，不一样的只是128--255的这一段。

至于亚洲国家的文字，使用的符号就更多了，汉字就多达10万左右。**一个字节只能表示256种符号，肯定是不够的，就必须使用多个字节表达一个符号**。比如，简体中文常见的编码方式是`GB2312`，使用**两个字节**表示一个汉字，所以理论上最多可以表示 **256 x 256 = 65536** 个符号。

> 注意，虽然都是用多个字节表示一个符号，但是GB类的汉字编码与 `Unicode` 和 `UTF-8` 是毫无关系的。


# Unicode

世界上存在着多种编码方式，同一个二进制数字可以被解释成不同的符号。因此，要想打开一个文本文件，就必须知道它的编码方式，否则用错误的编码方式解读，就会出现乱码。为什么电子邮件常常出现乱码？就是因为发信人和收信人使用的编码方式不一样。可以想象，如果有一种编码，将世界上所有的符号都纳入其中。每一个符号都给予一个独一无二的编码，那么乱码问题就会消失。这就是 Unicode，就像它的名字都表示的，这是一种所有符号的编码。Unicode 当然是一个很大的集合(Unicode 13.0 adds 5,930 characters, for a total of 143,859 characters, refer: http://www.unicode.org/versions/Unicode13.0.0/#Technical_Overview )。每个符号的编码都不一样，比如，`U+0639`表示阿拉伯字母`Ain`，`U+0041`表示英语的大写字母`A`，`U+4E25`表示汉字`严`。具体的符号对应表，可以查询`unicode.org`，或者专门的汉字对应表。

```
Unicode provides a unique number for every character,
no matter what the platform,
no matter what the program,
no matter what the language.
```

refer: https://www.unicode.org/standard/WhatIsUnicode.html

它从0开始，为每个符号指定一个编号，这叫做"码点"（code point）。比如，码点0的符号就是null（表示所有二进制位都是0）。

Unicode编号规则：

* 从0开始编号，u+0000 = null
* 最新版本：13.0.0（2020 March 10），Unicode 13.0 adds 5,930 characters, for a total of 143,859 characters.
* 全世界现有的符号当中，三分之二以上来自东亚文字。比如，中文"好"的码点（Code Point）是十六进制的`597D`。

```
// U+表示紧跟在后面的十六进制数是Unicode的码点
U+0000 = null
U+597D = 好
```
* Unicode只规定了每个字符的码点，到底用什么样的字节序表示这个码点，就涉及到编码方法。
* 最直观的编码方法是，每个码点使用四个字节表示，字节内容一一对应码点。这种编码方法就叫做`UTF-32`。比如，码点0就用四个字节的0表示，码点597D就在前面加两个字节的0。`UTF-32`的优点在于，转换规则简单直观，查找效率高。缺点在于浪费空间，同样内容的英语文本，它会比ASCII编码大四倍。这个缺点很致命，导致实际上没有人使用这种编码方法，HTML 5标准就明文规定，网页不得编码成`UTF-32`。

```
U+0000 = 0x0000 0000
U+597D = 0x0000 597D
```

* 人们真正需要的是一种节省空间的编码方法，这导致了`UTF-8`的诞生。`UTF-8`是一种变长的编码方法，字符长度从1个字节到4个字节不等。越是常用的字符，字节越短，最前面的128个字符，只使用1个字节表示，与`ASCII码`完全相同。

> JavaScript使用哪一种编码？

JavaScript语言采用Unicode字符集，但是只支持一种编码方法。这种编码既不是`UTF-16`，也不是`UTF-8`，更不是`UTF-32`。上面那些编码方法，JavaScript都不用。JavaScript用的是`UCS-2`！

互联网还没出现的年代，曾经有两个团队，不约而同想搞统一字符集。一个是1988年成立的Unicode团队，另一个是1989年成立的UCS团队。等到他们发现了对方的存在，很快就达成一致：世界上不需要两套统一字符集。1991年10月，两个团队决定合并字符集。也就是说，从今以后只发布一套字符集，就是Unicode，并且修订此前发布的字符集，UCS的码点将与Unicode完全一致。

UCS的开发进度快于Unicode，1990年就公布了第一套编码方法UCS-2，使用2个字节表示已经有码点的字符。（那个时候只有一个平面，就是基本平面，所以2个字节就够用了。）UTF-16编码迟至1996年7月才公布，明确宣布是UCS-2的超集，即基本平面字符沿用UCS-2编码，辅助平面字符定义了4个字节的表示方法。

两者的关系简单说，就是UTF-16取代了UCS-2，或者说UCS-2整合进了UTF-16。所以，现在只有UTF-16，没有UCS-2。

那么，为什么JavaScript不选择更高级的UTF-16，而用了已经被淘汰的UCS-2呢？答案很简单：非不想也，是不能也。因为在JavaScript语言出现的时候，还没有UTF-16编码。

1995年5月，Brendan Eich用了10天设计了JavaScript语言；10月，第一个解释引擎问世；次年11月，Netscape正式向ECMA提交语言标准（整个过程详见 [JavaScript诞生记](http://www.ruanyifeng.com/blog/2011/06/birth_of_javascript.html) ）。对比UTF-16的发布时间（1996年7月），就会明白Netscape公司那时没有其他选择，只有UCS-2一种编码方法可用！

JavaScript之后的ECMAScript 6（简称ES6）版本，大幅增强了Unicode支持，才可以正确识别字符（包括4字节的码点）。

> Java如何准确判断码点个数？

``` java
import java.io.*;
public class Test {

   public static void main(String args[]) {
      String Str1 = new String("Welcome to Tutorialspoint.com");
      String Str2 = new String("腾讯㘇" );

      System.out.print("String Length :" );
      System.out.println(Str1.length());

      System.out.print("String Length :" );
      System.out.println(Str2.length());

      System.out.print("Str2.codePointCount(0, Str2.length()):");
      System.out.println(Str2.codePointCount(0, Str2.length()));
   }
}
```

在[Compile and Execute Java Online (JDK 1.8.0)](https://www.tutorialspoint.com/compile_java_online.php)输出：

```
$javac Test.java
$java -Xmx128M -Xms16M Test
String Length :29
String Length :4
Str2.codePointCount(0, Str2.length()):3
```

refer:

* https://www.tutorialspoint.com/java/java_string_length.htm
* https://www.tutorialspoint.com/java/lang/string_codepointcount.htm

> Unicode 的问题？

需要注意的是，**Unicode 只是一个符号集，它只规定了符号的二进制代码，却没有规定这个二进制代码应该如何存储**。比如，汉字`严`的 Unicode 是十六进制数`4E25`，转换成二进制数足足有`15`位（`100111000100101`），也就是说，这个符号的表示至少需要2个字节。表示其他更大的符号，可能需要3个字节或者4个字节，甚至更多。

这里就有两个严重的问题：

* **第一个问题是，如何才能区别 Unicode 和 ASCII ？计算机怎么知道三个字节表示一个符号，而不是分别表示三个符号呢**？
* **第二个问题是，我们已经知道，英文字母只用一个字节表示就够了，如果 Unicode 统一规定，每个符号用三个或四个字节表示，那么每个英文字母前都必然有二到三个字节是0，这对于存储来说是极大的浪费，文本文件的大小会因此大出二三倍，这是无法接受的。**

它们造成的结果是：

* 出现了 Unicode 的多种存储方式，也就是说有许多种不同的二进制格式，可以用来表示 Unicode。
* Unicode 在很长一段时间内无法推广，直到互联网的出现。


refer:

* http://www.unicode.org/glossary/
* https://unicode.org/main.html
* http://www.unicode.org/versions/Unicode13.0.0/
* https://home.unicode.org/


# UTF-8

互联网的普及，强烈要求出现一种统一的编码方式。`UTF-8`就是在互联网上使用最广的一种`Unicode`的实现方式。其他实现方式还包括`UTF-16`（字符用两个字节或四个字节表示）和`UTF-32`（字符用四个字节表示），不过在互联网上基本不用。

> 注意，这里的关系是，UTF-8 是 Unicode 的实现方式之一。
> A Unicode code point is an integer (ranging, at the moment, from `U+0000` to `U+10FFFF`).
> 1 to 4 bytes are Unicode's UTF-8 encoding version.

UTF-8 最大的一个特点，就是它是**一种变长的编码方式。它可以使用1--4个字节表示一个符号，根据不同的符号而变化字节长度**。

> UTF-8 的编码规则

很简单，只有二条：

1. 对于**单字节**的符号，字节的第一位设为0，后面7位为这个符号的 Unicode 码。**因此对于英语字母，UTF-8 编码和 ASCII 码是相同的**。
2. 对于**n字节的符号**（n > 1），第一个字节的前n位都设为1，第n + 1位设为0，后面字节的前两位一律设为10。剩下的没有提及的二进制位，全部为这个符号的 Unicode 码。


下表总结了编码规则，字母x表示可用编码的位。

| Unicode符号范围(十六进制) | UTF-8编码方式（二进制）
| -- | --
| 0000 0000-0000 007F | 0xxxxxxx
| 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
| 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
| 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx


跟据上表，解读 UTF-8 编码非常简单。如果一个字节的第一位是0，则这个字节单独就是一个字符；如果第一位是1，则连续有多少个1，就表示当前字符占用多少个字节。

例子：以汉字`严`为例，演示如何实现 UTF-8 编码

`严`的 Unicode 是`\u4E25`（100 1110 0010 0101），根据上表，可以发现4E25处在第三行的范围内（0000 0800 - 0000 FFFF），因此`严`的 UTF-8 编码需要**三个字节**，即格式是`1110xxxx 10xxxxxx 10xxxxxx`。然后，从`严`的最后一个二进制位开始，依次从后向前填入格式中的x，多出的位补0。这样就得到了，`严`的 UTF-8 编码是`11100100 10111000 10100101`，转换成十六进制就是`E4B8A5`。


|字符|UTF-8编码10进制|UTF-8编码16进制|Unicode编码10进制|Unicode编码16进制
| -- | -- | -- | -- | --
| 严 | 14989477 | E4B8A5 | 20005 | 4E25


> Unicode 与 UTF-8 之间的转换

通过上一节的例子，可以看到严的 Unicode码 是`4E25`，UTF-8 编码是`E4B8A5`，两者是不一样的。它们之间的转换可以通过程序实现。

UTF-8编码转换：http://www.mytju.com/classcode/tools/encode_utf8.asp

> Little endian 和 Big endian

UCS-2编码（用两个字节存入字符的 Unicode 码） 格式可以存储 Unicode 码（码点不超过`0xFFFF`）。以汉字`严`为例，Unicode 码是`4E25`，需要用两个字节存储，一个字节是`4E`，另一个字节是`25`。存储的时候，`4E`在前，`25`在后，这就是**Big endian**方式；`25`在前，`4E`在后，这是**Little endian**方式。

```
这两个古怪的名称来自英国作家斯威夫特的《格列佛游记》。在该书中，小人国里爆发了内战，战争起因是人们争论，吃鸡蛋时究竟是从大头(Big-endian)敲开还是从小头(Little-endian)敲开。为了这件事情，前后爆发了六次战争，一个皇帝送了命，另一个皇帝丢了王位。
```

第一个字节在前，就是"大头方式"（Big endian），第二个字节在前就是"小头方式"（Little endian）。

那么很自然的，就会出现一个问题：计算机怎么知道某一个文件到底采用哪一种方式编码？

Unicode 规范定义，每一个文件的最前面分别加入一个**表示编码顺序的字符**，这个字符的名字叫做**"零宽度非换行空格"（zero width no-break space）**，用`FEFF`表示。这正好是两个字节，而且`FF`比`FE`大1。如果一个文本文件的头两个字节是`FE FF`，就表示该文件采用**大头方式**；如果头两个字节是`FF FE`，就表示该文件采用**小头方式**。

> 测试验证

在Windows上打开"记事本"程序notepad.exe，新建一个文本文件，内容就是一个`严`字，依次采用`ANSI`，`Unicode`，`Unicode big endian`和`UTF-8`编码方式保存。然后，用文本编辑软件UltraEdit 中的"十六进制功能"，观察该文件的内部编码方式。

* ANSI：文件的编码就是两个字节`D1 CF`，这正是严的 GB2312 编码，这也暗示 GB2312 是采用大头方式存储的。
* Unicode：编码是四个字节`FF FE 25 4E`，其中FF FE表明是小头方式存储，真正的编码是`4E25`。
* Unicode big endian：编码是四个字节`FE FF 4E 25`，其中FE FF表明是大头方式存储。
* UTF-8：编码是六个字节`EF BB BF E4 B8 A5`，前三个字节`EF BB BF`表示这是UTF-8编码，后三个`E4B8A5`就是严的具体编码，它的存储顺序与编码顺序是一致的。

refer:

* [The Absolute Minimum Every Software Developer Absolutely, Positively Must Know About Unicode and Character Sets](https://www.joelonsoftware.com/2003/10/08/the-absolute-minimum-every-software-developer-absolutely-positively-must-know-about-unicode-and-character-sets-no-excuses/)
* [程序员趣味读物：谈谈Unicode编码](https://www.pconline.com.cn/pcedu/empolder/gj/other/0505/616631.html)
* [RFC3629：UTF-8, a transformation format of ISO 10646（如果实现UTF-8的规定）](https://www.ietf.org/rfc/rfc3629.txt)



# URL encoding (percent-encoding)

## What is URL encoding?

**URL encoding** stands for encoding certain characters in a URL by replacing them with one or more character **triplets** that consist of the percent character "%" followed by two hexadecimal digits. The two hexadecimal digits of the triplet(s) represent the numeric value of the replaced character.

The term URL encoding is a bit **inexact（不准确）** because the encoding procedure is not limited to URLs ([Uniform Resource Locators](http://en.wikipedia.org/wiki/Url)), but can also be applied to any other URIs ([Uniform Resource Identifiers](http://en.wikipedia.org/wiki/Uniform_Resource_Identifier)) such as URNs ([Uniform Resource Names](http://en.wikipedia.org/wiki/Uniform_Resource_Name)). Therefore, the term **percent-encoding** should be preferred.

## Types of URI characters

The characters allowed in a URI are either **reserved** or **unreserved** (or a percent character as part of a percent-encoding).

> Percent-encoding reserved characters

When a character from the reserved set (a "reserved character") has special meaning (a "reserved purpose") in a certain context, and a URI scheme says that it is necessary to use that character for some other purpose, then the character must be percent-encoded. Percent-encoding a reserved character involves converting the character to its corresponding byte value in ASCII and then representing that value as a pair of hexadecimal digits. The digits, preceded by a percent sign (%) which is used as an [escape character], are then used in the URI in place of the reserved character. (For a non-ASCII character, it is typically converted to its byte sequence in [UTF-8], and then each byte value is represented. 例如，`你`对应的UTF-8编码是`\u4F60`，对应的URL编码是`%u4F60`)

The reserved character /, for example, if used in the "path" component of a URI, has the special meaning of being a [delimiter] between path segments. If, according to a given URI scheme, / needs to be in a path segment, then the three characters %2F or %2f must be used in the segment instead of a raw /.

**Reserved characters after percent-encoding**

| !	 | # |	$ |	& | ' |	( |	) | * | + | , | / | : | ; | = | ? | @ | [ | ]
| -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | --
| %21 | %23 | %24 | %26 | %27 | %28 | %29 | %2A | %2B | %2C | %2F | %3A | %3B | %3D | %3F | %40 | %5B | %5D


> Percent-encoding unreserved characters

Characters from the unreserved set never need to be percent-encoded.


## When and why would you use URL encoding?

When data that has been entered into HTML forms is submitted, the form field names and values are encoded and sent to the server in an HTTP request message using method GET or POST, or, historically, via email. The encoding used by default is based on a very early version of the general URI percent-encoding rules, with a number of modifications such as newline normalization and replacing spaces with "+" instead of "%20". The MIME type of data encoded this way is **application/x-www-form-urlencoded**, and it is currently defined (still in a very outdated manner) in the HTML and XForms specifications. In addition, the CGI specification contains rules for how web servers decode data of this type and make it available to applications.

When sent in an HTTP GET request, application/x-www-form-urlencoded data is included in the query component of the request URI. When sent in an HTTP POST request or via email, the data is placed in the body of the message, and the name of the media type is included in the message's Content-Type header.


# Base64

Base64 is used to turn binary data into text (encode) or text into binary (decode). To allow binary data to be transmitted with textual data it must be encoded. An example of this is an attachment in an email. This is done via the [MIME] implementation of Base64. The MIME implementation uses the characters `A-Z`, `a-z`, and `0-9` for the initial **62 values** that it can use to encode data.

For example, the text:

21个字符

```
Dan's Tools are cool!
```

Would be encoded as...

28个字符（总长度膨胀了33%）

```
RGFuJ3MgVG9vbHMgYXJlIGNvb2wh
```

## What is Base64 encoding?

The term **Base 64** is generic, and there are many implementations. [MIME], which stands for Multi-Purpose Internet Mail Extensions, is the most common that is seen today. It is used to transmit attachments via email over the Simple Mail Transfer Protocol (SMTP). Other examples of Base64 encoding are Radix-64 and YUI's Y64. Encoding data in Base64 results in it taking up roughly `33%` more space than the original data.

MIME Base64 encoding is the most common, and is based on the [RFC 1420] specification. It also uses a `=` character at the end of a string to signify whether the last character is a single or double byte.

## When and why would you use Base64 encoding?

You should use Base64 whenever you intend to transmit binary data in a textual format.


# Refer

* [Percent-encoding]
* [URL Encode and Decode From Dan's Tools]
* [Base64]
* [Base64 Encode From Dan's Tools]
* [字符编码笔记：ASCII，Unicode 和 UTF-8]
* [Unicode与JavaScript详解]
* http://www.blooberry.com/indexdot/html/topics/urlencoding.htm

[Percent-encoding]: https://en.wikipedia.org/wiki/Percent-encoding
[URL Encode and Decode From Dan's Tools]: https://www.url-encode-decode.com/
[RFC 3986]: (https://tools.ietf.org/html/rfc3986)
[ASCII]: https://en.wikipedia.org/wiki/ASCII
[escape character]: https://en.wikipedia.org/wiki/Escape_character
[delimiter]: https://en.wikipedia.org/wiki/Delimiter
[UTF-8]: https://en.wikipedia.org/wiki/UTF-8
[Base64]: https://en.wikipedia.org/wiki/Base64#Radix_64_applications_not_compatible_with_Base64
[Base64 Encode From Dan's Tools]: https://www.cleancss.com/base64-encode/?_ga=2.229358876.1655496866.1554943667-999204454.1554943666
[MIME]: http://en.wikipedia.org/wiki/MIME
[RFC 1420]: http://tools.ietf.org/html/rfc1421
[字符编码笔记：ASCII，Unicode 和 UTF-8]: http://www.ruanyifeng.com/blog/2007/10/ascii_unicode_and_utf-8.html
[Unicode与JavaScript详解]: https://www.ruanyifeng.com/blog/2014/12/unicode.html


