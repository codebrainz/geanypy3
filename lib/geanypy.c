#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "geanypy.h"
#include <limits.h>

#define GEANYPY_MIN_API_VERSION 211

struct GeanyPy
{
  PyThreadState *main_interpreter;
  PyThreadState *sub_interpreter;
  PyObject *module;
  GString *error;
  struct {
    PyObject *version_check;
    PyObject *set_info;
    PyObject *init;
    PyObject *cleanup;
    PyObject *configure;
    PyObject *help;
  } hooks;
  struct {
    char *name;
    char *description;
    char *version;
    char *author;
  } info;
};

static void GeanyPy_InterpEnter(GeanyPy *gp)
{
  g_return_if_fail(gp != NULL);
  g_return_if_fail(gp->main_interpreter != NULL);
  g_return_if_fail(gp->sub_interpreter != NULL);
  if (PyThreadState_Get() != gp->sub_interpreter)
    PyThreadState_Swap(gp->sub_interpreter);
}

static void GeanyPy_InterpLeave(GeanyPy *gp)
{
  g_return_if_fail(gp != NULL);
  g_return_if_fail(gp->main_interpreter != NULL);
  g_return_if_fail(gp->sub_interpreter != NULL);
  if (PyThreadState_Get() == gp->sub_interpreter)
    PyThreadState_Swap(gp->main_interpreter);
}

static PyObject *GeanyPy_GetFunction(GeanyPy *gp,
                                     const char *func_name,
                                     gboolean required)
{
  PyObject *func;

  func = PyObject_GetAttrString(gp->module, func_name);
  if (func == NULL) {
    if (required) {
      if (PyErr_Occurred())
        PyErr_Print();
      GeanyPy_SetError(gp, "Unable to locate function '%s' in plugin's module", func_name);
      return NULL;
    }
  }

  if (!PyCallable_Check(func)) {
    Py_DECREF(func);
    if (required)
      GeanyPy_SetError(gp, "The plugin's '%s' function is not callable", func_name);
    return NULL;
  }

  return func;
}

GeanyPy *GeanyPy_New(const char *module_name, const char *path)
{
  GeanyPy *gp;

  g_return_val_if_fail(module_name != NULL, NULL);

  if (!Py_IsInitialized())
    Py_Initialize();

  gp = g_slice_new0(GeanyPy);
  if (gp == NULL)
    return NULL;

  gp->main_interpreter = PyThreadState_Get();
  if (gp->main_interpreter == NULL) {
    g_critical("Failed to get the main Python interpreter");
    GeanyPy_Destroy(gp);
    return NULL;
  }

  gp->error = g_string_new("");

  /* Create a new sub-interpreter for the plugin */
  gp->sub_interpreter = Py_NewInterpreter();
  if (gp->sub_interpreter == NULL) {
    g_critical("Failed to create a new sub-interpreter for plugin");
    GeanyPy_Destroy(gp);
    return NULL;
  }

  GeanyPy_InterpEnter(gp);

  /* Add the plugin's path to sys.path if supplied */
  if (path != NULL) {
    PyObject *sys_path = PySys_GetObject("path");
    if (sys_path == NULL) {
      if (PyErr_Occurred())
        PyErr_Print();
      g_critical("Failed to retrieve sys.path sequence");
      GeanyPy_Destroy(gp);
      return NULL;
    }
    // TODO: append path
  }

  /* Import the plugin's module */
  gp->module = PyImport_ImportModule(module_name);
  if (gp->module == NULL) {
    if (PyErr_Occurred())
      PyErr_Print();
    g_critical("Failed to import the plugin's module");
    GeanyPy_Destroy(gp);
    return NULL;
  }

  /* Required functions */
  gp->hooks.set_info = GeanyPy_GetFunction(gp, "set_info", TRUE);
  if (gp->hooks.version_check == NULL) {
    g_critical("%s", GeanyPy_GetError(gp));
    GeanyPy_Destroy(gp);
    return NULL;
  }
  gp->hooks.init = GeanyPy_GetFunction(gp, "init", TRUE);
  if (gp->hooks.init == NULL) {
    g_critical("%s", GeanyPy_GetError(gp));
    GeanyPy_Destroy(gp);
    return NULL;
  }

  /* Optional functions */
  gp->hooks.version_check = GeanyPy_GetFunction(gp, "version_check", FALSE);
  gp->hooks.cleanup = GeanyPy_GetFunction(gp, "cleanup", FALSE);
  gp->hooks.configure = GeanyPy_GetFunction(gp, "configure", FALSE);
  gp->hooks.help = GeanyPy_GetFunction(gp, "help", FALSE);

  GeanyPy_InterpLeave(gp);

  return gp;
}

void GeanyPy_Destroy(GeanyPy *gp)
{
  if (gp == NULL)
    return;

  g_free(gp->info.name);
  g_free(gp->info.description);
  g_free(gp->info.version);
  g_free(gp->info.author);

  if (gp->error != NULL)
    g_string_free(gp->error, TRUE);

  GeanyPy_InterpEnter(gp);

  Py_XDECREF(gp->hooks.version_check);
  Py_XDECREF(gp->hooks.set_info);
  Py_XDECREF(gp->hooks.init);
  Py_XDECREF(gp->hooks.cleanup);
  Py_XDECREF(gp->hooks.configure);
  Py_XDECREF(gp->hooks.help);
  Py_XDECREF(gp->module);

  if (gp->sub_interpreter != NULL)
    Py_EndInterpreter(gp->sub_interpreter);

  g_slice_free(GeanyPy, gp);
}

int GeanyPy_VersionCheck(GeanyPy *gp, int abi_version)
{
  PyObject *api_vers;
  Py_ssize_t api_version;

  if (gp == NULL) {
    g_critical("Invalid GeanyPy state was passed");
    return -1;
  }

  GeanyPy_InterpEnter(gp);
  GeanyPy_ClearError(gp);

  if (gp->hooks.version_check == NULL &&
      !PyCallable_Check(gp->hooks.version_check))
  {
    GeanyPy_SetError(gp, "The plugin specified a 'version_check' but it's not callbable");
    GeanyPy_InterpLeave(gp);
    return -1;
  }

  if (abi_version != GEANY_ABI_VERSION) {
    GeanyPy_SetError(gp, "Mismatched Geany ABI versions (expected %d, got %d)",
                     GEANY_ABI_VERSION, abi_version);
    GeanyPy_InterpLeave(gp);
    return -1;
  }

  if (gp->hooks.version_check == NULL) {
    GeanyPy_InterpLeave(gp);
    return GEANYPY_MIN_API_VERSION;
  }

  api_vers = PyObject_CallObject(gp->hooks.version_check, NULL);
  if (api_vers == NULL) {
    if (PyErr_Occurred())
      PyErr_Print();
    GeanyPy_SetError(gp, "Error calling plugin's 'version_check' function");
    GeanyPy_InterpLeave(gp);
    return -1;
  }

  if (!PyNumber_Check(api_vers)) {
    GeanyPy_SetError(gp, "Return value of 'version_check' is not a number");
    Py_DECREF(api_vers);
    GeanyPy_InterpLeave(gp);
    return -1;
  }

  api_version = PyNumber_AsSsize_t(api_vers, NULL);
  Py_DECREF(api_vers);

  if (api_version < INT_MIN || api_version > INT_MAX) {
    GeanyPy_SetError(gp, "Return value of 'version_check' is out of range "
                     "(value %lld, expected %d-%d)",
                     api_version, INT_MIN, INT_MAX);
    GeanyPy_InterpLeave(gp);
    return -1;
  }

  GeanyPy_InterpLeave(gp);

  if (api_version < 0)
    return GEANYPY_MIN_API_VERSION;

  return (int) api_version;
}

static char *GeanyPy_GetUtf8StringFromDict(GeanyPy *gp, PyObject *dict,
                                           const char *key)
{
  PyObject *value;
  const char *result;
  char *temp_key;

  g_return_val_if_fail(dict != NULL, NULL);
  g_return_val_if_fail(PyMapping_Check(dict), NULL);
  g_return_val_if_fail(key != NULL, NULL);

  temp_key = g_strdup(key);
  value = PyMapping_GetItemString(dict, temp_key);
  g_free(temp_key);

  if (value == NULL) {
    if (PyErr_Occurred())
      PyErr_Print();
    GeanyPy_SetError(gp, "Failed to retrieve value for '%s' key", key);
    return NULL;
  }

  if (!PyUnicode_Check(value)) {
    GeanyPy_SetError(gp, "Value for key '%s' was not a string", key);
    Py_DECREF(value);
    return NULL;
  }

  result = PyUnicode_AsUTF8(value);
  Py_DECREF(value);

  if (result == NULL) {
    if (PyErr_Occurred())
      PyErr_Print();
    GeanyPy_SetError(gp,
      "Failed to encode value for key '%s' as a UTF-8 string", key);
    return NULL;
  }

  return g_strdup(result);
}

void GeanyPy_SetInfo(GeanyPy *gp, PluginInfo *info)
{
  PyObject *info_dict;

  g_return_if_fail(gp != NULL);
  g_return_if_fail(info != NULL);
  g_return_if_fail(gp->hooks.set_info != NULL);

  GeanyPy_InterpEnter(gp);
  GeanyPy_ClearError(gp);

  if (!PyCallable_Check(gp->hooks.set_info)) {
    GeanyPy_SetError(gp, "Plugin's 'set_info' function is not callable");
    GeanyPy_InterpLeave(gp);
    return;
  }

  info_dict = PyObject_CallObject(gp->hooks.set_info, NULL);
  if (info_dict == NULL) {
    if (PyErr_Occurred())
      PyErr_Print();
    GeanyPy_SetError(gp, "Failed calling plugin's 'set_info' function");
    GeanyPy_InterpLeave(gp);
    return;
  }

  if (!PyMapping_Check(info_dict)) {
    GeanyPy_SetError(gp, "Plugin's 'set_info' function returned an non-mapping object");
    Py_DECREF(info_dict);
    GeanyPy_InterpLeave(gp);
    return;
  }

  gp->info.name = GeanyPy_GetUtf8StringFromDict(gp, info_dict, "name");
  if (gp->info.name == NULL) {
    Py_DECREF(info_dict);
    GeanyPy_SetError(gp, "Plugin failed to provide required 'name' info");
    GeanyPy_InterpLeave(gp);
    return;
  }

  gp->info.description = GeanyPy_GetUtf8StringFromDict(gp, info_dict, "description");
  if (gp->info.description == NULL) {
    gp->info.description = g_strdup("");
    g_warning("Plugin did not provide 'description' info, which it should");
  }

  gp->info.version = GeanyPy_GetUtf8StringFromDict(gp, info_dict, "version");
  if (gp->info.version == NULL) {
    gp->info.version = g_strdup("Unspecified");
    g_warning("Plugin did not provide 'version' info, which it should");
  }

  gp->info.author = GeanyPy_GetUtf8StringFromDict(gp, info_dict, "author");
  if (gp->info.author == NULL) {
    gp->info.author = g_strdup("Anonymous");
    g_warning("Plugin did not provide 'author' info, which it should");
  }

  Py_XDECREF(info_dict);

  GeanyPy_InterpLeave(gp);

  info->name = gp->info.name;
  info->description = gp->info.description;
  info->version = gp->info.version;
  info->author = gp->info.author;
}

void GeanyPy_Init(GeanyPy *gp, G_GNUC_UNUSED GeanyData *data)
{
  PyObject *result;

  g_return_if_fail(gp != NULL);
  g_return_if_fail(data != NULL);
  g_return_if_fail(gp->hooks.init != NULL);

  GeanyPy_InterpEnter(gp);
  GeanyPy_ClearError(gp);

  if (!PyCallable_Check(gp->hooks.init)) {
    GeanyPy_SetError(gp, "Plugin's 'init' function is not callable");
    GeanyPy_InterpLeave(gp);
    return;
  }

  result = PyObject_CallObject(gp->hooks.init, NULL);
  if (result == NULL) {
    if (PyErr_Occurred())
      PyErr_Print();
    GeanyPy_SetError(gp, "Failed calling plugin's 'init' function");
  }

  Py_XDECREF(result);

  GeanyPy_InterpLeave(gp);
}

void GeanyPy_Cleanup(GeanyPy *gp)
{
  g_return_if_fail(gp != NULL);

  GeanyPy_InterpEnter(gp);
  GeanyPy_ClearError(gp);

  if (gp->hooks.cleanup != NULL && !PyCallable_Check(gp->hooks.cleanup)) {
    GeanyPy_SetError(gp, "Plugin's 'cleanup' function is not callable");
    GeanyPy_InterpLeave(gp);
    return;
  }

  if (gp->hooks.cleanup != NULL) {
    PyObject *result = PyObject_CallObject(gp->hooks.cleanup, NULL);
    if (result == NULL) {
      if (PyErr_Occurred())
        PyErr_Print();
      GeanyPy_SetError(gp, "Failed calling plugin's 'cleanup' function");
    }
    Py_XDECREF(result);
  }

  GeanyPy_InterpLeave(gp);
}

GtkWidget *GeanyPy_Configure(GeanyPy *gp, GtkDialog *dialog)
{
  g_return_val_if_fail(gp != NULL, NULL);
  g_return_val_if_fail(GTK_IS_DIALOG(dialog), NULL);

  GeanyPy_InterpEnter(gp);
  GeanyPy_ClearError(gp);

  if (gp->hooks.configure != NULL && !PyCallable_Check(gp->hooks.configure)) {
    GeanyPy_SetError(gp, "Plugin's 'configure' function is not callable");
    GeanyPy_InterpLeave(gp);
    return NULL;
  }

  PyErr_SetString(PyExc_NotImplementedError,
                  "GeanyPy_Configure is not implemented");

  GeanyPy_InterpLeave(gp);

  return NULL;
}

void GeanyPy_Help(GeanyPy *gp)
{
  PyObject *result;

  g_return_if_fail(gp != NULL);

  GeanyPy_InterpEnter(gp);
  GeanyPy_ClearError(gp);

  if (gp->hooks.help != NULL && !PyCallable_Check(gp->hooks.help)) {
    GeanyPy_SetError(gp, "Plugin's 'help' function is not callable");
    GeanyPy_InterpLeave(gp);
    return;
  }

  result = PyObject_CallObject(gp->hooks.help, NULL);
  if (result == NULL) {
    if (PyErr_Occurred())
      PyErr_Print();
    GeanyPy_SetError(gp, "Failed to call plugin's 'help' hook function");
  }

  Py_XDECREF(result);

  GeanyPy_InterpLeave(gp);
}

int GeanyPy_HasError(GeanyPy *gp)
{
  g_return_val_if_fail(gp != NULL, 0);
  return (gp->error->len > 0);
}

void GeanyPy_ClearError(GeanyPy *gp)
{
  g_return_if_fail(gp != NULL);
  GeanyPy_SetError(gp, NULL);
}

void GeanyPy_SetError(GeanyPy *gp, const char *fmt, ...)
{
  g_return_if_fail(gp != NULL);
  if (fmt == NULL || fmt[0] == '\0')
    g_string_assign(gp->error, "");
  else {
    va_list ap;
    va_start(ap, fmt);
    g_string_vprintf(gp->error, fmt, ap);
    va_end(ap);
  }
}

const char *GeanyPy_GetError(GeanyPy *gp)
{
  g_return_val_if_fail(gp != NULL, NULL);
  return gp->error->str;
}
