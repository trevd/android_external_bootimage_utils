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

/* libarchive archive.h */
#include <archive.h>

#include <private/api.h>


__LIBBOOTIMAGE_PRIVATE_API__ int check_archive_read_memory(struct archive **ap,char* data , uint64_t data_size)
{
	if ( check_archive_read_initialization(ap) == -1 ){
		return -1;
	}
	D("ap[0]=%p",ap[0]);
	int r = archive_read_open_memory(ap[0], data,data_size);
	if (r != ARCHIVE_OK){
		archive_error_string(ap[0]);
		D("r=%d %s",r,archive_error_string(ap[0]));
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
/* check_output_name - checks the validity of the name argument */
__LIBBOOTIMAGE_PRIVATE_API__ int check_output_name(char* name)
{
	D("name=%s",name);
	if ( name == NULL ){
		errno = EBIOUTNAME ;
		return -1;
	}
	int size = utils_paranoid_strnlen(name,PATH_MAX);
	if ( size > PATH_MAX-1){
		errno = EBIOUTNAMELEN ;
		return -1;
	}
	if ( size <= 0){
		errno = EBIOUTNAMELEN ;
		return -1;
	}
	errno = EBIOK;
	return size ;


}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_structure(struct bootimage* bi)
{
	/* Validate inputs */
	D("bi=%p",bi);
	if ( bi == NULL ) {
		errno = EBINULL;
		D("bi=%p errno=%d [ EBINULL ]",bi,errno);
		return -1;
	}
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_file_stat_size(struct bootimage* bi ,char* file_name)
{
	/* Stat the boot image file an store it in the structure */
    if (stat(file_name, &bi->stat) == -1){
		D("bi->stat.st_size [ %jd ]",  bi->stat.st_size);
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
__LIBBOOTIMAGE_PRIVATE_API__ int check_file_name_and_access(char* file_name)
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



__LIBBOOTIMAGE_PRIVATE_API__ int check_ramdisk_entryname(char* entry_name)
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
	D("bi->header->ramdisk_size=%d",bi->header->ramdisk_size) ;
	if ( bi->header->ramdisk_size <= 0  ){
		errno = EBIRDMEMSIZE ;
		return -1;

	}
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_kernel(struct bootimage* bi)
{

	/* check we have a valid header which is needed for the kernel size */
	if ( bi->header == NULL || bi->header->magic[0] == 0 ){
		errno = EBIHEADMEM ;
		D("bi->header=%p  bi->header->magic[0]=%c errno=%d [ EBIKERNELMEMSIZE ]",bi->header , bi->header->magic[0], EBIHEADMEM) ;
		return -1;
	}
	/* check we have a "valid" kernel size in the header */
	if ( bi->header->kernel_size <= 0  ){
		errno = EBIKERNELMEMSIZE ;
		D("bi->header->kernel_size=%d errno=%d [ EBIKERNELMEMSIZE ]",bi->header->kernel_size,EBIKERNELMEMSIZE) ;
		return -1;

	}
	/* check we at least have a valid pointer to the compressed kernel memory */
	if ( bi->kernel == NULL ){
		errno = EBIKERNELMEM ;
		D("bi->kernel=NULL errno=%d [ EBIKERNELMEM ]",EBIKERNELMEM) ;
		return -1;
	}

	errno = EBIOK;
	return 0;
}

/* check_bootimage_file_read_magic - acts as a check wrapper for ultimately calling bootimage_file_read_magic */
__LIBBOOTIMAGE_PRIVATE_API__  int check_bootimage_file_read_magic(struct bootimage* bi,char* file_name)
{
	if ( check_bootimage_structure(bi) == -1){
		D("bootimage_file_read check_bootimage_structure failed [ %p ]", bi);
		return -1;
	}

	if( check_file_name_and_access(file_name) == -1 ){
		D("bootimage_file_read check_bootimage_file_name failed [ %p ]", bi);
		return -1;
	}

	if( check_bootimage_file_stat_size(bi,file_name) == -1 ){
		return -1;
	}
	bootimage_file_read_magic(bi,file_name);
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_utils_structure(struct bootimage_utils* biu)
{
	/* Validate inputs */
	D("bi=%p",biu);
	if ( biu == NULL ) {
		errno = EBIUNULL;
		D("biu=%p errno=%d [ EBINULL ]",biu,errno);
		return -1;
	}
	if ( ( biu->filetype < 0 ) || ( biu->filetype > BOOTIMAGE_UTILS_FILETYPE_MAX ) ) {
		errno = EBIUFILETYPE ;
		D("biu=%p errno=%d [ EBIUFILETYPE ]",biu,errno);
		return -1;

	}

	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int validate_file_stat_size(struct stat* st,char* file_name)
{
	/* Stat the boot image file an store it in the structure */

    if (stat(file_name, st) == -1){
		D("biu->stat.st_size [ %jd ]",  st->st_size);
		errno = EBIUSTAT;
		return -1 ;
	}
	/* File size is zero or could not be determined  or
	   File size is less than a minimum know page size
	   This is probably not good.  */

	if (  ( st->st_size <= 0 )  ) {
		errno = EBIFSIZE;
		return -1 ;
	}
	D("biu->stat.st_size [ %jd ]",  st->st_size);
	errno = EBIOK;
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__  int check_bootimage_utils_file_type(struct bootimage_utils* biu)
{
	/* File Type Identification strategy

	   Factory Image Identification
	   Check for a known factory image file name.
	   Check if the file is a valid gzip magic.
	*/

	struct factory_images* fi =  &factory_image_info[0] ;
	while ( fi->name != NULL ){
		/* D("fi=%p fi->name=%s fi->name_length=%d",fi,fi->name,fi->name_length); */
		if ( strncmp(fi->name,biu->file_name,fi->name_length) == FACTORY_IMAGE_STRNCMP_MATCH_FOUND ){
			D("Potential Factory Image File Found file_name=%s",biu->file_name);
			break ;
		}

		fi++;
	}

	if ( fi != NULL ) {
		/* For this to be a potentially bona-fida factory image the Gzip identifaction must be at the
		   start of the data */

		char* magic = utils_memmem(biu->compressed_data,biu->stat.st_size,FACTORY_IMAGE_MAGIC_GZIP,FACTORY_IMAGE_MAGIC_GZIP_SIZE);
		D("Factory Image Gzip Magic %p : biu-data[0] %p",magic ,biu->compressed_data ) ;
		if ( magic == biu->compressed_data ){
			unsigned int uncompressed_size = archive_gzip_get_uncompressed_size(biu->compressed_data,biu->stat.st_size);
			archive_extract_memory_factory_image_zip_file_to_memory(biu,fi);
			//D("Factory Image Gzip Magic %p : biu-data[0] %p",magic ,biu->compressed_data ) ;
		}

	}
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__  int check_bootimage_utils_file_read(struct bootimage_utils* biu,char* file_name)
{
	if ( check_bootimage_utils_structure(biu) == -1){
		D("check_bootimage_utils_structure failed [ %p ]", biu);
		return -1;
	}

	if( check_file_name_and_access(file_name) == -1 ){
		D("check_file_name_and_access failed [ %p ]", biu);
		return -1;
	}
	biu->file_name = file_name;
	if( validate_file_stat_size(&biu->stat,file_name) == -1 ){
		D("check_file_name_and_access failed [ %p ]", biu);
		return -1;
	}
	D("biu->compressed_data=%p",biu->compressed_data);

	biu->compressed_data = calloc(biu->stat.st_size,sizeof(char));
	utils_read_all(file_name,biu->stat.st_size,biu->compressed_data );
	D("biu->compressed_data=%p",biu->compressed_data);
	errno = EBIOK;
	return 0;
}
