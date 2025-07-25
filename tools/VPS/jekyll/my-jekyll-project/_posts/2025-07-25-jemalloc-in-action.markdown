---
layout: post
title:  "Jemalloc in Action"
date:   2025-07-25 12:30:00 +0800
categories: [Linux Performance]
---

* Do not remove this line (it will not be displayed)
{:toc}


# [Background](https://github.com/jemalloc/jemalloc/wiki/Background)

## History

`jemalloc` started out as the memory allocator for a programming language runtime Jason Evans was developing in 2005, but language design changes made the allocator superfluous. At the time, [FreeBSD](http://www.freebsd.org/) was in need of an SMP-scalable allocator, so he integrated jemalloc into FreeBSD's libc, and then made a long series of improvements to scalability and fragmentation behavior.

In late 2007, the [Mozilla Project](http://www.mozilla.org/) was hard at work improving [Firefox's memory usage](http://www.mozilla.com/firefox/) for the 3.0 release, and jemalloc was used to solve fragmentation problems for Firefox on [Microsoft Windows](http://www.microsoft.com/windows/) platforms. You can read [here](https://web.archive.org/web/20160321215546/https://blog.pavlov.net/2008/03/11/firefox-3-memory-usage/) about the fruits of that labor. Many general jemalloc enhancements resulted from Firefox-related efforts. More recently, in 2010 Mozilla sponsored integration of [Apple Mac OS X](http://www.apple.com/macosx/) support into the stand-alone jemalloc, and in 2012 contributed [MinGW](http://www.mingw.org/) Windows support.

Starting in 2009, Jason Evans adapted jemalloc to handle the extreme loads [Facebook](http://www.facebook.com/) servers commonly operate under, and added numerous features that support development and monitoring. **Facebook uses `jemalloc` in many components that are integral to serving its website, and as of 2017 a small Facebook team drives ongoing development and maintenance**.


* **起源：语言运行时项目（2005年）**

  + 初始目标：Jason Evans 为自研编程语言运行时开发的内存分配器。
  + 项目转折：语言设计变更导致分配器不再被需要，**jemalloc 面临闲置**。
  + 技术基础：此时已具备多核扩展性（SMP-scalable） 和碎片控制的核心设计雏形。

* **首次落地：成为 FreeBSD 默认分配器（2005年）**

  + 契机：FreeBSD 急需支持多核并发的内存分配器。
  + 关键动作：
    - Jason Evans 将 jemalloc 集成至 FreeBSD 的 libc 库。
    - 持续优化可扩展性（多线程性能）和碎片控制能力。
  + 意义：标志 jemalloc **通过操作系统级验证**，奠定工业级可靠性基础。

* **关键突破：解决 Firefox 内存问题（2007年）**

  + 场景需求：Mozilla 为 Firefox 3.0 优化 Windows 平台的内存碎片问题。
  + 技术贡献：
    - jemalloc 显著降低 Firefox 在 Windows 的内存碎片。
    - Mozilla 的实践推动 jemalloc 通用性增强（如跨平台支持、调试工具）。
  + 后续合作：
    - 2010年：Mozilla 赞助 macOS 支持开发。
    - 2012年：贡献 MinGW 工具链的 Windows 移植。

* **大规模实战：Facebook 时代（2009年起）**

  + 挑战背景：Facebook 需应对亿级用户负载下的极端内存压力。
  + 深度适配：
    - Jason Evans **为 Facebook 服务器定制优化，强化高并发稳定性与大规模内存管理**。
    - 新增开发监控特性（如堆分析、实时指标导出）。

  + 持续演进：
    - 2017年：Facebook 成立专职团队主导开发维护，推动 jemalloc 成为超大规模服务基石。
    - 应用于 Facebook 核心服务（如 Web 服务、缓存系统）。


> **历史启示**
>
> 1. 从实验到工业级：从“被废弃”的语言项目组件，成长为FreeBSD、Firefox、Facebook信赖的核心基础设施。
> 2. 开源协作力量：Mozilla、Facebook 等企业的实际需求驱动技术进化，反哺社区版本。




## Intended use

`jemalloc` is a general purpose `malloc(3)` implementation that emphasizes **fragmentation avoidance** and **scalable concurrency support**. It is intended for use as the system-provided memory allocator, as in FreeBSD's libc library, as well as for linking into C/C++ applications. jemalloc provides many introspection, memory management, and tuning features beyond the standard allocator functionality. As an extreme example, arenas can be used as pool allocators; i.e. an arena can be used for general purpose allocation, and then the entire arena destroyed as a single operation.


`jemalloc` 的历史发展脉络和关键里程碑：

* 诞生与早期采用 (2005年):

  + `jemalloc` 最初是作为一个通用的 `malloc(3)` 实现被开发出来的。
  + 它的**核心设计目标**是**避免内存碎片化和提供可扩展的并发支持**（即在高并发、多线程环境下依然能高效工作）。
  + **2005 年**是其发展史上最重要的早期里程碑：**`jemalloc` 首次被采用作为 FreeBSD 操作系统 `libc` 库的默认内存分配器**。这标志着它经受住了操作系统的严格考验，其性能和可靠性获得了关键认可。

* 功能扩展与开发者支持 (2010年):

  + 在成为 FreeBSD 默认分配器并积累一定声誉后，`jemalloc` 的发展方向在 2010年发生了重要转变。
  + **开发重点拓宽**，不再局限于核心的内存分配效率，**开始大力引入面向开发者的高级功能**：
    - **堆分析 (Heap Profiling)**: 帮助开发者分析应用程序的内存使用模式，定位内存泄漏或消耗过大的区域。
    - **广泛的监控/调优钩子 (Extensive Monitoring/Tuning Hooks)**: 提供丰富的接口和工具，允许开发者更细致地监控 `jemalloc` 的内部状态（如内存分配情况、碎片情况等），并进行运行时调优以适应特定应用需求。
  + **这标志着 `jemalloc` 从一个“纯粹”的高效分配器，演进为一个更全面、更注重可观测性和可调性的内存管理解决方案**。


* 持续发展与现代定位 (2010年后至今):

  + **与 FreeBSD 的紧密集成**：文本明确指出，现代 `jemalloc` 版本持续地被集成回 FreeBSD。这说明：
    - `jemalloc` 在 FreeBSD 中保持着基础且关键的地位。
    - `jemalloc` 的多功能性 (versatility) 对于满足操作系统复杂多变的需求至关重要。
    - 这是一个双向受益的过程：FreeBSD 提供了重要的生产环境测试平台，而 jemalloc 的改进也直接回馈给 FreeBSD。

  + **持续的演进目标**：
    - **成为广泛苛刻应用的最佳选择**：开发努力的趋势是让 `jemalloc` 成为众多高要求、高性能应用程序的最佳内存分配器之一。这体现了其追求卓越和广泛适用性的目标。
    - **解决实际弱点**：开发工作持续聚焦于消除或减轻那些对现实世界应用程序产生实际负面影响的缺点。这表明开发团队非常注重解决用户在实际部署中遇到的真实问题，不断进行优化和修复。


# Adoption

Major uses of `jemalloc` include:

* [FreeBSD](http://www.freebsd.org/)
* [NetBSD](http://www.netbsd.org/)
* [Mozilla Firefox](http://www.mozilla.org/firefox/)
* [Varnish](https://www.varnish-cache.org/)
* [Cassandra](http://cassandra.apache.org/)
* [Redis](http://redis.io/)
* [hhvm](https://github.com/facebook/hiphop-php)
* [MariaDB](https://mariadb.org/)
* [Aerospike](http://www.aerospike.com/)
* [Android](https://github.com/android/platform_bionic)



# jemalloc 和 tcmalloc 的对比

| **维度**  | **jemalloc** | **tcmalloc**
| -- | -- | --
| 内存碎片率 | 碎片率最低（<5%），长期运行稳定。采用红黑树管理空闲内存，支持跨区域合并 | 中等碎片率（~8%），频繁分配释放后可能出现“内存漏斗”现象
| 分配速度 | 1. 中等对象（4KB–1MB）分配最快 2. 多线程下延迟标准差 ≤15μs | 1. 小对象（<256B）分配最快（380万次/秒，比 ptmalloc 快42%）2. 大对象（>1MB）与 jemalloc差距 <10%
| 多线程支持 | 1. Arena 分区技术减少锁竞争，64+线程时扩展性最优 2. 线程数量固定时性能最佳 | 1. ThreadCache 无锁分配小对象，但 CentralHeap 可能周期性停顿（如每3秒50ms延迟）2. 动态线程场景适应性更强
| 元数据开销 | 约 2% 额外空间 | 约 1% 额外空间（小对象）
| 大对象处理 | 直接调用 mmap，与 tcmalloc 性能接近 | 按 4KB 页对齐分配，自旋锁优化

选择建议：

* 选择 jemalloc：当线程模型固定、追求长期稳定与低碎片（如数据库、核心服务）。
* 选择 tcmalloc：当线程动态变化、小对象分配密集或需快速恢复（如微服务、CI/CD流水线）。



# [Install & Build](https://github.com/jemalloc/jemalloc/blob/dev/INSTALL.md)


Building and installing a packaged release of `jemalloc` can be as simple as typing the following while in the root directory of the source tree:

``` bash
./configure
make
make install
```

You can uninstall the installed build artifacts like this:

``` bash
make uninstall
```

## Advanced configuration


The 'configure' script supports numerous options that allow control of which functionality is enabled, where `jemalloc` is installed, etc. Optionally, pass any of the following arguments (not a definitive list) to 'configure':

* `--help`

Print a definitive list of options.

* `--prefix=<install-root-dir>`

Set the base directory in which to install. For example:

``` bash
  ./configure --prefix=/usr/local
```

will cause files to be installed into `/usr/local/include`, `/usr/local/lib`, and `/usr/local/man`.

* `--with-version=(<major>.<minor>.<bugfix>-<nrev>-g<gid>|VERSION)`

The `VERSION` file is mandatory for successful configuration, and the following steps are taken to assure its presence:

1. If `--with-version=..--g` is specified, generate `VERSION` using the specified value.
2. If `--with-version` is not specified in either form and the source directory is inside a git repository, try to generate `VERSION` via `'git describe'` invocations that pattern-match release tags.
3. If `VERSION` is missing, generate it with a bogus version: `0.0.0-0-g0000000000000000000000000000000000000000`

Note that `--with-version=VERSION` bypasses (1) and (2), which simplifies `VERSION` configuration when embedding a jemalloc release into another project's git repository.

* `--with-rpath=<colon-separated-rpath>`

Embed one or more library paths, so that libjemalloc can find the libraries it is linked to. This works only on ELF-based systems.

* `--with-mangling=<map>`

Mangle public symbols specified in which is a comma-separated list of `name:mangled` pairs.

For example, to use ld's `--wrap` option as an alternative method for overriding libc's malloc implementation, specify something like:

```
--with-mangling=malloc:__wrap_malloc,free:__wrap_free[...]
```

Note that mangling happens prior to application of the prefix specified by `--with-jemalloc-prefix`, and mangled symbols are then ignored when applying the prefix.

* `--with-jemalloc-prefix=<prefix>`

Prefix all public APIs with . For example, if is `"prefix_"`, API changes like the following occur:

``` cpp
malloc()         --> prefix_malloc()
malloc_conf      --> prefix_malloc_conf
/etc/malloc.conf --> /etc/prefix_malloc.conf
MALLOC_CONF      --> PREFIX_MALLOC_CONF
```

This makes it possible to use `jemalloc` at the same time as the system allocator, or even to **use multiple copies of `jemalloc` simultaneously**.

**By default, the prefix is ""**, except on OS X, where it is "je_". On OS X, jemalloc overlays the default malloc zone, but makes no attempt to actually replace the "malloc", "calloc", etc. symbols.

* `--without-export`

Don't export public APIs. This can be useful when building `jemalloc` as a static library, or to avoid exporting public APIs when using the zone allocator on OSX.

* `--with-private-namespace=<prefix>`

Prefix all library-private APIs with `je_`. For shared libraries, symbol visibility mechanisms prevent these symbols from being exported, but for static libraries, naming collisions are a real possibility. By default, is empty, which results in a symbol prefix of `je_` .

* `--with-install-suffix=<suffix>`

Append to the base name of all installed files, such that multiple versions of jemalloc can coexist in the same installation directory. For example, `libjemalloc.so.0` becomes `libjemalloc.so.0`.


* `--with-malloc-conf=<malloc_conf>`

Embed `<malloc_conf>` as a run-time options string that is processed prior to the malloc_conf global variable, the `/etc/malloc.conf` symlink, and the `MALLOC_CONF` environment variable. For example, to change the default decay time to 30 seconds:

``` bash
--with-malloc-conf=decay_ms:30000
```

* `--enable-debug`

Enable assertions and validation code. This incurs a substantial performance hit, but is very useful during application development.


* `--disable-stats`

Disable statistics gathering functionality. See the "opt.stats_print" option documentation for usage details.

* `--enable-prof`

Enable **heap profiling** and **leak detection** functionality. See the "opt.prof" option documentation for usage details. When enabled, there are several approaches to backtracing, and the configure script chooses the first one in the following list that appears to function correctly:

1. libunwind (requires `--enable-prof-libunwind`)
2. frame pointer (requires `--enable-prof-frameptr`)
3. libgcc (unless `--disable-prof-libgcc`)
4. gcc intrinsics (unless `--disable-prof-gcc`)

* `--enable-prof-libunwind`

Use the `libunwind` library (http://www.nongnu.org/libunwind/) for stack backtracing.


* `--enable-prof-frameptr`

Use the optimized frame pointer unwinder for stack backtracing. Safe to use in mixed code (with and without frame pointers) - but requires frame pointers to produce meaningful stacks. **Linux only**.


* `--disable-prof-libgcc`

Disable the use of libgcc's backtracing functionality.


* `--disable-prof-gcc`

Disable the use of gcc intrinsics for backtracing.


* `--with-static-libunwind=<libunwind.a>`

Statically link against the specified `libunwind.a` rather than dynamically linking with `-lunwind`.

* `--disable-fill`

Disable support for junk/zero filling of memory. See the "opt.junk" and "opt.zero" option documentation for usage details.

* `--disable-zone-allocator`

Disable zone allocator for Darwin. This means jemalloc won't be hooked as the default allocator on OSX/iOS.

* `--enable-utrace`

Enable utrace(2)-based allocation tracing. This feature is not broadly portable (FreeBSD has it, but Linux and OS X do not).


* `--enable-xmalloc`

Enable support for optional immediate termination due to out-of-memory errors, as is commonly implemented by "xmalloc" wrapper function for malloc. See the "opt.xmalloc" option documentation for usage details.

* `--enable-lazy-lock`

Enable code that wraps `pthread_create()` to detect when an application switches from single-threaded to multi-threaded mode, so that it can avoid mutex locking/unlocking operations while in single-threaded mode. In practice, this feature usually has little impact on performance unless thread-specific caching is disabled.


* `--disable-syscall`

Disable use of syscall(2) rather than {open,read,write,close}(2). This is intended as a workaround for systems that place security limitations on syscall(2).


* `--disable-cxx`

Disable C++ integration. This will cause new and delete operator implementations to be omitted.


**The following environment variables (not a definitive list) impact configure's behavior:**


* `CFLAGS="?"`

* `CXXFLAGS="?"`

Pass these flags to the C/C++ compiler. Any flags set by the configure script are prepended, which means explicitly set flags generally take precedence. Take care when specifying flags such as `-Werror`, because configure tests may be affected in undesirable ways.

* `EXTRA_CFLAGS="?"`

* `EXTRA_CXXFLAGS="?"`

Append these flags to `CFLAGS`/`CXXFLAGS`, without passing them to the compiler(s) during configuration. This makes it possible to add flags such as `-Werror`, while allowing the configure script to determine what other flags are appropriate for the specified configuration.


* `CPPFLAGS="?"`

Pass these flags to the C preprocessor. Note that `CFLAGS` is not passed to 'cpp' when 'configure' is looking for include files, so you must use `CPPFLAGS` instead if you need to help 'configure' find header files.

* `LD_LIBRARY_PATH="?"`

'ld' uses this colon-separated list to find libraries.

* `LDFLAGS="?"`

Pass these flags when linking.

* `PATH="?"`

'configure' uses this to find programs.


## Advanced compilation


To build only parts of jemalloc, use the following targets:

```
build_lib_shared
build_lib_static
build_lib
build_doc_html
build_doc_man
build_doc
```

To install only parts of jemalloc, use the following targets:

```
install_bin
install_include
install_lib_shared
install_lib_static
install_lib_pc
install_lib
install_doc_html
install_doc_man
install_doc
```

To clean up build results to varying degrees, use the following make targets:

```
clean
distclean
relclean
```



# Documentation

`jemalloc` has evolved substantially over its lifetime, so although the older documentation is still broadly informative, many of the details are obsolete.

* [jemalloc(3) manual page](https://jemalloc.net/jemalloc.3.html): The manual page fully describes the API and options supported by jemalloc, and includes a brief summary of its internals.

* [Wiki](https://github.com/jemalloc/jemalloc/wiki): The wiki contains miscellaneous documentation such as links to [papers and presentations](https://github.com/jemalloc/jemalloc/wiki/Background#additional-documentation), FAQ answers, and tutorials.

# Project activity

The `jemalloc` project is managed on [GitHub](https://github.com/jemalloc/jemalloc). In order of increasing interest level, the following are useful entry points:

* [Releases atom feed](https://github.com/jemalloc/jemalloc/releases.atom): Track [releases](https://github.com/jemalloc/jemalloc/releases) using an atom feed reader of your choice.

* [Primary jemalloc repository](https://github.com/jemalloc/jemalloc): This is the primary jemalloc repository, with a small set of active branches including:

    + [master](https://github.com/jemalloc/jemalloc/tree/master): The master branch tracks stable releases.
    + [dev](https://github.com/jemalloc/jemalloc/tree/dev): The dev branch tracks current development, and at any given time may not be production-ready.
    + [stable-3](https://github.com/jemalloc/jemalloc/tree/stable-3)/[stable-4](https://github.com/jemalloc/jemalloc/tree/stable-4): The stable-* branches are bugfix-only branches based on the last releases for previous major versions. The versioned release history is linear, but relevant fixes may continue to be applied to the `stable-*` branches indefinitely.


* [Issues](https://github.com/jemalloc/jemalloc/issues): The issue tracking system is used for all formal communication, including feature, bug, and release tracking, as well as questions (the [question](https://github.com/jemalloc/jemalloc/issues?utf8=%E2%9C%93&q=label%3Aquestion%20) label is added during triage). As a starting point, see [milestones](https://github.com/jemalloc/jemalloc/milestones), aka future releases.


* [Development activity subscription](https://github.com/jemalloc/jemalloc/subscription): Watch all development activity via GitHub notifications.

* [Gitter](https://gitter.im/jemalloc/jemalloc): Day-to-day chatter focused on jemalloc development. The primary jemalloc developers conduct much of their informal communication here; feel welcome to observe and/or join in.


# Legacy

The [jemalloc-announce](https://jemalloc.net/mailman/jemalloc-announce/index.html) and [jemalloc-discuss](https://jemalloc.net/mailman/jemalloc-discuss/index.html) mailing lists were retired in September 2016, but their archives remain available here.





# Refer

* https://jemalloc.net/
* https://github.com/jemalloc/jemalloc
* https://github.com/jemalloc/jemalloc/releases












