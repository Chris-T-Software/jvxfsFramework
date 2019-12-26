/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
/**
 * @file defines.h
 * @brief System type definitions.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-21
 * @copyright Copyright (c) 2019
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_DEFINES_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_DEFINES_H

#include <switch.h>

#ifdef __cplusplus
#define JVX_FS_LIB_BEGIN extern "C" {
#define JVX_FS_LIB_END }
#else
#define JVX_FS_LIB_BEGIN
#define JVX_FS_LIB_END
#endif

#if (defined _WIN32 || defined _WIN64) && defined _MSC_VER
#ifndef __func__
#define __func__ __FUNCTION__
#endif
#endif

#if (!defined __cplusplus || __cplusplus < 201103L) && !defined thread_local
    #if __STDC_VERSION__ >= 201112
        #define thread_local _Thread_local
    #elif (defined _WIN32 || defined _WIN64) && defined _MSC_VER
        #define thread_local __declspec(thread) 
    #elif defined __GNUC__ || defined __clang__ || defined __SUNPRO_C
        #define thread_local __thread
    #endif
#endif

JVX_FS_LIB_BEGIN

#define JVXFS_CALLER_INFO __func__, __FILE__, __LINE__

typedef enum
{
    JVXFS_STATUS_SUCCESS,
    JVXFS_STATUS_ERROR,
    JVXFS_STATUS_NOT_INITIALIZING,
    JVXFS_STATUS_INVALID_ARGUMENT,
    JVXFS_STATUS_OUT_OF_BOUNDS,
    JVXFS_STATUS_ELEMENT_NOT_FOUND,
    JVXFS_STATUS_DUPLICATE_ENTRY,
    JVXFS_STATUS_RESOURCE_UNINITIALIZED,
    JVXFS_STATUS_RESOURCE_NOT_FOUND,
    JVXFS_STATUS_RESOURCE_EXISTING,
    JVXFS_STATUS_RESOURCE_EXCEPTION,
    JVXFS_STATUS_ALLOCATION_FAILED,
    JVXFS_STATUS_INVALID_FORMAT,
    JVXFS_STATUS_PENDING_CONFIGURATION,
    JVXFS_STATUS_NO_VIEW_SELECTED,
    JVXFS_STATUS_WRONG_APP_TYPE,
    JVXFS_STATUS_APP_INSTANCE_NOT_FOUND,
    JVXFS_STATUS_MEDIABUG_ERROR
} jvxfs_status_t;

typedef enum
{
    JVXFS_LOG_NONE = 0,
    JVXFS_LOG_CRITICAL = 2,
    JVXFS_LOG_ERROR = 3,
    JVXFS_LOG_WARNING = 4,
    JVXFS_LOG_NOTICE = 5,
    JVXFS_LOG_INFO = 6
} jvxfs_log_level_t;

typedef void jvxfs_error_t;
typedef void jvxfs_module_t;
typedef void jvxfs_app_t;
typedef void jvxfs_app_instance_t;
typedef void jvxfs_view_t;
typedef void jvxfs_sigprog_config_t;
typedef void jvxfs_sigproc_processor_t;

typedef struct
{
    jvxfs_app_t* app;
    switch_core_session_t* session;
    const char* directive;
    const char* parameters;
} jvxfs_directive_data_t;

typedef void(*jvxfs_directive_func_app_t)(jvxfs_view_t*, jvxfs_directive_data_t*, void*);
typedef void(*jvxfs_directive_func_session_t)(jvxfs_view_t*, jvxfs_directive_data_t*, jvxfs_app_instance_t*, void*);
#define JVXFS_DIRECTIVE_NAME_MAX_LENGTH 32

typedef void jvxfs_sigproc_media_t;

typedef enum
{
    JVXFS_SP_EXEC_AUTO = 0,
    JVXFS_SP_EXEC_ASYNC = 0,
    JVXFS_SP_EXEC_SYNC = 1
} jvxfs_sigproc_exec_t;

typedef enum
{
    JVXFS_SP_ENABLE_SYNC_UPDATE,
    JVXFS_SP_DISABLE_SYNC_UPDATE
} jvxfs_sigproc_update_flag_t;

typedef void(*jvxfs_algorithm_construct_t)(void**, jvxfs_sigproc_media_t*, const char*);
typedef void(*jvxfs_algorithm_initialize_t)(void*, jvxfs_sigproc_media_t*);
typedef void(*jvxfs_algorithm_process_t)(void*, jvxfs_sigproc_media_t*);
typedef void(*jvxfs_algorithm_terminate_t)(void*);
typedef void(*jvxfs_algorithm_destruct_t)(void**);
typedef void(*jvxfs_algorithm_update_t)(void* hdl, jvxfs_sigproc_exec_t exec);

typedef struct
{
    jvxfs_algorithm_construct_t construct;
    jvxfs_algorithm_initialize_t initialize;
    jvxfs_algorithm_process_t process;
    jvxfs_algorithm_terminate_t terminate;
    jvxfs_algorithm_destruct_t destruct;
    jvxfs_algorithm_update_t update;
    jvxfs_sigproc_update_flag_t flag;
} jvxfs_algorithm_vtable_t;

JVX_FS_LIB_END

#endif