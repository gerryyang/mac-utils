
```
cd my-jekyll-project  
 
git status 

# you can use "git stash" to set aside any work 
# in progress you might have

# this command will generate a template Gemfile,
# calculate the dependency graph and install the 
# dependencies with bundler  
jekyll new . --force 

# Now you can make  any changes you need to the 
# Gemfile, the template is well commented  with 
# regions to comment and uncomment based on common 
# uses like  gem-based themes and setup for publishing 
# to github pages.

# this command will test that the gemfile works and 
# serve your project to localhost:4000  
bundle exec jekyll serve 

# Open a web browser and go to the url localhost:4000 
# to make sure your website looks as expected

git add Gemfile Gemfile.lock
git commit -m "Dependency management handling via bundler"
git push

# If you did stash any work above, reapply it 
# now with "git stash pop"
```


# TODO

http://www.linux-kongress.org/2009/slides/compiler_survey_felix_von_leitner.pdf

https://www.jianshu.com/p/dab1c0ecbac0

http://www.bytesoft.org/tcc-intro/

https://cdn.ttgtmedia.com/searchWebServices/downloads/Business_Activities.pdf

https://cs.brown.edu/courses/cs227/archives/2012/papers/weaker/cidr07p15.pdf

https://www.enterpriseintegrationpatterns.com/patterns/conversation/TryConfirmCancel.html

http://www.infoq.com/cn/interviews/soa-chengli

http://www.infoq.com/cn/news/2018/08/rocketmq-4.3-release


http://gmssl.org/


动态库和静态库，mv，为什么会core

gcc -g -shared -Wl,-soname,libfunc_wrap.so.0 -o libfunc_wrap.so.0.0 libfunc_wrap.o -lc -L./ -lfunc-static
/usr/bin/ld: .//libfunc-static.a(libfunc-static.o): relocation R_X86_64_32 against `.rodata' can not be used when making a shared object; recompile with -fPIC
.//libfunc-static.a: could not read symbols: Bad value
collect2: error: ld returned 1 exit status

https://stackoverflow.com/questions/19364969/compilation-fails-with-relocation-r-x86-64-32-against-rodata-str1-8-can-not
http://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html#Code-Gen-Options
https://en.wikipedia.org/wiki/Position-independent_code

https://stackoverflow.com/questions/9637551/relocation-r-x86-64-32s-against-rodata-while-compiling-on-64-bit-platform?rq=1
https://www.cprogramming.com/tutorial/shared-libraries-linux-gcc.html

https://zhuanlan.zhihu.com/p/32684212



https://github.com/scylladb/seastar

LD_PRELOAD用法
https://blog.csdn.net/iEearth/article/details/49952047

内存优化总结:ptmalloc、tcmalloc和jemalloc
https://blog.csdn.net/junlon2006/article/details/77854898

性能调优相关：

http://www.brendangregg.com/overview.html
https://access.redhat.com/documentation/zh-cn/red_hat_enterprise_linux/6/html/performance_tuning_guide/
http://www.keil.com/support/man/docs/armclang_ref/armclang_ref_vvi1466179578564.htm
http://www.brendangregg.com/blog/2017-05-09/cpu-utilization-is-wrong.html


redis:
https://blog.csdn.net/Aquester/article/details/82767217

RDMA:
https://en.wikipedia.org/wiki/Remote_direct_memory_access
https://blog.csdn.net/u011459120/article/details/78469098

云栖大会2018
https://yunqi.youku.com/

阿里巴巴中台战略
https://www.kancloud.cn/architect-jdxia/architect/519490


Apache Lucene Core
https://lucene.apache.org/core/


Is file append atomic in UNIX?
https://stackoverflow.com/questions/1154446/is-file-append-atomic-in-unix
https://www.notthewizard.com/2014/06/17/are-files-appends-really-atomic/
https://www.zhihu.com/question/20906432
https://stackoverflow.com/questions/5200923/are-unix-reads-and-writes-to-a-single-file-atomically-serialized
https://nullprogram.com/blog/2016/08/03/


blog to https
https://www.jianshu.com/p/34b5a5233f8f

haproxy
https://serversforhackers.com/c/using-ssl-certificates-with-haproxy
http://www.haproxy.org/
http://cbonte.github.io/haproxy-dconv/1.9/intro.html

参考博客
https://imququ.com/post/archives.html
https://ericchiang.github.io/
https://www.owent.net/about.html

技术博客
https://blog.tsunanet.net/

du和df的区别
https://www.cnblogs.com/heyonggang/p/3644736.html


urlencode
https://stackoverflow.com/questions/9086296/why-url-encode-or-which-characters-to-encode
http://www.blooberry.com/indexdot/html/topics/urlencoding.htm

gitbook
https://docs.gitbook.com/

https://www.tutorialspoint.com/cprogramming/c_passing_arrays_to_functions.htm

raft可视化
[Raft: Understandable Distributed Consensus]: http://thesecretlivesofdata.com/raft


# TODO

* 博客https改造
* nginx端口转发，通过二级域名

* gcc attri
https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Function-Attributes.html

