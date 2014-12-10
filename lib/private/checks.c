#include <errno.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


#include <api/bootimage.h>
#include <api/errors.h>
#include <private/bootimage.h>
#include <private/checks.h>



__LIBBOOTIMAGE_PRIVATE_API__ int check_output_name(const char* name)
{

	if ( name == NULL ){
		errno = EBIOUTNAME ;
		return -1;
	}
	if ( strnlen(name,PATH_MAX) > PATH_MAX-1){
		errno = EBIOUTNAMELEN ;
		return -1;
	}
	return 0 ;


}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_structure(struct bootimage* bi)
{
	/* Validate inputs */
	if ( bi == NULL ) {
		errno = EBINULL;
		return -1;
	}
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_file_stat_size(struct bootimage* bi ,const char* file_name)
{
	/* Stat the boot image file an store it in the structure */
	fprintf(stdout,"check_bootimage_file_stat_size bi->stat.st_size [ %u ]\n",  bi->stat.st_size);
    if (stat(file_name, &bi->stat) == -1){
		fprintf(stdout,"check_bootimage_file_stat_size bi->stat.st_size [ %u ]\n",  bi->stat.st_size);
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
	fprintf(stdout , "bi->header->ramdisk_size=%d\n",bi->header->ramdisk_size) ;
	if ( bi->header->ramdisk_size == 0  ){
		errno = EBIRDMEMSIZE ;
		return -1;

	}
	return 0;
}
