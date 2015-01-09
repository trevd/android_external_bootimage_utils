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
 * file : lib/include/api/bootimage_file_extract.h
 *
 */

#ifndef _31556c36_7d28_11e4_9d27_5404a601fa9d
#define _31556c36_7d28_11e4_9d27_5404a601fa9d




__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_header(char* file_name, char* header_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_header_block(char* file_name, char* header_block_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_kernel(char* file_name, char* kernel_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_kernel_config(char* file_name, char* kernel_config_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_kernel_config_gz(char* file_name, char* kernel_config_gz_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_kernel_ramdisk(char* file_name, char* kernel_ramdisk_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_kernel_ramdisk_archive(char* file_name, char* kernel_ramdisk_dir_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_kernel_ramdisk_entry(char* file_name, char* kernel_ramdisk_entry_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_uncompressed_kernel(char* file_name, char* uncompressed_kernel_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_ramdisk(char* file_name, char* ramdisk_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_ramdisk_archive(char* file_name, char* ramdisk_dir_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_ramdisk_entry(char* file_name, char* ramdisk_entry_name);

#endif
