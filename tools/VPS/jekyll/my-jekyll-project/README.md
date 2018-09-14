
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