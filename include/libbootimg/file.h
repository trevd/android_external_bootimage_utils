#ifndef _18125992_b60e_11e2_afaa_5404a601fa9d
#define _18125992_b60e_11e2_afaa_5404a601fa9d

#define KNOWN_FILE_TYPE_ID_COUNT 11

#define KNOWN_FILE_TYPE_ID_RAMDISK_NONE 0
#define KNOWN_FILE_TYPE_ID_RAMDISK_GZIP 1
#define KNOWN_FILE_TYPE_ID_RAMDISK_LZO 2
#define KNOWN_FILE_TYPE_ID_RAMDISK_LZMA 3
#define KNOWN_FILE_TYPE_ID_RAMDISK_BZIP 4
#define KNOWN_FILE_TYPE_ID_RAMDISK_XZ 5
#define KNOWN_FILE_TYPE_ID_RAMDISK_LZ4 6
#define KNOWN_FILE_TYPE_ID_RAMDISK_CPIO 7
#define KNOWN_FILE_TYPE_ID_KERNEL_ZIMAGE 8 
#define KNOWN_FILE_TYPE_ID_KERNEL_UNCOMPRESSED 9
#define KNOWN_FILE_TYPE_ID_BOOT_IMAGE 10



typedef struct file_type file_type ;

struct file_type{
    
    unsigned known_file_type_id ;
    unsigned char* start_addr ;
    unsigned char* end_addr ;

};



/* NAME
  	getKNOWN_FILE_types_inFILE - locate file types within a binary blob
    
 SYNOPSIS

	#include <utils.h>

	unsigned getKNOWN_FILE_types_inFILE(const char *filename);

 DESCRIPTION
    
	getKNOWN_FILE_types_inFILE() function locates  file types within a binary blob

 RETURN VALUE
  	
  	On success, zero is returned, On error, -1 is return and errno is set appropriately
	
 
 */ 
unsigned get_known_types_in_file(const char *filename,file_type** types, unsigned* total_types_found);



#endif
