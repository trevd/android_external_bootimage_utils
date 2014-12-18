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
 * file : lib/api/bootimage_extract.c
 *
 */
#define TRACE_TAG TRACE_API_BOOTIMAGE_EXTRACT
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>

#include <api/bootimage.h>
#include <api/bootimage_extract.h>


#include <private/api.h>

__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_header(struct bootimage* bi,const char* header_name)
{
	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( header_name == NULL ) {
		header_name = DEFAULT_NAME_HEADER;
	}

	if ( check_output_name ( header_name ) == -1 ) {
		return -1 ;
	}
	D("bi %u\n",bi->header_size);

	FILE* fi = fopen(header_name,"w+b");
	if ( fi == NULL ){
		return -1 ;
	}


	fwrite(bi->header,bi->header_size,1,fi);
	fclose(fi);
	return 0;

}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_header_block(struct bootimage* bi,const char* header_block_name)
{
	D("bi=%p",bi);
	if ( check_bootimage_structure(bi) == -1 ){
		D("check_bootimage_structure failed");
		return -1;
	}

	if ( header_block_name == NULL ) {
		header_block_name = DEFAULT_NAME_HEADER_BLOCK;
	}
	D("header_block_name:%s bi->header_size:%u",header_block_name,bi->header_size);
	if ( check_output_name ( header_block_name ) == -1 ) {
		D("bi=%p",bi);
		return -1 ;
	}

	D("bi %u\n",bi->header_size);

	FILE* fi = fopen("test","w+b");
	if ( fi == NULL ){
		return -1 ;
	}

	fwrite(bi->header,bi->header_size,1,fi);
	fclose(fi);
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel(struct bootimage* bi,const char* kernel_name)
{
	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( kernel_name == NULL ) {
		kernel_name = DEFAULT_NAME_KERNEL;
	}

	if ( check_output_name ( kernel_name ) == -1 ) {
		return -1 ;
	}
	D("bi %ld\n",bi->header->kernel_size);

	FILE* fi = fopen(kernel_name,"w+b");
	if ( fi == NULL ){
		return -1 ;
	}

	fwrite(bi->kernel,bi->header->kernel_size,1,fi);
	fclose(fi);
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_config(struct bootimage* bi,const char* kernel_config_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_config_gz(struct bootimage* bi,const char* kernel_config_gz_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_ramdisk(struct bootimage* bi,const char* kernel_ramdisk_dir_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_ramdisk_archive(struct bootimage* bi,const char* kernel_ramdisk_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_kernel_ramdisk_entry(struct bootimage* bi,const char* kernel_ramdisk_entry_name)
{
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_uncompressed_kernel(struct bootimage* bi,const char* uncompressed_kernel_name)
{
	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( check_bootimage_kernel(bi) == -1 ){
		return -1;
	}
	if ( bootimage_kernel_decompress(bi) == -1 ){
		return -1;
	}

	if ( uncompressed_kernel_name == NULL ) {
		uncompressed_kernel_name = DEFAULT_NAME_KERNEL_UNCOMPRESSED;
	}
	D("uncompressed_kernel_name:%s",uncompressed_kernel_name);
	if ( check_output_name ( uncompressed_kernel_name ) == -1 ) {
		D("check_output_name failed");
		return -1 ;
	}

	FILE* fi = fopen(uncompressed_kernel_name,"w+b");
	if ( fi == NULL ){
		D("fopen failed");
		return -1 ;
	}
	D("uncompressed_kernel:%p size=%u",bi->uncompressed_kernel ,bi->uncompressed_kernel_size);
	fwrite(bi->uncompressed_kernel ,bi->uncompressed_kernel_size,1,fi);
	fclose(fi);

	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_ramdisk(struct bootimage* bi,const char* ramdisk_dir_name)
{
	D("bi=%p",bi);

	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( check_bootimage_ramdisk(bi) == -1 ){
		return -1;
	}

	if ( ramdisk_dir_name == NULL ) {
		ramdisk_dir_name = DEFAULT_NAME_RAMDISK_DIRECTORY;
	}

	int ramdisk_dir_name_length = check_output_name ( ramdisk_dir_name );
	if ( ramdisk_dir_name_length == -1 ) {
		return -1 ;
	}
	if ( ( ramdisk_dir_name[ramdisk_dir_name_length-1] == '/' ) || ( ramdisk_dir_name[ramdisk_dir_name_length-1] == '\\' ) ){
		ramdisk_dir_name_length -= 1 ;
	}

	DIR* output_dir =  mkdir_and_parents_umask ( ramdisk_dir_name, 0755 , 0);
	if ( output_dir == NULL ) {
		return -1 ;
	}


	if ( archive_extract_all_memory_directory(bi->ramdisk , bi->header->ramdisk_size,ramdisk_dir_name) == -1 ){
		int ie = errno ;
		if ( closedir(output_dir) == -1 ){
			return -1 ;
		}
		errno = ie ;
		return -1 ;

	}
	if ( closedir(output_dir) == -1 ){
		return -1 ;
	}
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_ramdisk_archive(struct bootimage* bi,const char* ramdisk_name)
{
	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( ramdisk_name == NULL ) {
		ramdisk_name = DEFAULT_NAME_RAMDISK_ARCHIVE;
	}
	D("ramdisk_name=%s\n",ramdisk_name);
	if ( check_output_name ( ramdisk_name ) == -1 ) {
		return -1 ;
	}
	D("ramdisk_size=%u\n",bi->header->ramdisk_size);


	FILE* fi = fopen(ramdisk_name,"w+b");
	if ( fi == NULL ){
		return -1 ;
	}

	fwrite(bi->ramdisk,bi->header->ramdisk_size,1,fi);
	fclose(fi);
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_ramdisk_entry(struct bootimage* bi,const char* ramdisk_entry_name,const char* output_file)
{
	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( check_bootimage_ramdisk(bi) == -1 ){
		return -1;
	}

	if ( check_output_name ( output_file ) == -1 ) {
		return -1 ;
	}


	int ret = archive_extract_memory_file(bi->ramdisk, bi->header->ramdisk_size,ramdisk_entry_name,output_file);

	return ret;
}
