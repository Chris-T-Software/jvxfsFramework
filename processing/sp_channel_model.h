/**
 * @file sp_channel_model.h
 * @brief Wrapper for Freeswitch telephone channel information.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-12-07
 */

#ifndef LIB_JVX_FS_FRAMEWORK_PROCESSING_CHANNEL_MODEL_H
#define LIB_JVX_FS_FRAMEWORK_PROCESSING_CHANNEL_MODEL_H

#include <stdint.h>
#include <switch.h>
#include "sp_defines.h"

JVX_FS_LIB_BEGIN

typedef void(*jvxfs_channel_observer_t)(jvxfs_channel_model_t*, void*);

typedef enum
{
    JVXFS_CHANNEL_IGNORING,
    JVXFS_CHANNEL_CATCHING,
    JVXFS_CHANNEL_REPLACING
} jvxfs_channel_fetching_t;

jvxfs_status_t jvxfs_channel_create_model(jvxfs_channel_model_t** mod, jvxfs_error_t* err, switch_memory_pool_t* pool);

jvxfs_sigproc_channel_t jvxfs_channel_which_link(jvxfs_channel_model_t* mod);

jvxfs_channel_fetching_t jvxfs_channel_how_fetched(jvxfs_channel_model_t* mod);

uint32_t jvxfs_channel_get_samplerate(jvxfs_channel_model_t* mod);
uint32_t jvxfs_channel_get_original_samplerate(jvxfs_channel_model_t* mod);
uint32_t jvxfs_channel_get_frame_size(jvxfs_channel_model_t* mod);
uint32_t jvxfs_channel_get_optimal_fft_size(jvxfs_channel_model_t* mod);
uint8_t jvxfs_channel_get_number_channels(jvxfs_channel_model_t* mod);
uint32_t jvxfs_channel_get_frame_duration_us(jvxfs_channel_model_t* mod);
jvxfs_sigproc_datatype_t jvxfs_channel_get_datatype(jvxfs_channel_model_t* mod);

jvxfs_status_t jvxfs_channel_add_observer(jvxfs_channel_model_t* mod, jvxfs_channel_observer_t func, void* data);
void jvxfs_channel_remove_observer(jvxfs_channel_model_t* mod, jvxfs_channel_observer_t func);

JVX_FS_LIB_END

#endif