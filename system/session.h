/**
 * @file session.h
 * @brief Freeswitch session wrapper module.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-21
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_SESSION_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_SESSION_H

#include <stdlib.h>
#include <switch.h>
#include "defines.h"

JVX_FS_LIB_BEGIN

jvxfs_status_t jvxfs_session_add_app_instance(switch_core_session_t* session, jvxfs_app_t* app, jvxfs_app_instance_t* instance);

jvxfs_app_instance_t* jvxfs_session_remove_app_instance(switch_core_session_t* session, const char* name);

jvxfs_status_t jvxfs_session_exec_directive(jvxfs_directive_data_t* data, jvxfs_view_t* view,
    jvxfs_directive_func_session_t directive, void* user);

const char* jvxfs_session_get_extension_name(switch_core_session_t* session);
const char* jvxfs_session_get_extension_number(switch_core_session_t* session);

JVX_FS_LIB_END

#endif