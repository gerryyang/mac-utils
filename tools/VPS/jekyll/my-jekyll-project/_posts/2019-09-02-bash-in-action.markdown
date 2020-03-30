---
layout: post
title:  "Bash in Action"
date:   2019-09-02 17:00:00 +0800
categories: [Bash, 编程语言]
---

* Do not remove this line (it will not be displayed)
{:toc}


# Bash Utils

## argument

如何将参数传给bash脚本，包括固定参数，非固定参数，以及参数选项的用法。

* 固定参数

Arguments are accessed inside a script using the variables $1, $2, $3, etc., where $1 refers to the first argument, $2 to the second argument, and so on. 

* 变参参数

If you have a variable number of arguments, you can use the "$@" variable, which is an array of all the input parameters. This means you can use a for-loop to iteratively process each one.

```
#!/bin/bash

for FILE1 in "$@"
do
 wc $FILE1
done
```

如果是固定个数的参数，可以通过$1，$2，等来获取参数，而如果参数个数不固定，可以通过`$@`来遍历获取每个参数。

* 参数选项

```
#!/bin/bash

while getopts u:d:p:f: option
do
 case "${option}"
 in
 u) USER=${OPTARG};;
 d) DATE=${OPTARG};;
 p) PRODUCT=${OPTARG};;
 f) FORMAT=$OPTARG;;
 esac
done
```

如果flag后面带有冒号，那么代表此flag需要带有value。相反，如果没有冒号，则此flag可以不需要value。也就是，如果指定了某个flag需要带有value，但是没有传value，就会报类似`No arg for -u option`这样的错误。


## group argument

```
#!/bin/bash

func() {
	echo "$1" # 输出第一个参数
}

INFO="a b c"
func $INFO
```

output:

```
a
```

如果需要将参数作为一个group传递，需要`Enclose the variable in double quotes to preserve white spaces`。

```
#!/bin/bash

func() {
	echo "$1"
}

INFO="a b c"
func "$INFO"   # 注意使用双引号
```

output:

```
a b c
```

refer:

* [passing-a-group-of-words-as-a-bash-script-argument](https://stackoverflow.com/questions/41405764/passing-a-group-of-words-as-a-bash-script-argument)



## Loop

```
#!/bin/bash
for i in {1..5}
do
   echo "$i"
done
```

```
#!/bin/bash
START=1
END=5
echo "Countdown"
 
for (( c=$START; c<=$END; c++ ))
do
	echo -n "$c "
	sleep 1
done
 
echo
echo "Boom!"
```

output:

```
Countdown
1 2 3 4 5 
Boom!
```

```
#!/bin/bash
START=1
END=5
## save $START, just in case if we need it later ##
i=$START
while [[ $i -le $END ]]
do
    echo "$i"
    ((i = i + 1))
done
```

```
#!/bin/bash
START=1
END=5
for i in $(eval echo "{$START..$END}")
do
	echo "$i"
done
```

```
#!/bin/bash
 
## define an array ##
arrayname=( Dell HP Oracle )
 
## get item count using ${arrayname[@]} ##
for m in "${arrayname[@]}"
do
  echo "${m}"
  # do something on $m #
done
```

output:

```
Dell
HP
Oracle
```

refer:

* [HowTo: Iterate Bash For Loop Variable Range Under Unix / Linux](https://www.cyberciti.biz/faq/unix-linux-iterate-over-a-variable-range-of-numbers-in-bash/)


## [How to Check if a File or Directory Exists in Bash](https://linuxize.com/post/bash-check-if-file-exists/)


In Bash, you can use the `test` command to check whether a file exists and determine the type of the file.

``` bash
test EXPRESSION
[ EXPRESSION ]
[[ EXPRESSION ]]
```

> If you want your script to be portable you should prefer using the old test `[` command which is available on all POSIX shells. The new upgraded version of the test command `[[` (double brackets) is supported on most modern systems using Bash, Zsh, and Ksh as a default shell.

``` bash
FILE=/etc/resolv.conf
if test -f "$FILE"; then
    echo "$FILE exist"
fi

if [ -f "$FILE" ]; then
    echo "$FILE exist"
fi

if [[ -f "$FILE" ]]; then
    echo "$FILE exist"
fi

if [ -f "$FILE" ]; then
    echo "$FILE exist"
else 
    echo "$FILE does not exist"
fi

test -f /etc/resolv.conf && echo "$FILE exist"

[ -f /etc/resolv.conf ] && echo "$FILE exist"

[[ -f /etc/resolv.conf ]] && echo "$FILE exist"

FILE=/etc/docker
if [ -d "$FILE" ]; then
    echo "$FILE is a directory"
fi

FILE=/etc/docker
if [ ! -f "$FILE" ]; then
    echo "$FILE does not exist"
fi

# Check if Multiple Files Exist
FILE=/etc/docker
if [ -f /etc/resolv.conf -a -f /etc/hosts ]; then
    echo "$FILE is a directory"
fi

if [ -f /etc/resolv.conf && -f /etc/hosts ]; then
    echo "$FILE is a directory"
fi

[ -f /etc/resolv.conf -a -f /etc/hosts ] && echo "both files exist"

[[ -f /etc/resolv.conf && -f /etc/hosts ]] && echo "both files exist"

```

> Always use double quotes to avoid issues when dealing with files containing whitespace in their names.

**File test operators**

```
-b FILE - True if the FILE exists and is a block special file.
-c FILE - True if the FILE exists and is a special character file.
-d FILE - True if the FILE exists and is a directory.
-e FILE - True if the FILE exists and is a file, regardless of type (node, directory, socket, etc.).
-f FILE - True if the FILE exists and is a regular file (not a directory or device).
-G FILE - True if the FILE exists and has the same group as the user running the command.
-h FILE - True if the FILE exists and is a symbolic link.
-g FILE - True if the FILE exists and has set-group-id (sgid) flag set.
-k FILE - True if the FILE exists and has a sticky bit flag set.
-L FILE - True if the FILE exists and is a symbolic link.
-O FILE - True if the FILE exists and is owned by the user running the command.
-p FILE - True if the FILE exists and is a pipe.
-r FILE - True if the FILE exists and is readable.
-S FILE - True if the FILE exists and is socket.
-s FILE - True if the FILE exists and has nonzero size.
-u FILE - True if the exists and set-user-id (suid) flag is set.
-w FILE - True if the FILE exists and is writable.
-x FILE - True if the FILE exists and is executable.
```



# Other

* [Bash Programming Tutorial](https://ianding.io/2019/08/30/bash-programming-tutorial/)
* [Don't Overuse hjkl in Vim](https://ianding.io/2019/08/16/dont-overuse-hjkl-in-vim/)
* [Multi-file Search and Replace in Vim](https://ianding.io/2019/08/22/multi-file-search-and-replace-in-vim/)




