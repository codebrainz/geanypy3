AC_PREREQ([2.62])

# Initialize build system tools
AC_INIT([geanypy], [0.01], [mbrush@codebrainz.ca])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([lib/geanypy.c])
AC_CONFIG_HEADERS([lib/config.h])
AM_INIT_AUTOMAKE([1.11.1 -Wall foreign])

# Configure build system and tools
AM_SILENT_RULES([yes])

# fix for automake >= 1.12
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Check for utilities
AC_PROG_CC
LT_INIT([disable-static])
PKG_PROG_PKG_CONFIG([0.25])

AM_PATH_PYTHON([3.3])

# Check for Cython compiler
#AC_PATH_PROG([CYTHON], [cython], [no])
#AS_IF([test "x$CYTHON" = "xno"],
#      [AC_MSG_ERROR([Failed to locate the Cython compiler])])

# Check for headers, libraries and packages
AC_CHECK_HEADERS([limits.h])
PKG_CHECK_MODULES([GEANY], [geany >= 1.23])
AX_PYTHON_DEVEL([>= '3.3'])
AX_PYTHON_LIBRARY(,[AC_MSG_ERROR([Cannot determine location of the Python DSO])])
AC_SUBST([PYTHON])

# Generate build files
AC_CONFIG_FILES([
    Makefile
    lib/Makefile
    lib/geanypy.pc
    tools/Makefile
])
AC_OUTPUT
