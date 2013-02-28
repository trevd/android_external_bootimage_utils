#ifndef _BOOTIMAGE_RAMDISK_H_
#define _BOOTIMAGE_RAMDISK_H_
#include "file.h"
#define CPIO_HEADER_SIZE sizeof(cpio_newc_header_t)
typedef struct {
		   char    c_magic[6];
		   char    c_ino[8];
		   char    c_mode[8];
		   char    c_uid[8];
		   char    c_gid[8];
		   char    c_nlink[8];
		   char    c_mtime[8];
		   char    c_filesize[8];
		   char    c_devmajor[8];
		   char    c_devminor[8];
		   char    c_rdevmajor[8];
		   char    c_rdevminor[8];
		   char    c_namesize[8];
		   char    c_check[8];
	   } cpio_newc_header_t;
typedef struct  {
	cpio_newc_header_t cpio_header;
	unsigned long  	file_size ;
	unsigned long  	name_size ;
	mode_t  	mode ;
	byte_p entry_start_p;
	byte_p file_start_p; 
	byte_p next_header_p;
	char * file_name ;
	size_t parent_directory_length;
	int is_trailer ;
	int is_directory ; 
	int is_file ;
	int is_link ;
}cpio_entry_t;
int process_uncompressed_ramdisk(const byte_p ramdisk_cpio_data ,unsigned size, char  *ramdisk_dirname);
size_t uncompress_gzip_ramdisk_memory(const byte_p compressed_data ,const size_t compressed_data_size,byte_p uncompressed_data,size_t uncompressed_max_size);
size_t compress_gzip_ramdisk_memory(const byte_p uncompressed_data , size_t uncompressed_data_size,byte_p compressed_data,size_t compressed_max_size);
long find_file_in_ramdisk_entries(const byte_p data);
cpio_entry_t populate_cpio_entry(const byte_p data );
cpio_entry_t create_cpio_entry_file(const byte_p cpio_data,const size_t cpio_size);
byte_p extract_cpio_entry(cpio_entry_t cpio_entry,char * target_filename);
unsigned long pack_ramdisk_directory( byte_p ramdisk_cpio_data);
byte_p modify_ramdisk_entry(const byte_p cpio_data,size_t *new_cpio_size,cpio_entry_t cpio_entry);
#endif

