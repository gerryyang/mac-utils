---
layout: post
title:  "Bash in Action"
date:   2019-09-02 17:00:00 +0800
categories: [Bash]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Bash Tools

* [Execute Bash Shell Online (GNU Bash v4.4)](https://www.tutorialspoint.com/execute_bash_online.php)

# Shell Style Guide

* [Google's Shell Style Guide](https://google.github.io/styleguide/shellguide.html#s7-naming-conventions)
* https://unix.stackexchange.com/questions/42847/are-there-naming-conventions-for-variables-in-shell-scripts

## Names convention

Environment variables or shell variables introduced by the operating system, shell startup scripts, or the shell itself, etc., are usually all in **CAPITALS**.

To prevent your variables from conflicting with these variables, it is a good practice to use **lower_case** variable names.

> **Variable Names**: Lower-case, with underscores to separate words. Ex: `my_variable_name`
>
> **Constants and Environment Variable Names**: All caps, separated with underscores, declared at the top of the file. Ex: `MY_CONSTANT`

# Shell-Parameter-Expansion

https://www.gnu.org/savannah-checkouts/gnu/bash/manual/bash.html#Shell-Parameter-Expansion

## ${#parameter}

**The length in characters of the expanded value of parameter is substituted**. If parameter is `*` or `@`, the value substituted is the number of positional parameters. If parameter is an array name subscripted by `*` or `@`, the value substituted is the number of elements in the array. If parameter is an indexed array name subscripted by a negative number, that number is interpreted as relative to one greater than the maximum index of parameter, so negative indices count back from the end of the array, and an index of -1 references the last element.

``` bash
myvar="some string"
echo ${#myvar}    # 11
```

* https://stackoverflow.com/questions/17368067/length-of-string-in-bash





# Tips

## [What is the difference between ${var}, "$var", and "${var}" in the Bash shell?](https://stackoverflow.com/questions/18135451/what-is-the-difference-between-var-var-and-var-in-the-bash-shell)

``` bash
# define a array
declare -a groups

groups+=("123")
groups+=("456")

# print
# 123
# 456
for group in "${groups[@]}"; do
    echo $group
done

# print
# 123
#for group in $groups; do
#    echo $group
#done

# print
# 123
#for group in ${groups}; do
#    echo $group
#done
```

Question:

Only the first one accomplishes what I want: to iterate through each element in the array. I'm not really clear on the differences between `$groups`, `"$groups"`, `${groups}` and `"${groups}"`. If anyone could explain it, I would appreciate it.

Answers:

> Braces (`$var` vs. `${var}`)

In most cases, `$var` and `${var}` are the same:

``` bash
var=foo
echo $var
# foo
echo ${var}
# foo
```

The braces are only needed to resolve **ambiguity** in expressions:

``` bash
var=foo
echo $varbar
# Prints nothing because there is no variable 'varbar'
echo ${var}bar
# foobar
```

> Quotes (`$var` vs. `"$var"` vs. `"${var}"`)

When you add **double quotes** around a variable, you tell the shell to **treat it as a single word**, even if it contains **whitespaces**:

``` bash
var="foo bar"
for i in "$var"; do # Expands to 'for i in "foo bar"; do...'
    echo $i         #   so only runs the loop once
done
# foo bar
```

Contrast that behavior with the following:

``` bash
var="foo bar"
for i in $var; do # Expands to 'for i in foo bar; do...'
    echo $i       #   so runs the loop twice, once for each argument
done
# foo
# bar
```

As with `$var` vs. `${var}`, the braces are only needed for **disambiguation(消除模棱两可情况)**, for example:

``` bash
var="foo bar"
for i in "$varbar"; do # Expands to 'for i in ""; do...' since there is no
    echo $i            #   variable named 'varbar', so loop runs once and
done                   #   prints nothing (actually "")

var="foo bar"
for i in "${var}bar"; do # Expands to 'for i in "foo barbar"; do...'
    echo $i              #   so runs the loop once
done
# foo barbar
```

Note that `"${var}bar"` in the second example above could also be written `"${var}"bar`, in which case you don't need the braces anymore, i.e. `"$var"bar`. **However, if you have a lot of quotes in your string these alternative forms can get hard to read** (and therefore hard to maintain). [This page](http://tldp.org/LDP/Bash-Beginners-Guide/html/sect_03_03.html) provides a good introduction to quoting in Bash.

> Arrays (`$var` vs. `$var[@]` vs. `${var[@]}`)

Now for your array. According to the [bash manual](http://linux.die.net/man/1/bash):

> Referencing an array variable without a subscript is equivalent to referencing the array with a subscript of 0.

In other words, if you don't supply an index with [], you get the first element of the array:

``` bash
foo=(a b c)
echo $foo
# a
```

Which is exactly the same as

``` bash
foo=(a b c)
echo ${foo}
# a
```

To get all the elements of an array, you need to use `@` as the index, e.g. `${foo[@]}`. The braces are required with arrays because without them, the shell would expand the `$foo` part first, giving the first element of the array followed by a literal `[@]`:

``` bash
foo=(a b c)
echo ${foo[@]}
# a b c
echo $foo[@]
# a[@]
```

[This page](http://tldp.org/LDP/Bash-Beginners-Guide/html/sect_10_02.html) is a good introduction to arrays in Bash.

> Quotes revisited (`${foo[@]}` vs. `"${foo[@]}"`)

You didn't ask about this but it's a subtle difference that's good to know about. If the elements in your array could contain whitespace, you need to use double quotes so that each element is treated as a separate "word:"

``` bash
foo=("the first" "the second")
for i in "${foo[@]}"; do # Expands to 'for i in "the first" "the second"; do...'
    echo $i              #   so the loop runs twice
done
# the first
# the second
```

Contrast this with the behavior without double quotes:

``` bash
foo=("the first" "the second")
for i in ${foo[@]}; do # Expands to 'for i in the first the second; do...'
    echo $i            #   so the loop runs four times!
done
# the
# first
# the
# second
```


refer:

* https://stackoverflow.com/questions/255898/how-to-iterate-over-arguments-in-a-bash-script
* [Parameter expansion](http://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html)



## [How to check if a string contains a substring in Bash](https://stackoverflow.com/questions/229551/how-to-check-if-a-string-contains-a-substring-in-bash)

You can use [Marcus's answer (* wildcards)](https://stackoverflow.com/a/229585/3755692) outside a case statement, too, if you use double brackets:

``` bash
string='My long string'
if [[ $string == *"My long"* ]]; then
  echo "It's there!"
fi
```

Note that spaces in the needle string need to be placed between **double quotes**, and the `*` wildcards should be outside. Also note that a simple comparison operator is used (i.e. `==`), not the regex operator `=~`.

If you prefer the regex approach:

``` bash
string='My string';

if [[ $string =~ "My" ]]; then
   echo "It's there!"
fi
```

I am not sure about using an if statement, but you can get a similar effect with a case statement:

``` bash
case "$string" in
  *foo*)
    # Do stuff
    ;;
esac
```

## [How to portability use "${@:2}"?](https://stackoverflow.com/questions/56822216/how-to-portability-use-2)

Neither `${@:2}` nor `${*:2}` is portable, and many shells will reject both as invalid syntax. If you want to process all arguments except the first, you should get rid of the first with a `shift`.

At this point, the first argument is in "$first" and the positional parameters are shifted down one.


``` bash
#!/bin/bash

if [[ $# -eq 1 ]]; then
        echo "$1"
else
        echo "$1 $(printf '%q' "${@:2}")"
fi

echo "another way..."

first="${1}"
shift
echo The arguments after the first are:
for x; do echo "$x"; done
```

* https://linuxize.com/post/bash-printf-command/





# Shell Parameter Expansion (Default value)

``` bash
if [ -z "${VARIABLE}" ]; then
    FOO='default'
else
    FOO=${VARIABLE}
fi
```

To get the assigned value, or default if it's missing:

``` bash
FOO="${VARIABLE:-default}"  # If variable not set or null, use default.
# If VARIABLE was unset or null, it still is after this (no assignment done).
```

Or to assign default to VARIABLE at the same time:

``` bash
FOO="${VARIABLE:=default}"  # If variable not set or null, set it to default.
```

refer:

* [Assigning default values to shell variables with a single command in bash](https://stackoverflow.com/questions/2013547/assigning-default-values-to-shell-variables-with-a-single-command-in-bash)
* [3.5 Shell Expansions](https://tiswww.case.edu/php/chet/bash/bashref.html#Shell-Expansions)


# Compare `#!/bin/bash --login` with `#!/bin/bash`

The main difference is that a login shell executes your profile when it starts. From the man page:

```
When bash is invoked as an interactive login shell, or as a non-interactive shell with the --login option, it first reads and executes commands from the file /etc/profile, if that file exists. After reading that file, it looks for ~/.bash_profile, ~/.bash_login, and ~/.profile, in that order, and reads and executes commands from the first one that exists and is readable. The --noprofile option may be used when the shell is started to inhibit this behavior.

When a login shell exits, bash reads and executes commands from the file ~/.bash_logout, if it exists.
```

https://stackoverflow.com/questions/25677790/bin-bash-login-vs-bin-bash

# Align

For bash, use the `printf` command with alignment flags.

``` bash
echo "Usage: $0 [option] [value]"
printf "%-16s %-64s\n" -h 查看帮助
printf "%-16s %-64s\n" -h xxxx
```

```
Usage: main.sh [option] [value]
-h               查看帮助
-h               xxxx
```

* %s %c %d %f 都是格式替代符，％s 输出一个字符串，％d 整型输出，％c 输出一个字符，％f 输出实数，以小数形式输出
* %-10s 指一个宽度为 10 个字符（- 表示左对齐，没有则表示右对齐），任何字符都会被显示在 10 个字符宽的字符内，如果不足则自动以空格填充，超过也会将内容全部显示出来
* %-4.2f 指格式化为小数，其中 .2 指保留2位小数


* https://stackoverflow.com/questions/994461/right-align-pad-numbers-in-bash
* https://www.runoob.com/linux/linux-shell-printf.html

# PreCheck

``` bash
CURTIME="date +'%Y-%m-%d %H:%M:%S'"
NOW="echo [\`$CURTIME\`][PID:$$]"

function CHECK()
{
    if [ $? -ne 0 ]; then
        echo -e "\033[031;1m[ERROR]\033[0m $1"
        exit 1
    else
        echo -e "\033[032;1m[OK]\033[0m $1"
    fi
}

function print_usages()
{
    echo "Usage: $0 a b c"
}

function job_start()
{
    echo "`eval $NOW` job_start"
}

function job_success()
{
    MSG="$*"
    echo "`eval $NOW` job_success:[$MSG]"
    exit 0
}

function job_fail()
{
    MSG="$*"
    echo "`eval $NOW` job_fail:[$MSG]"
    exit 1
}

if [[ $# -lt 3 ]]; then
    print_usages
    job_fail "params invalid"
fi

set -x
a=$1
```

# The ‘ls’ command – how to show seconds

``` bash
# show also the seconds (and not only seconds but also microseconds)
$ ls --full-time
$ ls -l --time-style=full-iso
```

```
$ ls -l core_worldsvr_1632113278.5873
-rw------- 1 user00 users 794185728 Sep 20 12:47 core_worldsvr_1632113278.5873
$ ls -l core_worldsvr_1632113278.5873 --full-time
-rw------- 1 user00 users 794185728 2021-09-20 12:47:59.224668459 +0800 core_worldsvr_1632113278.5873
$ ls -l core_worldsvr_1632113278.5873 --time-style=full-iso
-rw------- 1 user00 users 794185728 2021-09-20 12:47:59.224668459 +0800 core_worldsvr_1632113278.5873
```

# CPU Monitor

``` bash
#!/bin/bash

dir_head_prefix="./datacpu"

G_FORMATE_TIME=`date +"%Y%m%d%H%M%S"`
dir_head=${dir_head_prefix}"_"${G_FORMATE_TIME}
G_SLEEP_INTERVAL=10

echo $#
echo $*
[ $# -lt 2 ]  && {  echo "Usage: sh monitor_cpu_single_core.sh ps的name 监控时间(单位:mins)";  exit 1; }

if [ ! -d ${dir_head} ];then
	mkdir -p ${dir_head}
else
	rm -rf ${dir_head}/*
fi

core_num=$(cat /proc/cpuinfo  | grep processor | wc -l )

function get_sum()
{
	local input=$1
	input=($input)
	num=${#input[@]}
	sum=0
	for((i=0;i<$num;i++))
	do
		let sum=$sum+${input[i]}

	done
	echo $sum
}

run_time_second=$2

let run_time_second=$run_time_second*60

#manager_name="ETC: manager 0 ger "
manager_name=$1
start_time=`date +%s`
let end_time=$run_time_second+$start_time
now=`date +%s`

echo $manager_name
echo $run_time_second

while [[ $now -lt $end_time ]]
do
	child_ids=$(ps -ef | grep -e "$manager_name" | grep -v celery  | grep -v memmonitor_ros | grep -v ".sh" | grep -v grep |  awk -F ' ' '{ print $2}')
        child_ids=($child_ids)
	child_names=$(ps -ef | grep -e "$manager_name" | grep -v celery  | grep -v memmonitor_ros | grep -v ".sh" | grep -v grep |  awk -F ' ' '{ print $8"-"$9}')
	child_names=($child_names)

	echo "manager_pid: $manager_pid"

	totalCpuTime=$(cat /proc/stat | grep -w cpu | sed "s/cpu/0/g")
	total1=$(get_sum "${totalCpuTime[*]}")
	#i=1 tr -d "   PID TTY          TIME CMD"
	for((i=0;i < ${#child_ids[@]};i++))
	do
		each=${child_ids[i]}
		process=""
		process=$(cat /proc/$each/stat | awk -F  " " '{print $14,$15,$16,$17}')
		pro1[i]=$(get_sum "${process[*]}")
	done

	for((i=0;i < ${#child_names[@]};i++))
	do
		each=${child_names[i]}
		each_name=`echo $each | tr -d " " | sed "s/\//_/g"`
		logfile[i]=${dir_head}"/"${child_ids[i]}"-"${each_name}.log
	done


	sleep ${G_SLEEP_INTERVAL}

	totalCpuTime=$(cat /proc/stat | grep -w cpu | sed "s/cpu/0/g")
	total2=$(get_sum "${totalCpuTime[*]}")

	for((i=1;i < ${#child_ids[@]};i++))
	do
		each=${child_ids[i]}
		process=""
		process=$(cat /proc/$each/stat | awk -F  " " '{print $14,$15,$16,$17}')
		pro2[i]=$(get_sum "${process[*]}")
	done

	let diff_cpu=$total2-$total1
	for((i=1;i < ${#child_ids[@]};i++))
	do
		diff_pro=0
		rate=0
		let diff_pro=${pro2[i]}-${pro1[i]}
		rate=`awk 'BEGIN{ rate='$core_num'*100*'$diff_pro'/'$diff_cpu'; print rate}'`

		rate=`echo $rate| awk '{if($1>100) $1=100;print $1}'` # 100校验

		echo -e "`date +"%Y-%m-%d %H:%M:%S"`\t$rate" >> ${logfile[i]}
	done


	now=`date +%s`
done
```

# Make sure only one instance to run

A solution that does not require additional tools would be prefered.

1. Use a lock directory. Directory creation is atomic under linux and unix and *BSD and a lot of other OSes.

``` bash
if mkdir $LOCKDIR
then
    # Do important, exclusive stuff
    if rmdir $LOCKDIR
    then
        echo "Victory is mine"
    else
        echo "Could not remove lock dir" >&2
    fi
else
    # Handle error condition
    ...
fi
```

2. `pidof -o %PPID -x $0` gets the PID of the existing script if its already running or exits with error code 1 if no other script is running

``` bash
#!/bin/bash

# Check if another instance of script is running
pidof -o %PPID -x $0 >/dev/null && echo "ERROR: Script $0 already running" && exit 1
```

* [How to make sure only one instance of a bash script runs?](https://unix.stackexchange.com/questions/48505/how-to-make-sure-only-one-instance-of-a-bash-script-runs)

# Add User

``` bash
#!/bin/bash

if [ $# -ne 1 ]
then
  echo "Usage: $0 USER_NAME"
  exit 1
fi
USER_NAME=$1

USER_GROUP_NAME="users"
PWD="123"
USER_HOME="/data/home/${USER_NAME}"
useradd ${USER_NAME} -d ${USER_HOME} -g ${USER_GROUP_NAME}

echo ${USER_NAME}:${PWD} | chpasswd

# $ cat /etc/sudoers
# Sudoers allows particular users to run various commands as
# the root user, without needing the root password.
# $ cat /etc/sudoers | grep wheel
# Allows people in group wheel to run all commands
# %wheel  ALL=(ALL)       ALL
# echo "${USER_NAME}   ALL=(ALL)       ALL" >> /etc/sudoers
usermod -aG wheel ${USER_NAME}

chown -R ${USER_NAME}:${USER_GROUP_NAME}  ${USER_HOME}
```

# 1>&2 / 2>&1

``` bash
if test $# -ne 1; then
    echo "Usage: `basename $0 .sh` <process-id>" 1>&2
    exit 1
fi
```

* There are 3 file descriptors, `stdin`, `stdout` and `stderr` (std=standard).
* `1` represents `stdout` and `2` `stderr`

| Usage | Example
| -- | --
| stdout 2 file | ls -l > ls-l.txt
| stderr 2 file | grep da * 2> grep-errors.txt
| stdout 2 stderr | grep da * 1>&2
| stderr 2 stdout | grep * 2>&1
| stderr and stdout 2 file | program > file.txt 2>&1


* https://tldp.org/HOWTO/Bash-Prog-Intro-HOWTO-3.html
* https://stackoverflow.com/questions/818255/in-the-shell-what-does-21-mean

# Substring Removal

* `${string#substring}` Deletes **shortest** match of `$substring` from **front** of `$string`.
* `${string##substring}` Deletes **longest** match of `$substring` from **front** of `$string`.
* `${string%substring}` Deletes **shortest** match of `$substring` from **back** of `$string`.
* `${string%%substring}` Deletes **longest** match of `$substring` from **back** of `$string`.

``` bash
stringZ=abcABC123ABCabc
#       |----|          shortest
#       |----------|    longest

echo ${stringZ#a*C}      # 123ABCabc
# Strip out shortest match between 'a' and 'C'.

echo ${stringZ##a*C}     # abc
# Strip out longest match between 'a' and 'C'.



# You can parameterize the substrings.

X='a*C'

echo ${stringZ#$X}      # 123ABCabc
echo ${stringZ##$X}     # abc
                        # As above.

# Rename all filenames in $PWD with "TXT" suffix to a "txt" suffix.
# For example, "file1.TXT" becomes "file1.txt" . . .

SUFF=TXT
suff=txt

for i in $(ls *.$SUFF)
do
  mv -f $i ${i%.$SUFF}.$suff
  #  Leave unchanged everything *except* the shortest pattern match
  #+ starting from the right-hand-side of the variable $i . . .
done ### This could be condensed into a "one-liner" if desired.


stringZ=abcABC123ABCabc
#                    ||     shortest
#        |------------|     longest

echo ${stringZ%b*c}      # abcABC123ABCa
# Strip out shortest match between 'b' and 'c', from back of $stringZ.

echo ${stringZ%%b*c}     # a
# Strip out longest match between 'b' and 'c', from back of $stringZ.
```

Refer: https://tldp.org/LDP/abs/html/string-manipulation.html


# Concatenate Strings

The simplest way to concatenate two or more string variables is to write them one after another:

``` bash
VAR1="Hello,"
VAR2=" World"
VAR3="$VAR1$VAR2"
echo "$VAR3"
```

You can also concatenate one or more variable with literal strings. In the example above variable `VAR1` is enclosed in curly braces to protect the variable name from surrounding characters. When the variable is followed by another valid variable-name character you must enclose it in curly braces `${VAR1}`.

``` bash
VAR1="Hello, "
VAR2="${VAR1}World"
echo "$VAR2"
```

Another way of concatenating strings in bash is by appending variables or literal strings to a variable using the `+=` operator:

``` bash
VAR1="Hello, "
VAR1+=" World"
echo "$VAR1"
```

The following example is using the `+=` operator to concatenate strings in bash for loop :

``` bash
VAR=""
for ELEMENT in 'Hydrogen' 'Helium' 'Lithium' 'Beryllium'; do
  VAR+="${ELEMENT} "
done

echo "$VAR"
```

* https://linuxize.com/post/bash-concatenate-strings/

# Compare

## Compare Strings

* `string1 = string2` and `string1 == string2` - The equality operator returns true if the operands are equal.
    + Use the `=` operator with the test `[` command.
    + Use the `==` operator with the `[[` command for pattern matching.
* `string1 != string2` - The inequality operator returns true if the operands are not equal.
* `string1 =~ regex` - The regex operator returns true if the left operand matches the extended regular expression on the right.
* `string1 > string2` - The greater than operator returns true if the left operand is greater than the right sorted by lexicographical (alphabetical) order.
* `string1 < string2` - The less than operator returns true if the right operand is greater than the right sorted by lexicographical (alphabetical) order.
* `-z string` - True if the string length is zero.
* `-n string` - True if the string length is non-zero.

示例：

``` bash
if [[ $1 = "start" ]]; then
  # string compare
  # 等于 = (plain sh) / == (only bash)
  # 不等于 !=
fi

if [[ -n $VarName ]]; then
  # not empty, it's the opposite of -z
fi

if [[ -z "$1" ]]; then
  echo "This succeeds if $1 is null or unset"
fi

# or check
if [ "$fname" = "a.txt" ] || [ "$fname" = "c.txt" ]

[[ "string1" == "string2" ]] && echo "Equal" || echo "Not equal"
```

## Check if a String Contains a Substring

There are multiple ways to check if a string contains a substring. One approach is to use surround the substring with asterisk symbols `*` which means match all characters.

``` bash
#!/bin/bash

VAR='GNU/Linux is an operating system'
if [[ $VAR == *"Linux"* ]]; then
  echo "It's there."
fi
```

Another option is to use the regex operator `=~` as shown below. The period followed by an asterisk `.*` matches zero or more occurrences any character except a newline character.

``` bash
#!/bin/bash

VAR='GNU/Linux is an operating system'
if [[ $VAR =~ .*Linux.* ]]; then
  echo "It's there."
fi
```

## Comparing Strings with the Case Operator

Instead of using the `test` operators you can also use the `case` statement to compare strings:

``` bash
#!/bin/bash

VAR="Arch Linux"

case $VAR in

  "Arch Linux")
    echo -n "Linuxize matched"
    ;;

  Fedora | CentOS)
    echo -n "Red Hat"
    ;;
esac
```

## Lexicographic Comparison

Lexicographical comparison is an operation where two strings are compared alphabetically by comparing the characters in a string sequentially from left to right. This kind of comparison is rarely used. The following scripts compare two strings lexicographically:

``` bash
#!/bin/bash

VAR1="Linuxize"
VAR2="Ubuntu"

if [[ "$VAR1" > "$VAR2" ]]; then
    echo "${VAR1} is lexicographically greater then ${VAR2}."
elif [[ "$VAR1" < "$VAR2" ]]; then
    echo "${VAR2} is lexicographically greater than ${VAR1}."
else
    echo "Strings are equal"
fi
```

## Compare Digital

``` bash
if [[ $# -eq 0 ]]; then
  # digital compare
  # -eq / -lt / -le / -gt / -ge / -ne
fi

./run.sh
if [[ $? -ne 0 ]];then
  echo "run.sh failed"
  exit 1
fi
```

## Others

``` bash
num=4; if (test $num -gt 5); then echo "yes"; else echo "no"; fi

file="/etc/passwd"; if [ -e $file ]; then echo "whew"; else echo "uh-oh"; fi

if [[ -f "proc.pid" ]]; then
  pid=`cat proc.pid`
fi
```

* [Is double square brackets [[ ]] preferable over single square brackets [ ] in Bash?](https://stackoverflow.com/questions/669452/is-double-square-brackets-preferable-over-single-square-brackets-in-ba)
* [Bash test builtin command](https://www.computerhope.com/unix/bash/test.htm)
* https://unix.stackexchange.com/questions/47584/in-a-bash-script-using-the-conditional-or-in-an-if-statement
* [How to Compare Strings in Bash](https://linuxize.com/post/how-to-compare-strings-in-bash/)
* https://linuxhint.com/bash-test-command/

# ForceStopAll

``` bash
ps aux | grep `pwd` | grep -v grep | awk '{print $2}' | xargs kill -9
```

# Color

``` bash
function PrintColor {
        echo -e "\e[1;31m$@\e[0m"
}
PrintColor "Oops!"
```

定义常用变量

``` bash
ColorRedBeg="\e[1;31m"
ColorGreenBeg="\e[1;32m"
ColorYellowBeg="\e[1;33m"
ColorBlueBeg="\e[1;34m"
ColorMagentaBeg="\e[1;35m"
ColorCyanBeg="\e[1;36m"
ColorWhiteBeg="\e[1;37m"
ColorEnd="\e[m"

function Usage()
{
    printf "$ColorBlueBeg%-16s\n$ColorEnd" "Usage: $0 [option] [value]"
    printf "%-16s\n" "选项说明:"
    printf "$ColorGreenBeg%-32s %-64s\n$ColorEnd" "-h" "查看帮助"
}
```

``` bash
#!/bin/bash

# prints colored text
print_style () {

    if [ "$2" == "info" ] ; then
        COLOR="96m";
    elif [ "$2" == "success" ] ; then
        COLOR="92m";
    elif [ "$2" == "warning" ] ; then
        COLOR="93m";
    elif [ "$2" == "danger" ] ; then
        COLOR="91m";
    else #default color
        COLOR="0m";
    fi

    STARTCOLOR="\e[$COLOR";
    ENDCOLOR="\e[0m";

    printf "$STARTCOLOR%b$ENDCOLOR" "$1";
}

print_style "This is a green text " "success";
print_style "This is a yellow text " "warning";
print_style "This is a light blue with a \t tab " "info";
print_style "This is a red text with a \n new line " "danger";
print_style "This has no color";
```

* https://stackoverflow.com/questions/5412761/using-colors-with-printf


# CurrentPath

``` bash
#!/bin/bash

echo $0                            # 脚本名
echo $(dirname $0)                 # 获取当前脚本的相对路径
echo $(readlink -f $0)             # readlink 是显示符号链接所指向的位置，如果 $0 不是符号链接，就显示文件本身的绝对路径
echo $(dirname $(readlink -f $0))  # 获取当前脚本的绝对路径

```

执行：

```
$sh dirname.sh
dirname.sh
.
/data/home/gerryyang/test/bash/dirname.sh
/data/home/gerryyang/test/bash

$cd ..
$sh bash/dirname.sh
bash/dirname.sh
bash
/data/home/gerryyang/test/bash/dirname.sh
/data/home/gerryyang/test/bash
```

# Function

``` bash
#!/bin/bash
function quit {
   exit
}
function hello {
   echo Hello!
}
hello
quit
echo foo
```

Functions with parameters sample

``` bash
#!/bin/bash
function quit {
   exit
}
function e {
    echo $1
}
e Hello
e World
quit
echo foo
```

* https://tldp.org/HOWTO/Bash-Prog-Intro-HOWTO-8.html
* [How to Return a String from Bash Functions](https://linuxhint.com/return-string-bash-functions/)

# Chmod

```
#!/bin/bash

find ../ -type f |  grep -E "*\.sh" | xargs chmod +x
find ../ -type f |  grep -E "*\.sh" | xargs dos2unix
```


# Bash Tips

## 日志输出

通过指定日志级别控制日志输出。

``` bash
#!/bin/bash

_log() {
    if [ "$_DEBUG" == "true" ]; then
        echo 1>&2 "$@"
    fi
}

_log "do something..."
echo "Hello $USER"
```

执行：

```
$ sh ./test.sh
Hello gerryyang

$ _DEBUG=true sh ./test.sh
do something...
Hello gerryyang
```


## Debug

``` bash
set -e  # exit immediately on error
set -x  # display all commands
```

``` bash
#!/bin/bash

echo "Hello $USER,"
echo "Today is $(date +'%Y-%m-%d')"
```

执行或调试：

```
sh ./test.sh
Hello gerryyang,
Today is 2020-08-03

$ sh -x ./test.sh
+ echo 'Hello gerryyang,'
Hello gerryyang,
++ date +%Y-%m-%d
+ echo 'Today is 2020-08-03'
Today is 2020-08-03
```

输出行号，需设置 `export PS4='+${BASH_SOURCE}:${LINENO}:${FUNCNAME[0]}: '`

```
$ sh -x ./test.sh
+./test.sh:3:: echo 'Hello gerryyang,'
Hello gerryyang,
++./test.sh:4:: date +%Y-%m-%d
+./test.sh:4:: echo 'Today is 2020-08-03'
Today is 2020-08-03
```

调试部份的脚本：

``` bash
#!/bin/bash

echo "Hello $USER,"
set -x
echo "Today is $(date +'%Y-%m-%d')"
set +x
```

调试：

```
$ sh -x./test.sh
+./test.sh:3:: echo 'Hello gerryyang,'
Hello gerryyang,
+./test.sh:4:: set -x
++./test.sh:5:: date +%Y-%m-%d
+./test.sh:5:: echo 'Today is 2020-08-03'
Today is 2020-08-03
+./test.sh:6:: set +x
```

可以不用`-x`调试选项：

```
$ sh ./test.sh
Hello gerryyang,
++./test.sh:5:: date +%Y-%m-%d
+./test.sh:5:: echo 'Today is 2020-08-03'
Today is 2020-08-03
+./test.sh:6:: set +x
```

如果需要更强大的功能，可以使用[BASH Debugger](http://bashdb.sourceforge.net/)。


# Bash Utils

## 特殊变量

变量名只能包含数字、字母和下划线，因为某些包含其他字符的变量有特殊含义，这样的变量被称为`特殊变量`。

* `$0`  当前脚本的文件名
* `$n`  传递给脚本或函数的参数。n 是一个数字，表示第几个参数。例如，第一个参数是`$1`，第二个参数是`$2`。
* `$#`  传递给脚本或函数的参数个数。
* `$*`  传递给脚本或函数的所有参数。
* `$@`  传递给脚本或函数的所有参数。被双引号(" ")包含时，与 `$*` 稍有不同。`$*` 和 `$@` 都表示传递给函数或脚本的所有参数，不被双引号(" ")包含时，都以`"$1" "$2" … "$n"` 的形式输出所有参数；被双引号(" ")包含时，`"$*"` 会将所有的参数作为一个整体，以`"$1 $2 … $n"`的形式输出所有参数；`"$@"` 会将各个参数分开，以`"$1" "$2" … "$n"`的形式输出所有参数。
* `$?`  上个命令的退出状态，或函数的返回值。
* `$$`  当前Shell进程ID。对于 Shell 脚本，就是这些脚本所在的进程ID。

``` bash
#! /bin/bash

echo "File Name: $0"
echo "First Parameter : $1"
echo "First Parameter : $2"
echo "Quoted Values: $@"
echo "Quoted Values: $*"
echo "Total Number of Parameters : $#"
```

执行：

```
$./test.sh Zara Ali
File Name : ./test.sh
First Parameter : Zara
Second Parameter : Ali
Quoted Values: Zara Ali
Quoted Values: Zara Ali
Total Number of Parameters : 2
```

`$*` 和 `$@` 用法测试：


``` bash
#! /bin/bash

echo "\$*=" $*
echo "\"\$*\"=" "$*"

echo "\$@=" $@
echo "\"\$@\"=" "$@"

echo "print each param from \$*"
for var in $*
do
    echo "$var"
done

echo "print each param from \$@"
for var in $@
do
    echo "$var"
done

echo "print each param from \"\$*\""
for var in "$*"
do
    echo "$var"
done

echo "print each param from \"\$@\""
for var in "$@"
do
    echo "$var"
done
```

## BASH_SOURCE 变量

保存了当前脚本的路径，包括文件名。

``` bash
CUR_DIR=$(dirname $(readlink -f $BASH_SOURCE))
PROJ_DIR=`readlink -f $CUR_DIR/../..`
```

## FUNCNAME 变量

``` bash
make()
{
    echo "$FUNCNAME: do nothing"
}
```


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

The basic syntax of getopts is (see: man bash):

``` bash
getopts OPTSTRING VARNAME [ARGS...]
```

where:

`OPTSTRING` is string with list of expected arguments,

`h` - check for option `-h` **without** parameters; gives error on unsupported options;
`h:` - check for option `-h` **with** parameter; gives errors on unsupported options;
`abc` - check for options `-a`, `-b`, `-c`; **gives** errors on unsupported options;
`:abc` - check for options `-a`, `-b`, `-c`; **silences** errors on unsupported options;

Notes: In other words, colon in front of options allows you handle the errors in your code. Variable will contain `?` in the case of unsupported option, `:` in the case of missing value.

`OPTARG` - is set to current argument value,

`OPTERR` - indicates if Bash should display error messages.

So the code can be:

``` bash
#!/usr/bin/env bash
usage() { echo "$0 usage:" && grep " .)\ #" $0; exit 0; }
[ $# -eq 0 ] && usage
while getopts ":hs:p:" arg; do
  case $arg in
    p) # Specify p value.
      echo "p is ${OPTARG}"
      ;;
    s) # Specify strength, either 45 or 90.
      strength=${OPTARG}
      [ $strength -eq 45 -o $strength -eq 90 ] \
        && echo "Strength is $strength." \
        || echo "Strength needs to be either 45 or 90, $strength found instead."
      ;;
    h | *) # Display help.
      usage
      exit 0
      ;;
  esac
done
```

Example usage:

```
$ ./foo.sh
./foo.sh usage:
    p) # Specify p value.
    s) # Specify strength, either 45 or 90.
    h | *) # Display help.
$ ./foo.sh -s 123 -p any_string
Strength needs to be either 45 or 90, 123 found instead.
p is any_string
$ ./foo.sh -s 90 -p any_string
Strength is 90.
p is any_string
```

See: [Small getopts tutorial](http://wiki.bash-hackers.org/howto/getopts_tutorial) at Bash Hackers Wiki

* https://stackoverflow.com/questions/16483119/an-example-of-how-to-use-getopts-in-bash
* [Handling positional parameters](https://wiki.bash-hackers.org/scripting/posparams)

## pushd/popd

``` bash
pushd () {
    command pushd "$@" > /dev/null
}

popd () {
    command popd "$@" > /dev/null
}
```

* https://stackoverflow.com/questions/25288194/dont-display-pushd-popd-stack-across-several-bash-scripts-quiet-pushd-popd

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

How can I read from variable with while read line? See [§3.6.7 "Here Strings" in the Bash Reference Manual](http://www.gnu.org/software/bash/manual/bashref.html#Here-Strings).

``` bash
GIT_DIFF=$(git diff --cached --name-status)
while read ST FILE; do
echo $ST "," $FILE
done <<< $GIT_DIFF
```

refer:

* [HowTo: Iterate Bash For Loop Variable Range Under Unix / Linux](https://www.cyberciti.biz/faq/unix-linux-iterate-over-a-variable-range-of-numbers-in-bash/)
* [How do I "read" a variable on a while loop](https://stackoverflow.com/questions/13122441/how-do-i-read-a-variable-on-a-while-loop)

## Array

``` bash
#!/bin/bash

LIST="a,b,c"
ARRAY=(${LIST//,/ })
echo "${ARRAY[@]}"
echo "${ARRAY[*]}"

for ITEM in "${ARRAY[@]}"
do
        echo $ITEM
done

for ITEM in "${ARRAY[*]}"
do
        echo $ITEM
done

my_array=(foo bar)
echo "${my_array[@]}"
my_array[2]=newone
echo "${my_array[@]}"
```

输出：

```
a b c
a b c
a
b
c
a b c
foo bar
foo bar newone
```

* https://linuxconfig.org/how-to-use-arrays-in-bash-script

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

* https://stackoverflow.com/questions/669452/are-double-square-brackets-preferable-over-single-square-brackets-in-b

# Parameter Substitution

If parameter not set, use default.

> ${parameter-default}, ${parameter:-default}

``` bash
GDB=${GDB:-/usr/bin/gdb}
echo $GDB # /usr/bin/gdb
```

* https://tldp.org/LDP/abs/html/parameter-substitution.html
* https://stackoverflow.com/questions/4437573/bash-assign-default-value

# Here document

The `cat <<EOF` syntax is very useful when working with multi-line text in Bash, eg. when assigning multi-line string to a shell variable, file or a pipe.

* Assign multi-line string to a shell variable

``` bash
sql=$(cat <<EOF
SELECT foo, bar FROM db
WHERE foo='baz'
EOF
)
echo $sql
```

*  Pass multi-line string to a file in Bash

``` bash
cat <<EOF > print.sh
> #!/bin/bash
> echo \$PWD
> echo $PWD
> EOF
```

The `print.sh` file now contains:

``` bash
#!/bin/bash
echo $PWD
echo /home/user
```

* Pass multi-line string to a pipe in Bash

``` bash
cat <<EOF | grep 'b' | tee b.txt
foo
bar
baz
EOF
```

The b.txt file contains bar and baz lines. The same output is printed to stdout.

* https://en.wikipedia.org/wiki/Here_document#Unix_shells
* https://stackoverflow.com/questions/2500436/how-does-cat-eof-work-in-bash
* https://linuxhint.com/bash-heredoc-tutorial/


# Command


## command

Use `command` to **bypass** "normal function lookup".

```
command [-pVv] command [arg ...]

Run command with args suppressing the normal shell function lookup. Only builtin commands or commands found in the PATH are executed.

If the `-p` option is given, the search for command is performed using a default value for PATH that is guaranteed to find all of the standard utilities.

If either the `-V` or `-v` option is supplied, **a description of command is printed**.

The `-v` option causes a single word indicating the command or file name used to invoke command to be displayed;
The `-V` option produces a more  verbose description.
If the `-V` or `-v` option is supplied, the exit status is 0 if command was found, and 1 if not.

If neither option is supplied and an error occurred or command cannot be found, the exit status is 127. Otherwise, the exit status of the command builtin is the exit status of command.
```

``` bash
function CheckCmdExists()
{
  command -v "$1" >/dev/null 2>&1
}
```

* https://askubuntu.com/questions/512770/what-is-the-bash-command-command

## eval

``` bash
#!/bin/bash

f()
{
    echo "abc"
    exit 1
}

OUTPUT=`eval f`
if [[ $? -ne 0 ]]; then
    printf "error: %s\n" "${OUTPUT}"
else
    printf "ok: %s\n" "${OUTPUT}"
fi
```

```
$./eval.sh
error: abc
```


`eval` is part of POSIX. It's an interface which can be a shell built-in.

It's described in the "POSIX Programmer's Manual": http://www.unix.com/man-page/posix/1posix/eval/

> eval - construct command by concatenating arguments

It will take an argument and construct a command of it, which will then be executed by the shell. This is the example from the manpage:

``` bash
foo=10 x=foo
y='$'$x
echo $y
$foo
eval y='$'$x
echo $y
10

```

1. In the first line you define `$foo` with the value `'10'` and `$x` with the value `'foo'`.
2. Now define `$y`, which consists of the string `'$foo'`. The dollar sign must be escaped with `'$'`.
3. To check the result, `echo $y`.
4. The result will be the string `'$foo'`
5. Now we repeat the assignment with `eval`. It will first evaluate `$x` to the string `'foo'`. Now we have the statement `y=$foo` which will get evaluated to `y=10`.
6. The result of echo `$y` is now the value `'10'`.


https://unix.stackexchange.com/questions/23111/what-is-the-eval-command-in-bash


## set

Bash 执行脚本的时候，例如，` bash script.sh` 会创建一个新的 Shell，script.sh 是在一个新的 Shell 里面执行。这个 Shell 就是脚本的执行环境，Bash 默认给定了这个环境的各种参数。set 命令用来修改 Shell 环境的运行参数，也就是可以定制环境。一共有十几个参数可以定制，[官方手册](https://www.gnu.org/software/bash/manual/html_node/The-Set-Builtin.html)有完整清单。如果命令行下不带任何参数，直接运行set，会显示所有的环境变量和 Shell 函数。

``` bash
# 遇到不存在的变量不会忽略，而是报错并不再执行后续语句
set -u

# 用来在运行结果之前，先输出执行的那一行命令，这对于调试复杂的脚本是很有用的
set -x

# 错误处理，脚本只要发生错误，就终止执行
# set -e根据返回值来判断，一个命令是否运行失败
# set +e表示关闭-e选项，set -e表示重新打开-e选项
set -e

# set -e有一个例外情况，就是不适用于管道命令
# 所谓管道命令，就是多个子命令通过管道运算符（|）组合成为一个大的命令。Bash 会把最后一个子命令的返回值，作为整个命令的返回值。也就是说，只要最后一个子命令不失败，管道命令总是会执行成功，因此它后面命令依然会执行，set -e就失效了
# set -o pipefail用来解决这种情况，只要一个子命令失败，整个管道命令就失败，脚本就会终止执行
set -o pipefail
```

通常的错误处理方法：对比 `set -e`

``` bash
# 只要command有非零返回值，脚本就会停止执行
command || exit 1

# 如果两个命令有继承关系，只有第一个命令成功了，才能继续执行第二个命令
command1 && command2
```

``` bash
# 写法一
command || { echo "command failed"; exit 1; }

# 写法二
if ! command; then
    echo "command failed";
    exit 1;
fi

# 写法三
command
if [ "$?" -ne 0 ]; then
    echo "command failed";
    exit 1;
fi
```

总结：set命令的上面这四个参数，一般都放在一起使用。

``` bash
# 这两种写法建议放在所有 Bash 脚本的头部
# 另一种办法是在执行 Bash 脚本的时候，从命令行传入这些参数: bash -euxo pipefail script.sh

# 写法一
set -euxo pipefail

# 写法二
set -eux
set -o pipefail
```

* http://www.ruanyifeng.com/blog/2017/11/bash-set.html
* https://www.gnu.org/software/bash/manual/html_node/The-Set-Builtin.html

## cp

``` bash
# copy file preserving directory path
mkdir test
cp --parents `find . -name "*.gcno"` test
```

[How to copy file preserving directory path in Linux?](https://serverfault.com/questions/180853/how-to-copy-file-preserving-directory-path-in-linux)

## find

### 根据文件名查找文件

1. 使用 `find` 命令查找当前目录及其子目录中的所有文件，文件名以 "test" 开头，后跟一个数字。
2. 使用 `! -name "*.cpp"` 参数，排除以 ".cpp" 结尾的文件。
3. 对于找到的每个符合条件的文件，使用 `-exec rm {} \;` 参数执行 `rm` 命令以删除文件。

``` bash
find . -type f -name "test[0-9]*" ! -name "*.cpp" -exec rm {} \;
```

在删除文件之前，可以先检查哪些文件将被删除，只需将 `rm` 命令替换为 `echo` 命令：

``` bash
find . -type f -name "test[0-9]*" ! -name "*.cpp" -exec echo {} \;
```

### 根据大小查找文件

``` bash
find /media/d/ -type f -size +50M ! \( -name "*deb" -o -name "*vmdk" \)
```

> ! expression : Negation of a primary; the unary NOT operator.
>
> ( expression ): True if expression is true.
>
> expression -o expression: Alternation of primaries; the OR operator. The second expression shall not be evaluated if the first expression is true.
>
> Note that parenthesis, both opening and closing, are prefixed by a backslash (\) to prevent evaluation by the shell.

删除某个目录下的 coredump 文件：

``` bash
#!/bin/bash

CORE_FILES=`find /data/home/gerryyang -type f -size +50M -name "*core*"`
if [[ -n $CORE_FILES ]]; then
  rm $CORE_FILES
else
  echo "no find core files"
fi
```

* https://unix.stackexchange.com/questions/50612/how-to-combine-2-name-conditions-in-find
* https://pubs.opengroup.org/onlinepubs/009695399/utilities/find.html


## awk

The awk utility shall execute programs written in the awk programming language, which is specialized for textual data manipulation. An awk program is a sequence of patterns and corresponding actions. When input is read that matches a pattern, the action associated with that pattern is carried out.

Input shall be interpreted as a sequence of records. By default, a record is a line, less its terminating <newline>, but this can be changed by using the RS built-in variable. Each record of input shall be matched in turn against each pattern in the program. For each pattern matched, the associated action shall be executed.

The awk utility shall interpret each input record as a sequence of fields where, by default, a field is a string of non- <blank> non- <newline> characters. This default <blank> and <newline> field delimiter can be changed by using the FS built-in variable or the -F sepstring option. The awk utility shall denote the first field in a record $1, the second $2, and so on. The symbol $0 shall refer to the entire record; setting any other field causes the re-evaluation of $0. Assigning to $0 shall reset the values of all other fields and the NF built-in variable.


| 特殊变量 | 含义
| -- | --
| `NF` | Number Field
| `$NF` | Last Field (最后一个字段)


* https://www.tutorialspoint.com/awk/index.htm
* [UNDERSTANDING AWK – PRACTICAL GUIDE](https://devarea.com/understanding-awk-practical-guide/#.ZAmnr-xBw0Q)

### Summing values of a column using awk command

``` bash
awk '{s+=$1;}END{print s}'
```

https://stackoverflow.com/questions/28445020/summing-values-of-a-column-using-awk-command


### 历史文章

https://blog.csdn.net/delphiwcdj/category_859397.html?spm=1001.2014.3001.5482


## cut

``` bash
cut -d "delimiter" -f (field number) file.txt
```

https://www.geeksforgeeks.org/cut-command-linux-examples/


## sed

The sed utility is a stream editor that shall read one or more text files, make editing changes according to a script of editing commands, and write the results to standard output. The script shall be obtained from either the script operand string or a combination of the option-arguments from the -e script and -f script_file options.

```
$ cat tmp
foo
123
foo
456
$ cat tmp | sed -e "1,2s/foo/bar/"
bar
123
foo
456
$ cat tmp | sed -e "s/foo/bar/"
bar
123
bar
456
```

### 获取文件某行的内容

从输入文件 file 中提取第 100 行并将其输出到标准输出。在读取到第 100 行之前的所有行上，d 命令会删除它们，这样它们就不会被输出。当 sed 读取到第 100 行时，它会停止处理并退出

``` bash
sed '100q;d' file
```

* 100q: 当读取到第 100 行时，立即退出。q 代表 "quit"（退出），100 是行号。这意味着 sed 将在读取第 100 行之后停止处理。
* ;: 分号用于在同一个命令序列中分隔多个命令。
* d: 这是 "delete"（删除）命令，它会删除当前模式空间中的内容。在这种情况下，由于前面的 100q 命令，d 命令只会在读取到第 100 行之前的每一行上执行。这意味着所有在第 100 行之前的行都会被删除，不会输出到标准输出。

### More

[UNDERSTANDING SED – PRACTICAL GUIDE](https://devarea.com/understanding-sed-practical-guide/#.ZAmnYuxBw0Q)

## declare

这个脚本定义了三个测试函数 `foo`、`bar` 和 `baz`，然后使用 `declare -F` 命令和 `awk` 命令来获取所有的函数名。在执行 `declare -F` 命令时，它会列出所有已定义的函数名和函数定义的位置。使用 `awk` 命令可以提取函数名并将其输出。最后，它遍历所有的函数名并输出它们。

``` bash
#!/bin/bash

# 定义几个测试函数
function foo() {
    echo "foo"
}

function bar() {
    echo "bar"
}

function baz() {
    echo "baz"
}

DUMMY=$(declare -F)
echo $DUMMY

# 获取所有的函数名
function_names=$(declare -F | awk '{print $NF}')

# 遍历所有的函数名并输出
for name in $function_names; do
        echo $name
done
```

```
$./declare.sh
declare -f bar declare -f baz declare -f foo
bar
baz
foo
```


```
$ bash -c "help declare"
declare: declare [-aAfFgilrtux] [-p] [name[=value] ...]
    Set variable values and attributes.

    Declare variables and give them attributes.  If no NAMEs are given,
    display the attributes and values of all variables.

    Options:
      -f        restrict action or display to function names and definitions
      -F        restrict display to function names only (plus line number and
        source file when debugging)
      -g        create global variables when used in a shell function; otherwise
        ignored
      -p        display the attributes and value of each NAME

    Options which set attributes:
      -a        to make NAMEs indexed arrays (if supported)
      -A        to make NAMEs associative arrays (if supported)
      -i        to make NAMEs have the `integer' attribute
      -l        to convert NAMEs to lower case on assignment
      -r        to make NAMEs readonly
      -t        to make NAMEs have the `trace' attribute
      -u        to convert NAMEs to upper case on assignment
      -x        to make NAMEs export

    Using `+' instead of `-' turns off the given attribute.

    Variables with the integer attribute have arithmetic evaluation (see
    the `let' command) performed when the variable is assigned a value.

    When used in a function, `declare' makes NAMEs local, as with the `local'
    command.  The `-g' option suppresses this behavior.

    Exit Status:
    Returns success unless an invalid option is supplied or an error occurs.
```

Refer: https://linuxhint.com/bash_declare_command/

## wait

```
wait [n ...]
    Wait for each specified process and return its termination status. Each n may be a process ID or a job specification; if a job spec is given, all processes in that job's pipeline
    are waited for. If n is not given, all currently active child processes are waited for, and the return status is zero. If n specifies a non-existent process or job, the return
    status is 127. Otherwise, the return status is the exit status of the last process or job waited for.
```

``` bash
#!/bin/bash

CNT=$1
if [[ "$CNT" -lt 1 ]]; then
    CNT=1
fi

for (( i=0; i<"$CNT"; i++ ))
do
    echo "hello $i"
done

wait
echo "done"
```

* https://linuxhint.com/bash-wait-command-linux/

## bc (计算)

* 如果进行16进制计算，仅支持大写十六进制数字
* 需要先设置`obase`，再设置`ibase`

``` bash
$echo "ibase=16;91A2A80-91A29B0" | bc
208
$echo "obase=16;ibase=16;91A2A80-91A29B0" | bc
D0
```

* https://www.geeksforgeeks.org/bc-command-linux-examples/

## shift

[What is the purpose of using shift in shell scripts?](https://unix.stackexchange.com/questions/174566/what-is-the-purpose-of-using-shift-in-shell-scripts)

I have came across this script:

``` bash
#! /bin/bash

if (( $# < 3 )); then
  echo "$0 old_string new_string file [file...]"
  exit 0
else
  ostr="$1"; shift
  nstr="$1"; shift
fi

echo "Replacing \"$ostr\" with \"$nstr\""
for file in $@; do
  if [ -f $file ]; then
    echo "Working with: $file"
    eval "sed 's/"$ostr"/"$nstr"/g' $file" > $file.tmp
    mv $file.tmp $file
  fi
done
```

What is the meaning of the lines where they use shift? I presume the script should be used with at least arguments so...?

Answers:

`shift` is a bash built-in which kind of removes arguments from the beginning of the argument list. Given that the 3 arguments provided to the script are available in `$1`, `$2`, `$3`, then a call to `shift` will make `$2` the new `$1`. A `shift 2` will shift by two making new `$1` the old `$3`. For more information, see here:

* http://ss64.com/bash/shift.html
* http://www.tldp.org/LDP/Bash-Beginners-Guide/html/sect_09_07.html


## split

将一个文件按指定行数拆分成多个文件。

``` bash
#!/bin/bash

input_file="CSMsg.pb.cc"
output_prefix="split_file_CSMsg.pb."
max_lines_per_file=10000

line_count=$(wc -l < "$input_file")
total_files=$(( (line_count + max_lines_per_file - 1) / max_lines_per_file ))

split -l $max_lines_per_file --numeric-suffixes=1 --suffix-length=$(echo -n $total_files | wc -c) "$input_file" "$output_prefix"

for file in "$output_prefix"*; do
  mv "$file" "$file.cc"
done
```

```
$ ls -lSh
总用量 3.5M
-rw-r--r-- 1 gerryyang users 1.4M 1月   2 16:55 CSMsg.pb.cc
-rw-r--r-- 1 gerryyang users 761K 1月   2 16:55 CSMsg.pb.h
-rw-r--r-- 1 gerryyang users 403K 1月   2 17:09 split_file_CSMsg.pb.3.cc
-rw-r--r-- 1 gerryyang users 397K 1月   2 17:09 split_file_CSMsg.pb.1.cc
-rw-r--r-- 1 gerryyang users 363K 1月   2 17:09 split_file_CSMsg.pb.2.cc
-rw-r--r-- 1 gerryyang users 219K 1月   2 17:09 split_file_CSMsg.pb.4.cc
-rwxr-xr-x 1 gerryyang users  409 1月   2 17:01 test.sh
```

* `input_file="CSMsg.pb.cc"`：指定要拆分的输入文件名。
* `output_prefix="split_file_CSMsg.pb."`：指定拆分后的输出文件的前缀。
* `max_lines_per_file=10000`：指定每个拆分文件的最大行数。
* `line_count=$(wc -l < "$input_file")`：计算输入文件的总行数。
* `total_files=$(( (line_count + max_lines_per_file - 1) / max_lines_per_file ))`：计算需要创建的拆分文件的总数。
* `split -l $max_lines_per_file --numeric-suffixes=1 --suffix-length=$(echo -n $total_files | wc -c) "$input_file" "$output_prefix"`
  + 使用 `split` 命令按行数拆分输入文件
  + `--numeric-suffixes=1` 选项表示使用数字后缀（从 1 开始）为拆分文件命名
  + `--suffix-length` 选项用于设置后缀的长度，以便在文件名中使用前导零（例如，split_file001.cc、split_file002.cc 等）
* `for file in "$output_prefix"*; do mv "$file" "$file.cc"; done`：遍历所有拆分文件，并将它们重命名为 .cc 扩展名

运行此脚本后将得到多个拆分文件，每个文件包含原始输入文件的一部分内容。这有助于将较大的源文件拆分成更易于管理和阅读的较小文件。


## sort

对文件大小进行逆序排序，可以使用 ls 命令结合 sort 命令。以下命令将按照文件大小逆序排列所有以 .o 结尾的文件。

``` bash
ls -l *.o | sort -k5,5 -n -r | head
```

`sort -k 5,5 -n -r`：使用 sort 命令按照第 5 列（即文件大小）进行逆序排序。`-k 5,5` 表示按照第 5 列排序，`-n` 表示按照数值排序，`-r` 表示逆序排序。这里的两个数字 `5,5` 分别表示键的开始位置和结束位置，也就是说，只关注第 5 列。如果写的是 `5,6`，那就表示关注第 5 列和第 6 列，这两列的内容会被拼接在一起作为排序的关键字。


## uniq

需求：统计文件中不重复的行数。

* 方法 1：先排序再用 uniq。

``` bash
sort your_file.txt | uniq | wc -l
```

* 方法 2：直接使用 sort -u（更高效）

``` bash
# sort -u：直接去重并排序，比 sort | uniq 更高效
sort -u your_file.txt | wc -l
```

* 方法 3：使用 awk（不依赖排序）

``` bash
# !seen[$0]++：仅当行第一次出现时，seen[$0] 为 0（即 !0 为真），然后 count++
# END {print count}：最后打印不重复的行数
awk '!seen[$0]++ {count++} END {print count}' your_file.txt
```

> 总结：如果你的文件很大，推荐 `awk` 方法，因为它不需要排序，速度更快。如果文件较小，`sort -u` 是最简洁的方式。

完整的脚本：

``` bash
#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename="$1"

if [ ! -f "$filename" ]; then
    echo "Error: File '$filename' not found."
    exit 1
fi

# 方法 1: sort + uniq
unique_lines=$(sort "$filename" | uniq | wc -l)
echo "Number of unique lines (sort | uniq): $unique_lines"

# 方法 2: sort -u (更高效)
unique_lines=$(sort -u "$filename" | wc -l)
echo "Number of unique lines (sort -u): $unique_lines"

# 方法 3: awk (不依赖排序)
unique_lines=$(awk '!seen[$0]++ {count++} END {print count}' "$filename")
echo "Number of unique lines (awk): $unique_lines"
```


uniq 是 Linux/Unix 系统中用于**过滤或统计连续重复行**的命令，通常需要**先排序（sort）**才能正确去重。

> 注意：**`uniq file.txt` 直接对未排序的文件去重结果是可能不准确的。因为 uniq 只处理连续重复行，未排序的文件可能漏掉非连续的重复行。**。


``` bash
# 先排序（sort），再用 uniq 去重（仅保留不重复的行）
sort file.txt | uniq

# 统计不重复的行数
sort file.txt | uniq | wc -l

# 仅显示重复的行（至少出现 2 次）
sort file.txt | uniq -d

# 仅显示不重复的行（只出现 1 次）
sort file.txt | uniq -u

# 统计每行出现的次数
# -c（--count）显示每行的重复次数
sort file.txt | uniq -c

# 忽略大小写（-i）
sort file.txt | uniq -i

# 只检查前 N 个字符（-w）
# -w 5 只比较每行的前 5 个字符，后面的内容不影响去重
sort file.txt | uniq -w 5

# 跳过前 N 个字符（-s）
# -s 3 跳过每行的前 3 个字符，再进行比较
sort file.txt | uniq -s 3

# 跳过前 N 个字段（-f）
# -f 2 跳过前 2 个字段（以空格或制表符分隔），再进行比较
sort file.txt | uniq -f 2

# 结合 -c 和 -d 显示重复行的次数
# -cd 只显示 重复行及其出现次数
sort file.txt | uniq -cd

# 去重并保存到新文件
sort file.txt | uniq > unique_lines.txt
```


> 注意：uniq 命令的 -u（--unique）选项 和 不带 -u 时的区别。

`uniq`（不带 -u）默认行为是保留所有行的唯一副本，即：如果一行是**连续重复的**，只输出一次。如果一行**没有重复**，仍然会输出。

例如：apple 和 orange 是重复的，只输出一次。banana 和 grape 是唯一的，仍然输出。

```
$ cat file.txt
apple
apple
banana
orange
orange
grape

$ sort file.txt | uniq
apple
banana
grape
orange
```

`uniq -u` 仅输出完全没有重复的行（即 唯一行，仅出现 1 次的行）。所有重复的行（即使只出现 2 次）都会被过滤掉。

例如：apple 和 orange 是重复的，不输出。banana 和 grape 是唯一的，输出。

```
$ sort file.txt | uniq -u
banana
grape
```




# Example

## 比较版本号大小

将最小版本和当前版本通过管道传给 sort，使用 version sort 模式 (`-V`) 检查当前版本是否 >= 最小版本（当且仅当 `go_version >= MIN_GO_VERSION` 时，`sort -C` 返回 true）

``` bash
# 检查 Golang 版本
local go_version=$(go version | awk '{print $3}' | sed 's/go//')
if ! printf '%s\n%s' "$MIN_GO_VERSION" "$go_version" | sort -V -C ; then
    echo "❌ Golang version must be >= ${MIN_GO_VERSION}"
    exit 1
fi
```

``` bash
$ printf "1.23.0\n1.23.5" | sort -V -C
$ echo $?
0
$ printf "1.25.0\n1.23.5" | sort -V -C
$ echo $?
1
```

## parallel 统计当前目录下包含的文件数量

``` bash
ls | parallel 'echo -n {}" "; find {} -type f | wc -l'
```


## trap (在接收到指定信号时执行特定操作)

`trap` 是一个 shell 命令，用于在接收到指定信号时执行特定操作。它的语法如下：

``` bash
trap COMMAND SIGNALS
```

其中 COMMAND 是在接收到指定信号时要执行的命令，SIGNALS 是一个或多个要捕获的信号。

在 `trap "" TRAP` 的示例中，设置了一个空命令（""）作为 SIGTRAP 信号的处理程序。当脚本接收到 SIGTRAP 信号时，它将执行空命令，即什么也不做，从而实际上忽略了该信号。

以下是使用 trap 命令的更多示例：

* 捕获 SIGINT 信号（通常由 Ctrl+C 产生）并执行自定义操作：

``` bash
#!/bin/bash

trap "echo 'Caught SIGINT signal. Exiting...'; exit 1" INT

echo "Press Ctrl+C to exit..."
while true; do
    sleep 1
done
```

在这个示例中，当脚本接收到 SIGINT 信号时，它将打印一条消息并退出。

* 在脚本退出时执行清理操作：

``` bash
#!/bin/bash

function cleanup {
    echo "Cleaning up temporary files..."
    rm -f /tmp/some_temp_file
}

trap cleanup EXIT

echo "Creating temporary file..."
touch /tmp/some_temp_file

echo "Press Ctrl+C to exit or wait for 10 seconds..."
sleep 10
```

在这个示例中，定义了一个名为 `cleanup` 的函数，用于在脚本退出时删除临时文件。使用 `trap cleanup EXIT` 在脚本退出时调用 `cleanup` 函数。




## 检查是否是 root 用户

``` bash
# Check if the user has root privileges
if [ "$(id -u)" != "0" ]; then
  echo "This script must be run as root" 1>&2
  exit 1
fi
```

## 获取当前 CPU 数量

``` bash
# Get the number of CPU cores
num_cores=$(nproc)
```

```
nproc --help
Usage: nproc [OPTION]...
Print the number of processing units available to the current process,
which may be less than the number of online processors

      --all      print the number of installed processors
      --ignore=N  if possible, exclude N processing units
      --help     display this help and exit
      --version  output version information and exit

GNU coreutils online help: <https://www.gnu.org/software/coreutils/>
Full documentation at: <https://www.gnu.org/software/coreutils/nproc>
or available locally via: info '(coreutils) nproc invocation'
```

## 根据大小查找文件

``` bash
#!/bin/bash

# Check input arguments
if [ $# -ne 2 ]; then
    echo "Usage: $0 <directory> <file_size_in_kb>"
    exit 1
fi

# Check if the specified directory exists
if [ ! -d "$1" ]; then
    echo "Error: Directory $1 not found"
    exit 1
fi

# Check if the specified file size is a valid number
if ! [[ "$2" =~ ^[0-9]+$ ]]; then
    echo "Error: File size must be a positive integer"
    exit 1
fi

# Find files larger than the specified size
find "$1" -type f -size +"$2"k
```

## pstack

``` bash
#!/bin/sh

if test $# -ne 1; then
    echo "Usage: `basename $0 .sh` <process-id>" 1>&2
    exit 1
fi

if test ! -r /proc/$1; then
    echo "Process $1 not found." 1>&2
    exit 1
fi

# GDB doesn't allow "thread apply all bt" when the process isn't
# threaded; need to peek at the process to determine if that or the
# simpler "bt" should be used.

backtrace="bt"
if test -d /proc/$1/task ; then
    # Newer kernel; has a task/ directory.
    if test `/bin/ls /proc/$1/task | /usr/bin/wc -l` -gt 1 2>/dev/null ; then
        backtrace="thread apply all bt"
    fi
elif test -f /proc/$1/maps ; then
    # Older kernel; go by it loading libpthread.
    if /bin/grep -e libpthread /proc/$1/maps > /dev/null 2>&1 ; then
        backtrace="thread apply all bt"
    fi
fi

GDB=${GDB:-/usr/bin/gdb}

# Run GDB, strip out unwanted noise.
# --readnever is no longer used since .gdb_index is now in use.
$GDB --quiet -nx $GDBARGS /proc/$1/exe $1 <<EOF 2>&1 |
set width 0
set height 0
set pagination no
$backtrace
EOF
/bin/sed -n \
    -e 's/^\((gdb) \)*//' \
    -e '/^#/p' \
    -e '/^Thread/p'
```

## shopt (功能选项的开启和关闭)

* 在非交互式模式下 alias 扩展功能默认是关闭的
* shopt 是 shell 的内置命令，可以控制 shell 功能选项的开启和关闭，从而控制 shell 的行为

``` bash
shopt -s opt_name        # Enable (set) opt_name.
shopt -u opt_name        # Disable (unset) opt_name.
shopt opt_name           # Show current status of opt_name.
```

测试：

``` bash
#!/bin/bash --login

alias echo_hello="echo Hello"  # 命令别名

shopt expand_aliases      # alias 默认关闭
echo_hello                # 执行失败

shopt -s expand_aliases   # 用 shopt 开启 expand_aliases
shopt expand_aliases
echo_hello                # 执行成功
```

执行结果：

```
$./shopt.sh
expand_aliases  off
./shopt.sh:行5: echo_hello: 未找到命令
expand_aliases  on
Hello
```

## date

`time_t` 若使用 int 存储，最高位为符号位，因此实际存储大小为 31bit，则可以表示到`2038年 01月 19日 星期二 11:14:07 CST`这个时间。

``` bash
$date -d@`echo $((16#7FFFFFFF))`
2038年 01月 19日 星期二 11:14:07 CST
```

然而，当前时间为：1661223697，其十六进制的最高为是 `01`

``` bash
$date +%s
1661223697
```

因此，在当前时间到`2038年 01月 19日 星期二 11:14:07 CST`这个时间段内，最高两位都是`01`，因此可以将 `time_t` 压缩为 30bit 存储。

常用命令：

``` bash
# 1. 获取上一个月时间，例如，201211

date -d last-month +%Y%m

# 2. 显示 20121217

date +"%Y%m%d"

# 3. Unix时间与系统时间之间的转换

# 2012-03-16 14:32:22 +0800
date -d '1970-01-01 UTC 13379554 seconds' +"%Y-%m-%d %T %z"

date -d @1356969600

# 4. 得到从1970年1月1日00：00：00到目前经历的秒数

date +%s

# 5. 显示指定时间的时间戳

date -d "2010-07-20 10:25:30" +%s

# 6. 输出当前完整的日期
# https://stackoverflow.com/questions/49187200/convert-linux-date-to-yyyy-mm-ddthhmmssz-format

date -u +'%Y-%m-%dT%H:%M:%SZ'   # 2025-03-13T02:18:22Z (for UTC)
date +'%Y-%m-%dT%H:%M:%SZ'      # 2025-03-13T10:18:22Z
date -Is                        # 2025-03-13T10:21:28+08:00
```

## ps

``` bash
# -e 此参数的效果和 -A 参数相同，-A 显示所有进程
# -o 用户自定义格式
# lstart 启动时间
# etime 运行时长

$ ps -eo pid,lstart,etime | grep `pidof friendsvr`
17697 Tue Dec  7 12:17:52 2021    08:07:21
```

## 计算某个目录下所有文件（包括子目录中的文件）的哈希值

``` bash
find your_dir -type f -print0 | sort -z | xargs -0 sha1sum | sha1sum
```

# Q&A

## 如何获取脚本第十个参数， `$10` 这样正确吗

在 Bash 脚本中，要获取第十个参数，不能直接使用 `$10`，因为它会被解释为第一个参数 `$1` 后跟一个文字 0。为了正确获取第十个参数或更多的参数，需要使用大括号 `{}` 来明确参数的边界。正确的方式是使用 `${10}` 来获取第十个参数。这样 Bash 就能正确地解析并获取传递给脚本的第十个参数值。

## i=`expr $i + 1` 执行效率问题

由于创建子进程造成了 i=`expr $i + 1` 执行效率低。

``` bash
# 执行 100 万次大约需要 884 秒
i=`expr $i + 1`

# 执行 100 万次大约需要 18 秒
i=$(($i+1))

# 执行 100 万次大约需要 18 秒
let i+=1
```



## [Shebang](https://bash.cyberciti.biz/guide/Shebang)

You will always see `#!/bin/bash` or `#!/usr/bin/env bash` as the first line when writing or reading bash scripts. **Shebang** starts with `#!` characters and the path to the bash or other interpreter of your choice. Let us see what is **Shebang** in Linux and Unix bash shell scripts.

The `#!` syntax is used in scripts to indicate an interpreter for execution under UNIX / Linux operating systems. The directive must be the first line in the Linux shell script and must start with shebang `#!`. You can add argument after the **shebang** characters, which is optional. Make sure the interpreter is the full path to a binary file. For example: `/bin/bash`.

The syntax is:

``` bash
#!/path/to/interpreter [arguments]
#!/path/to/interpreter -arg1 -arg2
```

Most Linux shell and perl / python script starts with the following line. Bash or sh example:

``` bash
#!/bin/bash
```

Starting a Script With `#!`

* It is called a **shebang** or a "bang" line.
* It is nothing but the absolute path to the Bash interpreter.
* It consists of a number sign and an exclamation point character (#!), followed by the full path to the interpreter such as /bin/bash.
* All scripts under Linux execute using the interpreter specified on a first line.
* Almost all bash scripts often begin with `#!/bin/bash` (assuming that Bash has been installed in `/bin`)
* This ensures that Bash will be used to interpret the script, even if it is executed under another shell.
* The **shebang** was introduced by Dennis Ritchie between Version 7 Unix and 8 at Bell Laboratories. It was then also added to the BSD line at Berkeley.

## converting hex to decimal

[converting hex to decimal with bash](https://stackoverflow.com/questions/53440622/converting-hex-to-decimal-with-bash)

I've seen some strange things. I tried to convert hex to dec with bash Shell. I used very very simple command.

``` bash
$ g_receiverBeforeToken=1158e460913d00000
$ echo $((16#$g_receiverBeforeToken))
1553255926290448384
```

As you guys know, this result should be '20000000000000000000'. When I put in any other hex number, it was correct. But only 1553255926290448384 was weird.

Answers:

It's not just that number, it's any number over `7fffffffffffffff`, because it's using **64-bit** integers and that's the largest one. **16-digit** numbers over that wrap around and become negative.

``` bash
$ echo $((16#7fffffffffffffff))
9223372036854775807
$ echo $((16#7fffffffffffffff + 1))
-9223372036854775808
$ echo $((16#8000000000000000))
-9223372036854775808

$ echo $((16#ffffffffffffffff))
-1
$ echo $((16#ffffffffffffffff + 1))
0
$ echo $((16#10000000000000000))
0
```

## nohup

The meaning of `nohup` is ‘**no hangup**‘. Normally, when we log out from the system then all the running programs or processes are hangup or terminated. **If you want to run any program after log out or exit from Linux operating system then you have to use `nohup` command**. There are many programs that require many hours to complete. We don’t need to log in for long times to complete the task of the command. We can keep these type of programs running in the background by using `nohup` command and check the output later. Some example of using `nohup` command are memory check, server restart, synchronization etc.

* Using nohup command without ‘&’

When you run nohup command without ‘&’ then it returns to shell command prompt immediately after running that particular command in the background.

The output of the nohup command will write in nohup.out the file if any redirecting filename is not mentioned in nohup command.

``` bash
nohup bash sleep1.sh
cat nohup.out
```

You can execute the command in the following way to redirect the output to the output.txt file. Check the output of output.txt.

``` bash
nohup bash sleep2.sh > output.txt
cat output.txt
```

* Using nohup command with ‘&’

When nohup command use with ‘&’ then it doesn’t return to shell command prompt after running the command in the background. But if you want you can return to shell command prompt by typing ‘fg’

``` bash
nohup bash sleep1.sh &
fg
```

* Using nohup command to run multiple commands in the background

You can run multiple commands in the background by using nohup command. In the following command, mkdir and ls command are executed in the background by using nohup and bash commands. You can get the output of the commands by checking output.txt file.

``` bash
nohup bash -c 'mkdir myDir && ls'> output.txt
cat output.txt
```

* Start any process in the background by using nohup

When any process starts and the user closes the terminal before completing the task of the running process then the process stops normally. If the run the process with `nohup` then it will able to run the process in the background without any issue. For example, if you run the ping command normally then it will terminate the process when you close the terminal.

Run ping command with `nohup` command. Re-open the terminal and run pgrep command again. You will get the list of the process with process id which is running.


https://linuxhint.com/nohup_command_linux/


## pgrep

`pgrep` looks through the currently running processes and lists the process IDs which match the selection criteria to stdout. All the criteria have to match. For example,

```
pgrep -u root sshd
```

will only list the processes called `sshd` AND owned by `root`.

```
$pgrep unittestsvr
4110947
4110948

$pgrep -a unittestsvr
4110947 /data/home/gerryyang/JLib_Run/bin/unittestsvr/unittestsvr --id=60.59.59.1 --bus-key=3233 --svr-id-mask=7.8.8.9
4110948 /data/home/gerryyang/JLib_Run/bin/unittestsvr/unittestsvr --id=60.59.59.2 --bus-key=3233 --svr-id-mask=7.8.8.9
```

# [Bash Builtin Commands](https://www.gnu.org/software/bash/manual/html_node/Bash-Builtins.html#index-mapfile)

## [mapfile](https://www.gnu.org/software/bash/manual/html_node/Bash-Builtins.html#index-mapfile)

通过 malloc_info 统计调用 malloc_trim 前后内存变化

调用 malloc_trim 前 malloc_info 的信息如下：

egrep "fast|rest" info1.txt

```
<total type="fast" count="0" size="0"/>
<total type="rest" count="122591" size="1770769518"/>
<total type="fast" count="7" size="384"/>
<total type="rest" count="4" size="130851"/>
<total type="fast" count="24" size="1152"/>
<total type="rest" count="59" size="361322"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="1" size="3776"/>
<total type="fast" count="8" size="512"/>
<total type="rest" count="19" size="1007026"/>
<total type="fast" count="20" size="1072"/>
<total type="rest" count="5" size="916"/>
<total type="fast" count="624" size="35072"/>
<total type="rest" count="14409" size="146221496"/>
<total type="fast" count="18" size="928"/>
<total type="rest" count="10" size="841"/>
<total type="fast" count="15" size="720"/>
<total type="rest" count="9" size="456"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="2" size="127297"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="1" size="3184"/>
<total type="fast" count="11" size="656"/>
<total type="rest" count="25" size="4725320"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="18" size="307265"/>
<total type="fast" count="1726" size="55648"/>
<total type="rest" count="1540" size="2181699"/>
<total type="fast" count="14" size="1296"/>
<total type="rest" count="78" size="16150669"/>
<total type="fast" count="2467" size="97440"/>
<total type="rest" count="138771" size="1941991636"/>
```

调用 malloc_trim 后 malloc_info 的信息如下：

egrep "fast|rest" info2.txt

```
<total type="fast" count="0" size="0"/>
<total type="rest" count="122591" size="1770769518"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="4" size="131235"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="49" size="362464"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="1" size="3776"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="24" size="1007543"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="25" size="2008"/>
<total type="fast" count="28" size="1216"/>
<total type="rest" count="14045" size="146172332"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="28" size="1787"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="24" size="1191"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="2" size="127297"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="1" size="3184"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="32" size="4725983"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="19" size="307266"/>
<total type="fast" count="1" size="32"/>
<total type="rest" count="6" size="2240901"/>
<total type="fast" count="0" size="0"/>
<total type="rest" count="75" size="16151962"/>
<total type="fast" count="29" size="1248"/>
<total type="rest" count="136926" size="1942008447"/>
```

通过如下脚本计算前后两个 size 之合的变化：

``` bash
#!/bin/bash

# 检查是否提供了文件名参数
if [ -z "$1" ]; then
    echo "Usage: $0 input_file"
    exit 1
fi

input_file="$1"

# 从input_file中提取size属性的值，并将它们保存到sizes数组中
mapfile -t sizes < <(grep -oP 'size="\K\d+' "$input_file")

# 计算size之和
total_size=0
for size in "${sizes[@]}"; do
    total_size=$((total_size + size))
done

# 输出结果
echo "Total size: $total_size"
```






# Manual

* https://www.gnu.org/software/bash/manual/html_node/index.html#SEC_Contents


# Reference

* [The Open Group Base Specifications Issue 7, 2018 edition](https://pubs.opengroup.org/onlinepubs/9699919799/)
* [Bash Programming Tutorial](https://ianding.io/2019/08/30/bash-programming-tutorial/)
* [Don't Overuse hjkl in Vim](https://ianding.io/2019/08/16/dont-overuse-hjkl-in-vim/)
* [Multi-file Search and Replace in Vim](https://ianding.io/2019/08/22/multi-file-search-and-replace-in-vim/)




