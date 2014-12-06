#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <archive.h>
#include <archive_entry.h>

#include "api.h"
#include "bootimage.h"

#include "internal_api.h"
struct bootimage_ramdisk_entry {
	int64_t size ;
	unsigned char* data ;
	
};
static struct bootimage_ramdisk_entry* bootimage_extract_ramdisk_entry_internal(struct bootimage* bi,const char* entry_name,int entry_name_length)
{
	
	if ( check_bootimage_ramdisk(bi) == -1){
		return NULL ; 
	}
	errno = 0 ;
	struct archive *a = NULL;
	struct archive_entry *entry = NULL;
	
	int r = 0 ; 
	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
		
	printf("bi->header->ramdisk_size=%lu\n",bi->header->ramdisk_size);
	r = archive_read_open_memory(a, bi->ramdisk, bi->header->ramdisk_size);
	if (r != ARCHIVE_OK){
		
		return NULL;
	}
	struct bootimage_ramdisk_entry* ramdisk_entry = NULL;
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		if ( strncmp(entry_name,archive_entry_pathname(entry),entry_name_length) == 0){
			ramdisk_entry = calloc(1,sizeof(struct bootimage_ramdisk_entry));
			ramdisk_entry->size = archive_entry_size(entry);
			ramdisk_entry->data = calloc(ramdisk_entry->size,sizeof(unsigned char));
			archive_read_data(a,ramdisk_entry->data,ramdisk_entry->size) ;
			break;
		}
		archive_read_data_skip(a);
		
	}
	r = archive_read_free(a); 
	return ramdisk_entry ; 	
}
__LIBBOOTIMAGE_PUBLIC_API__ int bootimage_extract_ramdisk_entry_from_file(struct bootimage* bi,const char* file_name,const char* entry_name)
{
	
	/* Validate inputs */
	if ( check_bootimage_structure(bi) == -1){
		return -1;
	}
	int entry_name_length = check_ramdisk_entryname(entry_name) ;
	if ( entry_name_length == -1){
		return -1;
	}
	
	/*  check_bootimage_filename will be called by bootimage_read_filename 
		but we'll check this here to incase that should ever change */
	if( check_bootimage_file(bi,file_name) == -1 ){
		return -1;
	}

	/* Attempt to load the bootimage from the given filename */
	if ( bootimage_read_filename_internal(bi,file_name) == -1){
		/* Could not load the bootimage. errno will be set already 
		   so just return -1; */
		return -1;
	}
	struct bootimage_ramdisk_entry* ramdisk_entry = bootimage_extract_ramdisk_entry_internal(bi,entry_name,entry_name_length);
	if ( ramdisk_entry == NULL ) {
		return -1 ; 
	}
	FILE* file_out = fopen(entry_name,"w+b");
	if ( file_out == NULL ){
		return -1 ; 
	}
	fwrite(ramdisk_entry->data,ramdisk_entry->size,1,file_out); 
	fclose(file_out);
	free(ramdisk_entry->data);
	free(ramdisk_entry);
	
	return 0;
	
	
}
__LIBBOOTIMAGE_PUBLIC_API__ bootimage_extract_ramdisk(const struct bootimage* bi){
	
	
}
__LIBBOOTIMAGE_PUBLIC_API__ bootimage_file_extract_ramdisk(const char* file_name,const char* ramdisk_file_name){
	
}


