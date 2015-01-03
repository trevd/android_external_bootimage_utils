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
 * file : lib/include/api/bootimage_utils.h
 *
 */

#ifndef _ceeda38e_928e_11e4_a689_5404a601fa9d
#define _ceeda38e_928e_11e4_a689_5404a601fa9d


/* 	Opaque bootimage structure declaration. The full definition can be
	found in lib/include/private/bootimage_utils.h */
struct bootimage_utils ;

__LIBBOOTIMAGE_PUBLIC_API__ struct bootimage_utils* bootimage_utils_initialize();
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_utils_file_read(struct bootimage_utils* biu,const char* file_name);
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_utils_free(struct bootimage_utils** biup) ;


#endif
