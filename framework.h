/**
 * @file framework.h
 * @brief Library's global include header.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-12-02
 */

#ifndef LIB_JVX_FS_FRAMEWORK_MAIN_INCLUDE_H
#define LIB_JVX_FS_FRAMEWORK_MAIN_INCLUDE_H

#include <switch.h>

#include "system/defines.h"
#include "system/error.h"
#include "system/system.h"
#include "system/module.h"
#include "system/app.h"
#include "system/session.h"
#include "system/view.h"
#include "system/broadcast.h"

#include "processing.h"


#define JVXFS_FRAMEWORK_DEFINE_MODULE_SIMPLE(name) \
	JVX_FS_LIB_BEGIN \
        static SWITCH_MODULE_LOAD_FUNCTION(_jvxfs_framework_load); \
        static SWITCH_MODULE_SHUTDOWN_FUNCTION(_jvxfs_framework_shutdown); \
        SWITCH_STANDARD_APP(_jvxfs_framework_app); \
        SWITCH_STANDARD_API(_jvxfs_framework_api); \
    JVX_FS_LIB_END \
    static void start(jvxfs_module_t*); \
    static void stop(jvxfs_module_t*); \
    static jvxfs_module_t* _jvxfs_mod = NULL; \
	SWITCH_MODULE_DEFINITION(name, _jvxfs_framework_load, _jvxfs_framework_shutdown, NULL); \
    SWITCH_MODULE_LOAD_FUNCTION(_jvxfs_framework_load) \
    { \
        jvxfs_status_t res = jvxfs_system_create_module(&_jvxfs_mod, module_interface, pool, modname, _jvxfs_framework_app, _jvxfs_framework_api); \
        if (res == JVXFS_STATUS_SUCCESS) start(_jvxfs_mod); \
        return jvxfs_system_init_check(_jvxfs_mod, modname); \
    } \
    SWITCH_MODULE_SHUTDOWN_FUNCTION(_jvxfs_framework_shutdown) \
    { \
        if (_jvxfs_mod) { \
            jvxfs_system_prepare_end(_jvxfs_mod); \
            stop(_jvxfs_mod); \
        } \
        return jvxfs_system_terminate(&_jvxfs_mod); \
    } \
    void _jvxfs_framework_app(switch_core_session_t* session, const char* data) \
    { \
        jvxfs_system_load_app(_jvxfs_mod, session, data); \
    } \
    switch_status_t _jvxfs_framework_api(_In_opt_z_ const char *cmd, _In_opt_ switch_core_session_t *session, \
        _In_ switch_stream_handle_t *stream) \
    { \
        return jvxfs_system_exec_api(_jvxfs_mod, cmd, session, stream); \
    }

#endif