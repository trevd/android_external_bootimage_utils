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
 * file : lib/include/private/print.h
 * This file contains the strings used by the bootimage*_print functions
 */

#ifndef _ed8c3286_8197_11e4_a167_5404a601fa9d
#define _ed8c3286_8197_11e4_a167_5404a601fa9d

#define BOOTIMAGE_PRINT_HEADER "\
 Header: \n\
  kernel size        : %u\n\
  kernel addr        : 0x%8x\n\
  ramdisk size       : %u\n\
  ramdisk addr       : 0x%8x\n\
  second size        : %u\n\
  second addr        : 0x%8x\n\
  tags addr          : 0x%8x\n\
  page size          : %u\n\
  dt size            : %u\n\
  name               : %s\n\
  cmd line           : %s\n\
  extra cmd line     : %s\n\
"

#define BOOTIMAGE_PRINT_EXTRACT_HEADER "\
kernel_size=%u\n\
kernel_addr=0x%8x\n\
ramdisk_size=%u\n\
ramdisk_addr=0x%8x\n\
second_size=%u\n\
second_addr=0x%8x\n\
tags_addr=0x%8x\n\
page_size=%u\n\
dt_size=%u\n\
name=%s\n\
cmd_line=%s\n\
extra_cmd_line=%s\n\
"

#define BOOTIMAGE_PRINT_COMPRESSION_TYPE "\
  compression type   : %s\n\
"
/* #define BOOTIMAGE_PRINT_KERNEL_HEADER " */

__LIBBOOTIMAGE_PRIVATE_API__  int bootimage_structure_print_header(struct bootimage* bi);
#endif
