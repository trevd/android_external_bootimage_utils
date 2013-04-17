

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
	
	char *		filename	;
	char *  	header_filename 	;
	char *  	kernel_filename 	;
	char ** 	ramdisk_filenames 	;
	char *  	ramdisk_cpioname 	;
	char *  	ramdisk_imagename 	;
	unsigned char * ramdisk_directory 	;
	char *		second_filename		;
	char * 		output_filename 	;
	unsigned	ramdisk_filenames_count	;
	char ** 	property_names 	;
	unsigned 	property_count ;
};

int update_ramdisk_files(update_action* action,ramdisk_image* rimage){
    
    fprintf(stderr,"update_ramdisk_files %d\n",rimage->size);	
    // extract a single file from the ramdisk 
    int entry_index = 0 ; int filename_index = 0 ; 
    for (filename_index = 0 ; filename_index < action->ramdisk_filenames_count ; filename_index ++){
	for (entry_index = 0 ; entry_index < rimage->entry_count ; entry_index ++){
	    if(!strlcmp(rimage->entries[entry_index]->name_addr,action->ramdisk_filenames[filename_index])){
		    fprintf(stderr,"%s\n",rimage->entries[entry_index]->name_addr);
		    rimage->entries[entry_index]->data_addr = read_item_from_disk( rimage->entries[entry_index]->name_size, &rimage->entries[entry_index]->data_size);
		    update_ramdisk_header(rimage->entries[entry_index]->start_addr);
		    
		    
		    //FILE* ramdiskfile_fp = fopen(action->ramdisk_filenames[filename_index],"w+b");
		    if(write_item_to_disk_extended(rimage->entries[entry_index]->data_addr,rimage->entries[entry_index]->data_size,
			rimage->entries[entry_index]->mode,action->ramdisk_filenames[filename_index],rimage->entries[entry_index]->name_size)){
			
			fprintf(stderr,"error writing %s %d %s\n",action->ramdisk_filenames[filename_index],errno,strerror(errno));
		    }
		    break;
		}
	    }
	}
	
    
    return 0 ; 
}
int update_ramdisk_cpio(update_action* action,global_action* gaction,kernel_image* kimage){
    
    return 0;

}
int update_ramdisk_archive(update_action* action,global_action* gaction,kernel_image* kimage){
    
    return 0;

}
int update_kernel_image(update_action* action,global_action* gaction,kernel_image* kimage){
    
    return 0;

}

inline int update_boot_image_kernel_from_file(update_action* action,global_action* gaction,boot_image* bimage){
    
    
    D("action->kernel_addr=%p bimage->header->kernel_size=%u\n",action->kernel_filename,bimage->header->kernel_size);
    D("updating kernel from file action->kernel_filename=%s\n", action->kernel_filename);
    
    errno = 0; 
    unsigned new_size = 0 ;
    unsigned char* new_addr = read_item_from_disk(action->kernel_filename,&new_size );
    if(errno){
	bimage->kernel_addr = NULL ;
	bimage->header->kernel_size = 0 ;
	return errno;
    }
    
    
    if(bimage->header->kernel_size == new_size){
	
	// The Kernel Sizes Match, Check it's not coincidence
	if(is_md5_match(bimage->kernel_addr,bimage->header->kernel_size,new_addr, new_size)){
	    free(new_addr);
	    fprintf(stderr," updating kernel image skipped , files are the same.\n");
	    return 1;
	}
    }
    
    // Tell 'em how it's going down
    kernel_image kimage_current, kimage_new ;
    
    load_kernel_image_from_memory(new_addr,new_size,&kimage_new);
    load_kernel_image_from_memory(bimage->kernel_addr,bimage->header->kernel_size,&kimage_current);
    int version_length = kimage_dest.version_number_length > kimage_new.version_number_length ? kimage_dest.version_number_length : kimage_source.version_number_length ;
    
    fprintf(stderr," copying kernel from %s to %s\n\n",action->source,action->destination);
    
    fprintf(stderr," old kernel %-*.*s %u\n",version_length,kimage_dest.version_number_length,kimage_dest.version_number,bimage_dest.header->kernel_size);
    fprintf(stderr," new kernel %-*.*s %u\n\n",version_length,kimage_source.version_number_length,kimage_source.version_number,bimage_source.header->kernel_size);
    	
    D("action->kernel_addr=%p bimage->header->kernel_size=%u\n",action->kernel_filename,bimage->header->kernel_size);
    
    return 0;

}

int update_boot_image(update_action* action,global_action* gaction,boot_image* bimage){
    
    
    print_program_title();
    fprintf(stderr," Updating boot image \"%s\"\n\n",action->filename);
    
    errno = 0 ;
   
    char* current_working_directory = NULL; 
    getcwd(current_working_directory,PATH_MAX);

        
    if(action->kernel_filename){
	update_boot_image_kernel_from_file(action,gaction,bimage);
    }
    
    set_boot_image_padding(bimage);
    set_boot_image_content_hash(bimage);
    set_boot_image_offsets(bimage);
    
    D("writing action->output_filename %s\n",action->output_filename);
    write_boot_image(action->output_filename,bimage);
    
    return 0;
}

int update_file(update_action* action,global_action* gaction ){

   
    char* current_working_directory = NULL; 
    errno = 0 ; 
    int saved_error = 0 ;
    int return_value=0;
    unsigned action_size = 0;     
    getcwd(current_working_directory,PATH_MAX);
    
    char* action_data = read_item_from_disk(action->filename , &action_size);
    if(!action_data && errno){
	
	    //  file too large error. no point in contining
	    print_program_title();
	    fprintf(stderr," Cannot process \"%s\" - error : %d %s\n\n",action->filename,errno,strerror(errno));
	    return 0;    
	}

    
    D("read_item_from_disk completed %s %u errno=%d\n",action->filename,action_size,errno);
    boot_image bimage;
    if(!(return_value=load_boot_image_from_memory(action_data,action_size,&bimage))){
	D("%s is a boot image - load_boot_image_from_memory returned %d\n",action->filename,return_value);
	return_value = update_boot_image(action, gaction,&bimage);
	D("update_boot_image returned %d\n",return_value);
	if(bimage.start_addr != NULL ) free(bimage.start_addr); 
	return return_value;   
    
    }else{
	if(bimage.start_addr != NULL ) free(bimage.start_addr); 
	
	
    }
    

    kernel_image kimage;
    errno = 0 ; 
    if(!(return_value = load_kernel_image_from_memory(action_data,action_size,&kimage))){
	D("load_kernel_image_from_memory returns:%d\n", return_value); 
	
	return_value = update_kernel_image(&kimage,action,1);
	if(kimage.start_addr != NULL  )  free(kimage.start_addr);
        return return_value;
    }
    
    
    ramdisk_image rimage;
    init_ramdisk_image(&rimage);
    return_value = load_ramdisk_image_from_archive_memory(action_data,action_size,&rimage);
    
    if(!return_value){
	D("load_ramdisk_image_from_archive_memory returns:%d\n",rimage.entry_count); 
	return_value = update_ramdisk_archive(&rimage,action,1);
	free(rimage.start_addr); 
	return return_value;
    }
    
     if(!load_ramdisk_image_from_cpio_memory(action_data,action_size,&rimage)){
	return_value = update_ramdisk_cpio(&rimage,action,1);
	free(rimage.start_addr); 
	return return_value;
    }
    
    
    print_program_error_file_type_not_recognized(action->filename);
    
    return 0;
}



int process_update_action(int argc,char ** argv,global_action* gaction){

	
    // Initialize the action struct with NULL values
    update_action action;
    action.filename 	= NULL 	;
    action.header_filename  	= NULL 	;
    action.kernel_filename  	= NULL 	;
    action.ramdisk_filenames  	= NULL 	;
    action.ramdisk_cpioname  	= NULL 	;
    action.ramdisk_imagename  	= NULL 	;
    action.ramdisk_directory  	= NULL 	;
    action.second_filename  	= NULL 	;
    action.output_filename  	= NULL 	;
    action.property_names  	= NULL 	;
    action.ramdisk_filenames_count	= 0	;
    action.property_count = 0 ;

    FILE*file; int ramdisk_set = 0;
      
    while(argc > 0){
	    
	// check for a valid file name
	if(!action.filename && (file=fopen(argv[0],"r+b"))){
		
		fclose(file);
		action.filename = argv[0];
		D("action.bootimage_filename:%s\n",action.filename);
		// set full extract if this is the last token 
		// or if the next token is NOT a switch. 
		
		if(argc == 1 || ( argv[1][0]=='-' && argv[1][1]=='o') ||argv[1][0]!='-'){ 
		    D("extract all\n");
		    action.header_filename 	= (char*)"header";
		    action.kernel_filename 	= "kernel";
		    action.ramdisk_cpioname 	= "ramdisk.cpio";
		    action.ramdisk_imagename 	= "ramdisk.img";
		    action.ramdisk_directory 	= "ramdisk";
		    action.second_filename 	= "second";
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
		fprintf(stderr,"action.header_filename:%s\n",action.header_filename);
		
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
		fprintf(stderr,"action.kernel_filename:%s\n",action.kernel_filename);
		
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

	    fprintf(stderr,"action.output_filename:%s\n",action.output_filename);
		
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
		fprintf(stderr,"action.second_filename:%s\n",action.second_filename);
		
	} else if(!strlcmp(argv[0],"--properties") || !strlcmp(argv[0],"-p")) {
		
            // properties. This is a variable length char array
            // containing a list of properties to be modified in the 
            // default.prop
                    
            // if this is the last token or if the next token is a switch
            // we need to create an array with 1 entry 
              fprintf(stderr,"action.property\n");
            // work out how much memory is required
            int targc = 0 ; 
            for(targc=0; targc < argc-1 ; targc++ ){
                fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
                if(argv[targc+1] && argv[targc+1][0]=='-')
                  break;
                else
                action.property_count++;		
                
            }
            fprintf(stderr,"action.property_count %d argc %d\n",action.property_count,argc);
            // allocate the memory and assign null to the end of the array
            action.property_names = calloc(action.property_count,sizeof(unsigned char*)) ;
            // populate the array with the values 
            for(targc =0 ; targc < action.property_count; targc++) {
                argc--; argv++;
                action.property_names[targc] = argv[0]; 
                fprintf(stderr,"action.property_names[%d]:%s\n",targc,action.property_names[targc] );
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
		fprintf(stderr,"action.ramdisk_cpioname:%s\n",action.ramdisk_cpioname);
		
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
		fprintf(stderr,"action.ramdisk_directory:%s\n",action.ramdisk_directory);
		    
	    }else if(!strlcmp(argv[0],"--image") || !strlcmp(argv[0],"-i")) {
		    
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
		fprintf(stderr,"action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
		
	    } else if(!strlcmp(argv[0],"--files") || !strlcmp(argv[0],"-f")) {
		
		// ramdisk files. This is a variable length char array
		// containing a list of file to extract from the ramdisk
			
		// if this is the last token or if the next token is a switch
		// we need to create an array with 1 entry 
		
		// work out how much memory is required
		int targc = 0 ; 
		for(targc=0; targc < argc-1 ; targc++ ){
		   D("argv[%d] %s\n",targc,argv[targc]);
		    if(argv[targc+1] && argv[targc+1][0]=='-')
		      break;
		    else
		    action.ramdisk_filenames_count++;		
		    
		}
		fprintf(stderr,"action.ramdisk_filenames_count %d argc %d\n",action.ramdisk_filenames_count,argc);
		// allocate the memory and assign null to the end of the array
		action.ramdisk_filenames = calloc(action.ramdisk_filenames_count,sizeof(unsigned char*)) ;
		// populate the array with the values 
		for(targc =0 ; targc < action.ramdisk_filenames_count; targc++) {
		    argc--; argv++;
		    action.ramdisk_filenames[targc] = argv[0]; 
		    fprintf(stderr,"action.ramdisk_filenames[%d]:%s\n",targc,action.ramdisk_filenames[targc] );
		}
		ramdisk_set = 1 ;
	    }        
	}
        argc--; argv++ ;
    }
    // we must have at least a boot image to process
    if(!action.filename) 
	return print_program_error_file_name_not_found(action.filename);
    // output_file not set. use the bootimage filename
    if(!action.output_filename){
	D("no output set using bootimage_filename:%s\n",action.filename);
	action.output_filename = action.filename;
    }
	
    
    update_file(&action,gaction);
       
    return 0;
}
