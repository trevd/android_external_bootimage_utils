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
 * file : lib/private/checks.c
 *
 */
#define  TRACE_TAG   TRACE_PRIVATE_CHECKS
#include <errno.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <archive.h>
#include <api/bootimage.h>
#include <api/errors.h>
#include <private/api.h>

__LIBBOOTIMAGE_PRIVATE_API__ int check_archive_read_memory(struct archive **ap,char* data , uint64_t data_size)
{
	if ( check_archive_read_initialization(ap) == -1 ){
		return -1;
	}
	int r = archive_read_open_memory(ap[0], data,data_size);
	if (r != ARCHIVE_OK){
		D("r=%d",r);
		return -1;
	}
	D("ap[0]=%p",ap[0]);
	return 0;
}

__LIBBOOTIMAGE_PRIVATE_API__ int check_archive_read_initialization(struct archive **ap)
{
	struct archive* a = archive_read_new();
    if( a == NULL ){
        /* Failed to initialize libarchive reading */
        errno = EBIARCHIVEREAD;
        return -1 ;
    }
	/*  */
	if ( archive_read_support_filter_all(a) == ARCHIVE_WARN ){
		errno = EBIARCHIVEREADFILTER  ;
        return -1 ;
    }

    if ( archive_read_support_format_all(a) == ARCHIVE_FATAL ){
		errno = EBIARCHIVEREADFORMAT ;
        return -1 ;
    }
    if ( archive_read_support_format_raw(a) == ARCHIVE_FATAL ){
		errno = EBIARCHIVEREADFORMATRAW  ;
        return -1 ;
    }
    errno = EBIOK;
    ap[0]=a;
    return 0;


}
__LIBBOOTIMAGE_PRIVATE_API__ int check_output_name(const char* name)
{
	if ( name == NULL ){
		errno = EBIOUTNAME ;
		return -1;
	}
	int size = strnlen(name,PATH_MAX);
	if ( size > PATH_MAX-1){
		errno = EBIOUTNAMELEN ;
		return -1;
	}
	errno = EBIOK;
	return size ;


}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_structure(struct bootimage* bi)
{
	/* Validate inputs */
	if ( bi == NULL ) {
		errno = EBINULL;
		D("bi=%p errno=%d [ EBINULL ]",bi,errno);
		return -1;
	}
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_file_stat_size(struct bootimage* bi ,const char* file_name)
{
	/* Stat the boot image file an store it in the structure */
	fprintf(stdout,"check_bootimage_file_stat_size bi->stat.st_size [ %u ]",  bi->stat.st_size);
    if (stat(file_name, &bi->stat) == -1){
		fprintf(stdout,"check_bootimage_file_stat_size bi->stat.st_size [ %u ]",  bi->stat.st_size);
		errno = EBISTAT;
		return -1 ;
	}
	/* File size is zero or could not be determined  or
	   File size is less than a minimum know page size
	   This is probably not good.  */

	if (  ( bi->stat.st_size <= 0 ) || ( bi->stat.st_size < PAGE_SIZE_MIN ) ) {
		errno = EBIFSIZE;
		return -1 ;
	}
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_file_name(const char* file_name)
{
	/* Check the file_name is valid */
	if ( file_name == NULL ) {
		errno = EBIFNAME;
		return -1 ;
	}

	/* Does the file exist? , can we read it? */
	if ( access(file_name , R_OK ) == -1 ) {
		errno = EBIFACCESS ;
		return -1 ;
	}
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int check_out_file(const char* file_name)
{
	/* Check the file_name is valid */
	if ( file_name == NULL ) {
		errno = EBIFNAME;
		return -1 ;
	}

	/* Does the file exist? , can we read it? */
	if ( access(file_name , R_OK ) == -1 ) {
		errno = EBIFACCESS ;
		return -1 ;
	}
	errno = EBIOK;
	return 0;
}


__LIBBOOTIMAGE_PRIVATE_API__ int check_ramdisk_entryname(const char* entry_name)
{
	if ( entry_name == NULL ){
		errno = EBIRDENTRY ;
		return -1;
	}
	int entry_length = strnlen(entry_name,CPIO_FILE_NAME_MAX+1) ;
	if (  entry_length >= CPIO_FILE_NAME_MAX ){
		errno = EBIRDENTRYLENGTH ;
		return -1;

	}
	errno = EBIOK;
	return entry_length;
}

__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_ramdisk(struct bootimage* bi)
{
	if ( bi->ramdisk == NULL ){
		errno = EBIRDMEM ;
		return -1;
	}
	if ( bi->header == NULL || bi->header->magic[0] == 0 ){
		errno = EBIHEADMEM ;
		return -1;
	}
	fprintf(stdout , "bi->header->ramdisk_size=%d",bi->header->ramdisk_size) ;
	if ( bi->header->ramdisk_size == 0  ){
		errno = EBIRDMEMSIZE ;
		return -1;

	}
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_kernel(struct bootimage* bi)
{
	if ( bi->kernel == NULL ){
		errno = EBIKERNELMEM ;
		D("bi->kernel=NULL errno=%d [ EBIKERNELMEM ]",EBIKERNELMEM) ;
		return -1;
	}
	if ( bi->header == NULL || bi->header->magic[0] == 0 ){
		errno = EBIHEADMEM ;
		D("bi->header=%p  bi->header->magic[0]=%c errno=%d [ EBIKERNELMEMSIZE ]",bi->header , bi->header->magic[0], EBIHEADMEM) ;
		return -1;
	}

	if ( bi->header->kernel_size == 0  ){
		errno = EBIKERNELMEMSIZE ;
		D("bi->header->kernel_size=%d errno=%d [ EBIKERNELMEMSIZE ]",bi->header->kernel_size,EBIKERNELMEMSIZE) ;
		return -1;

	}
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__  int check_bootimage_file_read_magic(struct bootimage* bi,const char* file_name)
{
	if ( check_bootimage_structure(bi) == -1){
		fprintf(stderr,"bootimage_file_read check_bootimage_structure failed [ %p ]", bi);
		return -1;
	}

	if( check_bootimage_file_name(file_name) == -1 ){
		fprintf(stderr,"bootimage_file_read check_bootimage_file_name failed [ %p ]", bi);
		return -1;
	}

	if( check_bootimage_file_stat_size(bi,file_name) == -1 ){
		return -1;
	}
	bootimage_file_read_magic(bi,file_name);
	errno = EBIOK;
	return 0;
}
