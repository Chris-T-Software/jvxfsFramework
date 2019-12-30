/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
/**
 * @file view.h
 * @brief View module for input and output handling for directives.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-30
 * @copyright Copyright (c) 2019
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_VIEW_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_VIEW_H

#include <stdarg.h>
#include "defines.h"

JVX_FS_LIB_BEGIN

typedef enum
{
    JVXFS_VIEW_IN_CONSOLE,
    JVXFS_VIEW_IN_DTMF,
    JVXFS_VIEW_IN_EVENT,
    JVXFS_VIEW_IN_CALL
} jvxfs_view_input_t;

typedef enum
{
    JVXFS_VIEW_OUT_NONE = 0,
    JVXFS_VIEW_OUT_LOG = (1 << 0),
    JVXFS_VIEW_OUT_CONSOLE = (1 << 1),
    JVXFS_VIEW_OUT_EVENT = (1 << 2)
} jvxfs_view_output_t;

typedef uint32_t jvxfs_view_multi_output_t;

jvxfs_view_input_t jvxfs_view_get_origin(jvxfs_view_t* view);
jvxfs_view_multi_output_t jvxfs_view_get_destinations(jvxfs_view_t* view);
void jvxfs_view_set_destinations(jvxfs_view_t* view, jvxfs_view_multi_output_t dest);
void jvxfs_view_set_all_destinations(jvxfs_view_t* view);

jvxfs_status_t jvxfs_view_write_human_readable(jvxfs_view_t* view, const char* fmt, ...);
jvxfs_status_t jvxfs_view_vwrite_human_readable(jvxfs_view_t* view, const char* fmt, va_list args);

jvxfs_status_t jvxfs_view_write_machine_readable(jvxfs_view_t* view, const char* fmt, ...);
jvxfs_status_t jvxfs_view_vwrite_machine_readable(jvxfs_view_t* view, const char* fmt, va_list args);

jvxfs_status_t jvxfs_view_write_to_all(jvxfs_view_t* view, const char* fmt, ...);
jvxfs_status_t jvxfs_view_vwrite_to_all(jvxfs_view_t* view, const char* fmt, va_list args);

JVX_FS_LIB_END

#endif