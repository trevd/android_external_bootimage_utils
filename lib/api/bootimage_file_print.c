/*
 * Copyright (C) 2014 Trevor Drake
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
{
	return 0;
}
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
 * file : lib/api/bootimage_file_print.c
 *
 */
 #define  TRACE_TAG   TRACE_API_BOOTIMAGE_FILE_PRINT
#include <stdio.h>
#include <errno.h>
#include <api/bootimage.h>
#include <private/api.h>


__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_header(const char* file_name)
{


	struct bootimage* bi = bootimage_initialize();
	D("file_name:%s",file_name);
	if( bootimage_file_read(bi,file_name) == -1 ){
		int ie = errno ;
		if ( bootimage_free(&bi) == -1 ){
			return -1 ;
		}
		errno = ie ;
		return -1 ;
	}

	bootimage_structure_print_header(bi);


	if ( bootimage_free(&bi) == -1 ){
			return -1 ;
	}
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_header_fd(const char* file_name,int fd)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_kernel(const char* file_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_kernel_version(const char* file_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_ramdisk(const char* file_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_ramdisk_list(const char* file_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_ramdisk_file(const char* file_name,const char* ramdisk_file)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_ramdisk_files(const char* file_name,const char** ramdisk_files)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_print_all(const char* file_name)
{
	return 0;
}

