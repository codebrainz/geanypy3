#!/bin/sh

# Deletes all known built files

rm -rf              \
  aclocal.m4        \
  autom4te.cache/   \
  build/            \
  build-aux/        \
  configure         \
  Makefile          \
  Makefile.in       \
  lib/Makefile      \
  lib/Makefile.in   \
  tools/Makefile    \
  tools/Makefile.in \
  m4/libtool.m4     \
  m4/lt*.m4
