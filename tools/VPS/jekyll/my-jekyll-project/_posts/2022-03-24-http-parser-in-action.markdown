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


# Refer


* https://github.com/nodejs/llhttp
* https://github.com/nodejs/llhttp/blob/master/test/request/connection.md

