#ifndef _2638e606_9103_11e2_b47f_5404a601fa9d
#define _2638e606_9103_11e2_b47f_5404a601fa9d

#define RAMDISK_ENTRY_DATA_NON_CONTIGOUS -1

#define RAMDISK_COMPRESSION_UNKNOWN -1
#define RAMDISK_COMPRESSION_NONE 0
#define RAMDISK_COMPRESSION_GZIP 1
#define RAMDISK_COMPRESSION_LZO 2
#define RAMDISK_COMPRESSION_LZMA 3
#define RAMDISK_COMPRESSION_BZIP 4

#define RAMDISK_TYPE_UNKNOWN -1
#define RAMDISK_TYPE_NORMAL 1
#define RAMDISK_TYPE_RECOVERY 2
#define RAMDISK_TYPE_UBUNTU 3



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
    
    unsigned compression_type;
    unsigned char* start_addr ;
    unsigned size;
    unsigned type;
    
    unsigned entry_count;

    ramdisk_entry ** entries ;

    };

int load_ramdisk_image_from_archive_file(const char *filename, ramdisk_image* image);

int load_ramdisk_image_from_cpio_file(const char *filename, ramdisk_image* image);


int load_ramdisk_image_from_cpio_memory(char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image );

int load_ramdisk_image_from_archive_memory(char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image );

int save_ramdisk_entries_to_disk(ramdisk_image* image,unsigned char *directory_name);

unsigned char *pack_ramdisk_directory(char* directory_name, unsigned *cpio_size);

int print_ramdisk_info(ramdisk_image* rimage);

char *str_ramdisk_compression(int compression_type);

char *str_ramdisk_type(int type);

int update_ramdisk_header(unsigned char*entry_addr);

#endif
