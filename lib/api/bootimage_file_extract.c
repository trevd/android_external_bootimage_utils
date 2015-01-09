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
 * file : lib/api/bootimage_file_extract.c
 *
 */

#include <private/api.h>
#define TRACE_TAG TRACE_API_BOOTIMAGE_FILE_EXTRACT
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_extract_kernel(char* file_name,char* kernel_name)
{
	struct bootimage* bi = bootimage_initialize();
	D("file_name=%s",file_name);
	if ( bootimage_file_read(bi,file_name) == -1 ){
		E("bootimage_file_read");
		if ( bootimage_free(&bi) == -1 ){

			return -1 ;
		}
		return -1 ;
	}
	if ( bootimage_extract_kernel(bi,kernel_name) == -1 ) {
		E("bootimage_extract_kernel");
		if ( bootimage_free(&bi) == -1 ){
			return -1 ;
		}
		return -1 ;
	}
	if ( bootimage_free(&bi) == -1 ){
		return -1 ;
	}

	return 0;

}
