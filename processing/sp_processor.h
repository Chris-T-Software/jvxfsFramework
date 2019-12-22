/**
 * @file sp_processor.h
 * @brief Signal processing module.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-21
 * TODO: Codec info model, algorithm plugin
 */

#ifndef LIB_JVX_FS_FRAMEWORK_PROCESSING_PROCESSOR_H
#define LIB_JVX_FS_FRAMEWORK_PROCESSING_PROCESSOR_H

#include <switch.h>
#include "sp_defines.h"

JVX_FS_LIB_BEGIN

typedef enum
{
    JVXFS_SP_CONSTRUCTING,
    JVXFS_SP_INITIALIZING,
    JVXFS_SP_PROCESSING,
    JVXFS_SP_UPDATING,
    JVXFS_SP_TERMINATING,
    JVXFS_SP_HIBERNATING,
    JVXFS_SP_DESTRUCTING,
    JVXFS_SP_FAILED
} jvxfs_sigproc_state_t;

typedef void(*jvxfs_sigproc_mode_observer_t)(jvxfs_sigproc_processor_t*, void*);


jvxfs_status_t jvxfs_sigproc_create_processor(jvxfs_sigproc_processor_t** obj, jvxfs_app_t* app, jvxfs_error_t* err,
    switch_core_session_t* session, const char* args, void* data);

switch_core_session_t* jvxfs_sigproc_get_session(jvxfs_sigproc_processor_t* proc);

jvxfs_sigproc_state_t jvxfs_sigproc_get_state(jvxfs_sigproc_processor_t* proc);

jvxfs_sigproc_algo_mode_t jvxfs_sigproc_get_mode(jvxfs_sigproc_processor_t* proc);
jvxfs_status_t jvxfs_sigproc_set_mode(jvxfs_sigproc_processor_t* proc, jvxfs_sigproc_algo_mode_t mode);

jvxfs_status_t jvxfs_sigproc_add_mode_observer(jvxfs_sigproc_processor_t* proc, jvxfs_sigproc_mode_observer_t func, void* data);
void jvxfs_sigproc_remove_mode_observer(jvxfs_sigproc_processor_t* proc, jvxfs_sigproc_mode_observer_t func);

jvxfs_channel_model_t* jvxfs_sigproc_get_downlink_info(jvxfs_sigproc_processor_t* proc);

jvxfs_channel_model_t* jvxfs_sigproc_get_uplink_info(jvxfs_sigproc_processor_t* proc);

void jvxfs_sigproc_update(jvxfs_sigproc_processor_t* proc);



JVX_FS_LIB_END

#endif