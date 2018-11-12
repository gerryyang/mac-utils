
## Docs

| 文档 (doc/) | 用途
| -- | --
| intro.txt (HAProxy Starter Guide) | this is an introduction to haproxy, it explains what it is what it is not. Useful for beginners or to re-discover it when planning for an upgrade.
| architecture.txt | this is the architecture manual. It is quite old and does not tell about the nice new features, but it's still a good starting point when you know what you want but don't know how to do it.
| configuration.txt | this is the configuration manual. It recalls a few essential HTTP basic concepts, and details all the configuration file
syntax (keywords, units). It also describes the log and stats format. It is normally always up to date. If you see that something is missing from it, please report it as this is a bug. Please note that this file is huge and that it's generally more convenient to review Cyril Bonté's
HTML translation online here : http://cbonte.github.io/haproxy-dconv/configuration-1.6.html
| management.txt | it explains how to start haproxy, how to manage it at runtime, how to manage it on multiple nodes, how to proceed with seamless upgrades.
| coding-style.txt | this is for developers who want to propose some code to the project. It explains the style to adopt for the code. It is not very strict and not all the code base completely respects it, but contributions which diverge too much from it will be rejected.
| proxy-protocol.txt | this is the de-facto specification of the PROXY protocol which is implemented by HAProxy and a number of third party products.
| README | how to build HAProxy from sources
| gpl.txt / lgpl.txt | the copy of the licenses covering the software. See the 'LICENSE' file at the top for more information.
| other | the rest is mainly for developers.

* There are also a number of nice configuration examples in the "examples" directory as well as on several sites and articles on the net which are linked to from the haproxy web site.



## Install

依赖：

1. pcre

If your system supports PCRE (Perl Compatible Regular Expressions), then you really should build with libpcre which is between 2 and 10 times faster than other libc implementations.

http://www.pcre.org/

2. OpenSSL

It is possible to add native support for SSL using the GNU makefile, by passing "USE_OPENSSL=1".

https://www.openssl.org

3. Zlib

It is also possible to include native support for zlib to benefit from HTTP compression. For this, pass "USE_ZLIB=1"

http://www.zlib.net/


4. Libslz

It is possible to use libslz for a faster, memory less, but slightly less efficient compression, by passing "USE_SLZ=1". HAProxy can use SLZ since version 1.6.0 to compress HTTP responses.

http://1wt.eu/projects/libslz/



编译：

``` bash
#!/bin/bash

# setup.sh

export PREFIX_PATH=/data/home/gerryyang/LAMP/haproxy/install/haproxy-1.9-dev5

export SSL_PATH=/data/home/gerryyang/LAMP/haproxy/deps/install/openssl-1.1.1
export PCRE2_PATH=/data/home/gerryyang/LAMP/haproxy/deps/install/pcre2-10.32
export ZLIB_PATH=/data/home/gerryyang/LAMP/haproxy/deps/install/zlib-1.2.11

make PREFIX=$PREFIX_PATH TARGET=linux26 USE_PCRE2=1 PCRE2DIR=$PCRE2_PATH PCRE2_INC=$PCRE2_PATH/include PCRE2_LIB=$PCRE2_PATH/lib USE_OPENSSL=1 SSL_INC=$SSL_PATH/include SSL_LIB=$SSL_PATH/lib ADDLIB=-ldl USE_ZLIB=1 ZLIB_INC=$ZLIB_PATH/include ZLIB_LIB=$ZLIB_PATH/lib && make install
```

运行：

``` bash
#!/bin/bash

# init.sh

export SSL_PATH=/data/home/gerryyang/LAMP/haproxy/deps/install/openssl-1.1.1
export PCRE2_PATH=/data/home/gerryyang/LAMP/haproxy/deps/install/pcre2-10.32
export ZLIB_PATH=/data/home/gerryyang/LAMP/haproxy/deps/install/zlib-1.2.11

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SSL_PATH/lib:$PCRE2_PATH/lib:$ZLIB_PATH/lib
```

```
$./init.sh
$./haproxy --help
HA-Proxy version 1.9-dev5 2018/10/28
Copyright 2000-2018 Willy Tarreau <willy@haproxy.org>

Usage : haproxy [-f <cfgfile|cfgdir>]* [ -vdVD ] [ -n <maxconn> ] [ -N <maxpconn> ]
        [ -p <pidfile> ] [ -m <max megs> ] [ -C <dir> ] [-- <cfgfile>*]
        -v displays version ; -vv shows known build options.
        -d enters debug mode ; -db only disables background mode.
        -dM[<byte>] poisons memory with <byte> (defaults to 0x50)
        -V enters verbose mode (disables quiet mode)
        -D goes daemon ; -C changes to <dir> before loading files.
        -W master-worker mode.
        -q quiet mode : don't display messages
        -c check mode : only check config files and exit
        -n sets the maximum total # of connections (2000)
        -m limits the usable amount of memory (in MB)
        -N sets the default, per-proxy maximum # of connections (2000)
        -L set local peer name (default to hostname)
        -p writes pids of all children to this file
        -de disables epoll() usage even when available
        -dp disables poll() usage even when available
        -dR disables SO_REUSEPORT usage
        -dr ignores server address resolution failures
        -dV disables SSL verify on servers side
        -sf/-st [pid ]* finishes/terminates old pids.
        -x <unix_socket> get listening sockets from a unix socket
        -S <unix_socket>[,<bind options>...] new stats socket for the master
```


## Refer

https://serversforhackers.com/c/using-ssl-certificates-with-haproxy
