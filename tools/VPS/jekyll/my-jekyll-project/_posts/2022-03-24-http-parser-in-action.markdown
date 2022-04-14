---
layout: post
title:  "HTTP Parser in Action"
date:   2022-03-24 14:00:00 +0800
categories: [TCP/IP]
---

* Do not remove this line (it will not be displayed)
{:toc}


Port of [http_parser](https://github.com/nodejs/http-parser) to [llparse](https://github.com/nodejs/llparse). More details in [llhttp - new HTTP 1.1 parser for Node.js by Fedor Indutny | JSConf EU 2019](https://youtu.be/x3k_5Mi66sY), and [the slides and the presentations](https://indutny.github.io/jsconfeu-2019/reveal.js/index.html#/). (视频中，llhttp作者讲述了http_parser的历史，以及为什么开发新的llhttp)

> Node.js has been using a derivative of nginx’s parser with a lot of customization/rewrite since its inception. Despite being fast enough, the project architecture made it very hard to maintain in a long run. To mitigate that, the author has created a tool to generate the new HTTP parser called “llhttp” from the TypeScript code in understandable, verifiable, and maintainable way. Incidentally, the performance of “llhttp” is two times better than of the old parser. In this talk we’ll walk through the basics of generating such parsers and how “llhttp” works.

# Original parser

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
* llparse, Not hand-written! Not hand-optimized! is 2x faster!

# Numbers (性能数据)

| llhttp | http_parser
| -- | -- 
| 3,020,459 RPS | 1,406,180 RPS

This llhttp parser is a default in Node version 12. 

# Tests

* All original tests ported to markdown. Easy to read, easy to contribute. In-test textual description

# More optimizations

* vector insttructions (CPU)
* Unified docs (Quite ashamedly, the project is not well documented)
* Different parsers? e.g., SMTP, POP3


# Refer


* https://github.com/nodejs/llhttp
* https://github.com/nodejs/llhttp/blob/master/test/request/connection.md

