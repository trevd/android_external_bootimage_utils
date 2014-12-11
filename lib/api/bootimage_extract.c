#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <api/bootimage.h>
#include <api/bootimage_extract.h>

#include <archive.h>
#include <archive_entry.h>

#include <private/checks.h>
#include <private/bootimage.h>
#include <private/utils.h>

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
	printf("bi %u\n",bi->header_size);

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
	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( header_block_name == NULL ) {
		header_block_name = DEFAULT_NAME_HEADER_BLOCK;
	}

	if ( check_output_name ( header_block_name ) == -1 ) {
		return -1 ;
	}
	printf("bi %u\n",bi->header_size);

	FILE* fi = fopen(header_block_name,"w+b");
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
	printf("bi %u\n",bi->header->kernel_size);

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
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_ramdisk(struct bootimage* bi,const char* ramdisk_dir_name)
{
	if ( check_bootimage_structure(bi) == -1 ){
		return -1;
	}
	if ( ramdisk_dir_name == NULL ) {
		ramdisk_dir_name = DEFAULT_NAME_RAMDISK_DIRECTORY;
	}

	int ramdisk_dir_name_length = check_output_name ( ramdisk_dir_name );
	if ( ramdisk_dir_name_length == -1 ) {
		return -1 ;
	}
	if ( mkdir_and_parents ( ramdisk_dir_name, 0755 ) == -1 ) {
		return -1 ;
	}


	struct archive *a = NULL;
	struct archive_entry *entry = NULL;
	int r = 0 ;
	a = archive_read_new();
	/*  */
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);

	printf("bi->header->ramdisk_size=%lu\n",bi->header->ramdisk_size);
	r = archive_read_open_memory(a, bi->ramdisk, bi->header->ramdisk_size);
	if (r != ARCHIVE_OK){
		return -1;
	}
	uint64_t entry_size = 0 ;
	char* entry_data = NULL ;

	char* entry_full_pathname = NULL ;
	uint64_t entry_full_pathname_length = 0 ;
	uint64_t entry_pathname_length = 0 ;


	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		/* entry_pathname = calloc(PATH_MAX, sizeof(unsigned char)); */
		char* entry_pathname = archive_entry_pathname(entry);
		entry_pathname_length = strnlen(entry_pathname,PATH_MAX);
		entry_full_pathname_length = entry_pathname_length+ramdisk_dir_name_length+2;
		entry_full_pathname = calloc(entry_full_pathname_length,sizeof(unsigned char));
		snprintf(entry_full_pathname,entry_full_pathname_length,"%s/%s",ramdisk_dir_name,entry_pathname);
		fprintf(stdout,"entry_full_pathname:%s\n",entry_full_pathname);
		free(entry_full_pathname);
	}
	archive_read_free(a);

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

	if ( check_output_name ( ramdisk_name ) == -1 ) {
		return -1 ;
	}
	printf("bi %u\n",bi->header->ramdisk_size);


	FILE* fi = fopen(ramdisk_name,"w+b");
	if ( fi == NULL ){
		return -1 ;
	}

	fwrite(bi->ramdisk,bi->header->ramdisk_size,1,fi);
	fclose(fi);
	return 0;
}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_extract_ramdisk_entry(struct bootimage* bi,const char* ramdisk_entry_name)
{
	return 0;
}
