#ifndef _46760ac0_903b_11e2_929b_5404a601fa9d
#define _46760ac0_903b_11e2_929b_5404a601fa9d
#include <kernel.h>
#include <ramdisk.h>
typedef struct boot_image boot_image;

#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512

struct boot_image
{
    unsigned char* start_addr ; /*  pointer to the start of the image file in memory
                                    the creator of the boot_image struct is resposible
                                    for freeing the memory specified at this location */
    
    /* The addresses within the boot image memory to find the specific parts */
    
    unsigned char* header_addr; 
    unsigned char* kernel_addr;
    unsigned char* ramdisk_addr;
    unsigned char* second_addr;   
    
    /* Borrowed from AOSP Source */
    unsigned char magic[BOOT_MAGIC_SIZE];

    unsigned kernel_size;      /* size in bytes */
    unsigned kernel_phy_addr;  /* physical load addr */

    unsigned ramdisk_size;      /* size in bytes */
    unsigned ramdisk_phy_addr;  /* physical load addr */

    unsigned second_size;      /* size in bytes */
    unsigned second_phy_addr;  /* physical load addr */

    unsigned tags_phy_addr;    /* physical addr for kernel tags */
    unsigned page_size;        /* flash page size we assume */
    unsigned unused[2];        /* future expansion: should be 0 */

    unsigned char name[BOOT_NAME_SIZE]; /* asciiz product name */
    
    unsigned char cmdline[BOOT_ARGS_SIZE];

    unsigned id[8]; /* timestamp / checksum / sha1 / etc */
    
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
int write_boot_image(const char *filename, boot_image* image);
int write_boot_image_header_to_disk(const char *filename, boot_image* image);
int load_boot_image_header_from_disk(const char *filename, boot_image* image);
int set_boot_image_defaults(boot_image* image);
int set_boot_image_content_hash(boot_image* image);
int set_boot_image_padding(boot_image* image);
int set_boot_image_offsets(boot_image* image);
int print_boot_image_info(boot_image* image);
int print_boot_image_header_info(boot_image* image);
int copy_boot_image_header_info(boot_image* dest,boot_image* source);
#endif
