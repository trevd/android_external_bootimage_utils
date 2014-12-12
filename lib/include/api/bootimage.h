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
 * file : lib/include/api/bootimage.h
 *
 */

#ifndef _40589ef6_7d28_11e4_9dc9_5404a601fa9d
#define _40589ef6_7d28_11e4_9dc9_5404a601fa9d

struct bootimage ;

#define __LIBBOOTIMAGE_PUBLIC_API__  __attribute__((visibility("default")))

__LIBBOOTIMAGE_PUBLIC_API__ struct bootimage* bootimage_initialize();
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_free(struct bootimage** bip);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_read(struct bootimage* bi,const char* file_name);
#endif
