
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