
// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <string.h>	

// libbootimage headers
#include <utils.h>
#include <bootimage.h>

// internal program headers
#include <extract.h>

#define DEFAULT_BOOTIMAGE_NAME "boot"

typedef struct extract_action extract_action;

struct extract_action{
	
	char *		bootimage_filename	;
	char *  	header_filename 	;
	char *  	kernel_filename 	;
	char ** 	ramdisk_filenames 	;
	char *  	ramdisk_cpioname 	;
	char *  	ramdisk_imagename 	;
	unsigned char * ramdisk_directory 	;
	char *		second_filename		;
	unsigned char * output_directory 	;
	unsigned	ramdisk_filenames_count	;

};

int extract_bootimage(extract_action* action){
    
    boot_image bimage ;
    char* current_working_directory = NULL; 
    getcwd(current_working_directory,PATH_MAX);
    int return_value = load_boot_image(action->bootimage_filename,&bimage);
    //fprintf(stderr,"load_boot_image:%s %p %d %p\n",current_working_directory,action->bootimage_filename,return_value,bimage);
    if(return_value != 0){
        if(bimage.start_addr != NULL  ) free(bimage.start_addr);
        return return_value;
    }
    
    if(action->output_directory){
	fprintf(stderr,"action->output_directory:%s\n",action->output_directory);
	mkdir_and_parents(action->output_directory,0755);
	chdir((char*)action->output_directory);
    }
    
    if(action->header_filename)
	write_boot_image_header_to_disk(action->header_filename,&bimage);
	
    if(action->kernel_filename){
	if(write_item_to_disk(bimage.kernel_addr,bimage.kernel_size,33188,action->kernel_filename))
	    fprintf(stderr,"error writing %s %d %s\n",action->kernel_filename,errno,strerror(errno));
    }
    
    if(action->second_filename && bimage.second_size > 0){
	if(write_item_to_disk(bimage.second_addr,bimage.second_size,33188,action->second_filename))
		fprintf(stderr,"error writing %s %d %s\n",action->second_filename,errno,strerror(errno));
    }
    
    if(action->ramdisk_imagename){
	if(write_item_to_disk(bimage.ramdisk_addr,bimage.ramdisk_size,33188,action->ramdisk_imagename))
	    fprintf(stderr,"error writing %s %d %s\n",action->ramdisk_imagename,errno,strerror(errno));
    }
    
    
    if(action->ramdisk_cpioname || action->ramdisk_directory || action->ramdisk_filenames_count > 0){
		 
	ramdisk_image rimage; 
	return_value = load_ramdisk_image(bimage.ramdisk_addr,bimage.ramdisk_size,&rimage);
	//fprintf(stderr,"load_ramdisk_image function returns %d %s\n",return_value,strerror(return_value));
	if(return_value != 0){
	    if(rimage.start_addr != NULL  ) {
		free(rimage.start_addr);
		goto cleanup_bootimage;
	    }
	}
	
	if(action->ramdisk_cpioname){
	    
	    if(write_item_to_disk(rimage.start_addr,rimage.size,33188,action->ramdisk_cpioname))
		fprintf(stderr,"error writing %s %d %s\n",action->ramdisk_cpioname,errno,strerror(errno));
	    
	}
	if(action->ramdisk_directory)
	    save_ramdisk_entries_to_disk(&rimage,action->ramdisk_directory);
	
	// extract a single file from the ramdisk 
	int entry_index = 0 ; int filename_index = 0 ; 
	for (filename_index = 0 ; filename_index < action->ramdisk_filenames_count ; filename_index ++){
	    for (entry_index = 0 ; entry_index < rimage.entry_count ; entry_index ++){
		if(!strlcmp(rimage.entries[entry_index]->name_addr,action->ramdisk_filenames[filename_index])){
		    fprintf(stderr,"%s\n",rimage.entries[entry_index]->name_addr);
		    //FILE* ramdiskfile_fp = fopen(action->ramdisk_filenames[filename_index],"w+b");
		    if(write_item_to_disk_extended(rimage.entries[entry_index]->data_addr,rimage.entries[entry_index]->data_size,
			rimage.entries[entry_index]->mode,action->ramdisk_filenames[filename_index],rimage.entries[entry_index]->name_size)){
			
			fprintf(stderr,"error writing %s %d %s\n",action->ramdisk_filenames[filename_index],errno,strerror(errno));
		    }
		    break;
		}
	    }
	}
	free(rimage.start_addr);
    }
       
    
cleanup_bootimage:
    free(bimage.start_addr);
    return 0;
    
}
// process_extract_action - parse the command line switches
// although this code is repetitive we will favour readability
// over codesize ...... Ask me in 3 months time whether it was
// a good idea.
int process_extract_action(int argc,char ** argv){
	
    // Initialize the action struct with NULL values
    extract_action action;
    action.bootimage_filename 	= NULL 	;
    action.header_filename  	= NULL 	;
    action.kernel_filename  	= NULL 	;
    action.ramdisk_filenames  	= NULL 	;
    action.ramdisk_cpioname  	= NULL 	;
    action.ramdisk_imagename  	= NULL 	;
    action.ramdisk_directory  	= NULL 	;
    action.second_filename  	= NULL 	;
    action.output_directory  	= NULL 	;
    action.ramdisk_filenames_count	= 0	;

    FILE*file;
      
    while(argc > 0){
	    
	// check for a valid file name
	if(!action.bootimage_filename && (file=fopen(argv[0],"r+b"))){
		
		fclose(file);
		action.bootimage_filename = argv[0];
		fprintf(stderr,"action.bootimage_filename:%s\n",action.bootimage_filename);
		// set full extract if this is the last token 
		// or if the next token is NOT a switch. 
		
		if(argc == 1 || argv[1][0]!='-'){ 
		    fprintf(stderr,"extract all\n");
		    action.header_filename 	= "header";
		    action.kernel_filename 	= "kernel";
		    action.ramdisk_cpioname 	= "ramdisk.cpio";
		    action.ramdisk_imagename 	= "ramdisk.img";
		    action.ramdisk_directory 	= "ramdisk";
		    action.second_filename 	= "second";
		    // do we have an impiled output directory
		    if (argv[1]) action.output_directory = argv[1];

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
		
	}else if(!strlcmp(argv[0],"--cpio") || !strlcmp(argv[0],"-C")) {
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.ramdisk_cpioname = "ramdisk.cpio";
		}else{
			action.ramdisk_cpioname = argv[1];
			--argc; ++argv;
		}
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
		fprintf(stderr,"action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
	
	}else if(!strlcmp(argv[0],"--output") || !strlcmp(argv[0],"-o")) {
		
		// the output directory. if this does not exist then
		// we will attempt to create it.
		
		// if it does exists we will offer to either wipe or 
		// append.
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.output_directory = NULL;
		}else{
			action.output_directory = argv[1];
			--argc; ++argv;
		}

	    fprintf(stderr,"action.output_directory:%s\n",action.output_directory);
		
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
		
	}else if(!strlcmp(argv[0],"--files") || !strlcmp(argv[0],"-f")) {
		
		// ramdisk files. This is a variable length char array
		// containing a list of file to extract from the ramdisk
				
		// if this is the last token or if the next token is a switch
		// we need to create an array with 1 entry 
	    
		// work out how much memory is required
		int targc = 0 ; 
		for(targc=0; targc < argc-1 ; targc++ ){
		    fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
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
	}
	argc--; argv++ ;
    }
	
    // we must have at least a boot image to process
    if(!action.bootimage_filename){
	    fprintf(stderr,"no boot image:%s\n",action.bootimage_filename);
	    return EINVAL;
    }
    
    extract_bootimage(&action);
       
    return 0;
}


