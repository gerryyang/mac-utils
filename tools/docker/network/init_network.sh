#!/bin/bash
set -e

usage() {
	echo "usage: $0 container_name"
	exit 1
}

if [ "$1" = "" ]; then
	usage
fi
container_name=$1

echo "init container network..."

# init once
:<<gerry
brctl addbr br0
ip link set br0 up

ip addr add 172.27.208.238/25 dev br0; \
ip addr del 172.27.208.238/25 dev eth1; \
brctl addif br0 eth1; \
ip route del default; \
ip route add default via 172.27.208.129 dev br0
gerry

pid=$(docker inspect --format '{{.State.Pid}}' $container_name)
rm -rf /var/run/netns
mkdir -p /var/run/netns
ln -s /proc/$pid/ns/net /var/run/netns/$pid

ip link add veth-a type veth peer name veth-b
brctl addif br0 veth-a
ip link set veth-a up

ip link set veth-b netns $pid
ip netns exec $pid ip link set dev veth-b name eth0
ip netns exec $pid ip link set eth0 up
ip netns exec $pid ip addr add 172.27.208.135/25 dev eth0
ip netns exec $pid ip route add default via 172.27.208.129

echo "check..."
ping -c 1 -w 1 172.27.208.135
if [ "$?" == 0 ]; then
	echo "network is ok"
else
	echo "network is err"
fi

echo "done"
