#!/bin/sh

set -x
rm -f config.cache &&
libtoolize --force &&
aclocal &&
autoconf &&
autoheader &&
automake --foreign --add-missing
