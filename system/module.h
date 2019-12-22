/**
 * @file module.h
 * @brief Freeswitch module wrapper.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-21
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_MODULE_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_MODULE_H

#include <stdbool.h>
#include <switch.h>
#include "defines.h"

JVX_FS_LIB_BEGIN

typedef enum
{
    JVXFS_MODULE_INITIALIZING,
    JVXFS_MODULE_RUNNING,
    JVXFS_MODULE_TERMINATING,
    JVXFS_MODULE_FAILED
} jvxfs_module_state_t;

switch_memory_pool_t* jvxfs_module_get_memory_pool(jvxfs_module_t* mod);

jvxfs_status_t jvxfs_module_create_sigproc_app(jvxfs_module_t* mod, jvxfs_app_t** out,
    jvxfs_algorithm_construct_t func_cnst, jvxfs_algorithm_initialize_t func_init,
    jvxfs_algorithm_process_t func_proc, jvxfs_algorithm_terminate_t func_term,
    jvxfs_algorithm_destruct_t func_dest);
jvxfs_status_t jvxfs_module_get_sigproc_app(jvxfs_module_t* mod, jvxfs_app_t** out);
bool jvxfs_module_has_apps(jvxfs_module_t* mod);

jvxfs_error_t* jvxfs_module_get_error_handler(jvxfs_module_t* mod);

jvxfs_module_state_t jvxfs_module_get_state(jvxfs_module_t* mod);

jvxfs_status_t jvxfs_module_change_configfile(jvxfs_module_t* mod, const char* name);
void* jvxfs_module_get_config(jvxfs_module_t* mod);

JVX_FS_LIB_END

#endif