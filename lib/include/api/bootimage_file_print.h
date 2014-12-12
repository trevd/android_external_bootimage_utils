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
 * file : lib/include/api/bootimage_file_print.h
 *
 */

#ifndef _b15dfc64_8196_11e4_9db6_5404a601fa9d
#define _b15dfc64_8196_11e4_9db6_5404a601fa9d

__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_header(const char* file_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_header_fd(const char* file_name,int fd);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_kernel(const char* file_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_kernel_version(const char* file_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_ramdisk(const char* file_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_ramdisk_list(const char* file_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_ramdisk_file(const char* file_name,const char* ramdisk_file);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_ramdisk_files(const char* file_name,const char** ramdisk_files);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_all(const char* file_name);

#endif
