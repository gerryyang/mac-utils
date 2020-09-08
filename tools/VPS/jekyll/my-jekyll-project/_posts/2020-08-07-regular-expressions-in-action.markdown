---
layout: post
title:  "Regular Expressions in Action"
date:   2020-08-07 11:00:00 +0800
categories: 编程语言
---

* Do not remove this line (it will not be displayed)
{:toc}



# Matching IPv4 Addresses


```
# Simple regex to check for an IP address
^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$

# Accurate regex to check for an IP address
^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$

# Simple regex to extract IP addresses from longer text
\b(?:[0-9]{1,3}\.){3}[0-9]{1,3}\b

# Accurate regex to extract IP addresses from longer text
\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b

# Simple regex that captures the four parts of the IP address
^([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})$

# Accurate regex that captures the four parts of the IP address
^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$
```

refer: 

* [Regular Expressions Cookbook - 7.16. Matching IPv4 Addresses](https://www.oreilly.com/library/view/regular-expressions-cookbook/9780596802837/ch07s16.html)




  

	
	