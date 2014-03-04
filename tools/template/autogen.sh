#!/bin/sh
mkdir -p build-aux m4 || exit $?
autoreconf -vfi
