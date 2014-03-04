dnl GEANYPY_INIT
dnl Check for GeanyPy and Python
AC_DEFUN([GEANYPY_INIT], [

  PKG_PROG_PKG_CONFIG([0.25])

  PKG_CHECK_MODULES([GEANYPY], [geanypy], [
    ac_geanypy_moduledir=`$PKG_CONFIG --variable=moduledir geanypy`;
    ac_geanypy_plugindir=`$PKG_CONFIG --variable=plugindir geanypy`;
    ac_geanypy_python_version=`$PKG_CONFIG --variable=python_version geanypy`;
  ], [
    AC_MSG_ERROR([Unable to locate `geanypy' pkg-config package, is it installed?])
  ])

  AS_IF([test "x$ac_geanypy_moduledir" = "x"],
        [AC_MSG_ERROR([Unable to find GeanyPy module directory])])
  AS_IF([test "x$ac_geanypy_plugindir" = "x"],
        [AC_MSG_ERROR([Unable to find GeanyPy plugin directory])])
  AS_IF([test "x$ac_geanypy_python_version" = "x"],
        [AC_MSG_ERROR([Unable to find GeanyPy Python version])])

  AC_SUBST([GEANYPY_MODULE_DIR], [$ac_geanypy_moduledir])
  AC_SUBST([GEANYPY_PLUGIN_DIR], [$ac_geanypy_plugindir])
  AC_SUBST([GEANYPY_PYTHON_VERSION], [$ac_geanypy_python_version])

  AM_PATH_PYTHON([$ac_geanypy_python_version])

])
