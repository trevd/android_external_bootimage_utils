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
 * file : lib/private/print.c
 *
 */

#define  TRACE_TAG   TRACE_PRIVATE_PRINT
#include <private/api.h>
#include <stdio.h>
__LIBBOOTIMAGE_PRIVATE_API__  int bootimage_structure_print_header(struct bootimage* bi)
{

	fprintf(stdout,"\n"BOOTIMAGE_PRINT_HEADER"\n",	bi->header->kernel_size,
											bi->header->kernel_addr,
											bi->header->ramdisk_size,
											bi->header->ramdisk_addr,
											bi->header->second_size,
											bi->header->second_addr,
											bi->header->tags_addr,
											bi->header->page_size,
											bi->header->dt_size,
											bi->header->name,
											bi->header->cmdline,
											bi->header->extra_cmdline);
	return 0;
}
