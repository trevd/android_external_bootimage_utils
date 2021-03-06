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
 * file : lib/include/private/utils.h
 *
 */

#ifndef _b4cab7f2_80b0_11e4_a8e8_5404a601fa9d
#define _b4cab7f2_80b0_11e4_a8e8_5404a601fa9d
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
__LIBBOOTIMAGE_PRIVATE_API__  int utils_mkdir_and_parents_umask(char *path,unsigned mode, mode_t umask);
__LIBBOOTIMAGE_PRIVATE_API__  int utils_mkdir_and_parents(char *path,unsigned mode);
__LIBBOOTIMAGE_PRIVATE_API__ char* utils_dirname( char* s);
__LIBBOOTIMAGE_PRIVATE_API__ char* utils_basename( char* s);

__LIBBOOTIMAGE_PRIVATE_API__ ssize_t utils_write_all_fd (int fd, const void* buffer, ssize_t count);
__LIBBOOTIMAGE_PRIVATE_API__ ssize_t utils_write_all (char* file_name,mode_t mode, const void* buffer, ssize_t count);
__LIBBOOTIMAGE_PRIVATE_API__ int utils_read_all(char* file_name,char** buffer ,struct stat* st);


#endif
