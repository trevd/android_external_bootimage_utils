/*
 * Copyright (C) 2015 Trevor Drake
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
 * file : lib/include/api/utils.h
 *
 */

#ifndef _d6fb49bc_9786_11e4_8458_5404a601fa9d
#define _d6fb49bc_9786_11e4_8458_5404a601fa9d
__LIBBOOTIMAGE_PUBLIC_API__ ssize_t utils_sanitize_string(char* s,ssize_t maxlen) ;
__LIBBOOTIMAGE_PUBLIC_API__ unsigned char *utils_memmem(unsigned char *haystack, unsigned haystack_len, char* needle, unsigned needle_len);
#endif
