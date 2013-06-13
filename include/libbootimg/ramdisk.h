/*
 * ramdisk.h
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
#ifndef _2638e606_9103_11e2_b47f_5404a601fa9d
#define _2638e606_9103_11e2_b47f_5404a601fa9d


#include <compression.h>

#define CPIO_HEADER_MAGIC "070701"
#define CPIO_HEADER_MAGIC_SIZE 6


#define CPIO_TRAILER_MAGIC "TRAILER!!!"
#define CPIO_TRAILER_MAGIC_SIZE 10


#define RAMDISK_ENTRY_DATA_NON_CONTIGOUS -1


#define RAMDISK_COMPRESSION_UNKNOWN 0
#define RAMDISK_COMPRESSION_GZIP    COMPRESSION_GZIP_DEFLATE
#define RAMDISK_COMPRESSION_LZO     COMPRESSION_LZOP        
#define RAMDISK_COMPRESSION_XZ      COMPRESSION_XZ          
#define RAMDISK_COMPRESSION_LZMA    COMPRESSION_LZMA        
#define RAMDISK_COMPRESSION_BZIP2   COMPRESSION_BZIP2       
#define RAMDISK_COMPRESSION_LZ4     COMPRESSION_LZ4         
#define RAMDISK_COMPRESSION_NONE    9

#define RAMDISK_TYPE_UNKNOWN -1
#define RAMDISK_TYPE_NORMAL 1
#define RAMDISK_TYPE_RECOVERY 2
#define RAMDISK_TYPE_UBUNTU 3

#define RECOVERY_BRAND_UNKNOWN -1
#define RECOVERY_BRAND_NONE 0
#define RECOVERY_BRAND_STOCK 1
#define RECOVERY_BRAND_CLOCKWORK 2
#define RECOVERY_BRAND_CWM 3
#define RECOVERY_BRAND_COT 4
#define RECOVERY_BRAND_TWRP 5
#define RECOVERY_BRAND_4EXT 6

#define RECOVERY_VERSION_UNKNOWN -1

typedef struct ramdisk_image ramdisk_image;

typedef struct ramdisk_entry ramdisk_entry;

struct ramdisk_entry{
    
    unsigned char* start_addr ;
     
    unsigned size;

    unsigned long mode;    
    
    unsigned char* name_addr ;
    unsigned name_size ;
    unsigned name_offset ;
    unsigned name_padding ;
    
    unsigned char* data_addr ;
    unsigned data_size ;
    unsigned data_offset ;
    unsigned data_padding ;
    
    unsigned char* next_addr ;
    
    
    
    } ;

struct ramdisk_image {
    
    
    int compression_type;
    unsigned char* start_addr ;
    unsigned size;
    int type;
    int recovery_brand;   
    char *recovery_version;
    int recovery_version_size;
        
    unsigned entry_count;

    ramdisk_entry ** entries ;

    };


unsigned init_ramdisk_image(ramdisk_image* image);

unsigned load_ramdisk_image_from_archive_file(const char *filename, ramdisk_image* image);

unsigned load_ramdisk_image_from_cpio_file(const char *filename, ramdisk_image* image);

unsigned load_ramdisk_image_from_cpio_memory(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image );

unsigned load_ramdisk_image_from_archive_memory(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image );

unsigned save_ramdisk_entries_to_disk(ramdisk_image* image,char *directory_name);

unsigned char *pack_ramdisk_directory(char* directory_name, unsigned *cpio_size);

unsigned print_ramdisk_info(ramdisk_image* rimage);

char *str_ramdisk_compression(int compression_type);

/* int_ramdisk_compression - returns an integer representing the compression type
 * if the ramdisk is compression type is unknown the then RAMDISK_COMPRESSION_UNKNOWN
 * 
 */ 

unsigned int_ramdisk_compression(char * compression_type);

char *str_ramdisk_type(int type);

char *str_recovery_brand(int ramdisk_brand);

unsigned update_ramdisk_entry_cpio_newc_header_info(ramdisk_entry* entry);

unsigned char* pack_noncontiguous_ramdisk_entries(ramdisk_image* rimage);

ramdisk_image* get_initialized_ramdisk_image();

#endif
