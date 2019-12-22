/**
 * @file view_private.h
 * @brief Private internal view data structure.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-30
 * @warning Do not use types declared in this file. No future compatibility is granted.
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_VIEW_PRIVATE_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_VIEW_PRIVATE_H

#include "view.h"

JVX_FS_LIB_BEGIN

typedef struct
{
    jvxfs_view_input_t origin;
    jvxfs_view_multi_output_t dest;
    jvxfs_error_t* err;
    jvxfs_directive_data_t* data;
    switch_stream_handle_t* console;
} view_priv_t;

JVX_FS_LIB_END

#endif