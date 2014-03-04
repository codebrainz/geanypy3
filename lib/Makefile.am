# the helper library
lib_LTLIBRARIES = libgeanypy.la
libgeanypy_la_CPPFLAGS = -I. -DGEANYPY_COMPILING=1 $(PYTHON_CPPFLAGS)
libgeanypy_la_CFLAGS   = $(GEANY_CFLAGS)
libgeanypy_la_LIBADD   = $(GEANY_LIBS) $(PYTHON_LDFLAGS) \
                         $(PYTHON_EXTRA_LIBS) $(PYTHON_EXTRA_LDFLAGS)
libgeanypy_la_SOURCES  = geanypy.c geanypy.h

# public headers
geanypyincdir = $(includedir)/geanypy
geanypyinc_HEADERS = geanypy.h

# pkg-config file
geanypypkgdir = $(libdir)/pkgconfig
geanypypkg_DATA = geanypy.pc

# autoconf macro
geanypyacdir = $(datadir)/aclocal
dist_geanypyac_DATA = $(srcdir)/geanypy.m4

EXTRA_DIST = geanypy.pc.in
