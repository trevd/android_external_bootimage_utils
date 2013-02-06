int process_uncompressed_ramdisk(unsigned char *data ,unsigned size, char  *ramdisk_dirname);
unsigned long uncompress_gzip_ramdisk_memory(unsigned char *data ,unsigned size,unsigned char *uncompressed_data,unsigned uncompressed_max_size);
unsigned long compress_gzip_ramdisk_memory( unsigned char *data , unsigned size,unsigned char *compressed_data,unsigned compressed_max_size);
long find_file_in_ramdisk_entries(unsigned char *data,unsigned size,unsigned long offset);
unsigned long pack_ramdisk_directory(unsigned char* ramdisk_cpio_data);


