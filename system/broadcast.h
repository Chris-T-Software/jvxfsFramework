/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/**
 * @file broadcast.h
 * @brief Module for handling output outside of directives.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-12-05
 * @copyright Copyright (c) 2019
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_BROADCAST_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_BROADCAST_H

#include <stdarg.h>
#include <stdint.h>
#include "defines.h"

JVX_FS_LIB_BEGIN

//#define jvxfs_console_print(_lvl, _msg, ...) jvxfs_console_print_detailed(__func__, __FILE__, __LINE__, _lvl, _msg, PP_NARG(__VA_ARGS__))

void jvxfs_console_print_detailed(const char* function, const char* file, uint32_t line, jvxfs_log_level_t level, const char* fmt, ...);

void jvxfs_console_vprint_detailed(const char* function, const char* file, uint32_t line, jvxfs_log_level_t level, const char* fmt, va_list args);

JVX_FS_LIB_END

#endif