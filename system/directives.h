/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
/**
 * @file directives.h
 * @brief Default directives for apps provided by the library.
 * @author Christian Thierfeld
 * @version 1.0
 * @date 2019-12-01
 * @copyright Copyright (c) 2019
 */

#ifndef LIB_JVX_FS_FRAMEWORK_SYSTEM_DIRECTIVES_H
#define LIB_JVX_FS_FRAMEWORK_SYSTEM_DIRECTIVES_H

#include "defines.h"

JVX_FS_LIB_BEGIN

void jvxfs_directive_app_version(jvxfs_view_t* view, jvxfs_directive_data_t* rqst, void* data);

JVX_FS_LIB_END

#endif