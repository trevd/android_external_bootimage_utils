/*
 * kernel.h
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
#ifndef _f7d18500_9064_11e2_b0f1_5404a601fa9d
#define _f7d18500_9064_11e2_b0f1_5404a601fa9d
 
#include <ramdisk.h>
#include <compression.h>

#define KERNEL_COMPRESSION_NOT_SET      COMPRESSION_NOT_SET
#define KERNEL_COMPRESSION_GZIP         COMPRESSION_GZIP_DEFLATE
#define KERNEL_COMPRESSION_LZO          COMPRESSION_LZOP
#define KERNEL_COMPRESSION_LZMA         COMPRESSION_LZMA
#define KERNEL_COMPRESSION_XZ           COMPRESSION_XZ
#define KERNEL_COMPRESSION_BZIP2        COMPRESSION_BZIP2
#define KERNEL_COMPRESSION_NONE         COMPRESSION_NONE
#define KERNEL_COMPRESSION_NONE         COMPRESSION_NONE

// The magic number for a compressed arm linux kernel image 
#define KERNEL_ZIMAGE_MAGIC "\x18\x28\x6F\x01"
#define KERNEL_ZIMAGE_MAGIC_SIZE 4 

// The Magic Number for a decompressed linux kernel image 
#define KERNEL_IMAGE_MAGIC "\xD3\xF0\x21\xE3"
#define KERNEL_IMAGE_MAGIC_SIZE 4 

typedef struct kernel_image kernel_image;

struct kernel_image {
    
        /* start_addr - A Pointer to the memory location contain
         * the start of an uncompressed kernel image */ 
        unsigned char*  start_addr ;
        
        /* size - The Size in bytes of the uncompressed kernel 
         * in memory - end_addr is implied and a result of 
         * start_addr+size  */
        unsigned        size;
        
        /* ramdisk_addr - A Pointer to the embedded ramdisk data
         * as it is packed in the kernel.
         */
        unsigned char*  ramdisk_addr ;
        unsigned        ramdisk_size;
        unsigned        ramdisk_compression_type;
        unsigned char*  config_gz_addr ;
        unsigned char*  config_addr ;

        char*           version ;
        char*           version_number ;
        int             version_number_length ;

        unsigned        compression_type;
                
        unsigned        config_gz_size;
        unsigned        config_size;
        
};
#define boot_image_kernel_image_read

unsigned biki_read(unsigned char* kernel_addr,unsigned kernel_size,kernel_image* image );
unsigned biki_rd_read(kernel_image* image,ramdisk_image* rimage );

//int print_kernel_info(kernel_image* kimage);
//char *str_kernel_compression(unsigned compression) ;

unsigned biki_write(const char *filename,kernel_image* image);
unsigned biki_write_ramdisk(const char *filename,kernel_image* image);
unsigned biki_write_details(const char *filename,kernel_image* image);
unsigned biki_write_config(const char *filename,kernel_image* image);
unsigned biki_write_config_gzip(const char *filename,kernel_image* image);

#endif
