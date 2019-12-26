/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
/**
 * @file system.h
 * @brief System internal creation and destroying functions.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-21
 * @details The user should not call these functions himself.
 * @copyright Copyright (c) 2019
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_FRAMEWORK_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_FRAMEWORK_H

#include <stdbool.h>
#include <switch.h>
#include "defines.h"

JVX_FS_LIB_BEGIN

jvxfs_status_t jvxfs_system_create_module(jvxfs_module_t** mod, switch_loadable_module_interface_t** module_interface,
    switch_memory_pool_t* pool, const char* name, switch_application_function_t ptrApp, switch_api_function_t ptrApi);

switch_status_t jvxfs_system_init_check(jvxfs_module_t* mod, const char* name);

switch_status_t jvxfs_system_prepare_end(jvxfs_module_t* mod);

switch_status_t jvxfs_system_terminate(jvxfs_module_t** mod);

void jvxfs_system_load_app(jvxfs_module_t* mod, switch_core_session_t* session, const char* data);

switch_status_t jvxfs_system_exec_api(jvxfs_module_t* mod, _In_opt_z_ const char *cmd, 
    _In_opt_ switch_core_session_t *session, _In_ switch_stream_handle_t *stream);

jvxfs_status_t jvx_system_create_app(jvxfs_app_t** app, jvxfs_module_t* mod, const char* name,
    switch_application_interface_t* app_interface, switch_api_interface_t* api_interface);

jvxfs_status_t jvx_system_create_sigproc_app(jvxfs_app_t** app, jvxfs_module_t* mod, const char* name,
    switch_application_interface_t* app_interface, switch_api_interface_t* api_interface,
    jvxfs_algorithm_construct_t func_cnst, jvxfs_algorithm_initialize_t func_init,
    jvxfs_algorithm_process_t func_proc, jvxfs_algorithm_terminate_t func_term,
    jvxfs_algorithm_destruct_t func_dest);

jvxfs_status_t jvx_system_delete_app(jvxfs_app_t* app);

jvxfs_status_t jvxfs_system_create_sp_config(jvxfs_sigprog_config_t** obj, jvxfs_module_t* mod);

jvxfs_status_t jvx_system_delete_sp_config(jvxfs_sigprog_config_t* conf);

JVX_FS_LIB_END

#endif