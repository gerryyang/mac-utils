
# 使用jekyll的一些tips

1. 修改_config.yml后，不会自动更新需要重启jekyll服务


# [Requirements](https://jekyllrb.com/docs/installation/#requirements)

* Ruby >= 2.2.5
* RubyGems
* GCC and Make


## 更新Ruby

默认安装：

* yum install ruby
* ruby -v

添加ruby仓库：

* gem sources -a http://mirrors.aliyun.com/rubygems/

安装RAM (Ruby Version Manager)：

* gpg --keyserver hkp://keys.gnupg.net --recv-keys 409B6B1796C275462A1703113804BB82D39DC0E3 7D2BAF1CF37B13E2069D6956105BD0E739499BDB
* curl -sSL https://get.rvm.io | bash -s stable

for Centos：

* source /etc/profile.d/rvm.sh (使配置生效)
* rvm -v  (查看版本, 判断是否安装成功)

for Ubuntu：

* https://github.com/rvm/ubuntu_rvm


查看Ruby版本：

* rvm list known
* rvm install 3.0
* ruby -v (验证升级后的版本)


# [Using Jekyll with Bundler](https://jekyllrb.com/tutorials/using-jekyll-with-bundler/)

bundle - Ruby Dependency Management

> Bundler provides a consistent environment for Ruby projects by tracking and installing the exact gems and versions that are needed.

# [Quick-start guide](https://jekyllrb.com/docs/quickstart/)

```
# install gem
yum install rubygems

# Install Jekyll and Bundler gems through RubyGems
gem install jekyll bundler

# Create a new Jekyll site at ./myblog
jekyll new myblog

# Change into your new directory
cd myblog

# Build the site on the preview server
bundle exec jekyll serve

# Now browse to http://localhost:4000
```

# [Theme](https://jekyllrb.com/docs/themes/)

https://github.com/jekyll/minima

In the case of Minima, you see only the following files in your Jekyll site directory:

```
├── Gemfile
├── Gemfile.lock
├── _config.yml
├── _posts
│   └── 2016-12-04-welcome-to-jekyll.markdown
├── about.markdown
└── index.markdown
```

```
# bundle show minima
/root/LAMP/ruby/install/ruby-2.5.1/lib/ruby/gems/2.5.0/gems/minima-2.5.0
```

The Minima theme gem contains these files:

```
 ├── LICENSE.txt
 ├── README.md
 ├── _includes
 │   ├── disqus_comments.html
 │   ├── footer.html
 │   ├── google-analytics.html
 │   ├── head.html
 │   ├── header.html
 │   ├── icon-github.html
 │   ├── icon-github.svg
 │   ├── icon-twitter.html
 │   └── icon-twitter.svg
 ├── _layouts
 │   ├── default.html
 │   ├── home.html
 │   ├── page.html
 │   └── post.html
 ├── _sass
 │   ├── minima
 │   │   ├── _base.scss
 │   │   ├── _layout.scss
 │   │   └── _syntax-highlighting.scss
 │   └── minima.scss
 └── assets
     └── main.scss
```

refer:

* https://github.com/brianmaierjr/long-haul  
	+ TODO: npm install报错  https://github.com/gerryyang/long-haul

# Usage

https://jekyllrb.com/docs/usage/


# refer

https://github.com/jekyll/jekyll

https://jekyllrb.com/

https://jekyllrb.com/philosophy

https://jekyllrb.com/docs/troubleshooting/

https://jekyllrb.com/docs/installation/

https://jekyllrb.com/docs/installation/#ubuntu

https://jekyllrb.com/docs/usage/

https://jekyllrb.com/docs/quickstart/

[“授人以渔”的教你搭建个人独立博客](https://www.jianshu.com/p/8f843034c7ec)

[在Github上搭建Jekyll博客和创建主题](http://yansu.org/2014/02/12/how-to-deploy-a-blog-on-github-by-jekyll.html)




