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
 * file : lib/include/api/bootimage_print.h
 *
 */

#ifndef _83a76a1c_8196_11e4_8539_5404a601fa9d
#define _83a76a1c_8196_11e4_8539_5404a601fa9d
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_header(const struct bootimage bi);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_header_fd(const struct bootimage bi,int fd);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_kernel(const struct bootimage bi);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_kernel_version(const struct bootimage bi);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_ramdisk(const struct bootimage bi);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_ramdisk_list(const struct bootimage bi);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_ramdisk_file(const struct bootimage bi,const char* ramdisk_file);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_ramdisk_files(const struct bootimage bi,const char** ramdisk_files);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_all(const struct bootimage bi);
#endif
