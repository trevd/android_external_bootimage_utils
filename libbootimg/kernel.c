/*
 * kernel.c
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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <utils.h>
#include <kernel.h>
#include <compression.h>

#define ASCII_SPACE 0x20 // Space decimal 32

#define KERNEL_VERSION_STRING "Linux version"
#define KERNEL_VERSION_STRING_SIZE 13

#define KERNEL_IKCFG_ST "IKCFG_ST"
#define KERNEL_IKCFG_ED "IKCFG_ED"
#define KERNEL_IKCFG_MAGIC_SIZE 8
#define KERNEL_IKCFG_MISSING 0 

#define MAX_KERNEL_CONFIG_SIZE 0x00100000 // 1048576 bytes ( 1MB )

#define KERNEL_START_ADDR_SIZE 4 // 8 

#define MAX_KERNEL_SIZE (8192*1024)*4
int print_kernel_info(kernel_image* kimage){
    
    //fprintf(stderr,"  kernel_addr      :%p\n",bimage.kernel_addr); 
    //fprintf(stderr,"  kernel_size      :%u\n",bimage.kernel_size); 
    int klen = 0 ;
    if(kimage->version) klen = strlen(kimage->version);
   
    fprintf(stderr,"  Compression Type  :%s\n",str_kernel_compression(kimage->compression_type));
    fprintf(stderr,"  Uncompressed Size :%u\n",kimage->size);
    fprintf(stderr,"  Version           :%s",kimage->version);
    if(klen > 0 && kimage->version[klen-1]!='\n') {
        D("Adding newline kimage->version[%d]='%d'",klen,kimage->version[klen-1]);
        fprintf(stderr,"\n");
    }
    if(kimage->config_size > 0 ){
        fprintf(stderr,"  config.gz Size    :%u\n",kimage->config_size);
    }
    if(kimage->rimage->size != 0 ){
        fprintf(stderr,"\n");
        fprintf(stderr," Printing Embedded Initramfs information\n\n");
            print_ramdisk_info(kimage->rimage); 
    }
    //    fprintf(stderr,"  initrd Size       :%u\n",kimage->rimage->entry_count);
    //}   
    return 0;
    
}
// get_kernel_compression_type_and_offset - returns the offset of a compressed file and 
// populates the image->compression_type parameter with the compression type found
unsigned char * get_kernel_compression_type_and_offset(unsigned char* kernel_addr,unsigned kernel_size,unsigned char* kernel_magic_offset_p,kernel_image* image){
    
    D("kernel_addr=%p kernel_size=%u\n", kernel_addr,kernel_size);
    int compression = KERNEL_COMPRESSION_NONE ;
    unsigned char * archive_magic_offset_p = find_compressed_data_in_memory_start_at(kernel_addr,kernel_size,kernel_magic_offset_p,&compression);
    if(archive_magic_offset_p){
        image->compression_type = compression ;
        return archive_magic_offset_p;
    }
    return NULL;
}
unsigned get_kernel_version_information(kernel_image* image){

        image->version_number = 0 ;
        image->version = (char *)find_in_memory(image->start_addr,image->size,KERNEL_VERSION_STRING,KERNEL_VERSION_STRING_SIZE);
        if(image->version == NULL) {
                return -1 ; 
        }
        // find the first number string
        image->version_number = image->version + KERNEL_VERSION_STRING_SIZE + 1 ;
        char* first_space = strchr(image->version_number,ASCII_SPACE);
        if ( first_space == NULL ){
                D("Error find space char image->version_number was : %s\n",image->version_number);
                image->version_number = 0 ; 
        }
        D("first_space : %p image->version_number  %p: %s\n",first_space,image->version_number,image->version_number);
        image->version_number_length = first_space - image->version_number;
        D("image->version_number_length : %d\n",image->version_number_length);
        return 0;
    
}
unsigned decompress_zImage_kernel(unsigned char* kernel_addr,unsigned kernel_size,unsigned char* kernel_magic_offset_p,kernel_image* image){
        // Probe the kernel for it's compression type
        unsigned char * compressed_kernel_offset_p = get_kernel_compression_type_and_offset(kernel_addr,kernel_size,kernel_magic_offset_p,image);
        D("compressed_kernel_offset_p %p kernel_size=%u\n",compressed_kernel_offset_p,kernel_size) ;
            
        
        unsigned char *uncompressed_kernel_data = calloc(MAX_KERNEL_SIZE,sizeof(unsigned char)) ;
        long uncompressed_kernel_size = 0;
        errno = 0 ;
        switch(image->compression_type){
                case KERNEL_COMPRESSION_GZIP:
                        uncompressed_kernel_size = uncompress_gzip_memory(compressed_kernel_offset_p,kernel_size,uncompressed_kernel_data,MAX_KERNEL_SIZE);
                        break ;
                case KERNEL_COMPRESSION_LZO:{
                    uncompressed_kernel_size = uncompress_lzo_memory(compressed_kernel_offset_p,kernel_size,uncompressed_kernel_data,MAX_KERNEL_SIZE);
                    break;
                }
                case KERNEL_COMPRESSION_XZ:{
                    uncompressed_kernel_size = uncompress_xz_memory(compressed_kernel_offset_p,kernel_size,uncompressed_kernel_data,MAX_KERNEL_SIZE);
                    break;
                }
                case KERNEL_COMPRESSION_LZMA:{
                    uncompressed_kernel_size = uncompress_xz_memory(compressed_kernel_offset_p,kernel_size,uncompressed_kernel_data,MAX_KERNEL_SIZE);
                    break;
                }
                case KERNEL_COMPRESSION_BZIP2:{
                    uncompressed_kernel_size = uncompress_bzip2_memory(compressed_kernel_offset_p,kernel_size,uncompressed_kernel_data,MAX_KERNEL_SIZE);
                    break;
                }
                default:
                    break;
        }
        if(!uncompressed_kernel_size){
                errno = ENOEXEC ;
                D("errno: %u %s\n",errno,strerror(errno));
                free(uncompressed_kernel_data);
                return  -1;
        }
        D("uncompressed_kernel_size : %ld\n",uncompressed_kernel_size);
        // fill in the basic values    
        image->start_addr = uncompressed_kernel_data;
        image->size = uncompressed_kernel_size;
        return 0 ;
        
}
unsigned locate_initrd(kernel_image* image){
        
    
        unsigned char *  initrd_magic_offset_p = image->start_addr ;
        unsigned image_search_size = image->size ; 
        if (image->config_addr != NULL ) 
                image_search_size =image->config_addr - image->start_addr  ;
        D("kernel_addr=%p image_search_size=%u\n", image->start_addr,image_search_size);
        int compression = KERNEL_COMPRESSION_NONE ;
        do{
                initrd_magic_offset_p = find_compressed_data_in_memory_start_at(image->start_addr,image_search_size,initrd_magic_offset_p,&compression);
                if ( initrd_magic_offset_p == NULL ){
                        D("Failed to find ramdisk");
                        image->ramdisk_addr = NULL ;
                        image->ramdisk_size = 0 ;
                        return 0;
                }
                if(initrd_magic_offset_p == image->config_addr ){
                       D("Config.gz found.. go again\n");
                       initrd_magic_offset_p = initrd_magic_offset_p + sizeof(image->config_addr) ;
                       image->ramdisk_addr = NULL ;
                       image->ramdisk_size = 0 ;
                       continue;
                }
                image->ramdisk_addr  = initrd_magic_offset_p ;
                image_search_size =image->size ; 
                if (image->config_addr != NULL ) 
                        image_search_size =image->config_addr - initrd_magic_offset_p  ;
                D("kimage->ramdisk_add=%p image_search_size=%u\n", image->ramdisk_addr,image_search_size);
                break;

        }while ( initrd_magic_offset_p != NULL );
        image->ramdisk_size = image_search_size ; 
        D("kimage->ramdisk_add=%p image_search_size=%u\n", image->ramdisk_addr,image_search_size);
        return 0;
}

unsigned decompress_config_gz(kernel_image* image){
        // initialize our config size to zero
        image->config_size = 0 ;
        // find the config.gz if there is one
        image->config_addr = find_in_memory(image->start_addr,image->size,KERNEL_IKCFG_ST,KERNEL_IKCFG_MAGIC_SIZE);
        D("image->config_addr=%p\n",image->config_addr);
        if(image->config_addr){
        
                // Advance the config_addr position along to the start of the gzip "file"
                image->config_addr += KERNEL_IKCFG_MAGIC_SIZE;
                
            
                // find  the end
                unsigned char * config_end = find_in_memory_start_at(image->start_addr , image->size ,image->config_addr, KERNEL_IKCFG_ED, KERNEL_IKCFG_MAGIC_SIZE);
            
                // if we can't find a KERNEL_IKCFG_ED then something is very wrong
                if(!config_end){
                        errno = EINVAL;
                        free(image->start_addr);
                        return KERNEL_IKCFG_MISSING ;
                        
                }
               
                image->config_size = config_end - image->config_addr ;
        }
        return  image->config_size;
}


unsigned save_kernel_config_gzip(kernel_image* image){
        
        if(!image->config_addr || !image->config_size) {
                return errno = EINVAL ;
        }
        write_item_to_disk(image->config_addr,image->config_size,33188,"config.gz");
        unsigned char* uncompressed_config_data = calloc(MAX_KERNEL_CONFIG_SIZE,sizeof(unsigned char)); 
        
        
        long uncompressed_config_size = uncompress_gzip_memory(image->config_addr,image->config_size,uncompressed_config_data,MAX_KERNEL_CONFIG_SIZE);
        write_item_to_disk(uncompressed_config_data,uncompressed_config_size,33188,"config");
        
        if(uncompressed_config_data != NULL) free(uncompressed_config_data);
        return 0;
}
int load_kernel_image_from_memory(unsigned char* kernel_addr,unsigned kernel_size,kernel_image* image ){
    
        
        image->size = 0 ;
        int return_value = 0 ;
        errno = 0 ;
        D("kernel_addr=%p kernel_size=%u\n",kernel_addr,kernel_size);
        // Look for a decompressed kernel first
        image->compression_type = KERNEL_COMPRESSION_NONE ;
        
        
        
        // Look for the kernel zImage Magic
        unsigned char * kernel_magic_offset_p = find_in_memory(kernel_addr,kernel_size,KERNEL_ZIMAGE_MAGIC, KERNEL_ZIMAGE_MAGIC_SIZE );
        if(kernel_magic_offset_p != NULL){
                // Get the address that zImage starts at. 
                // This is normally found at offset 0x28 ( 40 ) in a standard zImage
                // which is 4 bytes along from the kernel magic offset which is 
                // normally found at 0x24 ( 36 )
                unsigned char* kernel_start_address = kernel_magic_offset_p + KERNEL_ZIMAGE_MAGIC_SIZE;

                // zImage ends address is normally stored at 0x2C ( 44 ) in a compressed image
                unsigned char* kernel_end_address = kernel_magic_offset_p+ KERNEL_ZIMAGE_MAGIC_SIZE + KERNEL_START_ADDR_SIZE ;
                D("kernel_start_address=%d%d%d%d kernel_end_address=%p\n",kernel_start_address[0],kernel_start_address[1],kernel_start_address[2],kernel_start_address[3],kernel_end_address);

                decompress_zImage_kernel(kernel_addr,kernel_size,kernel_magic_offset_p,image);
                if (errno > 0 ) 
                        return errno ;  
                
                //image->start_addr = NULL; 
                //D("kernel_magic_offset not found\n")
                //return_value = errno = ENOEXEC;
                
        }else {
                kernel_magic_offset_p = find_in_memory(kernel_addr,kernel_size,KERNEL_IMAGE_MAGIC, KERNEL_IMAGE_MAGIC_SIZE );
                if(kernel_magic_offset_p == NULL){
                        
                        image->start_addr = NULL; 
                        D("kernel_magic_offset not found\n")
                        return errno = ENOEXEC;
                        
                }
                D("kernel_magic_offset uncompressed image?!? %p\n",kernel_magic_offset_p);
                image->start_addr = kernel_addr ;
                image->size = kernel_size ;
        }
        
        
        D("kernel_magic_offset compressed image %p\n",kernel_magic_offset_p);
        get_kernel_version_information(image);
        
        decompress_config_gz(image);
        locate_initrd(image);
        
        image->rimage = calloc(1,sizeof(ramdisk_image)) ;
        if(image->ramdisk_size > 0 ){
                load_ramdisk_image_from_archive_memory(image->ramdisk_addr,image->ramdisk_size,image->rimage);
                D("rp->entry_count=%u\n",image->rimage->entry_count);
        }
    
exit:
    return return_value;
    
    
}
char *str_kernel_compression(unsigned compression){
        
    if(compression == 0)
        return "none";
    
    char* return_value = get_compression_name_from_index(compression);
    
    if(errno > 0 ) {
        errno = 0 ; 
        return "none";
    }
    return return_value;
}
