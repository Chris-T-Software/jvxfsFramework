/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
#include <stdlib.h>
#include "../system/error.h"
#include "../system/module.h"
#include "../system/system.h"
#include "sp_config.h"

typedef struct
{
    uint32_t* arrFs;
    size_t arrFsSize;
    jvxfs_sigproc_channel_t workChan;
    jvxfs_sigproc_working_flag_t workFlag;
    jvxfs_sigproc_datatype_t type;
    jvxfs_module_t* mod;
} conf_t;


jvxfs_status_t jvxfs_system_create_sp_config(jvxfs_sigprog_config_t** obj, jvxfs_module_t* mod)
{
    *obj = NULL;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(mod);
    const char* const error = "Could not create signal processing configuration.";
    if (jvxfs_module_get_state(mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_APP, error);
    }
    switch_memory_pool_t* pool = jvxfs_module_get_memory_pool(mod);
    conf_t* hdl = (conf_t*)switch_core_alloc(pool, sizeof(conf_t));
    if (!hdl) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_APP, error);
    }
    hdl->arrFs = NULL;
    hdl->arrFsSize = 0;
    hdl->workChan = JVXFS_SP_NO_LINK;
    hdl->workFlag = JVXFS_SP_DEFAULT;
    hdl->type = JVXFS_SP_DATA;
    hdl->mod = mod;
    *obj = hdl;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvx_system_delete_sp_config(jvxfs_sigprog_config_t* conf)
{
    conf_t* hdl = (conf_t*)conf;
    if (hdl->arrFs) {
        free(hdl->arrFs);
        hdl->arrFsSize = 0;
    }
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_sigproc_allow_samplerates_detailed(jvxfs_sigprog_config_t* conf, size_t number, ...)
{
    va_list args;
    va_start(args, number);
    jvxfs_status_t res = jvxfs_sigproc_vallow_samplerates_detailed(conf, number, args);
    va_end(args);
    return res;
}

jvxfs_status_t jvxfs_sigproc_vallow_samplerates_detailed(jvxfs_sigprog_config_t* conf, size_t number, va_list args)
{
    conf_t* hdl = (conf_t*)conf;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    const char* const error = "Could not store allowed samplerates.";
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_SP_CONFIG, error);
    }
    uint32_t* mem = (uint32_t*)malloc(sizeof(uint32_t) * number);
    if (!mem) {
        hdl->arrFsSize = 0;
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_SP_CONFIG, error);
    }
    if (hdl->arrFs) free(hdl->arrFs);
    hdl->arrFs = mem;
    hdl->arrFsSize = number;
    for (size_t i = 0; i < number; ++i) {
        hdl->arrFs[i] = va_arg(args, uint32_t);
    }
    return JVXFS_STATUS_SUCCESS;
}

bool jvxfs_sigproc_is_samplerate_allowed(jvxfs_sigprog_config_t* conf, uint32_t fs)
{
    conf_t* hdl = (conf_t*)conf;
    for (size_t i = 0; i < hdl->arrFsSize; ++i) {
        if (hdl->arrFs[i] == fs) return true;
    }
    return false;
}

jvxfs_status_t jvxfs_sigproc_set_working_channel(jvxfs_sigprog_config_t* conf, jvxfs_sigproc_channel_t chan, jvxfs_sigproc_working_flag_t flags)
{
    conf_t* hdl = (conf_t*)conf;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_SP_CONFIG, "Could not set working channel.");
    }
    hdl->workChan = chan;
    hdl->workFlag = flags;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_sigproc_channel_t jvxfs_sigproc_get_working_channel(jvxfs_sigprog_config_t* conf)
{
    conf_t* hdl = (conf_t*)conf;
    return hdl->workChan;
}

jvxfs_sigproc_buffer_t jvxfs_sigproc_buffers_channel(jvxfs_sigprog_config_t* conf)
{
    conf_t* hdl = (conf_t*)conf;
    if (hdl->workChan == JVXFS_SP_NO_LINK) {
        return JVXFS_SP_BUFFER_NO_LINK;
    } else if (hdl->workFlag == JVXFS_SP_CATCH_BOTH_LINKS) {
        return JVXFS_SP_BUFFER_BOTH_LINKS;
    } else if (hdl->workChan == JVXFS_SP_UPLINK) {
        return JVXFS_SP_BUFFER_UPLINK;
    } else {
        return JVXFS_SP_BUFFER_DOWNLINK;
    }
}

jvxfs_status_t jvxfs_sigproc_set_datatype(jvxfs_sigprog_config_t* conf, jvxfs_sigproc_datatype_t type)
{
    conf_t* hdl = (conf_t*)conf;
    jvxfs_error_t* err_hdl = jvxfs_module_get_error_handler(hdl->mod);
    if (jvxfs_module_get_state(hdl->mod) != JVXFS_MODULE_INITIALIZING) {
        return jvxfs_error_set_error(err_hdl, JVXFS_STATUS_NOT_INITIALIZING, JVXFS_LOG_ERROR, JVXFS_COMP_SP_CONFIG, "Could not set datatype.");
    }
    hdl->type = type;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_sigproc_datatype_t jvxfs_sigproc_get_datatype(jvxfs_sigprog_config_t* conf)
{
    conf_t* hdl = (conf_t*)conf;
    return hdl->type;
}