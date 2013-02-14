#ifndef _BOOTIMAGE_RAMDISK_H_
#define _BOOTIMAGE_RAMDISK_H_
#include "file.h"
int process_uncompressed_ramdisk(const byte_p ramdisk_cpio_data ,unsigned size, char  *ramdisk_dirname);
unsigned long uncompress_gzip_ramdisk_memory(const byte_p data_in ,unsigned size,byte_p uncompressed_data,unsigned uncompressed_max_size);
unsigned long compress_gzip_ramdisk_memory(const byte_p data_in , unsigned size,byte_p compressed_data,unsigned compressed_max_size);
long find_file_in_ramdisk_entries(const byte_p data);
unsigned long pack_ramdisk_directory( byte_p ramdisk_cpio_data);
byte_p modify_ramdisk_entry(const byte_p cpio_data,unsigned cpio_size,unsigned long *new_cpio_size);
#endif

