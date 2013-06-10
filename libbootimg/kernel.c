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



#define KERNEL_VERSION_STRING "Linux version"
#define KERNEL_VERSION_STRING_SIZE 13

#define KERNEL_IKCFG_ST "IKCFG_ST"
#define KERNEL_IKCFG_ED "IKCFG_ED"
#define KERNEL_IKCFG_SIZE 8

#define MAX_KERNEL_SIZE (8192*1024)*4
int print_kernel_info(kernel_image* kimage){
    
    //fprintf(stderr,"  kernel_addr      :%p\n",bimage.kernel_addr); 
    //fprintf(stderr,"  kernel_size      :%u\n",bimage.kernel_size); 
    int klen = 0 ;
    if(kimage->version) klen = strlen(kimage->version);
   
    fprintf(stderr,"  Compression Type   :%s\n",str_kernel_compression(kimage->compression_type));
    fprintf(stderr,"  Version            :%s",kimage->version);
    if(klen > 0 && kimage->version[klen-1]!='\n') {
        D("Adding newline kimage->version[%d]='%d'",klen,kimage->version[klen-1]);
        fprintf(stderr,"\n");
    }
    return 0;
    
}
char * uncompress_kernel_image(unsigned char* kernel_addr,unsigned kernel_size,unsigned char* compressed_kernel_offset_p,kernel_image* image){
    
    switch(image->compression_type){
        case KERNEL_COMPRESSION_GZIP: return "gzip" ; 
        case KERNEL_COMPRESSION_LZMA: return "lzma" ; 
        case KERNEL_COMPRESSION_LZO: return "lzo" ; 
        case KERNEL_COMPRESSION_XZ: return "xz" ; 
        case KERNEL_COMPRESSION_BZIP2: return "bzip2" ; 
        defaut: return "none";
    }
    return "none";

}
// get_kernel_compression_type_and_offset - returns the offset of a compressed file and 
// populates the image->compression_type parameter with the compression type found
unsigned char * get_kernel_compression_type_and_offset(unsigned char* kernel_addr,unsigned kernel_size,unsigned char* kernel_magic_offset_p,kernel_image* image){
    
    D("kernel_magic_offset_p : %p\n",kernel_magic_offset_p);
    
    // look for a gzip entry in the packed kernel image data 
    unsigned char * gzip_magic_offset_p = NULL;
    unsigned char * gzip_magic_offset_check_p = kernel_magic_offset_p;
    for(;;){
        gzip_magic_offset_check_p = find_in_memory_start_at(kernel_addr,kernel_size,gzip_magic_offset_check_p,GZIP_DEFLATE_MAGIC, GZIP_DEFLATE_MAGIC_SIZE );
        D("compression_type=GZIP gzip_magic_offset_check_p : %p gzip_magic_offset_p: %p\n",gzip_magic_offset_check_p,gzip_magic_offset_p);
        if(!gzip_magic_offset_check_p){
            D("BREAK!\n");
             break ;
         }
        
        gzip_magic_offset_p = gzip_magic_offset_check_p;
        gzip_magic_offset_check_p += 1;
       
    }
    
    
    if(gzip_magic_offset_p){
        D("compression_type=GZIP gzip_magic_offset_p : %p\n",gzip_magic_offset_p);
        image->compression_type = KERNEL_COMPRESSION_GZIP ;
        return gzip_magic_offset_p; 
    }
    // look for a lzop entry in the packed kernel image data 
    unsigned char * lzop_magic_offset_p = find_in_memory_start_at(kernel_addr,kernel_size,kernel_magic_offset_p,LZOP_MAGIC, LZOP_MAGIC_SIZE );
    if(lzop_magic_offset_p){
        D("compression_type=LZOP lzop_magic_offset_p : %p\n",lzop_magic_offset_p);
        image->compression_type = KERNEL_COMPRESSION_LZO ;
        return lzop_magic_offset_p; 
    }
    
    
    // look for a xz entry in the packed kernel image data 
    unsigned char * xz_magic_offset_p = NULL ;
    unsigned char * xz_magic_offset_check_p = kernel_magic_offset_p;
    for(;;){
        xz_magic_offset_check_p = find_in_memory_start_at(kernel_addr,kernel_size,xz_magic_offset_check_p,XZ_MAGIC, XZ_MAGIC_SIZE );
        D("compression_type=XZ xz_magic_offset_check_p : %p xz_magic_offset_p: %p\n",xz_magic_offset_check_p,xz_magic_offset_p);
        if(!xz_magic_offset_check_p){
            D("BREAK!\n");
             break ;
         }
        
        xz_magic_offset_p = xz_magic_offset_check_p;
        xz_magic_offset_check_p += 1;
       
    }  
    if(xz_magic_offset_p){
        D("compression_type=XZ xz_magic_offset_p : %p\n",xz_magic_offset_p);
        
        image->compression_type = KERNEL_COMPRESSION_XZ ;
        return xz_magic_offset_p; 
    }

    // look for a lzma entry in the packed kernel image data 
    unsigned char * lzma_magic_offset_p = find_in_memory_start_at(kernel_addr,kernel_size,kernel_magic_offset_p,LZMA_MAGIC, LZMA_MAGIC_SIZE );
    if(lzma_magic_offset_p){
        D("compression_type=LZMA lzma_magic_offset_p : %p\n",lzma_magic_offset_p);
        image->compression_type = KERNEL_COMPRESSION_LZMA ;
        return lzma_magic_offset_p; 
    }
    // look for a bzip2 entry in the packed kernel image data 
    unsigned char * bzip2_magic_offset_p = find_in_memory_start_at(kernel_addr,kernel_size,kernel_magic_offset_p,BZIP2_MAGIC, BZIP2_MAGIC_SIZE );
    if(bzip2_magic_offset_p){
        D("compression_type=BZIP2 bzip2_magic_offset_p : %p\n",bzip2_magic_offset_p);
        image->compression_type = KERNEL_COMPRESSION_BZIP2 ;
        return bzip2_magic_offset_p; 
    }
    return NULL;
     
    
}

int load_kernel_image_from_memory(unsigned char* kernel_addr,unsigned kernel_size,kernel_image* image ){
    
    // Look for the kernel zImage Magic
    int return_value = 0 ;
    
    D("kernel_addr=%p kernel_size=%u\n",kernel_addr,kernel_size);
    unsigned char * kernel_magic_offset_p = find_in_memory(kernel_addr,kernel_size,KERNEL_ZIMAGE_MAGIC, KERNEL_ZIMAGE_MAGIC_SIZE );
    if(!kernel_magic_offset_p){
        D("kernel_magic_offset not found\n")
        return_value = ENOEXEC;
        goto exit;
    
    }
    
    // Get the address that zImage starts at. 
    // This is normally found at offset 0x28 in a standard zImage
    // which is 4 bytes along from the kernel magic offset which is 
    // normally found at 0x24
    unsigned char* kernel_start_address = kernel_magic_offset_p+4;
    // zImage ends at 0x2C
    unsigned char* kernel_end_address = kernel_magic_offset_p+8;
    D("kernel_start_address=%d%d%d%d kernel_end_address=%p\n",kernel_start_address[0],kernel_start_address[1],kernel_start_address[2],kernel_start_address[3],kernel_end_address);
    
    
    
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
        
        default:
            break;
    }
    if(errno){
        D("errno: %u %s\n",errno,strerror(errno));
        free(uncompressed_kernel_data);
        return  uncompressed_kernel_size;
    }
    D("uncompressed_kernel_size : %ld\n",uncompressed_kernel_size);
    // fill in the basic values    
    image->start_addr = uncompressed_kernel_data;
    image->size = uncompressed_kernel_size;
    image->version = (char *)find_in_memory(uncompressed_kernel_data,uncompressed_kernel_size,KERNEL_VERSION_STRING,KERNEL_VERSION_STRING_SIZE);
    
    // find the first number string
    image->version_number = image->version + KERNEL_VERSION_STRING_SIZE + 1 ;
    char* first_space = strchr(image->version_number,32);
    D("first_space : %p\n",first_space);
    
    D("image->version_number  : %s\n",image->version_number);
    image->version_number_length = first_space - image->version_number;
    D("image->version_number_length : %d\n",image->version_number_length);
    
    
    // find the config.gz if there is one
    image->config_addr = find_in_memory(uncompressed_kernel_data,uncompressed_kernel_size,KERNEL_IKCFG_ST,KERNEL_IKCFG_SIZE);
    if(image->config_addr){
    
    
    // Advance the config_addr position along to the start of the gzip "file"
    image->config_addr += KERNEL_IKCFG_SIZE;
    
    // find  the end
    unsigned char * config_end = find_in_memory_start_at(uncompressed_kernel_data , uncompressed_kernel_size ,image->config_addr, KERNEL_IKCFG_ED, KERNEL_IKCFG_SIZE);
    
    // if we can't find a KERNEL_IKCFG_ED then something is very wrong
    if(!config_end){
        errno = EINVAL;
        return_value =errno;
        free(uncompressed_kernel_data);
        goto exit;
    }
    
    
    image->config_size = config_end - image->config_addr ;
    }
    
    
    
    
exit:
    return return_value;
    
    
}
char *str_kernel_compression(int compression_type){
        
    switch(compression_type){
        case KERNEL_COMPRESSION_GZIP: return "gzip" ; 
        case KERNEL_COMPRESSION_LZMA: return "lzma" ; 
        case KERNEL_COMPRESSION_LZO: return "lzo" ; 
        case KERNEL_COMPRESSION_XZ: return "xz" ; 
        case KERNEL_COMPRESSION_BZIP2: return "bzip2" ; 
        defaut: return "none";
    }
    return "none";
}
