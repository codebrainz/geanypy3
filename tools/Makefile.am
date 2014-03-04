bin_SCRIPTS = geanypy-genplugin
CLEANFILES = $(bin_SCRIPTS)
EXTRA_DIST = geanypy-genplugin.in

dist_noinst_SCRIPTS = cleantree.sh

geanypy-genplugin: geanypy-genplugin.in Makefile
	$(AM_V_GEN)sed -e 's,[@]pkgdatadir[@],$(pkgdatadir),g' < $< > $@ && \
		chmod +x $@

tmpldir = $(pkgdatadir)/template
dist_tmpl_DATA = \
	template/.gitignore \
	template/autogen.sh \
	template/configure.ac \
	template/Makefile.am \
	template/plugin.c \
	template/plugin.py \
	template/README.md
