/**
 * @file app.h
 * @brief Freeswitch app wrapper.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-21
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_APP_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_APP_H

#include <stdlib.h>
#include <stdarg.h>
#include <switch.h>
#include "defines.h"
#include "../utils/variadic.h"

JVX_FS_LIB_BEGIN

#define jvxfs_app_set_indexed_storage(_app, ...) jvxfs_app_set_indexed_storage_complete(_app, JVXFS_MACRO_COUNT_PARAMS(__VA_ARGS__), __VA_ARGS__)
#define jvxfs_app_get_indexed_storage(_app, ...) jvxfs_app_get_indexed_storage_complete(_app, JVXFS_MACRO_COUNT_PARAMS(__VA_ARGS__), __VA_ARGS__)

typedef jvxfs_status_t(*jvxfs_app_instance_factory_t)(jvxfs_app_instance_t**, jvxfs_app_t*, jvxfs_error_t*,
    switch_core_session_t*, const char*,void*);


const char* jvxfs_app_get_name(jvxfs_app_t* app);
const char* jvxfs_app_get_teaser(jvxfs_app_t* app);
jvxfs_status_t jvxfs_app_set_teaser(jvxfs_app_t* app, const char* teaser);
const char* jvxfs_app_get_description(jvxfs_app_t* app);
jvxfs_status_t jvxfs_app_set_description(jvxfs_app_t* app, const char* desc);
const char* jvxfs_app_get_version(jvxfs_app_t* app);
jvxfs_status_t jvxfs_app_set_version(jvxfs_app_t* app, const char* version);
const char* jvxfs_app_get_syntax(jvxfs_app_t* app);
jvxfs_status_t jvxfs_app_set_syntax(jvxfs_app_t* app, const char* syntax);

jvxfs_status_t jvxfs_app_set_indexed_storage_complete(jvxfs_app_t* app, size_t number, ...);
jvxfs_status_t jvxfs_app_vset_indexed_storage_complete(jvxfs_app_t* app, size_t number, va_list args);
jvxfs_status_t jvxfs_app_get_indexed_storage_element(jvxfs_app_t* app, size_t index, void** out);
jvxfs_status_t jvxfs_app_get_indexed_storage_complete(jvxfs_app_t* app, size_t number, ...);
jvxfs_status_t jvxfs_app_vget_indexed_storage_complete(jvxfs_app_t* app, size_t number, va_list args);
size_t jvxfs_app_count_indexed_storage_elements(jvxfs_app_t* app);
void jvxfs_app_clear_indexed_storage(jvxfs_app_t* app);

jvxfs_status_t jvxfs_app_add_directive(jvxfs_app_t* app, const char* name, jvxfs_directive_func_app_t func, void* data);

jvxfs_status_t jvxfs_app_add_session_directive(jvxfs_app_t* app, const char* name, jvxfs_directive_func_session_t func, void* data);

jvxfs_status_t jvxfs_app_call_directive(jvxfs_directive_data_t* data, jvxfs_view_t* view);

jvxfs_status_t jvxfs_app_set_instance_factory(jvxfs_app_t* app, jvxfs_app_instance_factory_t func);
jvxfs_status_t jvxfs_app_produce_instance(jvxfs_app_t* app, switch_core_session_t* session, const char* args);

jvxfs_status_t jvxfs_app_get_sigproc_config(jvxfs_app_t* app, jvxfs_sigprog_config_t** out);

jvxfs_status_t jvxfs_app_set_sigproc_update_func(jvxfs_app_t* app, jvxfs_algorithm_update_t func, jvxfs_sigproc_update_flag_t flag);

jvxfs_module_t* jvxfs_app_get_module(jvxfs_app_t* app);

JVX_FS_LIB_END

#endif