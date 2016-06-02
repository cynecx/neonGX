#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"
protoc -I=./def --cpp_out=./NeoNPong ./def/pong.proto
