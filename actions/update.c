/*
 * update.c
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

// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <string.h> 

// libbootimage headers
#include <utils.h>
#include <bootimage.h>
#include <ramdisk.h>
#include <program.h>
#include <compression.h>

// internal program headers
#include <actions.h>

typedef struct update_action update_action;

struct update_action{
    
    char *      filename    ;
    char *      header_filename     ;
    char *      kernel_filename     ;
    char **     ramdisk_filenames   ;
    char *      ramdisk_cpioname    ;
    char *      ramdisk_imagename   ;
    char *      ramdisk_directory   ;
    char *      second_filename     ;
    char *      output_filename     ;
    unsigned    ramdisk_filenames_count ;
    char **     property_names  ;
    unsigned    property_count ;
};



inline int update_ramdisk_cpio(update_action* action,global_action* gaction,ramdisk_image* rimage){
    
    return 0;

}
inline int update_ramdisk_archive_from_directory(update_action* action,global_action* gaction,ramdisk_image* rimage){
    return 0 ;
}
inline int update_ramdisk_archive_from_cpio(update_action* action,global_action* gaction,ramdisk_image* rimage){
    
    return 0 ;
}
inline int update_ramdisk_archive_from_files(update_action* action,global_action* gaction,ramdisk_image* rimage){
    
    errno = 0;
    
    unsigned entry_index = 0 ; 
    unsigned filename_index = 0 ; 
    
    for (filename_index = 0 ; filename_index < action->ramdisk_filenames_count ; filename_index ++){
    
        D("action->ramdisk_filenames[%d]=%s\n",filename_index,action->ramdisk_filenames[filename_index]);
        errno = 0 ;
        unsigned found = 0 ;
        for (entry_index = 0 ; entry_index < rimage->entry_count ; entry_index ++){
            
            ramdisk_entry* entry = rimage->entries[entry_index];
            
            if(!strlcmp((char*)entry->name_addr,
                    action->ramdisk_filenames[filename_index]))
            {
                
                D("entry[%u]->name_addr=%s\n",entry_index,entry->name_addr);
                
                unsigned old_data_size = entry->data_size ; 
                unsigned new_data_size = 0 ;
                
                entry->data_addr = read_item_from_disk((char*) entry->name_addr, &new_data_size);
                
                D("old rimage.size=%u %u data_padding:%u\n",rimage->size,old_data_size,entry->data_padding);
                entry->data_size = new_data_size;
                rimage->size -= old_data_size ;
                rimage->size += entry->data_size ;
                fprintf(stderr," ramdisk file \"%s\" updated from \"%s\"\n",action->ramdisk_filenames[filename_index],action->ramdisk_filenames[filename_index]);
                D("new rimage.size=%u %u data_padding:%u\n",rimage->size, entry->data_size,entry->data_padding);
                update_ramdisk_entry_cpio_newc_header_info(entry);        
                found = 1 ;
                break;
            }
        }
        if(!found){
            errno = ENOENT ;
            fprintf(stderr," ramdisk file updating %s : error #%d - %s\n",action->ramdisk_filenames[filename_index],errno,strerror(errno));   
        }
    }
    
       
    unsigned cpio_ramdisk_size = rimage->size; 
    unsigned char* cpio_data =  pack_noncontiguous_ramdisk_entries(rimage);
    unsigned ramdisk_size = 0;
    unsigned char* ramdisk_data = NULL;
    D("cpio_size:%u %p\n",cpio_ramdisk_size,cpio_data );
        
    if(cpio_data){
        ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
        ramdisk_size = compress_gzip_memory(cpio_data,cpio_ramdisk_size,ramdisk_data,cpio_ramdisk_size);
        write_item_to_disk(ramdisk_data,ramdisk_size,33188,action->filename);
        free(ramdisk_data);
        free(cpio_data);
        D("ramdisk_size :%u\n",ramdisk_size );

    }
    return 0;
    
}
int update_ramdisk_archive(update_action* action,global_action* gaction,ramdisk_image* rimage){
    
    print_program_title();
    fprintf(stderr," Updating ramdisk archive \"%s\"\n",action->filename);
    
    errno = 0 ;
    int ramdisk_processed = 1 ;
    char* current_working_directory = NULL; 
    getcwd(current_working_directory,PATH_MAX);

    if(action->ramdisk_directory){
        D("action->ramdisk_directory=%s\n",action->ramdisk_directory);
        ramdisk_processed = update_ramdisk_archive_from_directory(action,gaction,rimage);
    }else if(action->ramdisk_cpioname){
        D("action->ramdisk_cpioname=%s\n",action->ramdisk_cpioname);
        ramdisk_processed = update_ramdisk_archive_from_cpio(action,gaction,rimage);
    }else if(action->ramdisk_filenames_count){
        D("action->ramdisk_filenames_count=%u\n",action->ramdisk_filenames_count);
        ramdisk_processed = update_ramdisk_archive_from_files(action,gaction,rimage);
    }
    if(!ramdisk_processed){
        
    }
    
    fprintf(stderr,"\n");
    return 0;

}
int update_kernel_image(update_action* action,global_action* gaction,kernel_image* kimage){
    
    return 0;

}

inline int update_boot_image_ramdisk_from_files(update_action* action,global_action* gaction,boot_image** pbimage){
    
    D("action->ramdisk_filenames_count=%d\n",action->ramdisk_filenames_count);
    errno = 0;
    boot_image* bimage = (*pbimage);
    int return_value ; 
    ramdisk_image rimage; 
    return_value = load_ramdisk_image_from_archive_memory(bimage->ramdisk_addr,bimage->header->ramdisk_size,&rimage);
    
    unsigned entry_index = 0 ; 
    unsigned filename_index = 0 ; 
    
    for (filename_index = 0 ; filename_index < action->ramdisk_filenames_count ; filename_index ++){
    
    D("action->ramdisk_filenames[%d]=%s\n",filename_index,action->ramdisk_filenames[filename_index]);
    for (entry_index = 0 ; entry_index < rimage.entry_count ; entry_index ++){
        
        ramdisk_entry* entry = rimage.entries[entry_index];
        
        if(!strlcmp((char*)entry->name_addr,
                action->ramdisk_filenames[filename_index])){
            
            D("entry[%u]->name_addr=%s\n",entry_index,entry->name_addr);
            
            unsigned old_data_size = entry->data_size ; 
            unsigned new_data_size = 0 ;
            
            entry->data_addr = read_item_from_disk((char*) entry->name_addr, &new_data_size);
            D("old data_padding:%u\n",entry->data_padding);
            D("old: rimage.size=%u %u\n",rimage.size,old_data_size);
            entry->data_size = new_data_size;
            rimage.size -= old_data_size ;
            rimage.size += entry->data_size ;
            D("new data_padding:%u\n",entry->data_padding);
            D("new: rimage.size=%u %u\n",rimage.size, entry->data_size);
            update_ramdisk_entry_cpio_newc_header_info(entry);
                    
            break;
        }
        }
    }
    
    
    
   unsigned cpio_ramdisk_size = rimage.size; 

    unsigned char* cpio_data =  pack_noncontiguous_ramdisk_entries(&rimage);
    unsigned char* ramdisk_data = NULL;
    D("cpio_size:%u %p\n",cpio_ramdisk_size,cpio_data );
    write_item_to_disk(cpio_data,cpio_ramdisk_size,33188,"test.cpio");
    
    if(cpio_data){
        ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
        bimage->header->ramdisk_size = compress_gzip_memory(cpio_data,cpio_ramdisk_size,ramdisk_data,cpio_ramdisk_size);
        bimage->ramdisk_addr = ramdisk_data;

        free(cpio_data);
        D("bimage->header->ramdisk_size :%u\n",bimage->header->ramdisk_size );

    }
    return 0;
    
}

inline int update_boot_image_ramdisk_from_imagename(update_action* action,global_action* gaction,boot_image** pbimage){
    
    boot_image* bimage = (*pbimage);
    D("action->ramdisk_imagename=%s\n",action->ramdisk_imagename);
    errno = 0 ;
    unsigned ramdisk_size = 0; 
    unsigned char* ramdisk_data = read_item_from_disk(action->ramdisk_imagename,&ramdisk_size);
    if(errno){
        fprintf(stderr," Updating ramdisk image skipped - error %d %s\n",errno,strerror(errno));
        return errno ;
    }
    
    if( bimage->header->ramdisk_size == ramdisk_size ) {
        // The ramdisk sizes match, Check it's not coincidence
        if(is_md5_match(bimage->ramdisk_addr,bimage->header->ramdisk_size,ramdisk_data, ramdisk_size)){
            free(ramdisk_data);
            fprintf(stderr," Updating ramdisk image skipped, files are the same.\n");
            return 1;
        }
    }
    
    D("ramdisk_data=%p\n",ramdisk_data);
    D("ramdisk_size=%u\n",ramdisk_size);
    bimage->header->ramdisk_size = ramdisk_size;
    bimage->ramdisk_addr = ramdisk_data;
    fprintf(stderr," Updating ramdisk from archive \"%s\"\n",action->ramdisk_imagename);
    return 0 ;
    
    
}
inline int update_boot_image_ramdisk_from_cpio(update_action* action,global_action* gaction,boot_image** pbimage){
    
    boot_image* bimage = (*pbimage);
    D("action->ramdisk_cpioname=%s\n",action->ramdisk_cpioname);
    errno = 0 ;
    unsigned cpio_ramdisk_size = 0; 
    unsigned char* cpio_data = read_item_from_disk(action->ramdisk_cpioname,&cpio_ramdisk_size);
    if(errno){
    fprintf(stderr," Update ramdisk image skipped - error %d %s\n",errno,strerror(errno));
    return errno ;
    }
    
    D("cpio_ramdisk_size=%u\n",cpio_ramdisk_size);
    
    errno = 0;
    unsigned char *ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
    
    if(errno){
    fprintf(stderr," Update ramdisk image skipped - error %d %s\n",errno,strerror(errno));
    free(cpio_data) ;
    return errno ;
    }
    D("ramdisk_data=%p\n",ramdisk_data);
    
    unsigned ramdisk_size = compress_gzip_memory(cpio_data,cpio_ramdisk_size,ramdisk_data,cpio_ramdisk_size);
    if(errno){
    fprintf(stderr," Update ramdisk image skipped - error %d %s\n",errno,strerror(errno));
    free(ramdisk_data);
    free(cpio_data) ;
    return errno ;
    }
    D("ramdisk_size=%u\n",ramdisk_size);
    bimage->header->ramdisk_size = ramdisk_size;
    bimage->ramdisk_addr = ramdisk_data;
    fprintf(stderr," Updating ramdisk from cpio archive \"%s\"\n",action->ramdisk_cpioname);
        
    free(cpio_data);
    return 0 ;
    
}

inline int update_boot_image_ramdisk_from_directory(update_action* action,global_action* gaction,boot_image** pbimage){
    
    errno = 0;
    boot_image* bimage = (*pbimage);
    unsigned cpio_ramdisk_size = 0; 
    D("action->ramdisk_directory=%s\n",action->ramdisk_directory);
    unsigned char* cpio_data = pack_ramdisk_directory(action->ramdisk_directory,&cpio_ramdisk_size) ;
    D("cpio_ramdisk_size=%u\n",cpio_ramdisk_size);
    
    if(errno){
    fprintf(stderr," Update ramdisk image skipped - error %d %s\n",errno,strerror(errno));
    return errno;
    }
    
    
    unsigned char* ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
    bimage->header->ramdisk_size = compress_gzip_memory(cpio_data,cpio_ramdisk_size,ramdisk_data,cpio_ramdisk_size);
    bimage->ramdisk_addr = ramdisk_data;
    
    free(cpio_data);
    fprintf(stderr," Updating ramdisk from \"%s\"\n",action->ramdisk_directory);

    return 0;
}
inline int update_boot_image_kernel_from_file(update_action* action,global_action* gaction,boot_image** pbimage){
    
    boot_image* bimage = (*pbimage);
    D("action->kernel_addr=%p bimage->header->kernel_size=%u\n",action->kernel_filename,bimage->header->kernel_size);
    D("updating kernel from file action->kernel_filename=%s\n", action->kernel_filename);
    
    errno = 0; 
    unsigned new_size = 0 ;
    unsigned char* new_addr = read_item_from_disk(action->kernel_filename,&new_size );
    if(errno){
    fprintf(stderr," Updating ramdisk image skipped - error %d %s\n",errno,strerror(errno));
    return errno ;
    }
    
    
    if(bimage->header->kernel_size == new_size){
    
        // The Kernel Sizes Match, Check it's not coincidence
        if(is_md5_match(bimage->kernel_addr,bimage->header->kernel_size,new_addr, new_size)){
            free(new_addr);
            fprintf(stderr," Updating kernel image skipped , files are the same.\n");
            errno =EINVAL;
            return errno;
        }
    }
    
    // Tell 'em how it's going down
    kernel_image kimage_current, kimage_new ;
    
    load_kernel_image_from_memory(new_addr,new_size,&kimage_new);
    load_kernel_image_from_memory(bimage->kernel_addr,bimage->header->kernel_size,&kimage_current);
    
    
        
    
    fprintf(stderr," Updating kernel with  \"%s\" - %.*s -> %.*s\n",
                action->kernel_filename ,
                kimage_current.version_number_length,
                kimage_current.version_number,
                kimage_new.version_number_length,
                kimage_new.version_number
                );    
    
    
    if(kimage_current.start_addr != NULL  ) free(kimage_current.start_addr);
    if(kimage_new.start_addr != NULL  ) free(kimage_new.start_addr);
    
    bimage->kernel_addr = new_addr ; 
    bimage->header->kernel_size = new_size ;   
    
    return 0;

}

int update_boot_image(update_action* action,global_action* gaction,boot_image* bimage){
    
    
    print_program_title();
    fprintf(stderr," Updating boot image \"%s\"\n",action->filename);
    
    errno = 0 ;
   
    char* current_working_directory = NULL; 
    getcwd(current_working_directory,PATH_MAX);

    int kernel_processed = 1 ; int ramdisk_processed = 1 ;    
    if(action->kernel_filename){
        kernel_processed = update_boot_image_kernel_from_file(action,gaction,&bimage);
    }
    
    if(action->ramdisk_directory){
        D("action->ramdisk_directory=%s\n",action->ramdisk_directory);
        ramdisk_processed = update_boot_image_ramdisk_from_directory(action,gaction,&bimage);
    }else if(action->ramdisk_cpioname){
        D("action->ramdisk_cpioname=%s\n",action->ramdisk_cpioname);
        ramdisk_processed = update_boot_image_ramdisk_from_cpio(action,gaction,&bimage);
    }else if(action->ramdisk_imagename){
        D("action->ramdisk_imagename=%s\n",action->ramdisk_imagename);
        ramdisk_processed = update_boot_image_ramdisk_from_imagename(action,gaction,&bimage);
    }else if(action->ramdisk_filenames_count){
        D("action->ramdisk_filenames_count=%u\n",action->ramdisk_filenames_count);
        unsigned char* ram_addr = bimage->ramdisk_addr;
        update_boot_image_ramdisk_from_files(action,gaction,&bimage);
    D("old ramdisk:%p new_ramdisk:%p\n",ram_addr,bimage->ramdisk_addr);
    }
    
    set_boot_image_padding(bimage);
    set_boot_image_content_hash(bimage);
    set_boot_image_offsets(bimage);
    
    D("writing action->output_filename %s\n",action->output_filename);
    write_boot_image(action->output_filename,bimage);
    
    D("kernel_processed=%d %p\n",kernel_processed,bimage->kernel_addr);
    if(!kernel_processed)
    free(bimage->kernel_addr);
    
    if(!ramdisk_processed)
    free(bimage->ramdisk_addr);
    
    fprintf(stderr,"\n");
    return 0;
}

// STEP 2 :
// process_update_target_file - read the fill specified by action->filename
// into memory and determine the file type. If the file is valid call the file type
// specific update function - valid file types are
// Android Boot Images, Linux Kernel zImage, Bzip2, Lzo, Gzip, XZ Cpio Ramdisks and
// Standalone Cpio Ramdisks 
static int process_update_target_file(update_action* action,global_action* gaction ){

   
    char* current_working_directory = NULL; 
    errno = 0 ; 
    int saved_error = 0 ;
    int return_value=0;
    unsigned action_size = 0;     
    getcwd(current_working_directory,PATH_MAX);
    
    unsigned char* action_data = read_item_from_disk(action->filename , &action_size);
    if(!action_data && errno){
    
        //  file too large error. no point in contining
        print_program_title();
        fprintf(stderr," Cannot process \"%s\" - error : %d %s\n\n",action->filename,errno,strerror(errno));
        return 0;    
    }

    
    D("read_item_from_disk completed %s %u errno=%d\n",action->filename,action_size,errno);
    
    // Probe the file type - First check if we have a boot.img file
    boot_image bimage;
    if(!(return_value=load_boot_image_from_memory(action_data,action_size,&bimage))){
        D("%s is a boot image - load_boot_image_from_memory returned %d\n",action->filename,return_value);
        return_value = update_boot_image(action, gaction,&bimage);
        D("update_boot_image returned %d\n",return_value);
        if(bimage.start_addr != NULL ) free(bimage.start_addr); 
        return return_value;   
    
    }else{
        if(bimage.start_addr != NULL ) free(bimage.start_addr); 
        errno = 0 ; 
    }
        
    // Probe the file type - Second check if we have a linux kernel file
    kernel_image kimage;
    if(!(return_value = load_kernel_image_from_memory(action_data,action_size,&kimage))){
        D("load_kernel_image_from_memory returns:%d\n", return_value); 
        return_value = update_kernel_image(action,gaction,&kimage);
        if(kimage.start_addr != NULL  ) free(kimage.start_addr);
        return return_value;
    }else{
        if(kimage.start_addr != NULL ) free(kimage.start_addr); 
        errno = 0 ; 
    }
    
    // Probe the file type - Third check if we have a compressed ramdisk
    ramdisk_image rimage ; //=  get_initialized_ramdisk_image();
    if(!(return_value =  load_ramdisk_image_from_archive_memory(action_data,action_size,&rimage))){
        D("load_ramdisk_image_from_archive_memory returns:%d\n",rimage.entry_count); 
        return_value = update_ramdisk_archive(action,gaction,&rimage);
        if(rimage.start_addr != NULL ) free(rimage.start_addr); 
        return return_value;
    }else{
        if(rimage.start_addr != NULL ) free(rimage.start_addr); 
        errno = 0 ; 
    }
    
     if(!load_ramdisk_image_from_cpio_memory(action_data,action_size,&rimage)){
    return_value = update_ramdisk_cpio(action,gaction,&rimage);
    free(rimage.start_addr); 
    return return_value;
    }
    
    
    print_program_error_file_type_not_recognized(action->filename);
    
    return 0;
}


// STEP 1:
// process_update_action - parse command line switches for the update action
// and check the intended target exists
int process_update_action(unsigned argc,char ** argv,global_action* gaction){

    
    // Initialize the action struct with NULL values
    update_action action;
    action.filename                 = NULL  ;
    action.header_filename          = NULL  ;
    action.kernel_filename          = NULL  ;
    action.ramdisk_filenames        = NULL  ;
    action.ramdisk_cpioname         = NULL  ;
    action.ramdisk_imagename        = NULL  ;
    action.ramdisk_directory        = NULL  ;
    action.second_filename          = NULL  ;
    action.output_filename          = NULL  ;
    action.property_names           = NULL  ;
    action.ramdisk_filenames_count  = 0 ;
    action.property_count           = 0 ;

    FILE*file; int ramdisk_set = 0;
    
    while(argc > 0){
            
        // check for a valid file name
        if(!action.filename && (file=fopen(argv[0],"r+b"))){
            
            fclose(file);
            action.filename = argv[0];
            D("action.bootimage_filename:%s\n",action.filename);
            // set full update if this is the last token 
            // or if the next token is NOT a switch. 
            
            if(argc == 1 || ( argv[1][0]=='-' && argv[1][1]=='o') ||argv[1][0]!='-'){ 
                D("extract all\n");
                action.header_filename  = (char*)"header";
                action.kernel_filename  = "kernel";
                action.ramdisk_cpioname     = "ramdisk.cpio";
                action.ramdisk_imagename    = "ramdisk.img";
                action.ramdisk_directory    = "ramdisk";
                action.second_filename  = "second";
                // do we have an impiled output filename
                if (argv[1] && argv[1][0]!='-') action.output_filename = argv[1];

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
            
        }else if(!strlcmp(argv[0],"--output") || !strlcmp(argv[0],"-o")) {
            
            // the output directory. if this does not exist then
            // we will attempt to create it.
            
            // if it does exists we will offer to either wipe or 
            // append.
            
            // use the default filename if this is the last token
            // or if the next token is a switch
            if(argc == 1 || (argv[1][0]=='-')){
                action.output_filename = NULL;
            }else{
                action.output_filename = argv[1];
                --argc; ++argv;
            }

            D("action.output_filename:%s\n",action.output_filename);
            
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
            
        } else if(!strlcmp(argv[0],"--properties") || !strlcmp(argv[0],"-p")) {
            
                // properties. This is a variable length char array
                // containing a list of properties to be modified in the 
                // default.prop
                        
                // if this is the last token or if the next token is a switch
                // we need to create an array with 1 entry 
                D("action.property\n");
                // work out how much memory is required
                unsigned targc = 0 ; 
                for(targc=0; targc < argc-1 ; targc++ ){
                    fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
                    if(argv[targc+1] && argv[targc+1][0]=='-')
                      break;
                    else
                    action.property_count++;        
                    
                }
                D("action.property_count %d argc %d\n",action.property_count,argc);
                // allocate the memory and assign null to the end of the array
                action.property_names = calloc(action.property_count,sizeof(unsigned char*)) ;
                // populate the array with the values 
                for(targc =0 ; targc < action.property_count; targc++) {
                    argc--; argv++;
                    action.property_names[targc] = argv[0]; 
                    D("action.property_names[%d]:%s\n",targc,action.property_names[targc] );
                }
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
               D("argv[%d] %s\n",targc,argv[targc]);
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
    // we must have at least a filename to process
    if(!action.filename) 
        return print_program_error_file_name_not_found(action.filename);
    
    // output_file not set. use the bootimage filename
    if(!action.output_filename){
        D("no output set using bootimage_filename:%s\n",action.filename);
        action.output_filename = action.filename;
    }
    
    
    process_update_target_file(&action,gaction);
       
    return 0;
}
