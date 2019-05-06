---
layout: post
title:  "常用编码方法"
date:   2019-04-11 08:00:00 +0800
categories: encoding
---

* Do not remove this line (it will not be displayed)
{:toc}


# URL encoding (percent-encoding)

## What is URL encoding?

**URL encoding** stands for encoding certain characters in a URL by replacing them with one or more character **triplets** that consist of the percent character "%" followed by two hexadecimal digits. The two hexadecimal digits of the triplet(s) represent the numeric value of the replaced character.

The term URL encoding is a bit **inexact** because the encoding procedure is not limited to URLs ([Uniform Resource Locators](http://en.wikipedia.org/wiki/Url)), but can also be applied to any other URIs ([Uniform Resource Identifiers](http://en.wikipedia.org/wiki/Uniform_Resource_Identifier)) such as URNs ([Uniform Resource Names](http://en.wikipedia.org/wiki/Uniform_Resource_Name)). Therefore, the term **percent-encoding** should be preferred.

## Types of URI characters

The characters allowed in a URI are either **reserved** or **unreserved** (or a percent character as part of a percent-encoding). 

> Percent-encoding reserved characters

When a character from the reserved set (a "reserved character") has special meaning (a "reserved purpose") in a certain context, and a URI scheme says that it is necessary to use that character for some other purpose, then the character must be percent-encoded. Percent-encoding a reserved character involves converting the character to its corresponding byte value in ASCII and then representing that value as a pair of hexadecimal digits. The digits, preceded by a percent sign (%) which is used as an [escape character], are then used in the URI in place of the reserved character. (For a non-ASCII character, it is typically converted to its byte sequence in [UTF-8], and then each byte value is represented as above.)

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

```
Dan's Tools are cool!
```

Would be encoded as...

```
RGFuJ3MgVG9vbHMgYXJlIGNvb2wh
```

## What is Base64 encoding?

The term **Base 64** is generic, and there are many implementations. [MIME], which stands for Multi-Purpose Internet Mail Extensions, is the most common that is seen today. It is used to transmit attachments via email over the Simple Mail Transfer Protocol (SMTP). Other examples of Base64 encoding are Radix-64 and YUI's Y64. Encoding data in Base64 results in it taking up roughly `33%` more space than the original data.

MIME Base64 encoding is the most common, and is based on the [RFC 1420] specification. It also uses a = character at the end of a string to signify whether the last character is a single or double byte.

## When and why would you use Base64 encoding?

You should use Base64 whenever you intend to transmit binary data in a textual format.


# Refer

1. [Percent-encoding]
2. [URL Encode and Decode From Dan's Tools]
3. [Base64]
4. [Base64 Encode From Dan's Tools]
5. [字符编码笔记：ASCII，Unicode 和 UTF-8]

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
