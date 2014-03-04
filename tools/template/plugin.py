
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# $$${plugin_module}.py - $$${plugin_description}
#
#     Description : $$${plugin_description}
#     Version     : $$${plugin_version}
#     Author      : $$${plugin_author}
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Implementation notes:
# ---------------------
# This is the GeanyPy boilerplate template file that you should customize
# for your plugin. There are a few optional and required functions you must
# implement at the module level that are called by the C bootstrapper plugin
# loaded into Geany. Normally you will not put much "real" code here but will
# use this as the place to hook the Geany plugin into your actual code that
# does stuff, in other modules or packages. The functions in this file have
# a 1:1 correspondence to the functions used in the C API, so it will be
# useful to also refer to that documentation:
#   http://www.geany.org/manual/reference/pluginsymbols_8c.html
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# The version check is not required, if specified it should return the
# minimum Geany API version or -1 to use the minimum version required by
# the GeanyPy library.
# See `PLUGIN_VERSION_CHECK` macro in Geany plugin C API
#~def version_check():
#~    return -1

# The set info is required. It should return a dictionary containing the
# following keys: 'name', 'description', 'version', 'author'.
# See `PLUGIN_SET_INFO` macro in Geany plugin C API
def set_info():
    return {
        "name":        "$$${plugin_name}",
        "description": "$$${plugin_description}",
        "version":     "$$${plugin_version}",
        "author":      "$$${plugin_author}",
    }

# The init function is required, and is called when the plugin is being
# loaded. Until this function is called, none of the other
# functions/code/modules should do much, and especially not with the Geany API.
# See `plugin_init` function in Geany plugin C API
def init():
    pass

# The cleanup function is optional. It is called when the plugin is being
# unloaded. After this function is called, nothing should be done as the
# plugin will be unloaded, even if not fully disposed. This function is the
# ideal place to remove any GUI widgets installed or cleanup global state
# for the plugin.
# See `plugin_cleanup` function in Geany plugin C API
#~def cleanup():
#~    pass

# The configure function is optional. It is called to retrieve the widget
# that provides the plugin's configuration GUI. The 'dialog' argument is
# a 'Gtk.Dialog' widget that will host the configuration GUI (in a notebook).
# It can be used to connect to the response signals of the dialog. The return
# value is any 'Gtk.Widget' that provides the plugin's configuration GUI.
# See `plugin_configure` function in Geany plugin C API
#~def configure():
#~    pass

# The help function is optional. It is called when the user presses the Help
# button in Geany's Plugin Manager dialog. If this function is implemented
# it should cause some form of documentation to appear for the user. A typical
# use would be to open the online help documentation in a web browser, or
# even put the README or similar text file into a new Geany document.
# See `plugin_help` function in Geany plugin C API
#~def help():
#~    pass
