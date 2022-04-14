---
layout: post
title:  "Bash in Action"
date:   2019-09-02 17:00:00 +0800
categories: [Bash, 编程语言]
---

* Do not remove this line (it will not be displayed)
{:toc}

# Bash Tools

* [Execute Bash Shell Online (GNU Bash v4.4)](https://www.tutorialspoint.com/execute_bash_online.php)


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
| stderr and stdout 2 file | rm -f $(find / -name core) &> /dev/null 


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


# Compare

* `test` provides no output, but returns an exit status of 0 for "true" (test successful) and 1 for "false" (test failed).
* The `test` command may also be expressed with **single brackets** `[ ... ]`, as long as they are separated from all other arguments with **whitespace**. 

``` bash
num=4; if (test $num -gt 5); then echo "yes"; else echo "no"; fi

file="/etc/passwd"; if [ -e $file ]; then echo "whew"; else echo "uh-oh"; fi

if [[ $# -eq 0 ]]; then
  # digital compare
  # -eq / -lt / -le / -gt / -ge / -ne
fi

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

if [[ -f "proc.pid" ]]; then
  pid=`cat proc.pid`
fi

./run.sh 
if [[ $? -ne 0 ]];then
  echo "run.sh failed"
  exit 1
fi
```

* [Is double square brackets [[ ]] preferable over single square brackets [ ] in Bash?](https://stackoverflow.com/questions/669452/is-double-square-brackets-preferable-over-single-square-brackets-in-ba)
* [Bash test builtin command](https://www.computerhope.com/unix/bash/test.htm)

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

https://tldp.org/HOWTO/Bash-Prog-Intro-HOWTO-8.html


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


```
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

## cp

``` bash
# copy file preserving directory path
mkdir test
cp --parents `find . -name "*.gcno"` test
```

[How to copy file preserving directory path in Linux?](https://serverfault.com/questions/180853/how-to-copy-file-preserving-directory-path-in-linux)


## find

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

* https://unix.stackexchange.com/questions/50612/how-to-combine-2-name-conditions-in-find
* https://pubs.opengroup.org/onlinepubs/009695399/utilities/find.html

## awk

The awk utility shall execute programs written in the awk programming language, which is specialized for textual data manipulation. An awk program is a sequence of patterns and corresponding actions. When input is read that matches a pattern, the action associated with that pattern is carried out.

Input shall be interpreted as a sequence of records. By default, a record is a line, less its terminating <newline>, but this can be changed by using the RS built-in variable. Each record of input shall be matched in turn against each pattern in the program. For each pattern matched, the associated action shall be executed.

The awk utility shall interpret each input record as a sequence of fields where, by default, a field is a string of non- <blank> non- <newline> characters. This default <blank> and <newline> field delimiter can be changed by using the FS built-in variable or the -F sepstring option. The awk utility shall denote the first field in a record $1, the second $2, and so on. The symbol $0 shall refer to the entire record; setting any other field causes the re-evaluation of $0. Assigning to $0 shall reset the values of all other fields and the NF built-in variable.


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

## declare

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

# Example

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




# Reference

* [The Open Group Base Specifications Issue 7, 2018 edition](https://pubs.opengroup.org/onlinepubs/9699919799/)
* [Bash Programming Tutorial](https://ianding.io/2019/08/30/bash-programming-tutorial/)
* [Don't Overuse hjkl in Vim](https://ianding.io/2019/08/16/dont-overuse-hjkl-in-vim/)
* [Multi-file Search and Replace in Vim](https://ianding.io/2019/08/22/multi-file-search-and-replace-in-vim/)




