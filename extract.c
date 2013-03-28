
// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>

// libbootimage headers
#include <utils.h>
#include <bootimage.h>

// internal program headers
#include <extract.h>

typedef struct extract_action extract_action;

struct extract_action{
	
	char *		bootimage_filename	;
	char *  	header_filename 	;
	char *  	kernel_filename 	;
	
	char ** 	ramdisk_filenames 	;
	char *  	ramdisk_cpioname 	;
	char *  	ramdisk_imagename 	;
	unsigned char *  	ramdisk_directory 	;
	char *		second_filename		;
	unsigned char *  	output_directory 	;
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
	FILE* kernel_fp = fopen(action->kernel_filename,"w+b");
	fwrite(bimage.kernel_addr,bimage.kernel_size,1,kernel_fp);
	fclose(kernel_fp);
    }
    
    if(action->second_filename && bimage.second_size > 0){
	FILE* second_fp = fopen(action->second_filename,"w+b");
	fwrite(bimage.second_addr,bimage.second_size,1,second_fp);
	fclose(second_fp);
    }
    
    if(action->ramdisk_imagename){
	FILE* ramdisk_fp = fopen(action->ramdisk_imagename,"w+b");
	fwrite(bimage.ramdisk_addr,bimage.ramdisk_size,1,ramdisk_fp);
	fclose(ramdisk_fp);
    }
    
    
    if(action->ramdisk_cpioname || action->ramdisk_directory){
		 
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
	    FILE* ramdiskcpio_fp = fopen(action->ramdisk_cpioname,"w+b");
	    fwrite(rimage.start_addr,rimage.size,1,ramdiskcpio_fp);
	    fclose(ramdiskcpio_fp);
	}
	if(action->ramdisk_directory)
	    save_ramdisk_entries_to_disk(&rimage,action->ramdisk_directory);
	
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

    FILE*file;
      
    while(argc > 0){
	    
	// check for a valid file name
	if((file=fopen(argv[0],"r+b"))){
		
		fclose(file);
		action.bootimage_filename = argv[0];
		
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
		
	}else if(!strlcmp(argv[0],"--cpio") || !strlcmp(argv[0],"-C")) {
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.ramdisk_cpioname = "ramdisk.cpio";
		}else{
			action.ramdisk_cpioname = argv[1];
			--argc; ++argv;
		}
	
	}else if(!strlcmp(argv[0],"--directory") || !strlcmp(argv[0],"-d")) {
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.ramdisk_directory = "ramdisk";
		}else{
			action.ramdisk_directory = argv[1];
			--argc; ++argv;
		}
	
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
	}else if(!strlcmp(argv[0],"--files") || !strlcmp(argv[0],"-f")) {
		
		// ramdisk files. This is a variable length null terminated 
		// char array contain a list of file to extract from the ramdisk
		//
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.ramdisk_filenames = calloc(1,sizeof(unsigned char*)) ;
			action.ramdisk_filenames[0] = NULL ;
		}else{
			action.ramdisk_directory = argv[1];
			--argc; ++argv;
		}
	
	}
	argc--; argv++ ;
    }
	
    // we must have at least a boot image to process
    if(!action.bootimage_filename){
	    fprintf(stderr,"no boot image:%s\n",action.bootimage_filename);
	    return EINVAL;
    }
    
    fprintf(stderr,"action.bootimage_filename:%s\n",action.bootimage_filename);
    
    fprintf(stderr,"action.header_filename:%s\n",action.header_filename);
    fprintf(stderr,"action.kernel_filename:%s\n",action.kernel_filename);
    fprintf(stderr,"action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
    fprintf(stderr,"action.ramdisk_cpioname:%s\n",action.ramdisk_cpioname);
    fprintf(stderr,"action.ramdisk_directory:%s\n",action.ramdisk_directory);
    
    fprintf(stderr,"action.second_filename:%s\n",action.second_filename);
    fprintf(stderr,"action.output_directory:%s\n",action.output_directory);
    
    extract_bootimage(&action);
    
    

    
    return 0;
}


