/**
 * @file error.h
 * @brief Error handling mechanism.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-22
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_ERROR_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_ERROR_H

#include <stdbool.h>
#include <stdint.h>
#include <switch.h>
#include "defines.h"

JVX_FS_LIB_BEGIN

#define jvxfs_error_set_error(_obj, _stat, _level, _comp, _msg) jvxfs_error_set_error_detailed(_obj, _stat, _level, _comp, __func__, __FILE__, __LINE__, switch_time_now(), _msg)

typedef enum
{
    JVXFS_COMP_NONE,
    JVXFS_COMP_SYSTEM,
    JVXFS_COMP_VIEW,
    JVXFS_COMP_MODULE,
    JVXFS_COMP_APP,
    JVXFS_COMP_SESSION,
    JVXFS_COMP_SP_CONFIG,
    JVXFS_COMP_SP_PROCESSOR,
    JVXFS_COMP_OBSERVER
} jvxfs_component_t;

typedef struct
{
    jvxfs_status_t status;
    jvxfs_log_level_t level;
    jvxfs_component_t component;
    const char* message;
    const char* function;
    const char* file;
    uint32_t line;
    switch_time_t time;
} jvxfs_error_info_t;

typedef void(*jvxfs_error_callback_t)(const jvxfs_error_info_t*, void* data);

jvxfs_status_t jvxfs_error_create_error_handler(jvxfs_error_t** obj, switch_memory_pool_t* pool);

bool jvxfs_error_has_happend(jvxfs_error_t* obj);

jvxfs_status_t jvxfs_error_set_error_detailed(jvxfs_error_t* obj, jvxfs_status_t status, jvxfs_log_level_t level, jvxfs_component_t component,
    const char* function, const char* file, uint32_t line, switch_time_t time, const char* message);

jvxfs_status_t jvxfs_error_set_callback(jvxfs_error_t* obj, jvxfs_error_callback_t func, void* data);

void jvxfs_error_remove_callback(jvxfs_error_t* obj); 

void jvxfs_error_default_callback_function(const jvxfs_error_info_t* info, void* data);

const char* jvxfs_error_status_to_message(jvxfs_status_t stat);

JVX_FS_LIB_END

#endif