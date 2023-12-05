---
layout: post
title:  "HTTP Parser in Action"
date:   2022-03-24 14:00:00 +0800
categories: [TCP/IP]
---

* Do not remove this line (it will not be displayed)
{:toc}


Port of [http_parser](https://github.com/nodejs/http-parser) to [llparse](https://github.com/nodejs/llparse). More details in [llhttp - new HTTP 1.1 parser for Node.js by Fedor Indutny JSConf EU 2019](https://youtu.be/x3k_5Mi66sY), and [the slides and the presentations](https://indutny.github.io/jsconfeu-2019/reveal.js/index.html#/). (视频中，llhttp作者讲述了http_parser的历史，以及为什么开发新的llhttp)

> Node.js has been using a derivative of nginx’s parser with a lot of customization/rewrite since its inception. Despite being fast enough, the project architecture made it very hard to maintain in a long run. To mitigate that, the author has created a tool to generate the new HTTP parser called “llhttp” from the TypeScript code in understandable, verifiable, and maintainable way. Incidentally, the performance of “llhttp” is two times better than of the old parser. In this talk we’ll walk through the basics of generating such parsers and how “llhttp” works.

# Original parser (HTTP Parser)

## Pros (优势)

* "Good enough" performance
* Supports spec-violating clients
* Has lots of tests

## Cons (劣势)

* Rigidity of codebase
* Impossible to maintain
* Vulnerability-prone

## Original scanning

* "for" loop over input

Scanning means no buffering, so, it doesn't allocate memory itself. And it creates especially for request bodies because it could just need the slices of original buffers that came from that work instead of allocating the copying data. So, in the core principle of the HTTP parser, it's not copying.

* **huge** "switch" for states

All of this lived in a single function or 1,000 lines of code which is quite a terrible idea.

# llhttp

* Next major version
* Same principles
* Similar API


## New scanning

* Break "switch" into pieces
* Each piece has precise action
* Use "goto" between states

So, an obvious improvement would be to break this switch into pieces and make it such that each piece has precise action. It's sort of a unique philosophy. So, it would be just exactly about doing one small thing at a time. Go to statements would be used to jump between states. There would not be as much need for this for-loop at least not as much.


## DSL

* llhttp is a TS program
* Different sub-parsers in different files
* llparse transpiles TS program to C

DSL, No syntax checking, JS engine (V8) handles it!

* llparse, Builds a graph of states
* llparse, Can do static analysis
  + Infinite loop check
  + "peephole" optimizations
* llparse, Can generate different outputs: C and LLVM bitcode
* llparse, Not hand-written! Not hand-optimized! is **2x faster**!

## Numbers (性能数据)

| llhttp | http_parser
| -- | --
| 3,020,459 RPS | 1,406,180 RPS

This llhttp parser is a default in Node version 12.

## Tests

* All original tests ported to markdown. Easy to read, easy to contribute. In-test textual description

## More optimizations

* vector insttructions (CPU)
* Unified docs (Quite ashamedly, the project is not well documented)
* Different parsers? e.g., SMTP, POP3

# HTTP Parser (用法)

http-parser [is not actively maintained](https://github.com/nodejs/http-parser/issues/522). New projects and projects looking to migrate should consider [llhttp](https://github.com/nodejs/llhttp).

```
Maintainer notice #522


I'm moving on and as the last (semi-)active maintainer, that means http-parser is now effectively unmaintained.

It's a stable project for a stable protocol and I don't expect many issues but you should take the above into account when evaluating whether http-parser is the right choice for you.

For new projects and projects looking to migrate, I recommend https://github.com/nodejs/llhttp. Similar API, feature parity, well maintained and in active use.
```

This is a parser for HTTP messages written in C. It parses both requests and responses. The parser is designed to be used in performance HTTP applications. It does not make any syscalls nor allocations, it does not buffer data, it can be interrupted at anytime. Depending on your architecture, it only requires about 40 bytes of data per message stream (in a web server that is per connection).

Features:

* No dependencies
* Handles persistent streams (keep-alive).
* Decodes chunked encoding.
* Upgrade support
* Defends against buffer overflow attacks.

The parser extracts the following information from HTTP messages:

* Header fields and values
* Content-Length
* Request method
* Response status code
* Transfer-Encoding
* HTTP version
* Request URL
* Message body

## Usage

One `http_parser` object is used per TCP connection. Initialize the struct using `http_parser_init()` and set the callbacks. That might look something like this for a request parser:

``` cpp
http_parser_settings settings;
settings.on_url = my_url_callback;
settings.on_header_field = my_header_field_callback;
/* ... */

http_parser *parser = malloc(sizeof(http_parser));
http_parser_init(parser, HTTP_REQUEST);
parser->data = my_socket;
```

When data is received on the socket execute the parser and check for errors.

``` cpp
size_t len = 80*1024, nparsed;
char buf[len];
ssize_t recved;

recved = recv(fd, buf, len, 0);

if (recved < 0) {
  /* Handle error. */
}

/* Start up / continue the parser.
 * Note we pass recved==0 to signal that EOF has been received.
 */
nparsed = http_parser_execute(parser, &settings, buf, recved);

if (parser->upgrade) {
  /* handle new protocol */
} else if (nparsed != recved) {
  /* Handle error. Usually just close the connection. */
}
```

`http_parser` needs to know where the end of the stream is. For example, sometimes servers send responses without Content-Length and expect the client to consume input (for the body) until EOF. To tell `http_parser` about EOF, give `0` as the fourth parameter to `http_parser_execute()`. Callbacks and errors can still be encountered during an EOF, so one must still be prepared to receive them.

Scalar valued message information such as `status_code`, `method`, and the HTTP version are stored in the parser structure. This data is only temporally stored in `http_parser` and gets reset on each new message. If this information is needed later, copy it out of the structure during the `headers_complete` callback.

The parser decodes the transfer-encoding for both requests and responses transparently. That is, a chunked encoding is decoded before being sent to the on_body callback.

## The Special Problem of Upgrade

`http_parser` supports upgrading the connection to a different protocol. An increasingly common example of this is the WebSocket protocol which sends a request like

```
    GET /demo HTTP/1.1
    Upgrade: WebSocket
    Connection: Upgrade
    Host: example.com
    Origin: http://example.com
    WebSocket-Protocol: sample
```

followed by non-HTTP data.

(See [RFC6455](https://tools.ietf.org/html/rfc6455) for more information the WebSocket protocol.)

To support this, the parser will treat this as a normal HTTP message without a body, issuing both `on_headers_complete` and `on_message_complete` callbacks. However `http_parser_execute()` will stop parsing at the end of the headers and return.

The user is expected to check if `parser->upgrade` has been set to `1` after `http_parser_execute()` returns. Non-HTTP data begins at the buffer supplied offset by the return value of `http_parser_execute()`.

## Callbacks

During the `http_parser_execute()` call, the callbacks set in `http_parser_settings` will be executed. The parser maintains state and never looks behind, so buffering the data is not necessary. If you need to save certain data for later usage, you can do that from the callbacks.

There are two types of callbacks:

* notification `typedef int (*http_cb) (http_parser*);` Callbacks: `on_message_begin`, `on_headers_complete`, `on_message_complete`.
* data `typedef int (*http_data_cb) (http_parser*, const char *at, size_t length);` Callbacks: (requests only) `on_url`, (common) `on_header_field`, `on_header_value`, `on_body`;

Callbacks must return 0 on success. Returning a non-zero value indicates error to the parser, making it exit immediately.

For cases where it is necessary to pass local information to/from a callback, the `http_parser` object's `data` field can be used. An example of such a case is when using threads to handle a socket connection, parse a request, and then give a response over that socket. By instantiation of a thread-local struct containing relevant data (e.g. accepted socket, allocated memory for callbacks to write into, etc), a parser's callbacks are able to communicate data between the scope of the thread and the scope of the callback in a threadsafe manner. This allows `http_parser` to be used in multi-threaded contexts.

Example:

``` cpp
 typedef struct {
  socket_t sock;
  void* buffer;
  int buf_len;
 } custom_data_t;


int my_url_callback(http_parser* parser, const char *at, size_t length) {
  /* access to thread local custom_data_t struct.
  Use this access save parsed data for later use into thread local
  buffer, or communicate over socket
  */
  parser->data;
  ...
  return 0;
}

...

void http_parser_thread(socket_t sock) {
 int nparsed = 0;
 /* allocate memory for user data */
 custom_data_t *my_data = malloc(sizeof(custom_data_t));

 /* some information for use by callbacks.
 * achieves thread -> callback information flow */
 my_data->sock = sock;

 /* instantiate a thread-local parser */
 http_parser *parser = malloc(sizeof(http_parser));
 http_parser_init(parser, HTTP_REQUEST); /* initialise parser */
 /* this custom data reference is accessible through the reference to the
 parser supplied to callback functions */
 parser->data = my_data;

 http_parser_settings settings; /* set up callbacks */
 settings.on_url = my_url_callback;

 /* execute parser */
 nparsed = http_parser_execute(parser, &settings, buf, recved);

 ...
 /* parsed information copied from callback.
 can now perform action on data copied into thread-local memory from callbacks.
 achieves callback -> thread information flow */
 my_data->buffer;
 ...
}
```

In case you parse HTTP message in chunks (i.e. `read()` request line from socket, parse, read half headers, parse, etc) your data callbacks may be called more than once. `http_parser` guarantees that data pointer is only valid for the lifetime of callback. You can also `read()` into a heap allocated buffer to avoid copying memory around if this fits your application.

Reading headers may be a tricky task if you read/parse headers partially. Basically, you need to remember whether last header callback was field or value and apply the following logic:

```
(on_header_field and on_header_value shortened to on_h_*)
 ------------------------ ------------ --------------------------------------------
| State (prev. callback) | Callback   | Description/action                         |
 ------------------------ ------------ --------------------------------------------
| nothing (first call)   | on_h_field | Allocate new buffer and copy callback data |
|                        |            | into it                                    |
 ------------------------ ------------ --------------------------------------------
| value                  | on_h_field | New header started.                        |
|                        |            | Copy current name,value buffers to headers |
|                        |            | list and allocate new buffer for new name  |
 ------------------------ ------------ --------------------------------------------
| field                  | on_h_field | Previous name continues. Reallocate name   |
|                        |            | buffer and append callback data to it      |
 ------------------------ ------------ --------------------------------------------
| field                  | on_h_value | Value for current header started. Allocate |
|                        |            | new buffer and copy callback data to it    |
 ------------------------ ------------ --------------------------------------------
| value                  | on_h_value | Value continues. Reallocate value buffer   |
|                        |            | and append callback data to it             |
 ------------------------ ------------ --------------------------------------------
```

http_parser.h

``` cpp
/* Callbacks should return non-zero to indicate an error. The parser will
 * then halt execution.
 *
 * The one exception is on_headers_complete. In a HTTP_RESPONSE parser
 * returning '1' from on_headers_complete will tell the parser that it
 * should not expect a body. This is used when receiving a response to a
 * HEAD request which may contain 'Content-Length' or 'Transfer-Encoding:
 * chunked' headers that indicate the presence of a body.
 *
 * Returning `2` from on_headers_complete will tell parser that it should not
 * expect neither a body nor any futher responses on this connection. This is
 * useful for handling responses to a CONNECT request which may not contain
 * `Upgrade` or `Connection: upgrade` headers.
 *
 * http_data_cb does not return data chunks. It will be called arbitrarily
 * many times for each string. E.G. you might get 10 callbacks for "on_url"
 * each providing just a few characters more data.
 */
typedef int (*http_data_cb) (http_parser*, const char *at, size_t length);
typedef int (*http_cb) (http_parser*);
```

## Parsing URLs

A simplistic zero-copy URL parser is provided as `http_parser_parse_url()`. Users of this library may wish to use it to parse URLs constructed from consecutive `on_url` callbacks.

See examples of reading in headers:

* [partial example](http://gist.github.com/155877) in C
* [from http-parser tests](http://github.com/joyent/http-parser/blob/37a0ff8/test.c#L403) in C
* [from Node library](http://github.com/joyent/node/blob/842eaf4/src/http.js#L284) in Javascript

# llhttp (用法)

Port of [http_parser](https://github.com/nodejs/http-parser) to [llparse](https://github.com/nodejs/llparse).

## Why?

Let's face it, http_parser is practically unmaintainable. Even introduction of a single new method results in a significant code churn.

This project aims to:

Make it maintainable
Verifiable
Improving benchmarks where possible
More details in [Fedor Indutny's talk at JSConf EU 2019](https://youtu.be/x3k_5Mi66sY)


## How?

Over time, different approaches for improving http_parser's code base were tried. However, all of them failed due to resulting significant performance degradation.

This project is a port of http_parser to TypeScript. [llparse](https://github.com/nodejs/llparse) is used to generate the output C source file, which could be compiled and linked with the embedder's program (like [Node.js](https://github.com/nodejs/node)).


## Performance

So far `llhttp` **outperforms** `http_parser`:

|                 | input size |  bandwidth   |  reqs/sec  |   time  |
|:----------------|-----------:|-------------:|-----------:|--------:|
| **llhttp**      | 8192.00 mb | 1777.24 mb/s | 3583799.39 req/sec | 4.61 s |
| **http_parser** | 8192.00 mb | 694.66 mb/s | 1406180.33 req/sec | 11.79 s |

llhttp is faster by approximately **156%**.

## Maintenance

llhttp project has about 1400 lines of TypeScript code describing the parser itself and around 450 lines of C code and headers providing the helper methods. The whole http_parser is implemented in approximately 2500 lines of C, and 436 lines of headers.

All optimizations and multi-character matching in llhttp are generated automatically, and thus doesn't add any extra maintenance cost. On the contrary, most of http_parser's code is hand-optimized and unrolled. Instead describing "how" it should parse the HTTP requests/responses, a maintainer should implement the new features in http_parser cautiously, considering possible performance degradation and manually optimizing the new code.

## Verification

The state machine graph is encoded explicitly in llhttp. The [llparse](https://github.com/nodejs/llparse) automatically checks the graph for absence of loops and correct reporting of the input ranges (spans) like header names and values. In the future, additional checks could be performed to get even stricter verification of the llhttp.

## Usage

``` cpp
#include "llhttp.h"

llhttp_t parser;
llhttp_settings_t settings;

/* Initialize user callbacks and settings */
llhttp_settings_init(&settings);

/* Set user callback */
settings.on_message_complete = handle_on_message_complete;

/* Initialize the parser in HTTP_BOTH mode, meaning that it will select between
 * HTTP_REQUEST and HTTP_RESPONSE parsing automatically while reading the first
 * input.
 */
llhttp_init(&parser, HTTP_BOTH, &settings);

/* Parse request! */
const char* request = "GET / HTTP/1.1\r\n\r\n";
int request_len = strlen(request);

enum llhttp_errno err = llhttp_execute(&parser, request, request_len);
if (err == HPE_OK) {
  /* Successfully parsed! */
} else {
  fprintf(stderr, "Parse error: %s %s\n", llhttp_errno_name(err),
          parser.reason);
}
```

For more information on API usage, please refer to [src/native/api.h](https://github.com/nodejs/llhttp/blob/main/src/native/api.h).

## Build Instructions

Make sure you have [Node.js](https://nodejs.org/), npm and npx installed. Then under project directory run:

```
npm install
make
```

### Using with CMake

If you want to use this library in a CMake project you can use the snippet below.

```
FetchContent_Declare(llhttp
  URL "https://github.com/nodejs/llhttp/releases/download/v6.0.5/llhttp-release-v6.0.5.tar.gz")  # Using version 6.0.5

FetchContent_MakeAvailable(llhttp)

target_link_libraries(${EXAMPLE_PROJECT_NAME} ${PROJECT_LIBRARIES} llhttp ${PROJECT_NAME})
```

## llparse

Generating parsers in LLVM IR. [llparse.org](https://llparse.org/)

An API for compiling an incremental parser into a C output.

# PicoHTTPParser

* https://github.com/h2o/picohttpparser
* https://github.com/h2o/picohttpparser#benchmark


Parsing an HTTP request header 100000 times. [测试数据](https://github.com/fukamachi/fast-http/tree/6b9110347c7a3407310c08979aefd65078518478)

In this benchmark, fast-http is **6.4 times** faster than `http-parser`, a C equivalent.

| http-parser (C) |	fast-http
| -- | --
| 0.289s | 0.045s


The library exposes four functions: `phr_parse_request`, `phr_parse_response`, `phr_parse_headers`, `phr_decode_chunked`.

`phr_parse_request` 使用示例：


The example below reads an HTTP request from socket sock using read(2), parses it using phr_parse_request, and prints the details.

``` cpp
char buf[4096], *method, *path;
int pret, minor_version;
struct phr_header headers[100];
size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
ssize_t rret;

while (1) {
    /* read the request */
    while ((rret = read(sock, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR)
        ;
    if (rret <= 0)
        return IOError;
    prevbuflen = buflen;
    buflen += rret;
    /* parse the request */
    num_headers = sizeof(headers) / sizeof(headers[0]);
    pret = phr_parse_request(buf, buflen, &method, &method_len, &path, &path_len,
                             &minor_version, headers, &num_headers, prevbuflen);
    if (pret > 0)
        break; /* successfully parsed the request */
    else if (pret == -1)
        return ParseError;
    /* request is incomplete, continue the loop */
    assert(pret == -2);
    if (buflen == sizeof(buf))
        return RequestIsTooLongError;
}

printf("request is %d bytes long\n", pret);
printf("method is %.*s\n", (int)method_len, method);
printf("path is %.*s\n", (int)path_len, path);
printf("HTTP version is 1.%d\n", minor_version);
printf("headers:\n");
for (i = 0; i != num_headers; ++i) {
    printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
           (int)headers[i].value_len, headers[i].value);
}
```

# Nginx http

https://github.com/nginx/nginx/tree/master/src/http

# brpc http

* https://github.com/apache/incubator-brpc
* https://github.com/apache/incubator-brpc/blob/master/src/brpc/details/http_parser.h

# srpc http

* https://github.com/sogou/srpc
* https://github.com/sogou/workflow/blob/a5798eef9197ffd1ffb02dae4d24763a81c2efb8/src/protocol/http_parser.h

# trpc http

* https://git.woa.com/trpc-cpp/trpc-cpp/tree/master/trpc/util/http
* https://git.woa.com/trpc-cpp/trpc-cpp/blob/master/third_party/picohttpparser/picohttpparser.BUILD

# openssl

* https://github.com/openssl/openssl
* https://github.com/openssl/openssl/blob/master/include/openssl/http.h
* https://github.com/openssl/openssl/blob/master/apps/lib/http_server.c#L293

# ehttp

This library make http (with json) microservice easy

* https://github.com/hongliuliao/ehttp
* https://coder.social/hongliuliao/ehttp


# httpxx

* [C++ wrapper for C-based HTTP parser](https://github.com/AndreLouisCaron/httpxx)


# Q&A

## [What's the "Content-Length" field in HTTP header?](https://stackoverflow.com/questions/2773396/whats-the-content-length-field-in-http-header)

It's the number of bytes of data in the body of the request or response. The body is the part that comes after the blank line below the headers.

[4.3 Message Body](https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.3)

The message-body (if any) of an HTTP message is used to carry the entity-body associated with the request or response. The message-body differs from the entity-body only when a transfer-coding has been applied, as indicated by the Transfer-Encoding header field (section [14.41](https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.41)).

[4.4 Message Length](https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.4)

The transfer-length of a message is the length of the message-body as it appears in the message; that is, after any transfer-codings have been applied. When a message-body is included with a message, the transfer-length of that body is determined by one of the following (in order of precedence).

For compatibility with HTTP/1.0 applications, **HTTP/1.1 requests containing a message-body MUST include a valid Content-Length header field** unless the server is known to be HTTP/1.1 compliant. If a request contains a message-body and a Content-Length is not given, the server SHOULD respond with 400 (bad request) if it cannot determine the length of the message, or with 411 (length required) if it wishes to insist on receiving a valid Content-Length.

All HTTP/1.1 applications that receive entities MUST accept the "chunked" transfer-coding (section [3.6](https://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.6)), thus allowing this mechanism to be used for messages when the message length cannot be determined in advance.

Messages MUST NOT include both a Content-Length header field and a non-identity transfer-coding. If the message does include a non- identity transfer-coding, the Content-Length MUST be ignored.

When a Content-Length is given in a message where a message-body is allowed, its field value MUST exactly match the number of OCTETs in the message-body. HTTP/1.1 user agents MUST notify the user when an invalid length is received and detected.

[14.13 Content-Length](https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.13)

The Content-Length entity-header field indicates the size of the entity-body, in decimal number of OCTETs, sent to the recipient or, in the case of the HEAD method, the size of the entity-body that would have been sent had the request been a GET.

Applications SHOULD use this field to indicate the transfer-length of the message-body, unless this is prohibited by the rules in section [4.4](https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.4).

**Any Content-Length greater than or equal to zero is a valid value**. Section 4.4 describes how to determine the length of a message-body if a Content-Length is not given.

* [HEAD, 1xx, 204, 304 have content-length but do not have body](https://github.com/nodejs/http-parser/issues/251)


## [Is the Content-Length header required for a HTTP/1.0 response?](https://stackoverflow.com/questions/15991173/is-the-content-length-header-required-for-a-http-1-0-response)

A valid Content-Length field value is required on all HTTP/1.0 request messages containing an entity body.

If it is not required for the response, how does the client read the response when it's larger than 1MB?

解释：

When no Content-Length is received, the client keeps reading until the server closes the connection.

http://www.w3.org/Protocols/HTTP/1.0/draft-ietf-http-spec.html#Content-Length


## [What is HTTP "Host" header?](https://stackoverflow.com/questions/43156023/what-is-http-host-header)

Given that the TCP connection is already established when the HTTP request is sent, the IP address and port are implicitly known -- a TCP connection is an IP + Port. So, why do we need the Host header? Is this only needed for the case where there are multiple hosts mapped to the IP address implied in the TCP connection?

解释：

The [Host Header](https://www.rfc-editor.org/rfc/rfc7230#section-5.4) tells the webserver **which virtual host to use (if set up)**. You can even have the same virtual host using several aliases (= domains and wildcard-domains). In this case, you still have the possibility to read that header manually in your web app if you want to provide different behavior based on different domains addressed. This is possible because in your webserver you can (and if I'm not mistaken you must) set up one vhost to be the default host. This default vhost is used whenever the host header does not match any of the configured virtual hosts.

In the **MDN Documentation on the "Host" header** they actually phrase it like this:

> A Host header field must be sent in all HTTP/1.1 request messages. A 400 (Bad Request) status code will be sent to any HTTP/1.1 request message that lacks a Host header field or contains more than one.

I would always recommend going to the authoritative source when trying to understand the meaning and purpose of HTTP headers. [RFC7230](https://www.rfc-editor.org/rfc/rfc7230#section-5.4)

> The "Host" header field in a request provides the host and port information from the target URI, enabling the origin server to distinguish among resources while servicing requests for multiple host names on a single IP address.

## Connection: Keep-Alive

在HTTP 1.0中，没有官方的keepalive操作。通常是在现有协议上添加一个指数。如果浏览器支持keep-alive，它会在请求的包头中添加：Connection: Keep-Alive，然后当服务器收到请求，作出回应的时候，Connection: Keep-Alive 也添加一个头在响应中。这样做，连接就不会中断，而是保持连接。当客户端发送另一个请求时，它会使用同一个连接。这一直继续到客户端或服务器端认为会话已经结束，其中一方中断连接。


Connection 头（header） 决定当前的事务完成后，是否会关闭网络连接。如果该值是“keep-alive”，网络连接就是持久的，不会关闭，使得对同一个服务器的请求可以继续在该连接上完成。

```
Connection: keep-alive
Connection: close
```

* close 表明客户端或服务器想要关闭该网络连接，这是HTTP/1.0请求的默认值
* keep-alive 表明客户端想要保持该网络连接打开，HTTP/1.1的请求默认使用一个持久连接。这个请求头列表由头部名组成，这些头将被第一个非透明的代理或者代理间的缓存所移除：这些头定义了发出者和第一个实体之间的连接，而不是和目的地节点间的连接。

Keep-Alive 是一个通用消息头，允许消息发送者暗示连接的状态，还可以用来设置超时时长和最大请求数。

```
Connection: Keep-Alive
Keep-Alive: timeout=5, max=1000
```

> 需要将 The Connection 首部的值设置为  "keep-alive" 这个首部才有意义。同时需要注意的是，在HTTP/2 协议中， Connection 和 Keep-Alive  是被忽略的；在其中采用其他机制来进行连接管理。

* https://datatracker.ietf.org/doc/html/rfc2616#section-8
* https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Headers/Connection
* https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Headers/Keep-Alive
* https://zh.m.wikipedia.org/zh-hans/HTTP%E6%8C%81%E4%B9%85%E8%BF%9E%E6%8E%A5

## [How to send a header using a HTTP request through a cURL call?](https://stackoverflow.com/questions/356705/how-to-send-a-header-using-a-http-request-through-a-curl-call)

``` bash
# http1.0 curl 默认使用 http1.1 通信协议，若使用 http1.0 协议需要显示指定
curl --http1.0 http://hostname/resource

# set http header
curl --header "X-MyHeader: 123" www.google.com

# Get json
curl -i -H "Accept: application/json" -H "Content-Type: application/json" http://hostname/resource

# Get xml
curl -H "Accept: application/xml" -H "Content-Type: application/xml" -X GET http://hostname/resource

# Post
curl --data "param1=value1&param2=value2" http://hostname/resource
```

## [Http / 1.1 protocol expect: 100 continue](https://developpaper.com/http-http-1-1-protocol-expect-100-continue/)

Basic knowledge background:

The context of “expect: 100 continue”:

The purpose of designing 100 (continue) HTTP status code in http / 1.1 protocol is that before the client sends request message, http / 1.1 protocol allows the client to determine whether the server is willing to accept the message body sent by the client (based on request headers).
That is, the client and server allow the two sides to “shake hands” before the post (large) data. If the match is made, the client starts to send (large) data.
The reason for this is that if the client sends the request data directly, but the server rejects the request, this behavior will bring a lot of resource overhead.

The protocol requires http / 1.1 clients to:
If the client expects to wait for the response of “100 continue”, the request it sends must contain a header field of “expect: 100 continue”!


# http_parser Q&A

* [on_message_complete callback doesn't invoke after an answer](https://github.com/nodejs/http-parser/issues/327)
* [Cannot parse message without specifying Transfer-Encoding: chunked #434](https://github.com/nodejs/http-parser/issues/434)
* [100 continue support? #406](https://github.com/nodejs/http-parser/issues/406)

# Other HTTP Libs

## cpp-httplib

A C++ header-only HTTP/HTTPS server and client library. It's extremely easy to setup. Just include the httplib.h file in your code!

NOTE: This library uses 'blocking' socket I/O. If you are looking for a library with 'non-blocking' socket I/O, this is not the one that you want.

https://github.com/yhirose/cpp-httplib

# Refer


* https://github.com/nodejs/llhttp
* https://github.com/nodejs/llhttp/blob/master/test/request/connection.md


