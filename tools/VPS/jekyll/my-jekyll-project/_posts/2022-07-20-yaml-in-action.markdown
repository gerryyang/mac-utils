---
layout: post
title:  "YAML in Action"
date:   2022-07-20 12:30:00 +0800
categories: Docker
---

* Do not remove this line (it will not be displayed)
{:toc}


YAML 是一种较为人性化的数据序列化语言，可以配合目前大多数编程语言使用。YAML 的语法比较简洁直观，特点是使用空格来表达层次结构，其最大优势在于数据结构方面的表达，所以 YAML 更多应用于编写配置文件，其文件一般以 `.yaml` 或 `.yml` 为后缀。


# 基本语法

* 大小写敏感
* 用缩进表示层级关系
* 缩进不允许使用 `tab`，只允许空格
* 缩进的空格数不重要，只要相同层级的元素左对齐即可
* 用 `#` 表示注释


``` yaml
# YAML Ain't Markup Language

person:
  name: &name "mike" # &name 设置锚点
  occupation: "programmer"
  age: !!float 23 # 23.0  # \!!类型转换
  gpa: !!str 3.5 # "3.5"
  fav_num: 1e+10 # 科学计数法
  male: true # 布尔值
  brithday: 1994-02-06 14:33:22 # 时间戳
  flaws: null # 空
  hobbies: # 数组 区块格式
    - hiking
    - movies
    - riding bike
  movies: ["Dark Knight", "Good Will Hunting"] # 数组 内联格式
  friends:
    - name: "Steph"
      age: 22
    - { name: "Adam", age:22 }
  description: > # 字符串 > 折叠换行
    So pay attention to the product description
    and reviews if your primary goal is to
    whip up your morning beverage in record time.
  signature: | # 字符串 | 保留换行
    Mike
    Giraffe Academy
    email - giraffeacademy@gmail.com
  id: *name # *name 引用锚点

  base: &base
    var1: value1

  foo:
    <<: *base # var1: value1   << 合并标签，类型继承
    var2: value2
```



# Q&A

## [How do you do block comments in YAML?](https://stackoverflow.com/questions/2276572/how-do-you-do-block-comments-in-yaml)

How do I comment a block of lines in YAML?

YAML supports inline comments, but does not support block comments.

From [Wikipedia](http://en.wikipedia.org/wiki/YAML):

> Comments begin with the number sign ( `#` ), can start anywhere on a line, and continue until the end of the line

A comparison with JSON, also from [Wikipedia](https://en.wikipedia.org/wiki/YAML#Comparison_with_JSON):

> The syntax differences are subtle and seldom arise in practice: JSON allows extended charactersets like UTF-32, YAML requires a space after separators like comma, equals, and colon while JSON does not, and some non-standard implementations of JSON extend the grammar to include Javascript's /* ... */ comments. Handling such edge cases may require light pre-processing of the JSON before parsing as in-line YAML.


# Refer

* [YAML | In One Video](https://www.youtube.com/watch?v=cdLNKUoMc6c)







