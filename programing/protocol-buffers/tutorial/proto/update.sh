#!/bin/bash

PROTOCOL_DIR=./
PROTOCOL_SRC_DIR=../src

PROTOCOL_FILES="addressbook.proto"

function Proc {
	for file in $1
	do
		protoc --cpp_out=$PROTOCOL_SRC_DIR --proto_path=$PROTOCOL_DIR $file
		if [ $? -ne 0 ]; then
			echo "protoc $file failed"
			exit 1
		fi

		protoc -I$PROTOCOL_DIR -o $file.pb $file
		if [ $? -ne 0 ]; then
			echo "protoc $file failed"
			exit 1
		fi
	done
}

Proc "$PROTOCOL_FILES"

echo "ok"
