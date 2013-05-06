#include <stdio.h> 
#include <string.h> 
#include <kernel.h>
#include <ramdisk.h>
#include <bootimg.h>
#include <file.h>
#include <errno.h>
#include <utils.h>
#include <compression.h>


#define FILE_MAGIC_NULL NULL
#define FILE_MAGIC_NULL_SIZE 0
#define FILE_TYPE_MAX 255

typedef struct known_file_type known_file_type ;

struct known_file_type {
    
    unsigned id ;
    char * magic;
    unsigned magic_size;

} ;

known_file_type known_file_types[] = { 
    
    { KNOWN_FILE_TYPE_ID_RAMDISK_GZIP , GZIP_DEFLATE_MAGIC , GZIP_DEFLATE_MAGIC_SIZE } ,
    { KNOWN_FILE_TYPE_ID_RAMDISK_NONE , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE } ,
    { KNOWN_FILE_TYPE_ID_RAMDISK_GZIP , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE } ,
    { KNOWN_FILE_TYPE_ID_RAMDISK_LZO , LZOP_MAGIC , LZOP_MAGIC_SIZE } ,
    { KNOWN_FILE_TYPE_ID_RAMDISK_LZMA , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE } ,
    { KNOWN_FILE_TYPE_ID_RAMDISK_BZIP , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE } ,
    { KNOWN_FILE_TYPE_ID_RAMDISK_XZ , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE } ,
    { KNOWN_FILE_TYPE_ID_RAMDISK_LZ4 , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE } ,
    { KNOWN_FILE_TYPE_ID_RAMDISK_CPIO , CPIO_HEADER_MAGIC , CPIO_HEADER_MAGIC_SIZE } ,
    { KNOWN_FILE_TYPE_ID_KERNEL_ZIMAGE , KERNEL_ZIMAGE_MAGIC , KERNEL_ZIMAGE_MAGIC_SIZE } ,
    { KNOWN_FILE_TYPE_ID_KERNEL_UNCOMPRESSED , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE } ,
    { KNOWN_FILE_TYPE_ID_BOOT_IMAGE , BOOT_MAGIC, BOOT_MAGIC_SIZE },
    { 0 , NULL , 0 }
};




unsigned get_known_types_in_file(const char *filename,file_type** types, unsigned *total_types_found){

    errno = 0;
    unsigned file_size = 0;
    
    D("filename=%s\n",filename);
    
     
    unsigned char* file_addr = read_item_from_disk(filename,&file_size);
    if(!file_addr){
	return errno;
    }
    
      
    (*types) = calloc(FILE_TYPE_MAX, sizeof(file_type) );
    
    unsigned known_file_type_index = 0;
    unsigned types_index = 0 ;
    unsigned char* known_file_type_offset = file_addr ;
    
      
    
    for(known_file_type_index = 0 ; known_file_type_index < KNOWN_FILE_TYPE_ID_COUNT ; known_file_type_index++ ) {
	
	unsigned char* known_file_type_addr = find_in_memory_start_at( file_addr, file_size , known_file_type_offset,
					    known_file_types[known_file_type_index].magic , 
					    known_file_types[known_file_type_index].magic_size );
	
	// file type not found in data,
	// check the next type.
	if(!known_file_type_offset) continue;
	
	
	
	(*types)[types_index].known_file_type_id = known_file_types[known_file_type_index].id ;
	(*types)[types_index].start_addr = known_file_type_offset ;
	(*types)[types_index].end_addr = NULL ;
	
	
	known_file_type_offset = known_file_type_addr+1 ;
	
	types_index ++ ;
	 
	
    } 
    total_types_found = types_index ; 
     
    return 0;
    
    
    
    
}
