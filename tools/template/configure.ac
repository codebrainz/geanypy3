AC_PREREQ([2.62])
AC_INIT([$$${plugin_module}], [$$${plugin_version}], [$$${plugin_bugreport}])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([plugin.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11.1 -Wall foreign])
AM_SILENT_RULES([yes])
# fix for automake >= 1.12
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_CC
LT_INIT([disable-static])
GEANYPY_INIT
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
