#! /bin/sh

if test -z "$1" ; then
    echo "Missing SHA1"
    exit 1
fi

rm -f *.patch

git format-patch $1 && tar -cvzf patch.tgz *.patch || exit 1

rm -f *.patch
