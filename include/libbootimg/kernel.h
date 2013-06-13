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
 
#include <bootimage.h>
#include <compression.h>

#define KERNEL_COMPRESSION_NONE 0
#define KERNEL_COMPRESSION_GZIP     COMPRESSION_GZIP_DEFLATE
#define KERNEL_COMPRESSION_LZO      COMPRESSION_LZOP
#define KERNEL_COMPRESSION_LZMA     COMPRESSION_LZMA
#define KERNEL_COMPRESSION_XZ       COMPRESSION_XZ
#define KERNEL_COMPRESSION_BZIP2    COMPRESSION_BZIP2


#define KERNEL_ZIMAGE_MAGIC "\x18\x28\x6F\x01"
#define KERNEL_ZIMAGE_MAGIC_SIZE 4 


typedef struct kernel_image kernel_image;

struct kernel_image {
    
    //
    unsigned char* start_addr ;
    unsigned char* ramdisk_addr ;
    unsigned char* config_addr ;
    
    
    char* version ;
    char* version_number ;
    int version_number_length ;
    
    unsigned compression_type;
    unsigned size;
    unsigned config_size;
    unsigned ramdisk_size;
    };

int load_kernel_image_from_memory(unsigned char* kernel_addr,unsigned kernel_size,kernel_image* image );
int print_kernel_info(kernel_image* kimage);
char *str_kernel_compression(int compression_type);
    

#endif
