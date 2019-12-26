/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
#include <stdlib.h>
#include "../system/error.h"
#include "observer.h"

typedef struct
{
    jvxfs_observer_t func;
    void* data;
} callback_t;

typedef struct
{
    jvxfs_error_t* err;
    size_t size;
    size_t used;
    callback_t* observer;
    jvxfs_observerable_t* observerable;
    switch_thread_rwlock_t* lock;
} obs_t;

jvxfs_status_t jvxfs_observer_create(jvxfs_observer_handle_t** obs, jvxfs_observerable_t* obj, jvxfs_error_t* err, switch_memory_pool_t* pool)
{
    *obs = NULL;
    obs_t* hdl = (obs_t*)switch_core_alloc(pool, sizeof(obs_t));
    if (!hdl) {
        return jvxfs_error_set_error(err, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_OBSERVER,
            "Could not create observerable object.");
    }
    switch_status_t res = switch_thread_rwlock_create(&hdl->lock, pool);
    if (res != SWITCH_STATUS_SUCCESS) {
        return jvxfs_error_set_error(err, JVXFS_STATUS_RESOURCE_EXCEPTION, JVXFS_LOG_CRITICAL, JVXFS_COMP_OBSERVER,
            "Could not create rwlock.");
    }
    hdl->err = err;
    hdl->used = 0;
    hdl->observerable = obj;
    hdl->observer = NULL;
    hdl->size = 0;
    *obs = hdl;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_observer_destroy(jvxfs_observer_handle_t** obs)
{
    obs_t* hdl = (obs_t*)*obs;
    if (hdl->observer) free(hdl->observer);
    hdl->size = 0;
    hdl->used = 0;
    switch_thread_rwlock_destroy(hdl->lock);
    *obs = NULL;
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_observer_add(jvxfs_observer_handle_t* obs, jvxfs_observer_t func, void* data)
{
    obs_t* hdl = (obs_t*)obs;
    switch_status_t res = switch_thread_rwlock_wrlock(hdl->lock);
    if (res != SWITCH_STATUS_SUCCESS) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_RESOURCE_EXCEPTION, JVXFS_LOG_CRITICAL, JVXFS_COMP_OBSERVER,
            "Could not activate rwlock.");
    }
    if (hdl->used == hdl->size) {
        void* new = realloc(hdl->observer, sizeof(callback_t) * (hdl->size + 2));
        if (!new) {
            return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_ALLOCATION_FAILED, JVXFS_LOG_CRITICAL, JVXFS_COMP_OBSERVER,
                "Could not add observer.");
        }
        hdl->observer = new;
        hdl->size = hdl->size + 2;
    }
    hdl->observer[hdl->used].func = func;
    hdl->observer[hdl->used].data = data;
    ++(hdl->used);
    switch_thread_rwlock_unlock(hdl->lock);
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_observer_remove(jvxfs_observer_handle_t* obs, jvxfs_observer_t func)
{
    obs_t* hdl = (obs_t*)obs;
    bool wasFound = false;
    size_t i = 0;
    switch_status_t res = switch_thread_rwlock_wrlock(hdl->lock);
    if (res != SWITCH_STATUS_SUCCESS) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_RESOURCE_EXCEPTION, JVXFS_LOG_CRITICAL, JVXFS_COMP_OBSERVER,
            "Could not activate rwlock.");
    }
    for (i = 0; i < hdl->used; ++i) {
        if (hdl->observer[i].func == func) {
            wasFound = true;
        } else if (wasFound) {
            hdl->observer[i-1] = hdl->observer[i];
        }
    }
    if (wasFound) {
        hdl->observer[i-1].func = NULL;
        hdl->observer[i-1].data = NULL;
        --(hdl->used);
    }
    switch_thread_rwlock_unlock(hdl->lock);
    return JVXFS_STATUS_SUCCESS;
}

jvxfs_status_t jvxfs_observer_delete_all(jvxfs_observer_handle_t* obs)
{
    obs_t* hdl = (obs_t*)obs;
    switch_status_t res = switch_thread_rwlock_wrlock(hdl->lock);
    if (res != SWITCH_STATUS_SUCCESS) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_RESOURCE_EXCEPTION, JVXFS_LOG_CRITICAL, JVXFS_COMP_OBSERVER,
            "Could not activate rwlock.");
    }
    for (size_t i = 0; i < hdl->used; ++i) {
        hdl->observer[i].func = NULL;
        hdl->observer[i].data = NULL;
    }
    hdl->used = 0;
    switch_thread_rwlock_unlock(hdl->lock);
    return JVXFS_STATUS_SUCCESS;
}

size_t jvxfs_observer_count(jvxfs_observer_handle_t* obs)
{
    obs_t* hdl = (obs_t*)obs;
    switch_status_t res = switch_thread_rwlock_rdlock(hdl->lock);
    if (res != SWITCH_STATUS_SUCCESS) {
        jvxfs_error_set_error(hdl->err, JVXFS_STATUS_RESOURCE_EXCEPTION, JVXFS_LOG_CRITICAL, JVXFS_COMP_OBSERVER,
            "Could not activate rwlock.");
        return 0;
    }
    size_t tmp = hdl->used;
    switch_thread_rwlock_unlock(hdl->lock);
    return tmp;
}

jvxfs_status_t jvxfs_observer_notify(jvxfs_observer_handle_t* obs)
{
    obs_t* hdl = (obs_t*)obs;
    switch_status_t res = switch_thread_rwlock_rdlock(hdl->lock);
    if (res != SWITCH_STATUS_SUCCESS) {
        return jvxfs_error_set_error(hdl->err, JVXFS_STATUS_RESOURCE_EXCEPTION, JVXFS_LOG_CRITICAL, JVXFS_COMP_OBSERVER,
            "Could not activate rwlock.");
    }
    for (size_t i = 0; i < hdl->used; ++i) {
        hdl->observer[i].func(hdl->observerable, hdl->observer[i].data);
    }
    switch_thread_rwlock_unlock(hdl->lock);
    return JVXFS_STATUS_SUCCESS;
}