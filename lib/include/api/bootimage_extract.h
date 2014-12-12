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
 * file : lib/include/api/bootimage_extract.h
 *
 */

#ifndef _2a65bc00_7d28_11e4_8509_5404a601fa9d
#define _2a65bc00_7d28_11e4_8509_5404a601fa9d
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_header(struct bootimage* bi,const char* header_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_header_block(struct bootimage* bi,const char* header_block_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel(struct bootimage* bi,const char* kernel_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_config(struct bootimage* bi,const char* kernel_config_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_config_gz(struct bootimage* bi,const char* kernel_config_gz_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_ramdisk(struct bootimage* bi,const char* kernel_ramdisk_dir_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_ramdisk_archive(struct bootimage* bi,const char* kernel_ramdisk_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_ramdisk_entry(struct bootimage* bi,const char* kernel_ramdisk_entry_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_uncompressed_kernel(struct bootimage* bi,const char* uncompressed_kernel_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_ramdisk(struct bootimage* bi,const char* ramdisk_dir_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_ramdisk_archive(struct bootimage* bi,const char* ramdisk_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_ramdisk_entry(struct bootimage* bi,const char* ramdisk_entry_name);
#endif
