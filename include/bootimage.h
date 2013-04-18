#ifndef _46760ac0_903b_11e2_929b_5404a601fa9d
#define _46760ac0_903b_11e2_929b_5404a601fa9d
#include <kernel.h>
#include <ramdisk.h>
#include <bootimg.h>
typedef struct boot_image boot_image;
typedef union boot_image_header boot_image_header;

#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512


// boot_image_header at times need either it's members
// or the pointer
union boot_image_header{
    unsigned char* addr;
    boot_img_hdr* members;
};


struct boot_image
{
    unsigned char* start_addr ; /*  pointer to the start of the image file in memory
                                    the creator of the boot_image struct is resposible
                                    for freeing the memory specified at this location */
    
    /* The addresses within the boot image memory to find the specific parts */
    boot_img_hdr* header;
    unsigned char* kernel_addr;
    unsigned char* ramdisk_addr;
    unsigned char* second_addr;   
    
    
    /* Additional Boot Image Information */

    unsigned total_size;
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
int load_boot_image_from_memory(unsigned char* boot_image_addr,unsigned boot_image_size, boot_image* image);
int load_boot_image_from_file(const char *filename, boot_image* image);
int write_boot_image(char *filename, boot_image* image);
int write_boot_image_header_to_disk(const char *filename, boot_image* image);
int load_boot_image_header_from_disk(const char *filename, boot_image* image);
int set_boot_image_defaults(boot_image* image);
int set_boot_image_content_hash(boot_image* image);
int set_boot_image_padding(boot_image* image);
int set_boot_image_offsets(boot_image* image);
int print_boot_image_info(boot_image* image);
int print_boot_image_header_info(boot_image* image);
int print_boot_image_additional_info(boot_image* image);
int print_boot_image_header_hashes(boot_image* image);
int copy_boot_image_header_info(boot_image* dest,boot_image* source);
#endif
