/**
 * @file sp_defines.h
 * @brief Defintions of datatypes for signal processing.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-11-21
 */

#ifndef LIB_JVX_FS_FRAMEWORK_PROCESSING_DEFINES_H
#define LIB_JVX_FS_FRAMEWORK_PROCESSING_DEFINES_H

#include "../system/defines.h"

JVX_FS_LIB_BEGIN

typedef void jvxfs_channel_model_t;

typedef enum
{
    JVXFS_SP_NONE,
    JVXFS_SP_DATA,
    JVXFS_SP_16BIT_LE,
    JVXFS_SP_32BIT_LE,
    JVXFS_SP_64BIT_LE,
    JVXFS_SP_8BIT,
    JVXFS_SP_U16BIT_LE,
    JVXFS_SP_U32BIT_LE,
    JVXFS_SP_U64BIT_LE,
    JVXFS_SP_U8BIT
} jvxfs_sigproc_datatype_t;

typedef enum
{
    JVXFS_SP_NO_LINK,
    JVXFS_SP_DOWNLINK,
    JVXFS_SP_UPLINK
} jvxfs_sigproc_channel_t;

typedef enum
{
    JVXFS_SP_DEFAULT = 0,
    JVXFS_SP_CATCH_BOTH_LINKS = 1
} jvxfs_sigproc_working_flag_t;

typedef enum
{
    JVXFS_SP_BUFFER_NO_LINK,
    JVXFS_SP_BUFFER_DOWNLINK,
    JVXFS_SP_BUFFER_UPLINK,
    JVXFS_SP_BUFFER_BOTH_LINKS
} jvxfs_sigproc_buffer_t;

typedef enum
{
    JVXFS_SP_ALGO_OFF,
    JVXFS_SP_ALGO_ON,
    JVXFS_SP_ALGO_MUTE
} jvxfs_sigproc_algo_mode_t;

JVX_FS_LIB_END

#endif