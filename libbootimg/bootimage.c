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
#include <string.h>
#include <sys/stat.h>
#include <bootimage.h>
#include <utils.h>

// include api error header
#include <bitapi_error.h>

// include the android boot image header from the mkbootimg
// in the Android AOSP sources ( /system/core/mkbootimg/bootimg.h )
#include <bootimg.h>

// include the android sha header used by libmincrypt
// in the Android AOSP sources ( /system/core/libmincrypt/sha.h )
#include <sha.h>


// Private defines
// Header labels and sizes
#define HEADER_LABEL_KERNEL_ADDRESS "kernel_address:0x"
#define HEADER_LABEL_KERNEL_ADDRESS_LENGTH 17 // strlen( HEADER_LABEL_KERNEL_ADDRESS )
#define HEADER_LABEL_RAMDISK_ADDRESS "ramdisk_address:0x"
#define HEADER_LABEL_RAMDISK_ADDRESS_LENGTH 18 // strlen( HEADER_LABEL_RAMDISK_ADDRESS )
#define HEADER_LABEL_SECOND_ADDRESS "second_address:0x"
#define HEADER_LABEL_SECOND_ADDRESS_LENGTH 17 // strlen( HEADER_LABEL_SECOND_ADDRESS )
#define HEADER_LABEL_TAGS_ADDRESS "tags_address:0x"
#define HEADER_LABEL_TAGS_ADDRESS_LENGTH 15 // strlen( HEADER_LABEL_TAGS_ADDRESS )
#define HEADER_LABEL_PAGE_SIZE "page_size:"
#define HEADER_LABEL_PAGE_SIZE_LENGTH 10 // strlen( HEADER_LABEL_PAGE_SIZE )
#define HEADER_LABEL_NAME "name:"
#define HEADER_LABEL_NAME_LENGTH 5 // strlen( HEADER_LABEL_NAME )
#define HEADER_LABEL_CMDLINE "cmdline:"
#define HEADER_LABEL_CMDLINE_LENGTH 8 // strlen( HEADER_LABEL_CMDLINE )

#define NUMBER_BASE_HEX 16
#define NUMBER_BASE_DECIMAL 10


#ifndef BOOT_IMAGE_SIZE_MAX
#define BOOT_IMAGE_SIZE_MAX (8192*1024)*4
#endif

// The current maximum page size that has been seen in the wild
// 16384 - is used by google's mkbootimg program
// [2013-06-01] 131072 - seen on various oem images


#ifndef MAXIMUM_KNOWN_PAGE_SIZE
#define MAXIMUM_KNOWN_PAGE_SIZE 131072
#endif

// Allow for "Non Standard" Pagesizes used by some images
// The largest non standard pagesize witnessed in the wild is 
// 131072 bytes ( 0x00020000 ), Allowing 1MB should accomadate any
// future expansion for now
#ifndef MAXIMUM_ALLOWED_PAGE_SIZE
#define MAXIMUM_ALLOWED_PAGE_SIZE 0x00100000 // 1048576 bytes ( 1MB )
#endif

// Assign the 1MB ( MAXIMUM_KNOWN_PAGE_SIZE )
static unsigned char padding[MAXIMUM_KNOWN_PAGE_SIZE] = { 0, };

// calculate_padding - internal function to calculate the padding
// if any required by to page align a boot image section
// section_size - the size of a section without padding
// page_size - the size of the specified boot image page
// 
// Returns the number of bytes required to padding the section
// to the next page boundary on successful, Zero and sets the errno
// on failure. 
//
// Callers must check errno for failure as Zero is a valid padding size
static size_t calculate_padding( size_t section_size, unsigned page_size){
    
    errno = 0 ;  // clear the errno
    if(page_size == 0 || section_size == 0){ 
        D("Calculate Padding Error %d\n",errno);
        errno = EINVAL ;
        return  0 ;
    }
    unsigned pagemask = page_size - 1;
    size_t padding_size = page_size - (section_size & pagemask);
    if(padding_size == page_size) padding_size = 0 ; 
    D("Calculate Padding Returns %d\n",padding_size);
    return padding_size ; 
}
// set_boot_image_defaults - when creating a new boot image
unsigned set_boot_image_defaults(boot_image* image){    
    
     D("image: %p size:%d\n",image,sizeof(image)) ;
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
    D("header: %p %d\n",image->header,image->header_size) ;
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
    
    errno = 0 ;
    
    image->ramdisk_padding = calculate_padding(image->header->ramdisk_size,image->header->page_size);
    if(errno != 0) return errno ;
        
    image->header_padding = calculate_padding(image->header_size,image->header->page_size);
    if(errno != 0) return errno ;
    
    image->kernel_padding = calculate_padding(image->header->kernel_size,image->header->page_size);
    if(errno != 0) return errno ;
    
    if(image->header->second_size > 0){
        image->second_padding = calculate_padding(image->header->second_size,image->header->page_size); 
        if(errno != 0) return errno ;
    }
    return 0;
}
// set_boot_image_offsets - set the offsets in the image when creating a new image
// Note: kernel size and ramdisk size need to be set prior to calling this function
unsigned set_boot_image_offsets(boot_image*image)
{   
    errno = 0;
    if(!image || !image->header || !image->header->ramdisk_size || !image->header->kernel_size) {
        errno = EINVAL;
        return 1 ;
    }
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
    SHA_update(&ctx, image->kernel_addr, image->header->kernel_size);
    SHA_update(&ctx, &image->header->kernel_size, sizeof(image->header->kernel_size));
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

    fprintf(stderr,"  kernel_size      :%u\n",      image->header->kernel_size);
    fprintf(stderr,"  kernel_addr      :0x%08x\n",  image->header->kernel_addr);
    fprintf(stderr,"  ramdisk_size     :%u\n",      image->header->ramdisk_size);
    fprintf(stderr,"  ramdisk_addr     :0x%08x\n",  image->header->ramdisk_addr);
    fprintf(stderr,"  second_size      :%u\n",      image->header->second_size);
    fprintf(stderr,"  second_addr      :0x%08x\n",  image->header->second_addr);
    fprintf(stderr,"  tags_addr        :0x%08x\n",  image->header->tags_addr);
    fprintf(stderr,"  page_size        :%u\n",      image->header->page_size);       
    fprintf(stderr,"  name             :%.*s\n",    BOOT_IMAGE_NAME_SIZE,image->header->name);
    fprintf(stderr,"  cmdline          :%.*s\n",    BOOT_IMAGE_ARGS_SIZE,image->header->cmdline);
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
        
        fprintf(header_file,"kernel_size:%u"EOL,image->header->kernel_size);
        fprintf(header_file,HEADER_LABEL_KERNEL_ADDRESS"%08x"EOL,image->header->kernel_addr); 
        fprintf(header_file,"ramdisk_size:%u"EOL,image->header->ramdisk_size);
        fprintf(header_file,HEADER_LABEL_RAMDISK_ADDRESS"%08x"EOL,image->header->ramdisk_addr);
        fprintf(header_file,"second_size:%u"EOL,image->header->second_size);
        fprintf(header_file,HEADER_LABEL_RAMDISK_ADDRESS"%08x"EOL,image->header->second_addr);
        fprintf(header_file,HEADER_LABEL_TAGS_ADDRESS"%08x"EOL,image->header->tags_addr);
        fprintf(header_file,HEADER_LABEL_PAGE_SIZE"%u"EOL,image->header->page_size);
        fprintf(header_file,HEADER_LABEL_NAME"%.*s"EOL,BOOT_IMAGE_NAME_SIZE,image->header->name);
        fprintf(header_file,HEADER_LABEL_CMDLINE"%.*s"EOL,BOOT_IMAGE_ARGS_SIZE,image->header->cmdline);
       
        
        
        /*unsigned fn = fileno(header_file);
        
        if ( fn == -1 ) return errno;
                
        fsync(fn);*/
        fclose(header_file);
        
        errno = 0 ;
    }
    return errno;
}

// load_boot_image_header_from_disk - populates a boot_img_hdr which is pointed to by image->header 
// structure using the filename supplied.
// 
// NOTES: image must be a valid to a boot_image structure
unsigned load_boot_image_header_from_disk(const char *filename, boot_image* image){
    
    errno = 0 ;
    if((image == NULL) || (image->header == NULL )){
        errno = BITAPI_ERROR_INVAL ;
        return 0;
    }
    
    D("filename=%s\n",filename);
    FILE * header_file = fopen(filename,"r");
    if(header_file){
        char line[1024] ; 
        while ( fgets ( line, sizeof (line), header_file ) ) {
            char * strret = strchr(line,'\n');
            if((strret != NULL)) {
                    //D("Remove \\r in %s 0=%d \n",line,strret[0])//,strret[1])
                    strret[0]='\0' ; //strret[1]='\0' ;
            }
            
            if(!memcmp(HEADER_LABEL_KERNEL_ADDRESS  ,   line,   HEADER_LABEL_KERNEL_ADDRESS_LENGTH)){
                D("Matched Kernel Address\n");
                image->header->kernel_addr = strtol(line + HEADER_LABEL_SECOND_ADDRESS_LENGTH,NULL, NUMBER_BASE_HEX );
                D(HEADER_LABEL_KERNEL_ADDRESS"%08x\n",image->header->kernel_addr);
            }
            if(!memcmp(HEADER_LABEL_RAMDISK_ADDRESS  ,   line,   HEADER_LABEL_RAMDISK_ADDRESS_LENGTH)){                                
                D("Matched Ramdisk Address\n");
                image->header->ramdisk_addr = strtol(line + HEADER_LABEL_RAMDISK_ADDRESS_LENGTH ,NULL,NUMBER_BASE_HEX ); 
                D(HEADER_LABEL_RAMDISK_ADDRESS"%08x\n",image->header->ramdisk_addr);
            }
            if(!memcmp(HEADER_LABEL_TAGS_ADDRESS  ,   line,   HEADER_LABEL_TAGS_ADDRESS_LENGTH)){                                
                D("Matched Tags Address\n");
                image->header->tags_addr = strtol(line + HEADER_LABEL_TAGS_ADDRESS_LENGTH ,NULL,NUMBER_BASE_HEX ); 
                D(HEADER_LABEL_TAGS_ADDRESS"%08x\n",image->header->tags_addr);
            }
            if(!memcmp(HEADER_LABEL_SECOND_ADDRESS  ,   line,   HEADER_LABEL_SECOND_ADDRESS_LENGTH)){  
                D("Matched Second Address\n");                              
                image->header->second_addr = strtol(line + HEADER_LABEL_SECOND_ADDRESS_LENGTH ,NULL,NUMBER_BASE_HEX ); 
                D(HEADER_LABEL_SECOND_ADDRESS"%08x\n",image->header->second_addr);
            }
            if(!memcmp(HEADER_LABEL_PAGE_SIZE,line,HEADER_LABEL_PAGE_SIZE_LENGTH)){
                D("Matched PageSize Address\n");
                image->header->page_size = strtol(line + HEADER_LABEL_PAGE_SIZE_LENGTH ,NULL, NUMBER_BASE_DECIMAL ); 
                D(HEADER_LABEL_PAGE_SIZE"%d\n",image->header->page_size);
            }
            if(!memcmp(HEADER_LABEL_NAME,line,HEADER_LABEL_NAME_LENGTH)){
                
                int line_length = strlen( line+HEADER_LABEL_NAME_LENGTH ) ; 
                if(line_length > BOOT_IMAGE_NAME_SIZE) line_length = BOOT_IMAGE_NAME_SIZE;
                D("Matched Label Name line_length %d %d\n",line_length,line[HEADER_LABEL_NAME_LENGTH]);
                memcpy(image->header->name,line+HEADER_LABEL_NAME_LENGTH,line_length);
                D(HEADER_LABEL_NAME"%s",image->header->name);
            }
            
           
            if(!memcmp(HEADER_LABEL_CMDLINE,line,HEADER_LABEL_CMDLINE_LENGTH)){
                D("Matched CmdLine\n");
                int line_length = strlen( line+HEADER_LABEL_CMDLINE_LENGTH ) ; 
                if(line_length > BOOT_IMAGE_ARGS_SIZE) line_length = BOOT_IMAGE_ARGS_SIZE;
                D("Matched CmdLine line_length %d %d\n",line_length,line[HEADER_LABEL_CMDLINE_LENGTH]);
                memcpy(image->header->cmdline,line+HEADER_LABEL_CMDLINE_LENGTH,line_length);
                
                D(HEADER_LABEL_CMDLINE"%s",image->header->cmdline);
                
                
            }
              
        
        }
 
        fclose(header_file);
    }
    
    return 0;
}
/* load_boot_image - load android boot image into memory 
 * 
 * 
 * 
 * returns zero when successful, return errno on failure
 * */
// BI_API
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
    
    D("image->start_addr=%u\n",image->start_addr);
    D("magic_offset_p=%u\n",magic_offset_p);
    image->header =(boot_img_hdr*)magic_offset_p;
    image->header_padding = calculate_padding(image->header_size,image->header->page_size); 
    D("First 9 Bytes image->header=%.*s\n",BOOT_MAGIC_SIZE,image->header);
    
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
    //D("\n\n\tload_boot_image_from_memory\nStruct"

    
}


unsigned write_boot_image(char *filename,boot_image* image){
    
    errno = 0;
    FILE* boot_image_file_fp = fopen(filename,"w+b");
    if(!boot_image_file_fp)
        return errno;
    
    
    
    D("writing boot image %s header_size %u\n",filename,image->header_size);
    D("writing boot image %p\n",image->header);
    
   
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

