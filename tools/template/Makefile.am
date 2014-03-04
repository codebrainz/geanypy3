# List your source .py files here (including in packages/subdirectories)
pyplugin_sources = $$${plugin_module}.py


#=============================================================================
# You probably don't need to edit anything below here

# The actual Python plugin
#-------------------------
moduledir = $(GEANYPY_MODULE_DIR)
module_PYTHON = $(pyplugin_sources)

# The C bootstrapping plugin
#---------------------------
plugin_LTLIBRARIES = $$${plugin_module}.la
plugindir = $(GEANYPY_PLUGIN_DIR)
$$${plugin_module}_la_LDFLAGS = -module -avoid-version
$$${plugin_module}_la_CFLAGS = $(GEANYPY_CFLAGS)
$$${plugin_module}_la_CPPFLAGS = \
  -DGEANYPY_MODULE_DIR="\"$(GEANYPY_MODULE_DIR)\"" \
  -DGEANYPY_PLUGIN_DIR="\"$(GEANYPY_PLUGIN_DIR)\"" \
  -DGEANYPY_PLUGIN_MODULE="\"$$${plugin_module}\""
$$${plugin_module}_la_LIBADD = $(GEANYPY_LIBS)
$$${plugin_module}_la_SOURCES = plugin.c

ACLOCAL_AMFLAGS = -I m4
