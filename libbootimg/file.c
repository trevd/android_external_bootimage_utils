/*
 * file.c
 * 
 * Copyright 2013 Trevor Drake <trevd1234@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
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
    char* description ; 
    unsigned description_size ; 

} ;

known_file_type known_file_types[] = { 
    
    { KNOWN_FILE_TYPE_ID_RAMDISK_GZIP , GZIP_DEFLATE_MAGIC , GZIP_DEFLATE_MAGIC_SIZE,       KNOWN_FILE_TYPE_DESCRIPTION_RAMDISK_GZIP,   KNOWN_FILE_TYPE_DESCRIPTION_SIZE_RAMDISK_GZIP       },          
    { KNOWN_FILE_TYPE_ID_RAMDISK_NONE , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE,                 KNOWN_FILE_TYPE_DESCRIPTION_RAMDISK_NONE,   KNOWN_FILE_TYPE_DESCRIPTION_SIZE_RAMDISK_NONE       },
    { KNOWN_FILE_TYPE_ID_RAMDISK_LZO , LZOP_MAGIC , LZOP_MAGIC_SIZE  ,                         KNOWN_FILE_TYPE_DESCRIPTION_RAMDISK_LZO,     KNOWN_FILE_TYPE_DESCRIPTION_SIZE_RAMDISK_LZO        },
    { KNOWN_FILE_TYPE_ID_RAMDISK_LZMA , LZMA_MAGIC , LZMA_MAGIC_SIZE  ,              KNOWN_FILE_TYPE_DESCRIPTION_RAMDISK_LZMA,  KNOWN_FILE_TYPE_DESCRIPTION_SIZE_RAMDISK_LZMA       },
    { KNOWN_FILE_TYPE_ID_RAMDISK_BZIP2 , BZIP2_MAGIC , BZIP2_MAGIC_SIZE  ,              KNOWN_FILE_TYPE_DESCRIPTION_RAMDISK_BZIP2,  KNOWN_FILE_TYPE_DESCRIPTION_SIZE_RAMDISK_BZIP2  },
    { KNOWN_FILE_TYPE_ID_RAMDISK_XZ , XZ_MAGIC , XZ_MAGIC_SIZE ,                              KNOWN_FILE_TYPE_DESCRIPTION_RAMDISK_XZ,   KNOWN_FILE_TYPE_DESCRIPTION_SIZE_RAMDISK_XZ         },  
    { KNOWN_FILE_TYPE_ID_RAMDISK_LZ4 , LZ4_MAGIC , LZ4_MAGIC_SIZE ,               KNOWN_FILE_TYPE_DESCRIPTION_RAMDISK_LZ4,  KNOWN_FILE_TYPE_DESCRIPTION_SIZE_RAMDISK_LZ4        },
    { KNOWN_FILE_TYPE_ID_RAMDISK_CPIO , CPIO_HEADER_MAGIC , CPIO_HEADER_MAGIC_SIZE ,          KNOWN_FILE_TYPE_DESCRIPTION_RAMDISK_CPIO,     KNOWN_FILE_TYPE_DESCRIPTION_SIZE_RAMDISK_CPIO       },
    { KNOWN_FILE_TYPE_ID_KERNEL_ZIMAGE , KERNEL_ZIMAGE_MAGIC , KERNEL_ZIMAGE_MAGIC_SIZE,     KNOWN_FILE_TYPE_DESCRIPTION_KERNEL_ZIMAGE,     KNOWN_FILE_TYPE_DESCRIPTION_SIZE_KERNEL_ZIMAGE      },
    { KNOWN_FILE_TYPE_ID_KERNEL_UNCOMPRESSED , FILE_MAGIC_NULL , FILE_MAGIC_NULL_SIZE ,       KNOWN_FILE_TYPE_DESCRIPTION_KERNEL_UNCOMPRESSED,KNOWN_FILE_TYPE_DESCRIPTION_SIZE_KERNEL_UNCOMPRESSED  },
    { KNOWN_FILE_TYPE_ID_BOOT_IMAGE , BOOT_MAGIC, BOOT_MAGIC_SIZE,                             KNOWN_FILE_TYPE_DESCRIPTION_BOOT_IMAGE,  KNOWN_FILE_TYPE_DESCRIPTION_SIZE_BOOT_IMAGE         },  
    { 0 , NULL , 0 , NULL , 0}
};




unsigned get_known_types_in_file(const char *filename,file_type** types, unsigned *total_types_found){

    errno = 0;
    unsigned file_size = 0;
    
    D("filename=%s\n",filename);
    
     
    unsigned char* file_addr = read_item_from_disk(filename,&file_size);
    if(!file_addr){
    return errno;
    }
    D("file_size=%u\n",file_size);
      
    (*types) = calloc(FILE_TYPE_MAX, sizeof(file_type) );
    
    unsigned known_file_type_index = 0;
    unsigned types_index = 0 ;
    unsigned char* known_file_type_offset = file_addr ;
    
      
    
    for(known_file_type_index = 0 ; known_file_type_index < KNOWN_FILE_TYPE_ID_COUNT ; known_file_type_index++ ) {
    
    
    if(known_file_types[known_file_type_index].magic_size == FILE_MAGIC_NULL_SIZE)  continue;
    
     D("file_addr=%p\n",file_addr);
     //D("known_file_type_index=%u\n",known_file_type_index);
     //D("known_file_types[%d].magic =%s\n",known_file_type_index,  known_file_types[known_file_type_index].magic );
     //D("known_file_types[%d].magic_size=%u\n",known_file_type_index,  known_file_types[known_file_type_index].magic_size );
     
    unsigned char* known_file_type_addr = find_in_memory_start_at( file_addr, file_size , known_file_type_offset,
                        known_file_types[known_file_type_index].magic , 
                        known_file_types[known_file_type_index].magic_size );
    
    // file type not found in data,
    // check the next type.
     D("known_file_type_addr=%p\n",known_file_type_addr);
    if(!known_file_type_addr) {
        
        D("file_type:%s not found\n",known_file_types[known_file_type_index].description);
        continue;
    }else{
        D("file_type:%s found\n",known_file_types[known_file_type_index].description);
    }
    
     D("known_file_type_offset=%p %s\n",known_file_type_addr,known_file_types[known_file_type_index].description);
    
    
    
    (*types)[types_index].known_file_type_id = known_file_types[known_file_type_index].id ;
    (*types)[types_index].start_addr = known_file_type_offset ;
    (*types)[types_index].end_addr = NULL ;
    
    
    known_file_type_offset = known_file_type_addr ;
    
    types_index ++ ;
     
    
    } 
    (*total_types_found) = types_index ; 
     
    return 0;
    
    
    
    
}
