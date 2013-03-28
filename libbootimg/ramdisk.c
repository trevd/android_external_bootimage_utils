#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <utils.h>
#include <ramdisk.h>
#include <compression.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#define CPIO_HEADER_MAGIC "070701"
#define CPIO_HEADER_MAGIC_SIZE 6

typedef struct cpio_newc_header cpio_newc_header;

struct cpio_newc_header {
       char    c_magic[6];
       char    c_ino[8];
       char    c_mode[8];
       char    c_uid[8];
       char    c_gid[8];
       char    c_nlink[8];
       char    c_mtime[8];
       char    c_filesize[8];
       char    c_devmajor[8];
       char    c_devminor[8];
       char    c_rdevmajor[8];
       char    c_rdevminor[8];
       char    c_namesize[8];
       char    c_check[8];
} ;

#define MAX_RAMDISK_SIZE (8192*1024)*4

unsigned carve_out_entry_space(ramdisk_image* image){

    errno = 0;
    if(!image){
	errno = ENOEXEC;
	return -1;
    }
    if(!image->entry_count){
	errno = EINVAL ;
	return -1;
    }
    //fprintf(stderr,"DEBUG:carve_out_entry_space\n") ;   
    unsigned i; image->entries = calloc(image->entry_count, sizeof(ramdisk_entry*));
    for(i = 0; i < image->entry_count; i++) {
	image->entries[i] = calloc(1,sizeof(ramdisk_entry));	
	if (image->entries[i] == NULL) {
		break;
	    }
    }
       
    //fprintf(stderr,"DEBUG:carve_out_entry_space complete\n") ;   
    return errno; 

}
unsigned populate_ramdisk_entries(ramdisk_image* image){

     errno = 0;
     if(!image){
	errno = ENOEXEC;
	return -1;
    }
    if(!image->entry_count){
	errno = EINVAL ;
	return -1;
    }

    unsigned counter = 0;
    unsigned char * cpio_magic_offset_p = find_in_memory( image->start_addr, image->size,CPIO_HEADER_MAGIC, CPIO_HEADER_MAGIC_SIZE );
    //fprintf(stderr,"DEBUG:populate_ramdisk_entries\n") ;   
    while(cpio_magic_offset_p){
	//fprintf(stderr,"DEBUG:populate_ramdisk_entries  %p %d\n",cpio_magic_offset_p ,sizeof(ramdisk_entry)) ;   
	
	image->entries[counter]->start_addr = cpio_magic_offset_p;
	
	cpio_newc_header* header = (cpio_newc_header*)cpio_magic_offset_p;
	
	image->entries[counter]->mode = get_long_from_hex_field(header->c_mode);
	image->entries[counter]->name_size = get_long_from_hex_field(header->c_namesize);
	image->entries[counter]->name_addr = cpio_magic_offset_p+sizeof(cpio_newc_header);
	image->entries[counter]->name_padding = ((4 - ((sizeof(cpio_newc_header) + image->entries[counter]->name_size ) % 4)) % 4);
	image->entries[counter]->data_addr = image->entries[counter]->name_addr + 	image->entries[counter]->name_size + image->entries[counter]->name_padding ;
	image->entries[counter]->data_size = get_long_from_hex_field(header->c_filesize);
	image->entries[counter]->data_padding = ((4 - ((image->entries[counter]->data_size ) % 4)) % 4);
	image->entries[counter]->next_addr = image->entries[counter]->data_addr + (image->entries[counter]->data_size + image->entries[counter]->data_padding);
	
	
	counter += 1 ;
	
	    // Get the next entry
	cpio_magic_offset_p+=CPIO_HEADER_MAGIC_SIZE;
	cpio_magic_offset_p = find_in_memory_start_at(image->start_addr,image->size,cpio_magic_offset_p,CPIO_HEADER_MAGIC, CPIO_HEADER_MAGIC_SIZE );
	
	// Double check we are not exceeding the allocated array size
	if( counter > image->entry_count) break;
    }
    //fprintf(stderr,"DEBUG:populate_ramdisk_entries complete\n") ;   
    return 0;
	    
}
/*int qsort_ramdisk_comparer(const void* a, const void* b) {

  ramdisk_entry *e1 = (ramdisk_entry *)a;
  ramdisk_entry *e2 = (ramdisk_entry *)b;
    fprintf(stderr,"DEBUG:qsort_ramdisk_comparer %p %p\n",e1->start_addr ,e2->start_addr) ;   
  return strlcmp(e1->name_addr, e2->name_addr);
  
  
}*/

unsigned count_ramdisk_archive_entries(ramdisk_image* image){
    
    unsigned return_value = 0; 
    unsigned char * cpio_magic_offset_p = find_in_memory( image->start_addr, image->size,CPIO_HEADER_MAGIC, CPIO_HEADER_MAGIC_SIZE );
    while(cpio_magic_offset_p){
	return_value += 1;
	cpio_magic_offset_p+=CPIO_HEADER_MAGIC_SIZE;
	cpio_magic_offset_p = find_in_memory_start_at(image->start_addr,image->size,cpio_magic_offset_p,CPIO_HEADER_MAGIC, CPIO_HEADER_MAGIC_SIZE );
    }
    return return_value;
    
}
int load_ramdisk_image(unsigned char* ramdisk_addr,unsigned ramdisk_size,ramdisk_image* image ){


    int return_value = 0 ;
    errno = 0;
    // look for a gzip magic to make sure the ramdisk is the correct type
    unsigned char * gzip_magic_offset_p = find_in_memory(ramdisk_addr,ramdisk_size,GZIP_DEFLATE_MAGIC, GZIP_DEFLATE_MAGIC_SIZE );
    if(!gzip_magic_offset_p){
	return_value = ENOEXEC;
	goto exit;
    }
        
    image->compression_type = RAMDISK_COMPRESSION_GZIP ;
    
    unsigned char *uncompressed_ramdisk_data = calloc(MAX_RAMDISK_SIZE,sizeof(unsigned char)) ;
    
    long uncompressed_ramdisk_size = 0;
    
    image->size = uncompress_gzip_memory(gzip_magic_offset_p,ramdisk_size,uncompressed_ramdisk_data,MAX_RAMDISK_SIZE);
   
    if(!image->size){
	free(uncompressed_ramdisk_data);
	return  image->size;
    }
    
    image->start_addr = uncompressed_ramdisk_data ;
    
    image->entry_count = count_ramdisk_archive_entries(image);
    
    if(!image->entry_count){
	free(uncompressed_ramdisk_data);
	return -1;
    }
    
    carve_out_entry_space(image);
    carve_out_entry_space(image);
    
    populate_ramdisk_entries(image);
    
    //qsort(image->entries,image->entry_count-1,sizeof(ramdisk_entry*),qsort_ramdisk_comparer);

exit:
    return return_value;
    
}
int save_ramdisk_entries_to_disk(ramdisk_image* image,unsigned char *directory_name){

    char cwd[PATH_MAX];
    errno = 0;
    if(directory_name){
	mkdir_and_parents(directory_name,0777);
	getcwd(cwd,PATH_MAX);
	chdir((char *)directory_name);
    }
    
    unsigned i = 0;
    for(i = 0 ; i < image->entry_count ; i++){
	
	write_item_to_disk_extended(image->entries[i]->data_addr,image->entries[i]->data_size,image->entries[i]->mode, image->entries[i]->name_addr,image->entries[i]->name_size);
	
    }
    
    if(directory_name){
	chdir(cwd);
    }
    return 0;
    
}
char *str_ramdisk_type(int ramdisk_type){
    
    switch(ramdisk_type){
	case RAMDISK_TYPE_UNKNOWN: return "unknown" ; 
	case RAMDISK_TYPE_NORMAL: return "android" ; 
	case RAMDISK_TYPE_RECOVERY: return "recovery" ; 
	case RAMDISK_TYPE_UBUNTU: return "ubuntu";
	default: return "unknown";
    }
    return "unknown";
}

char *str_ramdisk_compression(int compression_type){
  
  switch(compression_type){
	case RAMDISK_COMPRESSION_GZIP: return "gzip" ; 
	case RAMDISK_COMPRESSION_LZMA: return "lzma" ; 
	case RAMDISK_COMPRESSION_LZO: return "lzo" ; 
	case RAMDISK_COMPRESSION_BZIP: return "bz2";
	default: return "none";
    }
    return "unknown";
    
}
