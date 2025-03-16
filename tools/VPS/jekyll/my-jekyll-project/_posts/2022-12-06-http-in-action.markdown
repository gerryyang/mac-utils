---
layout: post
title:  "HTTP in Action"
date:   2022-12-06 12:00:00 +0800
categories: [TCP/IP]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Hypertext Transfer Protocol

The [Hypertext Transfer Protocol](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol) (**HTTP**) is an [application layer](https://en.wikipedia.org/wiki/Application_layer) protocol in the [Internet protocol suite](https://en.wikipedia.org/wiki/Internet_protocol_suite) model for distributed, collaborative, [hypermedia](https://en.wikipedia.org/wiki/Hypermedia) information systems.

HTTP is the foundation of data communication for the [World Wide Web](https://en.wikipedia.org/wiki/World_Wide_Web), where [hypertext](https://en.wikipedia.org/wiki/Hypertext) documents include [hyperlinks](https://en.wikipedia.org/wiki/Hyperlink) to other resources that the user can easily access, for example by a mouse click or by tapping the screen in a web browser.

## HTTP/1

HTTP/1 was finalized and fully documented (as version 1.0) in 1996. It evolved (as version 1.1) in 1997 and then its specifications were updated in 1999, 2014, and 2022.

## HTTP/2

HTTP/2, published in 2015, provides a more efficient expression of HTTP's semantics "on the wire". It is now used by 41% of websites and supported by almost all web browsers (over 97% of users).

It is also supported by major web servers over [Transport Layer Security](https://en.wikipedia.org/wiki/Transport_Layer_Security) (**TLS**) using an [Application-Layer Protocol Negotiation](https://en.wikipedia.org/wiki/Application-Layer_Protocol_Negotiation) (**ALPN**) extension where TLS 1.2 or newer is required.

## HTTP/3

[HTTP/3](https://en.wikipedia.org/wiki/HTTP/3), the successor to HTTP/2, was published in 2022. It is now used by over 25% of websites and is supported by many web browsers (over 75% of users).

HTTP/3 uses [QUIC](https://en.wikipedia.org/wiki/QUIC) instead of [TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol) for the underlying transport protocol.

Like HTTP/2, it does not obsolesce previous major versions of the protocol. Support for HTTP/3 was added to Cloudflare and Google Chrome first, and is also enabled in Firefox. HTTP/3 has lower latency for real-world web pages, if enabled on the server, load faster than with HTTP/2, and even faster than HTTP/1.1, in some cases **over 3× faster than HTTP/1.1** (which is still commonly only enabled).


## HTTPS

Its secure variant named [HTTPS](https://en.wikipedia.org/wiki/HTTPS) is used by more than 80% of websites.


# URL (Uniform Resource Locator)

A [Uniform Resource Locator](https://en.wikipedia.org/wiki/URL) (**URL**), colloquially termed a **web address**, is a reference to a [web resource](https://en.wikipedia.org/wiki/Web_resource) that specifies its location on a [computer network](https://en.wikipedia.org/wiki/Computer_network) and a mechanism for retrieving it.

A **URL** is a specific type of Uniform Resource Identifier (**URI**), although many people use the two terms interchangeably. **URLs** occur most commonly to reference [web pages](https://en.wikipedia.org/wiki/Web_page) ([HTTP](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol)) but are also used for file transfer (FTP), email (mailto), database access (JDBC), and many other applications.


# URI (Uniform Resource Identifier)

A [Uniform Resource Identifier](https://en.wikipedia.org/wiki/Uniform_Resource_Identifier) (**URI**) is a unique sequence of characters that identifies a logical or physical resource used by web technologies. URIs may be used to identify anything, including real-world objects, such as people and places, concepts, or information resources such as web pages and books.


# Query string

A [query string](https://en.wikipedia.org/wiki/Query_string) is a part of a uniform resource locator (**URL**) that assigns values to specified parameters. A query string commonly includes fields added to a base URL by a Web browser or other client application, for example as part of an HTML, choosing the appearance of a page, or jumping to positions in multimedia content.

```
https://en.wikipedia.org/w/index.php?title=Query_string&action=edit
```

An address bar on Google Chrome showing a URL with the query string `title=Query_string&action=edit`

# URL encoding

Some characters cannot be part of a URL (for example, the space) and some other characters have a special meaning in a URL: for example, the character `#` can be used to further specify a subsection (or fragment) of a document. In HTML forms, the character `=` is used to separate a name from a value. The URI generic syntax uses [URL encoding](https://en.wikipedia.org/wiki/Percent-encoding#Percent-encoding_reserved_characters) to deal with this problem.


# POST (有状态，不满足幂等)

In computing, [POST](https://en.wikipedia.org/wiki/POST_(HTTP)) is a request method supported by HTTP used by the World Wide Web. By design, the POST request method requests that a web server accept the data enclosed in the body of the request message, most likely for storing it. It is often used when uploading a file or when submitting a completed web form.

In contrast, the HTTP [GET](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol#Request_methods) request method retrieves information from the server. As part of a GET request, some data can be passed within the URL's query string, specifying (for example) search terms, date ranges, or other information that defines the query.

As part of a POST request, an arbitrary amount of data of any type can be sent to the server in the body of the request message. A [header field](https://en.wikipedia.org/wiki/List_of_HTTP_header_fields) in the POST request usually indicates the message body's Internet media type.

# GET (无状态，幂等的)

The GET method requests that the target resource transfer a representation of its state. GET requests should only retrieve data and should have no other effect. (This is also true of some other HTTP methods.) For retrieving resources without making changes, GET is preferred over POST, as they can be addressed through a URL. This enables bookmarking and sharing and makes GET responses eligible for caching, which can save bandwidth.


#  Internet media type (application/x-www-form-urlencoded)

When a web browser sends a **POST** request from a web form element, the default [Internet media type](https://en.wikipedia.org/wiki/Internet_media_type) is "[application/x-www-form-urlencoded](https://en.wikipedia.org/wiki/Application/x-www-form-urlencoded)". This is a format for encoding [key-value pairs](https://en.wikipedia.org/wiki/Associative_array) with possibly duplicate keys. Each key-value pair is separated by an '&' character, and each key is separated from its value by an '=' character. Keys and values are both escaped by replacing spaces with the '+' character and then using [percent-encoding](https://en.wikipedia.org/wiki/Percent-encoding) on all other non-alphanumeric characters.

For example, the key-value pairs

```
Name: Gareth Wylie
Age: 24
Formula: a+b == 21
```

are encoded as

```
Name=Gareth+Wylie&Age=24&Formula=a%2Bb+%3D%3D+21
```


# Compatibility issues

According to the HTTP specification:

> Various ad hoc limitations on request-line length are found in practice. It is RECOMMENDED that all HTTP senders and recipients support, at a minimum, request-line lengths of 8000 octets (字节).

If the URL is too long, the web server fails with the [414 Request-URI Too Long](https://en.wikipedia.org/wiki/List_of_HTTP_status_codes#414) HTTP status code.

The common workaround for these problems is to use [POST](https://en.wikipedia.org/wiki/POST_(HTTP)) instead of [GET](https://en.wikipedia.org/wiki/GET_(HTTP)) and store the parameters in the request body. The length limits on request bodies are typically much higher than those on URL length. For example, the limit on POST size, by default, is 2 MB on IIS 4.0 and 128 KB on IIS 5.0. The limit is configurable on Apache2 using the `LimitRequestBody` directive, which specifies the number of bytes from 0 (meaning unlimited) to 2147483647 (2 GB) that are allowed in a request body.

**There are times when HTTP GET is less suitable even for data retrieval**. An example of this is when a great deal of data would need to be specified in the URL. Browsers and web servers can have limits on the length of the URL that they will handle without truncation or error. **Percent-encoding of reserved characters in URLs and query strings can significantly increase their length, and while Apache HTTP Server can handle up to 4,000 characters in a URL, Microsoft Internet Explorer is limited to 2,048 characters in any URL.** Equally, **HTTP GET should not be used where sensitive information**, such as usernames and passwords, have to be submitted along with other data for the request to complete. Even if **HTTPS** is used, preventing the data from being intercepted in transit, the browser history and the web server's logs will likely contain the full URL in plaintext, which may be exposed if either system is hacked. **In these cases, HTTP POST should be used**.

# Refer

* https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
* https://developer.mozilla.org/zh-CN/docs/Web/HTTP


