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
 * file : lib/include/private/archive.h
 *
 */

#ifndef _a7808ef4_8179_11e4_991e_5404a601fa9d
#define _a7808ef4_8179_11e4_991e_5404a601fa9d
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all_memory_directory( char* archive_data , uint64_t archive_size, char* output_dir_name);
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all(struct archive *a,char* output_dir_name);
__LIBBOOTIMAGE_PRIVATE_API__  char* archive_extract_entry(char* data,off_t data_size,char* name,size_t name_length,size_t* entry_size);
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_file(char* archive_data,off_t archive_data_size,char* name,size_t name_length);
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_memory_file( char* archive_data , uint64_t archive_size, char* entry_name, char* output_file_name);
__LIBBOOTIMAGE_PRIVATE_API__  unsigned int archive_gzip_get_uncompressed_size(char* data,off_t data_size);
#endif
