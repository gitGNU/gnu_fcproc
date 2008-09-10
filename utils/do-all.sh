#! /bin/sh

./bootstrap || exit 1
PATH=$PATH:/tmp/build/bin ./configure --prefix=/tmp/build || exit 1
make all install || exit 1

