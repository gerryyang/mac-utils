#!/bin/bash

pkill -f jekyll
#bundle exec jekyll serve --livereload >> run.log &
bundle exec jekyll serve  >> run.log &
