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
 * file : lib/include/private/trace.h
 *
 */

#ifndef _95a5126a_81b8_11e4_bea0_5404a601fa9d
#define _95a5126a_81b8_11e4_bea0_5404a601fa9d

/* IMPORTANT: if you change the following list, don't
 * forget to update the corresponding 'tags' table in
 * the adb_trace_init() function implemented in adb.c
 */

typedef enum {
	TRACE_ALL = 0,
	TRACE_API_BOOTIMAGE,
	TRACE_API_BOOTIMAGE_EXTRACT,
	TRACE_API_BOOTIMAGE_FILE,
	TRACE_API_BOOTIMAGE_FILE_EXTRACT,
	TRACE_API_BOOTIMAGE_FILE_PRINT,
	TRACE_API_BOOTIMAGE_PRINT,
	TRACE_PRIVATE_ARCHIVE,
	TRACE_PRIVATE_BOOTIMAGE,
	TRACE_PRIVATE_CHECKS,
	TRACE_PRIVATE_KERNEL,
	TRACE_PRIVATE_TRACE,
	TRACE_PRIVATE_UTILS,
} bitrace;

extern __LIBBOOTIMAGE_PRIVATE_API__  int trace_mask;

__LIBBOOTIMAGE_PRIVATE_API__ void  trace_init(void);

#define TRACING  ((trace_mask & (1 << TRACE_TAG)) != 0)

#  define  D(...)                                      \
        do {                                           \
            if (TRACING) {                         	   \
                int save_errno = errno;                \
                fprintf(stderr, "%16s:%d ",			   \
                        __FUNCTION__,__LINE__);		   \
                errno = save_errno;                    \
                fprintf(stderr, __VA_ARGS__ );         \
                fflush(stderr);                        \
                errno = save_errno;                    \
           } \
        } while (0)
#endif
