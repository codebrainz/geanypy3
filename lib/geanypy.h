#ifndef GEANYPY_H_
#define GEANYPY_H_ 1

#include <Python.h>
#include <geanyplugin.h>

G_BEGIN_DECLS

typedef struct GeanyPy GeanyPy;

G_GNUC_WARN_UNUSED_RESULT
GeanyPy *GeanyPy_New(const char *module_name, const char *path);
void GeanyPy_Destroy(GeanyPy*);

/* Proxy functions */
int GeanyPy_VersionCheck(GeanyPy*, int abi_version);
void GeanyPy_SetInfo(GeanyPy*, PluginInfo*);
void GeanyPy_Init(GeanyPy*, GeanyData*);
void GeanyPy_Cleanup(GeanyPy*);
GtkWidget *GeanyPy_Configure(GeanyPy*, GtkDialog*);
void GeanyPy_Help(GeanyPy*);

/* Error handling */
int GeanyPy_HasError(GeanyPy*);
void GeanyPy_ClearError(GeanyPy*);
void GeanyPy_SetError(GeanyPy*, const char*, ...);
const char *GeanyPy_GetError(GeanyPy*);


/* Geany sets these global variables in the plugin's DLL when it loads it */
extern GeanyData      *geany_data;
extern GeanyFunctions *geany_functions;
extern GeanyPlugin    *geany_plugin;


/*
 * Boilerplate so plugin's don't have to write it manually.
 */

#if !defined(GEANYPY_COMPILING) && !defined(GEANYPY_PLUGIN_MODULE)
# error "You must #define GEANYPY_PLUGIN_MODULE to your plugin's Python module"
#endif

#ifndef GEANYPY_MODULE_PATH
# define GEANYPY_MODULE_PATH NULL
#endif

#define GEANYPY_DECLARE_PLUGIN                                    \
  GeanyData *geany_data;                                          \
  GeanyFunctions *geany_functions;                                \
  GeanyPlugin *geany_plugin;                                      \
  static GeanyPy *plugin_state = NULL;                            \
  G_MODULE_EXPORT                                                 \
  const char *g_module_check_init(GModule *module) {              \
    (void) module;                                                \
    plugin_state = GeanyPy_New(GEANYPY_PLUGIN_MODULE,             \
                               GEANYPY_MODULE_PATH);              \
    if (plugin_state == NULL)                                     \
      return "Failed to initialize GeanyPy plugin";               \
    return NULL;                                                  \
  }                                                               \
  G_MODULE_EXPORT                                                 \
  void g_module_unload(GModule *module) {                         \
    (void) module;                                                \
    g_return_if_fail(plugin_state != NULL);                       \
    GeanyPy_Destroy(plugin_state);                                \
    plugin_state = NULL;                                          \
  }                                                               \
  G_MODULE_EXPORT                                                 \
  int plugin_version_check(int abi_vers) {                        \
    int api_vers;                                                 \
    g_return_val_if_fail(plugin_state != NULL, -1);               \
    api_vers = GeanyPy_VersionCheck(plugin_state, abi_vers);      \
    if (api_vers < 0) {                                           \
      if (GeanyPy_HasError(plugin_state))                         \
        g_critical("%s", GeanyPy_GetError(plugin_state));         \
      else                                                        \
        g_critical("Failed to retrieve API version from plugin"); \
    }                                                             \
    return api_vers;                                              \
  }                                                               \
  G_MODULE_EXPORT                                                 \
  void plugin_set_info(PluginInfo *info) {                        \
    g_return_if_fail(plugin_state != NULL);                       \
    GeanyPy_SetInfo(plugin_state, info);                          \
    if (GeanyPy_HasError(plugin_state))                           \
      g_critical("%s", GeanyPy_GetError(plugin_state));           \
  }                                                               \
  G_MODULE_EXPORT                                                 \
  void plugin_init(GeanyData *data) {                             \
    g_return_if_fail(plugin_state != NULL);                       \
    GeanyPy_Init(plugin_state, data);                             \
    if (GeanyPy_HasError(plugin_state))                           \
      g_critical("%s", GeanyPy_GetError(plugin_state));           \
  }                                                               \
  G_MODULE_EXPORT                                                 \
  void plugin_cleanup(void) {                                     \
    g_return_if_fail(plugin_state != NULL);                       \
    GeanyPy_Cleanup(plugin_state);                                \
    if (GeanyPy_HasError(plugin_state))                           \
      g_critical("%s", GeanyPy_GetError(plugin_state));           \
  }                                                               \
  G_MODULE_EXPORT                                                 \
  GtkWidget *plugin_configure(GtkDialog *dialog) {                \
    GtkWidget *widget;                                            \
    g_return_val_if_fail(plugin_state != NULL, NULL);             \
    widget = GeanyPy_Configure(plugin_state, dialog);             \
    if (widget == NULL && GeanyPy_HasError(plugin_state))         \
        g_critical("%s", GeanyPy_GetError(plugin_state));         \
    return widget;                                                \
  }                                                               \
  G_MODULE_EXPORT                                                 \
  void plugin_help(void) {                                        \
    g_return_if_fail(plugin_state != NULL);                       \
    GeanyPy_Help(plugin_state);                                   \
    if (GeanyPy_HasError(plugin_state))                           \
      g_critical("%s", GeanyPy_GetError(plugin_state));           \
  }


G_END_DECLS

#endif // GEANYPY_H_
