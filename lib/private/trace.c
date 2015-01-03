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
 * file : lib/private/trace.c
 *
 */
#define TRACE_TAG TRACE_PRIVATE_TRACE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <private/api.h>

__LIBBOOTIMAGE_PRIVATE_API__ int   trace_mask;

/*
 * Shamelessly borrowed and adapted from Android adb
 * read a comma/space/colum/semi-column separated list of tags
 * from the TRACE environment variable and build the trace
 * mask from it. note that '1' and 'all' are special cases to
 * enable all tracing
 */
__LIBBOOTIMAGE_PRIVATE_API__ void  trace_init(void)
{
    if (TRACING) {
	D("TRACING ALREADY INITIALIZED");
	return ;
    }
    const char*  p = getenv("BITRACE");
    const char*  q;

    static const struct {
        const char*  tag;
        int           flag;
    } tags[] = {
		{ "all", TRACE_ALL },
		{ "1", TRACE_ALL },
		{ "utils", TRACE_API_BOOTIMAGE_UTILS },
		{ "api", TRACE_API_BOOTIMAGE },
		{ "extract", TRACE_API_BOOTIMAGE_EXTRACT },
		{ "file", TRACE_API_BOOTIMAGE_FILE },
		{ "fextract", TRACE_API_BOOTIMAGE_FILE_EXTRACT },
		{ "fprint", TRACE_API_BOOTIMAGE_FILE_PRINT },
		{ "print", TRACE_API_BOOTIMAGE_PRINT },
		{ "archive", TRACE_PRIVATE_ARCHIVE },
		{ "bi", TRACE_PRIVATE_BOOTIMAGE },
		{ "checks", TRACE_PRIVATE_CHECKS },
		{ "kernel", TRACE_PRIVATE_KERNEL },
		{ "trace", TRACE_PRIVATE_TRACE },
		{ "putils", TRACE_PRIVATE_UTILS },
		{ "pprint", TRACE_PRIVATE_PRINT },
        { NULL, 0 }
    };

    if (p == NULL)
            return;

    /* use a comma/column/semi-colum/space separated list */
    while (*p) {
        int  len, tagn;

        q = strpbrk(p, " ,:;");
        if (q == NULL) {
            q = p + strlen(p);
        }
        len = q - p;

        for (tagn = 0; tags[tagn].tag != NULL; tagn++)
        {
            int  taglen = strlen(tags[tagn].tag);

            if (len == taglen && !memcmp(tags[tagn].tag, p, len) )
            {
                int  flag = tags[tagn].flag;
                if (flag == 0) {

                    trace_mask = ~0;

                    return;
                }
                trace_mask |= (1 << flag);
                break;
            }
        }
        p = q;
        if (*p)
            p++;
    }

}
