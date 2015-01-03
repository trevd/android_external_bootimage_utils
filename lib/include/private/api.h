/*
 * Copyright (C) 2014 Trevor Drake
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * file : lib/include/private/api.h
 *
 */


#ifndef _3453ef82_7d16_11e4_ba53_5404a601fa9d
#define _3453ef82_7d16_11e4_ba53_5404a601fa9d

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <biutils.h>

#ifndef __LIBBOOTIMAGE_PRIVATE_API__
#define __LIBBOOTIMAGE_PRIVATE_API__ __attribute__((visibility("hidden")))
#endif

#include <private/kernel.h>
#include <private/archive.h>
#include <private/bootimage.h>
#include <private/bootimage_utils.h>
#include <private/checks.h>
#include <private/errors.h>
#include <private/print.h>
#include <private/trace.h>
#include <private/utils.h>
#include <private/factory_images.h>

#endif
