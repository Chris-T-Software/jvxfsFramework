/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <string.h>
#include "../processing/sp_processor.h"
#include "app.h"
#include "system.h"
#include "error.h"
#include "view_private.h"
#include "module.h"

typedef struct
{
    switch_loadable_module_interface_t* interface;
    switch_application_function_t appFunc;
    switch_api_function_t apiFunc;
    jvxfs_app_t* app;
    jvxfs_error_t* err;
    jvxfs_module_state_t state;
} module_t;


jvxfs_status_t jvxfs_system_create_module(jvxfs_module_t** mod, switch_loadable_module_interface_t** module_interface,
    switch_memory_pool_t* pool, const char* name, switch_application_function_t ptrApp, switch_api_function_t ptrApi)
{
    * mod = NULL;
    module_t* hdl = (module_t*)switch_core_alloc(pool, sizeof(module_t));
    if (!hdl) return JVXFS_STATUS_ALLOCATION_FAILED;
    jvxfs_status_t res = jvxfs_error_create_error_handler(&hdl->err, pool);
    if (res != JVXFS_STATUS_SUCCESS) return res;
    *module_interface = switch_loadable_module_create_module_interface(pool, name);
    hdl->interface = *module_interface;
    hdl->appFunc = ptrApp;
    hdl->apiFunc = ptrApi;
    hdl->app = NULL;
    hdl->state = JVXFS_MODULE_INITIALIZING;
    *mod = hdl;
    return JVXFS_STATUS_SUCCESS;
}

switch_status_t jvxfs_system_init_check(jvxfs_module_t* mod, const char* name)
{
    if (!mod) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_CRIT, "Could not allocate RAM to create module \"%s\".\n", name);
        return SWITCH_STATUS_FALSE;
    }
    module_t* hdl = (module_t*)mod;
    bool wasError = jvxfs_error_has_happend(hdl->err);
    if (wasError) {
        switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Aborting start of module due to error.\n");
        hdl->state = JVXFS_MODULE_FAILED;
        return SWITCH_STATUS_FALSE;
    } else {
        hdl->state = JVXFS_MODULE_RUNNING;
        return SWITCH_STATUS_SUCCESS;
    }
}

switch_status_t jvxfs_system_prepare_end(jvxfs_module_t* mod)
{
    module_t* hdl = (module_t*)mod;
    hdl->state = JVXFS_MODULE_TERMINATING;
    return JVXFS_STATUS_SUCCESS;
}

switch_status_t jvxfs_system_terminate(jvxfs_module_t** mod)
{
    if (!*mod) return SWITCH_STATUS_SUCCESS;
    module_t* hdl = (module_t*)*mod;
    if (hdl->app) {
        jvx_system_delete_app(hdl->app);
        hdl->app = NULL;
    }
    *mod = NULL;
    return SWITCH_STATUS_SUCCESS;
}

void jvxfs_system_load_app(jvxfs_module_t* mod, switch_core_session_t* session, const char* data)
{
    module_t* hdl = (module_t*)mod;
    if (!hdl->app) {
        jvxfs_error_set_error(hdl->err, JVXFS_STATUS_PENDING_CONFIGURATION, JVXFS_LOG_ERROR, JVXFS_COMP_SYSTEM,
            "Trying to create instance of non-existing app.");
        return;
    }
    //switch_channel_t* channel = switch_core_session_get_channel(session);
    //const char* appName = switch_channel_get_variable(channel, SWITCH_CURRENT_APPLICATION_VARIABLE);
    /*jvxfs_status_t res =*/ jvxfs_app_produce_instance(hdl->app, session, data);
}

switch_status_t jvxfs_system_exec_api(jvxfs_module_t* mod, _In_opt_z_ const char *cmd, 
    _In_opt_ switch_core_session_t *session, _In_ switch_stream_handle_t *stream)
{
    module_t* hdl = (module_t*)mod;
    //const char* apiName = switch_event_get_header_nil(stream->param_event, "API-Command");
    char tmp[JVXFS_DIRECTIVE_NAME_MAX_LENGTH+1] = {0};
    const char* directive = NULL;
    const char* params = strchr(cmd, ' ');
    if (!params) {
        directive = cmd;
        params = "";
    } else {
        size_t len = params - cmd;
        if (len > JVXFS_DIRECTIVE_NAME_MAX_LENGTH) len = JVXFS_DIRECTIVE_NAME_MAX_LENGTH;
        strncpy(tmp, cmd, len);
        directive = tmp;
        ++params;
    }
    jvxfs_directive_data_t data = { .app = hdl->app, .session = session, .directive = directive,
        .parameters = params };
    view_priv_t view = { .origin = JVXFS_VIEW_IN_CONSOLE, .dest = JVXFS_VIEW_OUT_CONSOLE, .err = hdl->err, 
        .data = &data, .console = stream };
    jvxfs_status_t res = jvxfs_app_call_directive(&data, &view);
    return (res == JVXFS_STATUS_SUCCESS) ? SWITCH_STATUS_SUCCESS : SWITCH_STATUS_FALSE;
}

switch_memory_pool_t* jvxfs_module_get_memory_pool(jvxfs_module_t* mod)
{
    module_t* hdl = (module_t*)mod;
    return hdl->interface->pool;
}

jvxfs_status_t jvxfs_module_create_sigproc_app(jvxfs_module_t* mod, jvxfs_app_t** out,
    jvxfs_algorithm_construct_t func_cnst, jvxfs_algorithm_initialize_t func_init,
    jvxfs_algorithm_process_t func_proc, jvxfs_algorithm_terminate_t func_term,
    jvxfs_algorithm_destruct_t func_dest)
{
    *out = NULL;
    module_t* hdl = (module_t*)mod;
    const char* appName = &(hdl->interface->module_name[4]);
    switch_loadable_module_interface_t** module_interface = &(hdl->interface);
    switch_application_interface_t* app_interface;
	switch_api_interface_t* api_interface;
    SWITCH_ADD_APP(app_interface, appName, "", "", hdl->appFunc, "", SAF_NONE);
	SWITCH_ADD_API(api_interface, appName, "", hdl->apiFunc, "");
    jvxfs_status_t res = jvx_system_create_sigproc_app(&hdl->app, mod, appName, app_interface, api_interface,
        func_cnst, func_init, func_proc, func_term, func_dest);
    if (res != JVXFS_STATUS_SUCCESS) return res;
    res = jvxfs_app_set_instance_factory(hdl->app, jvxfs_sigproc_create_processor);
    *out = hdl->app;
    return res;
}

jvxfs_status_t jvxfs_module_get_sigproc_app(jvxfs_module_t* mod, jvxfs_app_t** out)
{
    module_t* hdl = (module_t*)mod;
    *out = hdl->app;
    return JVXFS_STATUS_SUCCESS;
}

bool jvxfs_module_has_apps(jvxfs_module_t* mod)
{
    module_t* hdl = (module_t*)mod;
    return hdl->app != NULL;
}

jvxfs_error_t* jvxfs_module_get_error_handler(jvxfs_module_t* mod)
{
    module_t* hdl = (module_t*)mod;
    return hdl->err;
}

jvxfs_module_state_t jvxfs_module_get_state(jvxfs_module_t* mod)
{
    module_t* hdl = (module_t*)mod;
    return hdl->state;
}