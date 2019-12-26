/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <string.h>
#include "app.h"
#include "error.h"
#include "module.h"
#include "session.h"

/**
 * TODO: fix if running app is called
 */
jvxfs_status_t jvxfs_session_add_app_instance(switch_core_session_t* session, jvxfs_app_t* app, void* instance)
{
    const char* name = jvxfs_app_get_name(app);
    jvxfs_error_t* err = jvxfs_module_get_error_handler(jvxfs_app_get_module(app));
    switch_channel_t* channel = switch_core_session_get_channel(session);
    void* tmp = switch_channel_get_private(channel, name);
    if (tmp) {
        return jvxfs_error_set_error(err, JVXFS_STATUS_DUPLICATE_ENTRY, JVXFS_LOG_WARNING, JVXFS_COMP_SESSION,
            "Existing app instance already running, stopped creating new one.");
    }
    switch_channel_set_private(channel, name, instance);
    return JVXFS_STATUS_SUCCESS;    
}

jvxfs_app_instance_t* jvxfs_session_remove_app_instance(switch_core_session_t* session, const char* name)
{
    switch_channel_t* channel = switch_core_session_get_channel(session);
    jvxfs_app_instance_t* tmp = switch_channel_get_private(channel, name);
    switch_channel_set_private(channel, name, NULL);
    return tmp;   
}

jvxfs_status_t jvxfs_session_exec_directive(jvxfs_directive_data_t* data, jvxfs_view_t* view,
    jvxfs_directive_func_session_t directive, void* user)
{
    switch_channel_t* channel = switch_core_session_get_channel(data->session);
    jvxfs_app_instance_t* inst = switch_channel_get_private(channel, jvxfs_app_get_name(data->app));
    if (!inst) return JVXFS_STATUS_APP_INSTANCE_NOT_FOUND;
    directive(view, data, inst, user);
    return JVXFS_STATUS_SUCCESS;
}

const char* jvxfs_session_get_extension_name(switch_core_session_t* session)
{
    return switch_channel_get_caller_profile(switch_core_session_get_channel(session))->caller_id_name;
}

const char* jvxfs_session_get_extension_number(switch_core_session_t* session)
{
    return switch_channel_get_caller_profile(switch_core_session_get_channel(session))->caller_id_number;
}