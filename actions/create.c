/*
 * create.c
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
 
// internal program headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <utils.h>
#include <actions.h>
#include <bootimage.h>
#include <ramdisk.h>
#include <program.h>
#include <compression.h>


typedef struct create_action create_action;

struct create_action{
    
    char *      bootimage_filename  ;
    char *      header_filename     ;
    char *      kernel_filename     ;
    char **     ramdisk_filenames   ;
    char *      ramdisk_cpioname    ;
    char *      ramdisk_imagename   ;
    char *      ramdisk_directory   ;
    char *      second_filename     ;
    char *      output_directory    ;
    unsigned    command_line        ;
    unsigned    board_name      ;
    unsigned    page_size       ;
    unsigned    ramdisk_filenames_count ;
};

int create_bootimage(create_action* action, global_action* gaction){
    
    // setup a new boot_image_struct to receive the new information
    print_program_title();
    
    fprintf(stderr," Creating Boot Image - \"%s\"\n",action->bootimage_filename) ;
    errno = 0;
    boot_image bimage ;
    int ramdisk_processed = 0 ;
    unsigned char* ramdisk_data;
    
    // set the physical address defaults and other boot_image structure defaults
    set_boot_image_defaults(&bimage);
    if (action->header_filename) {
    fprintf(stderr, " Creating Header From : \"%s\"\n",action->header_filename) ;
    load_boot_image_header_from_disk(action->header_filename,&bimage);
    }
    
    if(action->kernel_filename){
        
    unsigned kernel_size = 0; 
    if(!(bimage.kernel_addr = read_item_from_disk(action->kernel_filename,&kernel_size))){
        print_program_error_processing(action->kernel_filename);
        return errno ; 
    }
    bimage.header->kernel_size = kernel_size;      
    fprintf(stderr, " Creating Kernel From : \"%s\"\n",action->kernel_filename) ;
    

    }
    if(action->ramdisk_directory){
    
    unsigned cpio_ramdisk_size = 0; 
    
    unsigned char* cpio_data = pack_ramdisk_directory(action->ramdisk_directory,&cpio_ramdisk_size) ;
    
    if(cpio_data){
    
        ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
        if(!(bimage.header->ramdisk_size = compress_gzip_memory(cpio_data,cpio_ramdisk_size,ramdisk_data,cpio_ramdisk_size))){
        print_program_error_processing(action->ramdisk_directory);
        return errno ; 
        }
        bimage.ramdisk_addr = ramdisk_data;
        fprintf(stderr, " Creating Ramdisk From Directory : \"%s\"\n",action->ramdisk_directory) ;
        free(cpio_data);
        
    }
    }else if(action->ramdisk_cpioname ){
    
    unsigned cpio_ramdisk_size = 0;     
    unsigned char* cpio_data = read_item_from_disk(action->ramdisk_cpioname,&cpio_ramdisk_size);
    
    ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
    if(!(bimage.header->ramdisk_size = compress_gzip_memory(cpio_data,cpio_ramdisk_size,ramdisk_data,cpio_ramdisk_size))){
        print_program_error_processing(action->ramdisk_cpioname);
        return errno ; 
    }
    
    bimage.ramdisk_addr = ramdisk_data;
    fprintf(stderr, " Creating Ramdisk From : \"%s\"\n",action->ramdisk_cpioname) ;
    free(cpio_data);
    
    }else if(action->ramdisk_imagename ){

    if(!(bimage.ramdisk_addr =  read_item_from_disk(action->ramdisk_imagename,&bimage.header->ramdisk_size))){
        print_program_error_processing(action->ramdisk_imagename);
        return errno ; 
    }
    fprintf(stderr, " Creating Ramdisk From : \"%s\"\n",action->ramdisk_imagename) ;
      
    }
    
    if(action->second_filename){
    
    if(bimage.header->second_size == 0 ) 
        bimage.second_addr = NULL;
    else if(!(bimage.second_addr =  read_item_from_disk(action->second_filename,&bimage.header->second_size))){
        print_program_error_processing(action->second_filename);
        return errno ; 
    }
    fprintf(stderr, " Creating Second From : \"%s\"\n",action->second_filename) ;
    
    }
    fprintf(stderr, " Calculating Padding\n") ;    
    set_boot_image_padding(&bimage);
    fprintf(stderr, " Calculating Content Hashes\n") ;    
    set_boot_image_content_hash(&bimage);
    set_boot_image_offsets(&bimage);
    
    //print_boot_image_info(&bimage);
        
   fprintf(stderr, " Writing Booting Image\n") ;    
   if(write_boot_image(action->bootimage_filename,&bimage)){
    
    }
   
   
cleanup_bootimage:
    D("kernel_addr free\n");
    if(bimage.kernel_addr) free(bimage.kernel_addr);
    D("ramdisk_addr free\n");
    if(bimage.ramdisk_addr) free(bimage.ramdisk_addr);
    D("second_addr free\n");
    if(bimage.second_addr) free(bimage.second_addr);
    return errno;
}


// process_create_action - parse the command line switches
// although this code is repetitive we will favour readability
// over codesize ...... Ask me in 3 months time whether it was
// a good idea.
int process_create_action(unsigned argc,char ** argv,global_action* gaction){
    
    // Initialize the action struct with NULL values
    create_action action;
    action.bootimage_filename   = NULL  ;
    action.header_filename      = NULL  ;
    action.kernel_filename      = NULL  ;
    action.ramdisk_filenames    = NULL  ;
    action.ramdisk_cpioname     = NULL  ;
    action.ramdisk_imagename    = NULL  ;
    action.ramdisk_directory    = NULL  ;
    action.second_filename      = NULL  ;
    action.output_directory     = NULL  ;
    action.ramdisk_filenames_count  = 0 ;
    

    // work out a possible file name just in case we need it for 
    // error reporting , the possible filename should be at position zero 
    // but it maybe elsewhere, as info printing doesn't require any require 
    // filenames for switches we can look for the first argv that doesn't begin with "-"
    char* possible_filename = NULL;
    unsigned i = 0 ;
    for(i = 0 ; i < argc ; i++){
        if(argv[i][0]!='-'){
         possible_filename = argv[i];
         D("possible_filename at position %d - %s %s\n",i,argv[i],possible_filename);
         break ;
     }
    }
    
    FILE*file; int ramdisk_set = 0;
      
    while(argc > 0){
        
    // check for a valid file name
    if(!action.bootimage_filename){
        
        
        action.bootimage_filename = argv[0];
        D("action.bootimage_filename:%s\n",action.bootimage_filename);
        // set full extract if this is the last token 
        // or if the next token is NOT a switch. 
        
        if(argc == 1 || argv[1][0]!='-'){ 
            action.header_filename  = "header";
            action.kernel_filename  = "kernel";
            action.ramdisk_cpioname     = "ramdisk.cpio";
            action.ramdisk_imagename    = "ramdisk.img";
            action.ramdisk_directory    = "ramdisk";
            action.second_filename  = "second";
            // do we have an impiled output 
            //if (argv[1]) action.output_directory = argv[1];

        }
        
        
    }else if(!strlcmp(argv[0],"--header") || !strlcmp(argv[0],"-h")){
        
        // we have an header, do we have a filename
        
        // use the default filename if this is the last token
        // or if the next token is a switch
        if(argc == 1 || argv[1][0]=='-'){
            action.header_filename = "header";
        }else{
            action.header_filename = argv[1];
            --argc; ++argv;
        }
        D("action.header_filename:%s\n",action.header_filename);
        
    }else if(!strlcmp(argv[0],"--kernel") || !strlcmp(argv[0],"-k")){
        
        // we have a kernel, do we have a filename
        
        // use the default filename if this is the last token
        // or if the next token is a switch
        if(argc == 1 || (argv[1][0]=='-')){
            action.kernel_filename = "kernel";
        }else{
            action.kernel_filename = argv[1];
            --argc; ++argv;
        }
        D("action.kernel_filename:%s\n",action.kernel_filename);
        
    }else if(!strlcmp(argv[0],"--second") || !strlcmp(argv[0],"-s")) {
        
        // the second bootloader. AFAIK there is only 1 device
        // in existance that
        
        // use the default filename if this is the last token
        // or if the next token is a switch
        if(argc == 1 || (argv[1][0]=='-')){
            action.second_filename = "second";
        }else{
            action.second_filename = argv[1];
            --argc; ++argv;
        }
        D("action.second_filename:%s\n",action.second_filename);
        
    }else if (!ramdisk_set){

    if(!strlcmp(argv[0],"--cpio") || !strlcmp(argv[0],"-C")) {
        
        // use the default filename if this is the last token
        // or if the next token is a switch
        if(argc == 1 || (argv[1][0]=='-')){
        action.ramdisk_cpioname = "ramdisk.cpio";
        }else{
        action.ramdisk_cpioname = argv[1];
        --argc; ++argv;
        }
        ramdisk_set = 1 ;
        D("action.ramdisk_cpioname:%s\n",action.ramdisk_cpioname);
        
    }else if(!strlcmp(argv[0],"--directory") || !strlcmp(argv[0],"-d")) {
        
        // use the default filename if this is the last token
        // or if the next token is a switch
        if(argc == 1 || (argv[1][0]=='-')){
        action.ramdisk_directory = "ramdisk";
        }else{
        action.ramdisk_directory = argv[1];
        --argc; ++argv;
        }
        ramdisk_set = 1 ;
        D("action.ramdisk_directory:%s\n",action.ramdisk_directory);
        
    }else if(!strlcmp(argv[0],"--image") || !strlcmp(argv[0],"-i") || !strlcmp(argv[0],"-r")) {
        
        // the ramdisk image as it is in the boot.img
        // this is normally a cpio.gz file but we need to 
        // check that later on.
        
        // use the default filename if this is the last token
        // or if the next token is a switch
        if(argc == 1 || (argv[1][0]=='-')){
        action.ramdisk_imagename = "ramdisk.img";
        }else{
        action.ramdisk_imagename = argv[1];
        --argc; ++argv;
        }
        ramdisk_set = 1 ;
        D("action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
        
    } else if(!strlcmp(argv[0],"--files") || !strlcmp(argv[0],"-f")) {
        
        // ramdisk files. This is a variable length char array
        // containing a list of file to extract from the ramdisk
            
        // if this is the last token or if the next token is a switch
        // we need to create an array with 1 entry 
        
        // work out how much memory is required
        unsigned targc = 0 ; 
        for(targc=0; targc < argc-1 ; targc++ ){
            fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
            if(argv[targc+1] && argv[targc+1][0]=='-')
              break;
            else
            action.ramdisk_filenames_count++;       
            
        }
        D("action.ramdisk_filenames_count %d argc %d\n",action.ramdisk_filenames_count,argc);
        // allocate the memory and assign null to the end of the array
        action.ramdisk_filenames = calloc(action.ramdisk_filenames_count,sizeof(unsigned char*)) ;
        // populate the array with the values 
        for(targc =0 ; targc < action.ramdisk_filenames_count; targc++) {
            argc--; argv++;
            action.ramdisk_filenames[targc] = argv[0]; 
            D("action.ramdisk_filenames[%d]:%s\n",targc,action.ramdisk_filenames[targc] );
        }
        ramdisk_set = 1 ;
        }        
    }
        argc--; argv++ ;
    }
    // we must have at least a boot image to process
    if(!action.bootimage_filename){
        fprintf(stderr,"no boot image:%s\n",action.bootimage_filename);
        return EINVAL;
    }
   
    
    
    create_bootimage(&action,gaction);
       
    return 0;
}
