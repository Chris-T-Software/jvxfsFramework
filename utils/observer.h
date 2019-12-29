/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
/**
 * @file observer.h
 * @brief C-written Observer module.
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

/**
 * @addtogroup utils Utilities
 * @{
 * @defgroup observer Observer Module
 * @details Threadsafe implementation of observer pattern in C.
 * @{
 */

/**
 * @brief Handle type of observer module.
 */
typedef void jvxfs_observer_handle_t;

/**
 * @brief Handle type of observable modules.
 */
typedef void jvxfs_observable_t;

/**
 * @brief Callback type for observing functions.
 * @param[in] hdl   Handle of observable module.
 * @param[in] data  User data provided to jvxfs_observer_add().
 * @note The user has to take care about threadsafety of the 
 * observable module and the provided user data.
 */
typedef void(*jvxfs_observer_t)(jvxfs_observable_t* hdl, void* data);

/**
 * @brief Create new observer module for an observable module.
 * @param[out] obs  Handle of observer module.
 * @param[in] obj   Handle of observable module.
 * @param[in] err   Observable module's error handler.
 * @param[in] pool  Observable module's memory pool.
 * @return Status code.
 */
jvxfs_status_t jvxfs_observer_create(jvxfs_observer_handle_t** obs, jvxfs_observable_t* obj, jvxfs_error_t* err, switch_memory_pool_t* pool);

/**
 * @brief Destroy existing observer module.
 * @param[in,out] obs   Handle of observer module. Will be set to @em NULL.
 * @return Status code.
 */
jvxfs_status_t jvxfs_observer_destroy(jvxfs_observer_handle_t** obs);

/**
 * @brief Add an observing function.
 * @param[in] obs   Handle of observer module.
 * @param[in] func  Observing function.
 * @param[in] data  User data for observing function.
 * @return Status code.
 * @details This function is threadsafe and can be called simultaneously to other functions of this module.
 * @pre The user has to take care about threadsafety of @a data.
 */
jvxfs_status_t jvxfs_observer_add(jvxfs_observer_handle_t* obs, jvxfs_observer_t func, void* data);

/**
 * @brief Remove one observing function.
 * @param[in] obs   Handle of observer module.
 * @param[in] func  Observing function. 
 * @return Status code.
 * @details This function is threadsafe and can be called simultaneously to other functions of this module.
 */
jvxfs_status_t jvxfs_observer_remove(jvxfs_observer_handle_t* obs, jvxfs_observer_t func);

/**
 * @brief Remove all observing functions.
 * @param[in] obs   Handle of observer module. 
 * @return Status code.
 * @details This function is threadsafe and can be called simultaneously to other functions of this module.
 */
jvxfs_status_t jvxfs_observer_delete_all(jvxfs_observer_handle_t* obs);

/**
 * @brief Count number of observing functions.
 * @param[in] obs   Handle of observer module. 
 * @return   Number of registered observing functions.
 * @details This function is threadsafe and can be called simultaneously to other functions of this module.
 */
size_t jvxfs_observer_count(jvxfs_observer_handle_t* obs);

/**
 * @brief Notify observing functions about changes in the observable module.
 * @param[in] obs   Handle of observer module. 
 * @return Status code.
 * @details This function is threadsafe and can be called simultaneously to other functions of this module.
 */
jvxfs_status_t jvxfs_observer_notify(jvxfs_observer_handle_t* obs);

/**
 * @}
 * @}
 */

JVX_FS_LIB_END

#endif