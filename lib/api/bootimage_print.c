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
 * file : lib/api/bootimage_print.c
 *
 */
#define  TRACE_TAG   TRACE_API_BOOTIMAGE_PRINT
#include <api/bootimage.h>
#include <private/api.h>
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_header(struct bootimage* bi)
{
	D("bi=%p",bi);
	if ( check_bootimage_structure(bi) == -1 ){
		return -1 ;
	}
	bootimage_structure_print_header(bi);
	return 0 ;

}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_header_fd(struct bootimage* bi,int fd)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_kernel(struct bootimage* bi)
{
	D("bi=%p",bi);
	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( check_bootimage_kernel(bi) == -1 ){
		return -1;
	}
	if ( bootimage_kernel_decompress(bi) == -1 ){
		return -1;
	}


	//bootimage_structure_print_kernel(bi);
	return 0 ;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_kernel_version(struct bootimage* bi)
{
	D("bi=%p",bi);
	if ( check_bootimage_structure(bi) == -1 ){
		return -1 ;
	}
	bootimage_structure_print_header(bi);
	return 0 ;

}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_ramdisk(struct bootimage* bi)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_ramdisk_list(struct bootimage* bi)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_ramdisk_file(struct bootimage* bi,const char* ramdisk_file)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_ramdisk_files(struct bootimage* bi,const char** ramdisk_files)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_print_all(struct bootimage* bi)
{
	return 0;
}
