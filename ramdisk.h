#ifndef _BOOTIMAGE_RAMDISK_H_
#define _BOOTIMAGE_RAMDISK_H_
#include "file.h"
int process_uncompressed_ramdisk(const byte_p ramdisk_cpio_data ,unsigned size, char  *ramdisk_dirname);
size_t uncompress_gzip_ramdisk_memory(const byte_p compressed_data ,const size_t compressed_data_size,byte_p uncompressed_data,size_t uncompressed_max_size);
size_t compress_gzip_ramdisk_memory(const byte_p uncompressed_data , size_t uncompressed_data_size,byte_p compressed_data,size_t compressed_max_size);
long find_file_in_ramdisk_entries(const byte_p data);
unsigned long pack_ramdisk_directory( byte_p ramdisk_cpio_data);
byte_p modify_ramdisk_entry(const byte_p cpio_data,const size_t cpio_size,size_t *new_cpio_size);
#endif

