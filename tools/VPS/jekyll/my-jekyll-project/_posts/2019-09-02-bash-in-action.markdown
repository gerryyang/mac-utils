---
layout: post
title:  "Bash in Action"
date:   2019-09-02 17:00:00 +0800
categories: bash
---

* Do not remove this line (it will not be displayed)
{:toc}


# Loop

## Loop Variable Range

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

[HowTo: Iterate Bash For Loop Variable Range Under Unix / Linux](https://www.cyberciti.biz/faq/unix-linux-iterate-over-a-variable-range-of-numbers-in-bash/)



