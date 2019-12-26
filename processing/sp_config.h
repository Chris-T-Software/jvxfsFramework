/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
/**
 * @file sp_config.h
 * @brief Module for configuring signal processing.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-21
 * @copyright Copyright (c) 2019
 */

#ifndef LIB_JVX_FS_FRAMEWORK_PROCESSING_CONFIG_H
#define LIB_JVX_FS_FRAMEWORK_PROCESSING_CONFIG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "../system/defines.h"
#include "../utils/variadic.h"
#include "sp_defines.h"

JVX_FS_LIB_BEGIN

#define jvxfs_sigproc_allow_samplerates(_conf, ...) jvxfs_sigproc_allow_samplerates_detailed(_conf, JVXFS_MACRO_COUNT_PARAMS(__VA_ARGS__), __VA_ARGS__)


jvxfs_status_t jvxfs_sigproc_allow_samplerates_detailed(jvxfs_sigprog_config_t* conf, size_t number, ...);
jvxfs_status_t jvxfs_sigproc_vallow_samplerates_detailed(jvxfs_sigprog_config_t* conf, size_t number, va_list args);
bool jvxfs_sigproc_is_samplerate_allowed(jvxfs_sigprog_config_t* conf, uint32_t fs);

jvxfs_status_t jvxfs_sigproc_set_working_channel(jvxfs_sigprog_config_t* conf, jvxfs_sigproc_channel_t chan, jvxfs_sigproc_working_flag_t flags);
jvxfs_sigproc_channel_t jvxfs_sigproc_get_working_channel(jvxfs_sigprog_config_t* conf);
jvxfs_sigproc_buffer_t jvxfs_sigproc_buffers_channel(jvxfs_sigprog_config_t* conf);

jvxfs_status_t jvxfs_sigproc_set_datatype(jvxfs_sigprog_config_t* conf, jvxfs_sigproc_datatype_t type);
jvxfs_sigproc_datatype_t jvxfs_sigproc_get_datatype(jvxfs_sigprog_config_t* conf);

JVX_FS_LIB_END

#endif