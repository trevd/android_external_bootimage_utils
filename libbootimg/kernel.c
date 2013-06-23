/*
 * kernel.c - part of the libbootimage project
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
 
 /* 
  *     This file contains the public api for kernel file handling
  *     
  *     Function Prefix: biki_
  *     
  *     
  */ 
 
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <utils.h>
#include <kernel.h>
#include <ramdisk.h>
#include <compression.h>

#define ASCII_SPACE 0x20 // Space decimal 32

#define KERNEL_VERSION_STRING "Linux version"
#define KERNEL_VERSION_STRING_SIZE 13

#define KERNEL_IKCFG_ST "\x49\x4B\x43\x46\x47\x5F\x53\x54" // IKCFG_ST
#define KERNEL_IKCFG_ED "IKCFG_ED"
#define KERNEL_IKCFG_MAGIC_SIZE 8
#define KERNEL_IKCFG_MISSING 0 

#define MAX_KERNEL_CONFIG_SIZE 0x00100000 // 1048576 bytes ( 1MB )

#define KERNEL_START_ADDR_SIZE 4 // 8 

#define MAX_KERNEL_SIZE (8192*1024)*4

char *biki_compression_type_string(unsigned compression){
        
    if(compression == 0)
        return "none";
    
    char* return_value = get_compression_name_from_index(compression);
    
    if(errno > 0 ) {
        errno = 0 ; 
        return "none";
    }
    return return_value;
}

unsigned biki_write_details(const char *filename , kernel_image* kimage){
    
    //fprintf(stderr,"  kernel_addr      :%p\n",bimage.kernel_addr); 
    //fprintf(stderr,"  kernel_size      :%u\n",bimage.kernel_size); 
    int klen = 0 ;
    if(kimage->version) klen = strlen(kimage->version);
   
    fprintf(stderr,"  Compression Type  :%s\n",biki_compression_type_string(kimage->compression_type));
    fprintf(stderr,"  Uncompressed Size :%u\n",kimage->size);
    fprintf(stderr,"  Version           :%s",kimage->version);
    if(klen > 0 && kimage->version[klen-1]!='\n') {
        D("Adding newline kimage->version[%d]='%d'",klen,kimage->version[klen-1]);
        fprintf(stderr,"\n");
    }
    if(kimage->config_size > 0 ){
        fprintf(stderr,"  config.gz Size    :%u\n",kimage->config_gz_size);
    }
    if( kimage->ramdisk_size > 0 ){
         fprintf(stderr," Compressed Embedded Ramdisk Size :%u\n",kimage->ramdisk_size);   
    }
    //if(kimage-> != 0 ){
    //    fprintf(stderr,"\n");
    //    fprintf(stderr," Printing Embedded Initramfs information\n\n");
    //        print_ramdisk_info(kimage->rimage); 
    //}
    //    fprintf(stderr,"  initrd Size       :%u\n",kimage->rimage->entry_count);
    //}   
    return 0;
    
}
static unsigned long biki_kernel_version_information(kernel_image* image){

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
static unsigned biki_locate_initrd(kernel_image* image){
        
        D("kimage=%p\n",image->start_addr);
        unsigned long search_size = image->size;
        // search uptil to compressed config if preset
        if (image->config_gz_addr != NULL)
                search_size = image->config_gz_addr- image->start_addr  ;
        
        
        compression_helper helper ;
        
        helper.uncompressed_data_start = calloc(MAX_RAMDISK_SIZE,sizeof(unsigned char)) ;
        helper.uncompressed_data_size = MAX_RAMDISK_SIZE ;
         
        
        if(find_compressed_data_in_memory_start_at(image->start_addr,search_size,image->start_addr,&helper)!=0){
                // we might have a ramdisk here 
                image->ramdisk_addr = helper.compressed_data_start;
                image->ramdisk_size = helper.compressed_data_size;
                image->ramdisk_compression_type = helper.compression_type ;
                //biki_rd_read(image,image->ramdisk);
                D("kimage->ramdisk_addr=%p kimage->ramdisk_size=%ul\n", image->ramdisk_addr,image->ramdisk_size);
                return 0;
        }
        
        if(search_size !=  image->size){
                unsigned char* newstart = image->config_gz_addr + image->config_gz_size;
                //search_size = newstart - image->size ;
                 
                if(find_compressed_data_in_memory_start_at(image->start_addr,image->size,newstart, &helper) > 0 ){
                        // we might have a ramdisk here 
                        image->ramdisk_addr = helper.compressed_data_start;
                        image->ramdisk_size = helper.compressed_data_size;
                        D("kimage->ramdisk_addr=%p kimage->ramdisk_size=%ul\n", image->ramdisk_addr,image->ramdisk_size);
                }
        }
         return 0;     
        
}
static unsigned long biki_decompress_zImage(unsigned char* kernel_addr,unsigned kernel_size,unsigned char* kernel_magic_offset_p,kernel_image* image){
            
    D("kernel_addr=%p kernel_size=%u\n", kernel_addr,kernel_size);
    compression_helper helper ;
    helper.compression_type = KERNEL_COMPRESSION_NOT_SET ;
    helper.compressed_data_start = kernel_addr ;
    helper.compressed_data_size = kernel_size ;
    helper.uncompressed_data_start = calloc(MAX_KERNEL_SIZE,sizeof(unsigned char)) ;
    helper.uncompressed_data_size = MAX_KERNEL_SIZE ;
    
    if(find_compressed_data_in_memory_start_at(kernel_addr,kernel_size,kernel_magic_offset_p, &helper)> 0){

        image->compression_type = helper.compression_type ;
        image->start_addr = helper.uncompressed_data_start ;
        image->size = helper.uncompressed_data_size ;
        return helper.uncompressed_data_size;
    } 
        if(!helper.uncompressed_data_size){
                errno = ENOEXEC ;
                D("errno: %u %s\n",errno,strerror(errno));
                free(helper.uncompressed_data_start);
                return  -1;
        }
        D("uncompressed_kernel_size : %lu\n",helper.uncompressed_data_size);
        // fill in the basic values    
        image->start_addr = helper.uncompressed_data_start;
        image->size = helper.uncompressed_data_size;
        return 0 ;
        
}
unsigned biki_write_ramdisk(const char* filename ,kernel_image* image)
{
        return write_item_to_disk(image->ramdisk_addr,image->ramdisk_size,33188,filename);
}

unsigned biki_write_config_gzip(const char* filename ,kernel_image* image)
{       D("image->config_gz_size=%d\n",image->config_gz_size);
        return write_item_to_disk(image->config_gz_addr,image->config_gz_size,33188,filename);
}
unsigned biki_write_config(const char* filename ,kernel_image* image)
{
        D("uncompressed_config_size=%ul\n",image->config_size);
        write_item_to_disk(image->config_addr,image->config_size,33188,filename);
        return 0;
}
unsigned biki_write(const char* filename ,kernel_image* image)
{       D("\nkimage->size=%u\nkimage->start_addr=%p\n",image->size,image->start_addr);     
        return write_item_to_disk(image->start_addr, image->size,33188,filename);
}

unsigned biki_rd_read(kernel_image* image,ramdisk_image* rimage ){
        return bird_read(image->ramdisk_addr,image->ramdisk_size,rimage);
        
}
unsigned biki_read(unsigned char* kernel_addr,unsigned kernel_size,kernel_image* image ){
    
        int return_value = 0 ;
        errno = 0;
        if(image == NULL || kernel_addr == NULL) {
                return errno = EFAULT ;
        }
        
        
        image->size =   kernel_size ;
        image->start_addr       = kernel_addr ;
        image->ramdisk_addr     = NULL ;
        image->config_gz_addr   = NULL ;
        image->config_addr      = NULL ;
        
        image->version          = NULL;
        image->version_number   = NULL ;
        image->version_number_length =0;
        
        image->compression_type = KERNEL_COMPRESSION_NONE ;
        image->size             = 0;
        image->config_gz_size   = 0;
        image->config_size      = 0;
        image->ramdisk_size     = 0;
        
        
        
        
        
        errno = 0 ;
        D("kernel_addr=%p kernel_size=%u\n",kernel_addr,kernel_size);
        // Look for a decompressed kernel first
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

                biki_decompress_zImage(kernel_addr,kernel_size,kernel_magic_offset_p,image);
                
                D("Start Address=%p\n",image->start_addr);
                D("Size=%u\n",image->size);
                if (image->start_addr == NULL ) 
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
        image->config_gz_addr = find_in_memory(image->start_addr,image->size,KERNEL_IKCFG_ST,KERNEL_IKCFG_MAGIC_SIZE);  
        if(image->config_gz_addr){
                unsigned char * config_gz_end = find_in_memory_start_at(image->start_addr , image->size ,image->config_gz_addr, KERNEL_IKCFG_ED, KERNEL_IKCFG_MAGIC_SIZE);
                if(config_gz_end == NULL)
                        return errno    ;
                
                image->config_gz_addr += KERNEL_IKCFG_MAGIC_SIZE;
                image->config_gz_size = config_gz_end - image->config_gz_addr ;
                unsigned char * uncompressed_config_data = calloc(MAX_KERNEL_CONFIG_SIZE,sizeof(unsigned char)) ; 
                unsigned long uncompressed_config_size = uncompress_gzip_memory(image->config_gz_addr,image->config_gz_size,uncompressed_config_data,MAX_KERNEL_CONFIG_SIZE);
                D("Uncompressed Config Size=%lu\n",uncompressed_config_size);
                if(uncompressed_config_size > 0){
                        image->config_size = uncompressed_config_size ;
                        image->config_addr = uncompressed_config_data ;  
                }
                        
        }
                
        
       // D("kernel_magic_offset compressed image %p\n",kernel_magic_offset_p);
        biki_kernel_version_information(image);
        
        D("config_gz_size=%u\n",image->config_gz_size);
        D("image->config_size=%u\n",image->config_size);
        
        biki_locate_initrd(image);
        //decompress_config_gz(image);
        //ocate_initrd(image);
        //image->ramdisk_size = 0 ;
        //image->rimage = calloc(1,sizeof(ramdisk_image)) ;
        //if(image->ramdisk_size > 0 ){
        //        bird_read(image->ramdisk_addr,image->ramdisk_size,image->rimage);
         //       D("rp->entry_count=%u\n",image->rimage->entry_count);
        //}
    
exit:
    return return_value;
    
    
}

