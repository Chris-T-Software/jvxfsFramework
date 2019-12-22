#include <stdlib.h>
#include <string.h>
#include "../processing/sp_config.h"
#include "module.h"
#include "session.h"
#include "error.h"
#include "directives.h"
#include "system.h"
#include "app.h"

typedef struct list_drct
{
    const char* name;
    void* func;
    void* data;
    struct list_drct* next;
} list_drct_t;

typedef struct
{
    jvxfs_module_t* mod;
    switch_application_interface_t* app;
    switch_api_interface_t* api;
    const char* version;
    void** indexStore;
    size_t indexStoreSize;
    jvxfs_sigprog_config_t* spConfig;
    jvxfs_app_instance_factory_t factory;
    list_drct_t* drctAppStart;
    list_drct_t* drctAppStop;
    list_drct_t* drctInstStart;
    list_drct_t* drctInstStop;
    jvxfs_algorithm_vtable_t* vtable;
} app_t;

static jvxfs_status_t insert_list_item(app_t* hdl, const char* name, void* func, void* data, list_drct_t** start, list_drct_t** stop);
static void add_default_directives(app_t* hdl);


jvxfs_status_t jvx_system_create_app(jvxfs_app_t** app, jvxfs_module_t* mod, const char* name,
    switch_application_interface_t* app_interface, switch_api_interface_t* api_interface)
{
    *app = NULL;
    switch_memory_pool_t* pool = jvxfs_module_get_memory_pool(mod);
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(mod);
    if (jvxfs_module_get_state(mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not create FS app.");
    }
    app_t* hdl = (app_t*)switch_core_alloc(pool, sizeof(app_t));
    if (!hdl) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_MODULE,
            "Could not create FS app.");
    }
    hdl->mod = mod;
    hdl->app = app_interface;
    hdl->api = api_interface;
    hdl->version = "";
    hdl->indexStore = NULL;
    hdl->indexStoreSize = 0;
    hdl->factory = NULL;
    hdl->drctAppStart = NULL;
    hdl->drctAppStop = NULL;
    hdl->drctInstStart = NULL;
    hdl->drctInstStop = NULL;
    hdl->spConfig = NULL;
    hdl->vtable = NULL;
    add_default_directives(hdl);
    *app = hdl;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvx_system_create_sigproc_app(jvxfs_app_t** app, jvxfs_module_t* mod, const char* name,
    switch_application_interface_t* app_interface, switch_api_interface_t* api_interface,
    jvxfs_algorithm_construct_t func_cnst, jvxfs_algorithm_initialize_t func_init,
    jvxfs_algorithm_process_t func_proc, jvxfs_algorithm_terminate_t func_term,
    jvxfs_algorithm_destruct_t func_dest)
{
    *app = NULL;
    app_t* hdl = NULL;
    void* tmp = NULL;
    switch_memory_pool_t* pool = jvxfs_module_get_memory_pool(mod);
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(mod);
    if (!func_cnst || !func_init || !func_proc || !func_term) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_INVALID_ARGUMENT, JVXFS_LOG_CRITICAL, JVXFS_COMP_MODULE,
            "Could not create signal processing app, state functions missing.");
    }
    jvxfs_status_t res = jvx_system_create_app(&tmp, mod, name, app_interface, api_interface);
    if (res != JVXFS_STATUS_SUCCESS) return res;
    hdl = (app_t*)tmp;
    res = jvxfs_system_create_sp_config(&hdl->spConfig , mod);
    if (res != JVXFS_STATUS_SUCCESS) return res;
    jvxfs_algorithm_vtable_t* vtbl = (jvxfs_algorithm_vtable_t*)switch_core_alloc(pool, sizeof(jvxfs_algorithm_vtable_t));
    if (!vtbl) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_MODULE,
            "Could not create FS app.");
    }
    hdl->vtable = vtbl;
    vtbl->construct = func_cnst;
    vtbl->initialize = func_init;
    vtbl->process = func_proc;
    vtbl->terminate = func_term;
    vtbl->destruct = func_dest;
    vtbl->update = NULL;
    vtbl->flag = JVXFS_SP_DISABLE_SYNC_UPDATE;
    *app = hdl;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvx_system_delete_app(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    jvxfs_app_clear_indexed_storage(app);
    if (hdl->spConfig) {
        jvx_system_delete_sp_config(hdl->spConfig);
    }
    return JVXFS_STATUS_SUCCESS;
}

const char* jvxfs_app_get_name(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    return hdl->app->interface_name;
}

const char* jvxfs_app_get_teaser(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    return hdl->app->short_desc;
}

jvxfs_status_t jvxfs_app_set_teaser(jvxfs_app_t* app, const char* teaser)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not set teaser.");
    }
    hdl->app->short_desc = teaser;
    hdl->api->desc = teaser;
    return JVXFS_STATUS_SUCCESS;
}

const char* jvxfs_app_get_description(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    return hdl->app->long_desc;
}

jvxfs_status_t jvxfs_app_set_description(jvxfs_app_t* app, const char* desc)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not set description.");
    }
    hdl->app->long_desc = desc;
    return JVXFS_STATUS_SUCCESS;
}

const char* jvxfs_app_get_version(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    return hdl->version;
}

jvxfs_status_t jvxfs_app_set_version(jvxfs_app_t* app, const char* version)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not set version.");
    }
    hdl->version = version;
    return JVXFS_STATUS_SUCCESS;
}

const char* jvxfs_app_get_syntax(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    return hdl->api->syntax;
}

jvxfs_status_t jvxfs_app_set_syntax(jvxfs_app_t* app, const char* syntax)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not set syntax.");
    }
    hdl->app->syntax = syntax;
    hdl->api->syntax = syntax;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_app_set_indexed_storage_complete(jvxfs_app_t* app, size_t number, ...)
{
    va_list args;
    va_start(args, number);
    jvxfs_status_t res = jvxfs_app_vset_indexed_storage_complete(app, number, args);
    va_end(args);
    return res;
}

jvxfs_status_t jvxfs_app_vset_indexed_storage_complete(jvxfs_app_t* app, size_t number, va_list args)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (hdl->indexStore) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_RESOURCE_EXISTING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Indexed storage of app already set.");
    }
    hdl->indexStore = (void**)malloc(sizeof(void*) * number);
    if (!hdl->indexStore) {
        hdl->indexStoreSize = 0;
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_APP,
            "Could not create indexed storage for app.");
    }
    hdl->indexStoreSize = number;
    for (size_t i = 0; i < number; ++i) {
        hdl->indexStore[i] = va_arg(args, void*);
    }
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_app_get_indexed_storage_element(jvxfs_app_t* app, size_t index, void** out)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (index >= hdl->indexStoreSize) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_OUT_OF_BOUNDS, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Indexed storage of app does not contain so many elements.");
    }
    *out = hdl->indexStore[index];
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_app_get_indexed_storage_complete(jvxfs_app_t* app, size_t number, ...)
{
    va_list args;
    va_start(args, number);
    jvxfs_status_t res = jvxfs_app_vget_indexed_storage_complete(app, number, args);
    va_end(args);
    return res;
}

jvxfs_status_t jvxfs_app_vget_indexed_storage_complete(jvxfs_app_t* app, size_t number, va_list args)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (number > hdl->indexStoreSize) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_OUT_OF_BOUNDS, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Indexed storage of app does not contain so many elements.");
    }
    for (size_t i = 0; i < number; ++i) {
        void** ptr = va_arg(args, void**);
        *ptr = hdl->indexStore[i];
    }
    return JVXFS_STATUS_SUCCESS;
}

size_t jvxfs_app_count_indexed_storage_elements(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    return hdl->indexStoreSize;
}

void jvxfs_app_clear_indexed_storage(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    hdl->indexStoreSize = 0;
    if (hdl->indexStore) {
        free(hdl->indexStore);
        hdl->indexStore = NULL;
    }
}

jvxfs_status_t jvxfs_app_add_directive(jvxfs_app_t* app, const char* name, jvxfs_directive_func_app_t func, void* data)
{
    app_t* hdl = (app_t*)app;
    return insert_list_item(hdl, name, func, data, &hdl->drctAppStart, &hdl->drctAppStop);
}

jvxfs_status_t jvxfs_app_add_session_directive(jvxfs_app_t* app, const char* name, jvxfs_directive_func_session_t func, void* data)
{
    app_t* hdl = (app_t*)app;
    return insert_list_item(hdl, name, func, data, &hdl->drctInstStart, &hdl->drctInstStop);
}

jvxfs_status_t jvxfs_app_call_directive(jvxfs_directive_data_t* data, jvxfs_view_t* view)
{
    app_t* hdl = (app_t*)data->app;
    list_drct_t* item = (!data->session) ? hdl->drctAppStart : hdl->drctInstStart;
    list_drct_t* found = NULL;
    while (item != NULL) {
        if (strncmp(item->name, data->directive, JVXFS_DIRECTIVE_NAME_MAX_LENGTH) == 0) {
            found = item;
            break;
        } else {
            item = item->next;
        }
    }
    if (!found) return JVXFS_STATUS_ELEMENT_NOT_FOUND;
    if (!data->session) {
        jvxfs_directive_func_app_t func = (jvxfs_directive_func_app_t)found->func;
        func(view, data, found->data);
        return JVXFS_STATUS_SUCCESS;
    } else {
        jvxfs_directive_func_session_t func = (jvxfs_directive_func_session_t)found->func; 
        return jvxfs_session_exec_directive(data, view, func, found->data);
    }
}

jvxfs_status_t jvxfs_app_set_instance_factory(jvxfs_app_t* app, jvxfs_app_instance_factory_t func)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not set instance factory.");
    }
    if (!func) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_INVALID_ARGUMENT, JVXFS_LOG_CRITICAL, JVXFS_COMP_APP,
            "App instance factory function pointer must not be NULL.");
    }
    hdl->factory = func;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_app_produce_instance(jvxfs_app_t* app, switch_core_session_t* session, const char* args)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (!hdl->factory) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_PENDING_CONFIGURATION, JVXFS_LOG_CRITICAL, JVXFS_COMP_APP,
            "Cannot create instance without factory.");
    }
    jvxfs_app_instance_t* tmp = NULL;
    void* data = (hdl->vtable) ? hdl->vtable : NULL;
    jvxfs_status_t res = hdl->factory(&tmp, app, err_hdl, session, args, data);
    if (res != JVXFS_STATUS_SUCCESS) return res;
    return jvxfs_session_add_app_instance(session, app, tmp);
}

jvxfs_status_t jvxfs_app_get_sigproc_config(jvxfs_app_t* app, jvxfs_sigprog_config_t** out)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    *out = NULL;
    if (!hdl->spConfig) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_WRONG_APP_TYPE, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not return signal processing configuration.");
    }
    *out = hdl->spConfig;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_app_set_sigproc_update_func(jvxfs_app_t* app, jvxfs_algorithm_update_t func, jvxfs_sigproc_update_flag_t flag)
{
    app_t* hdl = (app_t*)app;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not set signal processing update function.");
    }
    if (!hdl->vtable) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_WRONG_APP_TYPE, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not find processing vtable.");
    }
    hdl->vtable->update = func;
    hdl->vtable->flag = flag;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_module_t* jvxfs_app_get_module(jvxfs_app_t* app)
{
    app_t* hdl = (app_t*)app;
    return hdl->mod;
}


jvxfs_status_t insert_list_item(app_t* hdl, const char* name, void* func, void* data, list_drct_t** start, list_drct_t** stop)
{
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Could not add new directive.");
    }
    if (!name | !func) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_INVALID_ARGUMENT, JVXFS_LOG_ERROR, JVXFS_COMP_APP,
            "Name or function of directive must not be NULL.");
    }
    switch_memory_pool_t* pool = jvxfs_module_get_memory_pool(hdl->mod);
    list_drct_t* item = (list_drct_t*)switch_core_alloc(pool, sizeof(list_drct_t));
    if (!item) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_APP,
            "Could not create directive item.");
    }
    item->name = name;
    item->func = func;
    item->data = data;
    item->next = NULL;
    if (*stop == NULL) {
        *start = item;
    } else {
        (*stop)->next = item;
    }
    *stop = item;
    return JVXFS_STATUS_SUCCESS;
}

void add_default_directives(app_t* hdl)
{
    jvxfs_app_add_directive(hdl, "version", jvxfs_directive_app_version, NULL);
}