
# 使用jekyll的一些tips

1. 修改_config.yml后，不会自动更新需要重启jekyll服务


# [Requirements](https://jekyllrb.com/docs/installation/#requirements)

* Ruby >= 2.2.5
* RubyGems
* GCC and Make

# [Using Jekyll with Bundler](https://jekyllrb.com/tutorials/using-jekyll-with-bundler/)

bundle - Ruby Dependency Management

> Bundler provides a consistent environment for Ruby projects by tracking and installing the exact gems and versions that are needed.



# [Quick-start guide](https://jekyllrb.com/docs/quickstart/)
``` 
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




