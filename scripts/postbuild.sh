#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR

cp ../public/index.html ../build/index.html
cp -R ../assets ../build/
