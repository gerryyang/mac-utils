---
layout: post
title:  "YAML in Action"
date:   2022-07-20 12:30:00 +0800
categories: Docker
---

* Do not remove this line (it will not be displayed)
{:toc}





# Q&A

## [How do you do block comments in YAML?](https://stackoverflow.com/questions/2276572/how-do-you-do-block-comments-in-yaml)

How do I comment a block of lines in YAML?

YAML supports inline comments, but does not support block comments.

From [Wikipedia](http://en.wikipedia.org/wiki/YAML):

> Comments begin with the number sign ( `#` ), can start anywhere on a line, and continue until the end of the line

A comparison with JSON, also from [Wikipedia](https://en.wikipedia.org/wiki/YAML#Comparison_with_JSON):

> The syntax differences are subtle and seldom arise in practice: JSON allows extended charactersets like UTF-32, YAML requires a space after separators like comma, equals, and colon while JSON does not, and some non-standard implementations of JSON extend the grammar to include Javascript's /* ... */ comments. Handling such edge cases may require light pre-processing of the JSON before parsing as in-line YAML.









