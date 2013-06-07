/*
 * bootimage.c
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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <bootimage.h>
#include <utils.h>

// include the android boot image header from the mkbootimg
// in the Android AOSP sources ( /system/core/mkbootimg/bootimg.h )
#include <bootimg.h>

// include the android sha header used by libmincrypt
// in the Android AOSP sources ( /system/core/libmincrypt/sha.h )
#include <sha.h>


// Private defines
#ifndef BOOT_IMAGE_SIZE_MAX
#define BOOT_IMAGE_SIZE_MAX (8192*1024)*4
#endif


// The current maximum page size as used by google's mkbootimg program
#ifndef MAXIMUM_KNOWN_PAGE_SIZE
#define MAXIMUM_KNOWN_PAGE_SIZE 16384
#endif

// Allow for "Non Standard" Pagesizes used by some images
// The largest non standard pagesize witnessed in the wild is 
// 131072 bytes ( 0x00020000 ), Allowing 1MB should accomadate any
// future expansion for now
#ifndef MAXIMUM_ALLOWED_PAGE_SIZE
#define MAXIMUM_ALLOWED_PAGE_SIZE 0x00100000 // 1048576 bytes ( 1MB )
#endif


static unsigned char padding[MAXIMUM_KNOWN_PAGE_SIZE] = { 0, };

static size_t calculate_padding(size_t size,unsigned page_size){
    unsigned pagemask = page_size - 1;
    size_t padding =page_size - (size & pagemask);
    if(padding==page_size) padding =0 ; 
    return padding ; 
}
// set_boot_image_defaults - when creating a new boot image
unsigned set_boot_image_defaults(boot_image* image){
    
    D("set_boot_image_defaults\n") ;
    
    image->header = calloc(1,sizeof(boot_img_hdr));
    
    memcpy(image->header->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE);
    
    image->header->kernel_addr = 0x10008000;
    
    image->header->ramdisk_addr = 0x11000000;

    image->header->second_addr = 0x10f00000;
    
    image->header->tags_addr = 0x10000100;
    
    image->header->page_size = 2048;
    
    image->header_offset = 0;
    //image->header = image->header->magic;
    
    image->start_addr = (unsigned char *) image->header;
    
    image->header_size = sizeof(boot_img_hdr);
    image->header_padding = calculate_padding(image->header_size,image->header->page_size);
    
    image->header->second_size = 0;
    image->second_addr = NULL;
    image->second_padding = 0;
    
    image->header->ramdisk_size = 0;
    image->header->kernel_size = 0;
    image->kernel_addr = NULL;
    image->ramdisk_addr = NULL;
    image->kernel_offset = image->header->page_size;
    
    image->header->name[0] = '\0';
    image->header->cmdline[0] = '\0';
    memset(image->header->id,0,8*sizeof(unsigned));
    
    return 0;   
    
} 


// set_boot_image_padding - work out the padding for each section
// Padding is required because boot images are page aligned 
unsigned set_boot_image_padding(boot_image* image){
    
    image->ramdisk_padding = calculate_padding(image->header->ramdisk_size,image->header->page_size);
    
    image->header_padding = calculate_padding(image->header_size,image->header->page_size);
    
    image->kernel_padding = calculate_padding(image->header->kernel_size,image->header->page_size);
    
    if(image->header->second_size > 0)
        image->second_padding = calculate_padding(image->header->second_size,image->header->page_size); 
        
    return 0;
}
// set_boot_image_offsets - set the offsets in the image when creating a new image
// Note: kernel size and ramdisk size need to be set prior to calling this function
unsigned set_boot_image_offsets(boot_image*image)
{
    
    image->kernel_offset = image->header_offset + image->header->page_size;
    image->ramdisk_offset = image->kernel_offset + image->header->kernel_size + image->kernel_padding;
    if(image->header->second_size > 0){
        image->second_offset = image->ramdisk_offset + image->header->ramdisk_size + image->ramdisk_padding;
    }else{
        image->second_offset = -1;
    }
    
    return 0;
}

unsigned set_boot_image_content_hash(boot_image* image)
{
    SHA_CTX ctx;
    SHA_init(&ctx);
    D("hash:kaddr %p ksize:%u\n",image->kernel_addr,image->header->kernel_size);
    SHA_update(&ctx, image->kernel_addr, image->header->kernel_size);
    SHA_update(&ctx, &image->header->kernel_size, sizeof(image->header->kernel_size));
    D("hash:raddr %p rsize:%u\n",image->ramdisk_addr,image->header->ramdisk_size);
    SHA_update(&ctx, image->ramdisk_addr, image->header->ramdisk_size);
    SHA_update(&ctx, &image->header->ramdisk_size, sizeof(image->header->ramdisk_size));
    SHA_update(&ctx, image->second_addr, image->header->second_size);
    SHA_update(&ctx, &image->header->second_size, sizeof(image->header->second_size));
    const uint8_t* sha = SHA_final(&ctx);
    memcpy(&image->header->id, sha, SHA_DIGEST_SIZE > sizeof(image->header->id) ? sizeof(image->header->id) : SHA_DIGEST_SIZE);
    return 0;
}
unsigned print_boot_image_additional_info(boot_image* image){
    
    
    fprintf(stderr,"  total_size       :%08u\n",image->total_size);
    fprintf(stderr,"  header_size      :%08u\n\n",image->header_size);
    
    fprintf(stderr,"  header_offset    :%08u\n",image->header_offset);
    fprintf(stderr,"  header_padding   :%08u\n\n",image->header_padding);

    fprintf(stderr,"  kernel_offset    :%08u\n",image->kernel_offset);
    fprintf(stderr,"  kernel_padding   :%08u\n\n",image->kernel_padding);
    
    fprintf(stderr,"  ramdisk_offset   :%08u\n",image->ramdisk_offset);
    fprintf(stderr,"  ramdisk_padding  :%08u\n",image->ramdisk_padding);
    
    if(image->header->second_size > 0){
        fprintf(stderr,"  second_offset    :%08d\n",image->second_offset);
        fprintf(stderr,"  second_padding   :%08d\n",image->second_padding);
    }
    return 0;
    }
unsigned print_boot_image_header_info(boot_image* image){

    fprintf(stderr,"  kernel_size      :%u\n",image->header->kernel_size);
    fprintf(stderr,"  kernel_addr      :0x%08x\n",image->header->kernel_addr);
    fprintf(stderr,"  ramdisk_size     :%u\n",image->header->ramdisk_size);
    fprintf(stderr,"  ramdisk_addr     :0x%08x\n",image->header->ramdisk_addr);
    fprintf(stderr,"  second_size      :%u\n",image->header->second_size);
    fprintf(stderr,"  second_addr      :0x%08x\n",image->header->second_addr);
    fprintf(stderr,"  tags_addr        :0x%08x\n",image->header->tags_addr);
    fprintf(stderr,"  page_size        :%u\n",image->header->page_size);
    fprintf(stderr,"  name             :%s\n",image->header->name);
    fprintf(stderr,"  cmdline          :%s\n",image->header->cmdline);
    return 0;
}
unsigned print_boot_image_header_hashes(boot_image* image){

    fprintf(stderr,"  id[0]            :%u\n",image->header->id[0]);
    fprintf(stderr,"  id[1]            :%u\n",image->header->id[1]);
    fprintf(stderr,"  id[2]            :%u\n",image->header->id[2]);
    fprintf(stderr,"  id[3]            :%u\n",image->header->id[3]);
    fprintf(stderr,"  id[4]            :%u\n",image->header->id[4]);
    fprintf(stderr,"  id[5]            :%u\n",image->header->id[5]);
    fprintf(stderr,"  id[6]            :%u\n",image->header->id[6]);
    fprintf(stderr,"  id[7]            :%u\n",image->header->id[7]);
    return 0;
}



unsigned write_boot_image_header_to_disk(const char *filename, boot_image* image){
    
    errno = 0 ;
    FILE * header_file = fopen(filename,"w");
    if(!header_file){
        return errno;
    }
    
    if(header_file){
        
        fprintf(header_file,"kernel_size:%u"EOL"kernel_address:0x%08x"EOL"ramdisk_size:%u"EOL"ramdisk_address:0x%08x"EOL"second_size:%u"EOL"second_address:0x%08x"EOL
        "tags_address:0x%08x"EOL"page_size:%u"EOL"name:%s"EOL"cmdline:%s"EOL,
        image->header->kernel_size,image->header->kernel_addr,image->header->ramdisk_size,image->header->ramdisk_addr,
        image->header->second_size,image->header->second_addr,image->header->tags_addr,image->header->page_size,image->header->name,image->header->cmdline);
        
        /*unsigned fn = fileno(header_file);
        
        if ( fn == -1 ) return errno;
                
        fsync(fn);*/
        fclose(header_file);
        
        errno = 0 ;
    }
    return errno;
}
unsigned load_boot_image_header_from_disk(const char *filename, boot_image* image){
    
    D("filename=%s\n",filename);
    FILE * header_file = fopen(filename,"r");
    if(header_file){
        char line[256];
        while ( fgets ( line, sizeof line, header_file ) ) {
            
            if(!memcmp("kernel_address:",line,15)){
                //fprintf(stderr,"%d %s\n",strlen(line+17),line+17);
                char *value = line+17;
                image->header->kernel_addr = strtol(value,NULL,16 ); /* write the line */
                D("kernel address=x%08x\n",image->header->kernel_addr);
            }
            if(!memcmp("ramdisk_address:",line,16)){
                image->header->ramdisk_addr = strtol(line+16,NULL,16 ); /* write the line */
            }
            if(!memcmp("tags_address:",line,13)){
                image->header->tags_addr = strtol(line+15,NULL,16 ); /* write the line */
            }
            if(!memcmp("second_address:",line,15)){
                image->header->second_addr = strtol(line+17,NULL,16 ); /* write the line */
            }
            if(!memcmp("page_size:",line,10)){
                //fprintf(stderr,"page_size: %s\n",line+10);
                image->header->page_size = strtol(line+10,NULL,10 ); /* write the line */
            }
            if(!memcmp("name:",line,5)){
                memcpy(image->header->name,line+5,strlen(line+5));
                image->header->name[strlen(line+5)-1]='\0';
            }
            if(!memcmp("cmdline:",line,8)){
                memcpy(image->header->cmdline,line+8,strlen(line+8));
                image->header->cmdline[strlen(line+8)-1]='\0';
                //fprintf(stderr,"%d %d %s %s\n",strlen(line+8),strlen(image->cmdline),line+8 , image->cmdline);
                
            }
        
        }
        fclose(header_file);
    }
    
    return 0;
}
/* load_boot_image - load android boot image into memory 
 * 
 * returns zero when successful, return errno on failure
 * */
unsigned load_boot_image_from_file(const char *filename, boot_image* image){

    errno = 0;
    unsigned boot_image_size = 0;
    D("filename=%s\n",filename);
    unsigned char* boot_image_addr = read_item_from_disk(filename,&boot_image_size);
    if(!boot_image_addr){
        return errno;
        
    }
    D("boot_image_addr %p %u\n",boot_image_addr,boot_image_size);
    unsigned return_value = load_boot_image_from_memory(boot_image_addr,boot_image_size,image);
    //free(boot_image_addr);
    
    return  return_value;
    
    // Look for the Android Boot Magic
}
unsigned load_boot_image_from_memory(unsigned char* boot_image_addr,unsigned boot_image_size, boot_image* image){


    D("boot_image_size=%u\n",boot_image_size);
    unsigned char * magic_offset_p = find_in_memory(boot_image_addr,boot_image_size,BOOT_MAGIC, BOOT_MAGIC_SIZE );
    if(!magic_offset_p){
        image->start_addr = NULL;
        errno = ENOEXEC;
        return ENOEXEC;
    }
    
    // set the image start to be a pointer to the data buffer in memory
    image->start_addr = boot_image_addr;
    
    // set the image total_size equal to the filesize
    image->total_size = boot_image_size;
    
    // Populate the AOSP boot_img_hdr struct from the magic offset
    // then we can jiggery pokery the start of the header to the image magic
    // Work out the header values
    image->header_size = sizeof(boot_img_hdr);
    image->header_offset = magic_offset_p - boot_image_addr; 
     
    image->header =(boot_img_hdr*) boot_image_addr + image->header_offset;
    image->header_padding = calculate_padding(image->header_size,image->header->page_size); 
    
    // Work out the kernel values   
    image->kernel_offset = image->header_offset + image->header->page_size;
    image->kernel_addr =  boot_image_addr + image->kernel_offset ;
    image->kernel_padding = calculate_padding(image->header->kernel_size,image->header->page_size);
    
    // Work out the ramdisk values
    image->ramdisk_offset = image->kernel_offset + image->header->kernel_size + image->kernel_padding;
    image->ramdisk_addr = boot_image_addr + image->ramdisk_offset;
    image->ramdisk_padding = calculate_padding(image->header->ramdisk_size,image->header->page_size);
    
    // Work out the second values
    if(image->header->second_size > 0){
        image->second_offset = image->ramdisk_offset + image->header->ramdisk_size + image->ramdisk_padding;
        image->second_addr = boot_image_addr + image->second_offset ;
        image->second_padding = calculate_padding(image->header->second_size,image->header->page_size);
    }else{
        image->second_offset = -1;
        image->second_addr = NULL ;
        image->second_padding = -1;
    }
    return 0;
    

    
}


unsigned write_boot_image(char *filename,boot_image* image){
    
    errno = 0;
    FILE* boot_image_file_fp = fopen(filename,"w+b");
    if(!boot_image_file_fp)
        return errno;
    //memcpy(image->magic,"TWAT", 4);
    D("writing boot image %s header_size %u\n",filename,image->header_size);
    D("writing boot image %p\n",image->header);
    //boot_img_hdr hdr;
    
    //memcpy(&hdr,image->header,sizeof(boot_img_hdr));
    
//  fprintf(stderr,"writing boot image %s header_size %u\n  ",filename,image->header->kernel_size);
    
    if(fwrite(image->header,1,image->header_size,boot_image_file_fp) !=  image->header_size) goto fail;
    D("writing boot image %p\n",image->header);
    //fclose(boot_image_file_fp);
    //return 0;
    
    if(fwrite(padding,1,image->header_padding,boot_image_file_fp) != image->header_padding) goto fail;
    

    
    if(fwrite(image->kernel_addr,1,image->header->kernel_size,boot_image_file_fp) !=  image->header->kernel_size) goto fail;
    
    //fprintf(stderr,"writing boot image kernel size %u %p\n",image->header->kernel_size,image->kernel_addr);
    
    if(image->kernel_padding > 0 )
        if(fwrite(padding,1,image->kernel_padding,boot_image_file_fp) != image->kernel_padding) goto fail;
    
    if(fwrite(image->ramdisk_addr,1,image->header->ramdisk_size,boot_image_file_fp) !=  image->header->ramdisk_size) goto fail;
    
    if(image->ramdisk_padding > 0 )
        if(fwrite(padding,1,image->ramdisk_padding,boot_image_file_fp) != image->ramdisk_padding) goto fail;    
    
    if(image->header->second_size){
        if(fwrite(image->second_addr,1,image->header->second_size,boot_image_file_fp) !=  image->header->second_size) goto fail;
    
        if(image->second_padding > 0 )  
            if(fwrite(padding,1,image->second_padding,boot_image_file_fp) != image->second_padding) goto fail;
    }
    fclose(boot_image_file_fp);
    return 0;
fail:
    D("write_boot_image failed %d\n",errno);
    fclose(boot_image_file_fp);
    return errno;   
}
