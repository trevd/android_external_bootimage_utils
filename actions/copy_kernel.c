/*
 * copy_kernel.c
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
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
// internal program headers
#include <actions.h>
#include <utils.h>
#include <program.h>
#include <bootimage.h>

typedef struct copy_kernel_action copy_kernel_action;

struct copy_kernel_action{
    char*   source ;
    char*   destination ;
    char*   output ;
};
int copy_kernel(copy_kernel_action* action){
    
    
    boot_image bimage_source , bimage_dest;
    errno = 0;
    print_program_title();
    // load the source boot image
    int return_value=load_boot_image_from_file(action->source,&bimage_source);
    if(errno){
    return_value = print_program_error_file_not_boot_image(action->source);
    
    goto fail_hard;
    }
    // load the destination boot image
    errno = 0;
    return_value=load_boot_image_from_file(action->destination,&bimage_dest);
    if(errno){
    fprintf(stderr," cannot open file \"%s\" as boot image - error %d - %s\n\n",action->destination ,errno , strerror(errno));
    goto fail_hard;
    }
    
    kernel_image kimage_source, kimage_dest ;
    errno = 0 ;

    // check to see if kernels matchs 
    if(bimage_source.header->kernel_size==bimage_dest.header->kernel_size){
    
    
    
    fprintf(stderr," nothing to update, source and destination kernels are the same\n\n");
    goto fail_hard;
    
    }
    // load the source kernel details 
    return_value = load_kernel_image_from_memory(bimage_source.kernel_addr,bimage_source.header->kernel_size,&kimage_source);
    if(errno){
    fprintf(stderr," cannot get kernel information in \"%s\" - error %d - %s\n",action->source ,errno , strerror(errno));
    goto fail_hard;
    }
    // load the source kernel details 
    return_value = load_kernel_image_from_memory(bimage_dest.kernel_addr,bimage_dest.header->kernel_size,&kimage_dest);
    if(errno){
    fprintf(stderr," cannot get kernel information in \"%s\" - error %d - %s\n",action->destination ,errno , strerror(errno));
    goto fail_hard;
    }
    
    // Tell 'em how it's going down
    int version_length = kimage_dest.version_number_length > kimage_source.version_number_length ? kimage_dest.version_number_length : kimage_source.version_number_length ;
    
    fprintf(stderr," copying kernel from %s to %s\n\n",action->source,action->destination);
    
    fprintf(stderr," old kernel %-*.*s %u\n",version_length,kimage_dest.version_number_length,kimage_dest.version_number,bimage_dest.header->kernel_size);
    fprintf(stderr," new kernel %-*.*s %u\n\n",version_length,kimage_source.version_number_length,kimage_source.version_number,bimage_source.header->kernel_size);
        
    
   
    
    D("bimage_dest.header->kernel_size:%u\n",bimage_dest.header->kernel_size);
    bimage_dest.header->kernel_size = bimage_source.header->kernel_size ;
    bimage_dest.kernel_addr = bimage_source.kernel_addr;
    D("bimage_dest.header->kernel_size:%u\n",bimage_dest.header->kernel_size);
    set_boot_image_padding(&bimage_dest);
    set_boot_image_content_hash(&bimage_dest);
    set_boot_image_offsets(&bimage_dest);
    
    //print_boot_image_info(&bimage_dest);
    
    write_boot_image(action->destination,&bimage_dest);
    if(kimage_source.start_addr != NULL  ) free(kimage_source.start_addr);
    if(kimage_dest.start_addr != NULL  ) free(kimage_dest.start_addr);
fail_hard:
    if(bimage_source.start_addr != NULL  ) free(bimage_source.start_addr);
    if(bimage_dest.start_addr != NULL  ) free(bimage_dest.start_addr);
    
    
    // we will create a clean
    
    
    return 0;
    

    
}
int process_copy_kernel_action(unsigned argc,char ** argv,program_options* options){
    
    copy_kernel_action action;
    action.source   = NULL  ;
    action.destination  = NULL  ;
    action.output   = NULL  ;
    FILE*file; 
    while(argc > 0){
    
    if(!action.source && (file=fopen(argv[0],"r+b"))){
        
        fclose(file);
        action.source = argv[0];
        printf("action.source:%s\n",action.source);
        argc--; argv++ ;
        
        if(!action.destination && (file=fopen(argv[0],"r+b"))){
        fclose(file);
        action.destination = argv[0];
        D("action.destination:%s\n",action.destination);
           
        }
    }
    argc--; argv++ ;
    }
    // we must have a source and destination image to process
    if(!action.source){
    
        print_program_title();
        fprintf(stderr," No source specified\n\n");
        errno = EINVAL ;
        return EINVAL;
    }
    // we must have a source and destination image to process
    if(!action.destination){
        print_program_title();
        fprintf(stderr," No destination specified\n\n");
        errno = EINVAL ;
        return EINVAL;
    }
    copy_kernel(&action);
    return 0;
}
