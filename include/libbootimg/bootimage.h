/*
 * bootimage.h
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
#ifndef _46760ac0_903b_11e2_929b_5404a601fa9d
#define _46760ac0_903b_11e2_929b_5404a601fa9d
#include <kernel.h>
#include <ramdisk.h>
#include <bootimg.h>
typedef struct boot_image boot_image;
typedef struct BOOT_IMAGE_DATA BOOT_IMAGE_DATA;

#ifndef BOOT_MAGIC_SIZE
#define BOOT_MAGIC_SIZE 8
#endif
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512
#define BOOT_IMAGE_NAME_SIZE BOOT_NAME_SIZE
#define BOOT_IMAGE_ARGS_SIZE BOOT_ARGS_SIZE


struct BOOT_IMAGE_DATA {
	
	unsigned char* oem;
	unsigned char* header ;
	unsigned char* kernel ;
	unsigned char* ramdisk ;
	unsigned char* second ;
};
typedef boot_img_hdr BOOT_IMAGE_HEADER;

struct boot_image
{
    
    BOOT_IMAGE_DATA* data ; /*  pointer to the start of the image file in memory
                                    the creator of the boot_image struct is resposible
                                    for freeing the memory specified at this location */
    
    /* The addresses within the boot image memory to find the specific parts */
    BOOT_IMAGE_HEADER* header;
        
    /* Additional Boot Image Information */

    unsigned long size;
    unsigned header_size;
    
    unsigned header_padding;
    unsigned header_offset;      /* the position of the ANDROID! magic. 
                                   this value is added to the start to get the 
                                   header */
    
    unsigned kernel_padding;
    unsigned kernel_offset;
    
    unsigned ramdisk_padding;
    unsigned ramdisk_offset;
    
    unsigned second_padding;
    unsigned second_offset;
    
   
    
};

//~ unsigned load_boot_image_from_memory(unsigned char* boot_image_addr,unsigned boot_image_size, boot_image* image);
//~ unsigned load_boot_image_from_file(const char *filename, boot_image* image);
//~ unsigned write_boot_image(char *filename, boot_image* image);
//~ unsigned write_boot_image_header_to_disk(const char *filename, boot_image* image);
//~ unsigned load_boot_image_header_from_disk(const char *filename, boot_image* image);
//~ unsigned set_boot_image_defaults(boot_image* image);
//~ unsigned set_boot_image_content_hash(boot_image* image);
//~ 
//~ unsigned set_boot_image_padding(boot_image* image);
//~ unsigned set_boot_image_offsets(boot_image* image);
//~ unsigned print_boot_image_info(boot_image* image);
//~ unsigned print_boot_image_header_info(boot_image* image);
//~ unsigned print_boot_image_additional_info(boot_image* image);
//~ unsigned print_boot_image_header_hashes(boot_image* image);

boot_image* boot_image_allocate();
void boot_image_free(boot_image* image); 

boot_image* abu_bootimage_init();
void abu_bootimage_destroy(boot_image* image);

// abu_bootimage_open_archive - populates boot_image structure
// from zip file 
unsigned int abu_bootimage_open_archive(boot_image* image,const char* filename);


boot_image* abu_bootimage_open(boot_image* image,const char* filename);

#endif
