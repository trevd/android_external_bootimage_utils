#ifndef _BOOTIMAGE_RAMDISK_H_
#define _BOOTIMAGE_RAMDISK_H_
#include "file.h"
#define CPIO_HEADER_SIZE sizeof(cpio_newc_header_t)
#define PROPERTY_KEY_MAX   32
#define PROPERTY_VALUE_MAX  92

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

typedef union _start_t {
		cpio_newc_header_t* cpio_header;
		byte_p position;
	}  start_t ;
typedef struct _cpio_entry_list_t{
	start_t start ;
	byte_p name;
	int name_padding;
	byte_p data;
	size_t data_size;
	byte_p next;
} cpio_entry_list_t;
typedef struct _default_property_list_t{
	char *key;
	char *value;
} default_property_list_t;

//int get_cpio_entries(byte_p ramdisk_cpio_data,size_t size,cpio_entry_list_t*** entriesp);
byte_p update_default_properties_in_gzip(FILE* boot_image_file,size_t ramdisk_size ,default_property_list_t** default_properties,int default_properties_total,size_t*gzipped_ramdisk_size);
default_property_list_t** get_default_properties_from_file(FILE*boot_image_file,size_t ramdisk_size,int* default_property_count);
default_property_list_t** get_default_properties(char *data, int* property_count);
cpio_entry_list_t**  get_cpio_entries(byte_p ramdisk_cpio_data,size_t size,int* cpio_entry_count);
cpio_entry_list_t** get_cpio_entries_from_archive(byte_p ramdisk_data,size_t ramdisk_size,size_t* uncompressed_ramdisk_size ,int* cpio_entry_count );
cpio_entry_list_t**  get_cpio_entries_from_file(FILE*boot_image_file,size_t ramdisk_size,size_t* uncompressed_ramdisk_size,int* cpio_entry_count);
int free_cpio_entry_memory(cpio_entry_list_t** cpio_entries,int cpio_entries_total) ;
int free_default_properties_memory(default_property_list_t** properties,int properties_total) ;
int process_uncompressed_ramdisk(const byte_p ramdisk_cpio_data ,unsigned size, char  *ramdisk_dirname);
size_t uncompress_gzip_ramdisk_memory(const byte_p compressed_data ,const size_t compressed_data_size,byte_p uncompressed_data,size_t uncompressed_max_size);
size_t compress_gzip_ramdisk_memory(const byte_p uncompressed_data , size_t uncompressed_data_size,byte_p compressed_data,size_t compressed_max_size);
long find_file_in_ramdisk_entries(const byte_p data);
void extract_cpio_list_entry(cpio_entry_list_t* cpio_entry);
//cpio_entry_t create_cpio_entry_file(const byte_p cpio_data,const size_t cpio_size);
size_t pack_ramdisk_directory( byte_p ramdisk_cpio_data);
byte_p get_contiguous_cpio_stream(cpio_entry_list_t** cpio_entries,int cpio_entries_total,size_t size);
byte_p compress_cpio_entries_to_gzip(cpio_entry_list_t** cpio_entries,int cpio_entries_total,size_t cpio_size,size_t* ramdisk_size);
//byte_p modify_ramdisk_entry(const byte_p cpio_data,size_t *new_cpio_size,cpio_entry_t cpio_entry);
#endif

