/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
/**
 * @file observer.h
 * @brief Threadsafe implementation of observer pattern in C.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-12-07
 * @copyright Copyright (c) 2019
 */

#ifndef LIB_JVX_FS_FRAMEWORK_UTILS_OBSERVER_H
#define LIB_JVX_FS_FRAMEWORK_UTILS_OBSERVER_H

#include <stddef.h>
#include <switch.h>
#include "../system/defines.h"

JVX_FS_LIB_BEGIN

typedef void jvxfs_observer_handle_t;
typedef void jvxfs_observerable_t;

typedef void(*jvxfs_observer_t)(jvxfs_observerable_t*, void*);

jvxfs_status_t jvxfs_observer_create(jvxfs_observer_handle_t** obs, jvxfs_observerable_t* obj, jvxfs_error_t* err, switch_memory_pool_t* pool);
jvxfs_status_t jvxfs_observer_destroy(jvxfs_observer_handle_t** obs);

jvxfs_status_t jvxfs_observer_add(jvxfs_observer_handle_t* obs, jvxfs_observer_t func, void* data);
jvxfs_status_t jvxfs_observer_remove(jvxfs_observer_handle_t* obs, jvxfs_observer_t func);
jvxfs_status_t jvxfs_observer_delete_all(jvxfs_observer_handle_t* obs);
size_t jvxfs_observer_count(jvxfs_observer_handle_t* obs);
jvxfs_status_t jvxfs_observer_notify(jvxfs_observer_handle_t* obs);

JVX_FS_LIB_END

#endif