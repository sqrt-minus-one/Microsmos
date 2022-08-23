#!/bin/bash

set -e

code="$PWD"
app_name="nat_traversal"

WARNINGS="-Wall -Werror -Wshadow -Wno-array-bounds -Wno-stringop-overflow -Wno-unused-value -Wno-missing-braces -Wno-unused-variable -Wno-unused-function  -Wno-write-strings"

link_opts="-pthread -no-pie"
opts="-g -D$DEBUG_BUILD"

mkdir -p build
cd build > /dev/null

gcc $WARNINGS $opts $link_opts -I$code/code $OPTIMIZE $code/code/os/linux/linux_main.c -o $app_name

cd $code > /dev/null

