/* info.c - functions to display boot image information */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
// internal program headers
#include <actions.h>
#include <utils.h>
#include <bootimage.h>
#include <program.h>
#include <help.h>

typedef struct info_action info_action;

struct info_action{
	
	char *		filename;
	int	  	header 	;
	int	  	additional ;
	int	  	kernel 	;
	int	 	ramdisk ;
	int		second  ;
};
int info_kernel( kernel_image* kimage,info_action* action,int print_title){
    
    if(print_title){
	print_program_title();
	fprintf(stderr," Printing Kernel Information for \"%s\"\n\n",action->filename);
    }else{
	fprintf(stderr," Kernel:\n");
    }
    
    print_kernel_info(kimage);
    fprintf(stderr,"\n");
    
    return 0 ;
}
int info_ramdisk(ramdisk_image* rimage,info_action* action,int print_title){
    
    if(print_title){
	print_program_title();
	fprintf(stderr," Printing Ramdisk Information for \"%s\"\n\n",action->filename);
    }else{
	fprintf(stderr," Ramdisk:\n");
    }
    print_ramdisk_info(rimage);
    fprintf(stderr,"\n");
    return 0;
    
}

int info_boot_image(info_action* action,global_action* gaction ,boot_image* bimage){
       
    D("\n");
    
    print_program_title();
    
    fprintf(stderr," Printing boot image information for \"%s\"\n\n",action->filename);
    if(action->header){
	
	fprintf(stderr," Header:\n");
	print_boot_image_header_info(bimage);
	fprintf(stderr,"\n");
    }
  
    if(action->additional){
	fprintf(stderr," File structure:\n");
	print_boot_image_additional_info(bimage);
	fprintf(stderr,"\n");
	   

    }
    
    
    kernel_image kimage;
    if(action->kernel && !load_kernel_image_from_memory(bimage->kernel_addr,bimage->header->kernel_size,&kimage)){
	info_kernel(&kimage,action,0) ;
	if(kimage.start_addr != NULL  )  free(kimage.start_addr);
        
    }
    if(action->ramdisk){
	ramdisk_image rimage;
	if(!load_ramdisk_image_from_archive_memory(bimage->ramdisk_addr,bimage->header->ramdisk_size,&rimage)){
	    info_ramdisk(&rimage,action,0);
	    free(rimage.start_addr);
	}
    }
    return 0 ;
}
int info_file(info_action* action,global_action* gaction ){

   
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
	return_value = info_boot_image(action, gaction,&bimage);
	D("info_boot_image returned %d\n",return_value);
	if(bimage.start_addr != NULL ) free(bimage.start_addr); 
	return return_value;   
    
    }else{
	if(bimage.start_addr != NULL ) free(bimage.start_addr); 
	
	
    }
    

    kernel_image kimage;
    errno = 0 ; 
    if(!(return_value = load_kernel_image_from_memory(action_data,action_size,&kimage))){
	D("load_kernel_image_from_memory returns:%d\n", return_value); 
	
	return_value = info_kernel(&kimage,action,1);
	if(kimage.start_addr != NULL  )  free(kimage.start_addr);
        return return_value;
    }
    
    
    ramdisk_image rimage;
    init_ramdisk_image(&rimage);
    return_value = load_ramdisk_image_from_archive_memory(action_data,action_size,&rimage);
    
    if(!return_value){
	D("load_ramdisk_image_from_archive_memory returns:%d\n",rimage.entry_count); 
	return_value = info_ramdisk(&rimage,action,1);
	free(rimage.start_addr); 
	return return_value;
    }
    
     if(!load_ramdisk_image_from_cpio_memory(action_data,action_size,&rimage)){
	return_value = info_ramdisk(&rimage,action,1);
	free(rimage.start_addr); 
	return return_value;
    }
    
    
    print_program_title();
    fprintf(stderr," Cannot process \"%s\" - file type not a recognized\n\n",action->filename);    
    
    return 0;
}


int process_info_action(int argc,char ** argv,global_action* gaction){
    
    
    D("argc=%d argv[0]=%s\n",argc,argv[0]);
    if(!strlcmp(argv[0],"--help") || !strlcmp(argv[0],"-h")){
	
	return print_info_action_help(gaction);
    }
    
    info_action action ;
    action.filename 	= NULL 	;
    action.kernel	= 0 	;
    action.header	= 0 	;
    action.ramdisk	= 0 	;
    action.second	= 0 	;
    action.additional	= 0 	;
    
    // a variable for the file check
    FILE*file; 
       
    // work out a possible file name just in case we need it for 
    // error reporting , the possible filename should be at position zero 
    // but it maybe elsewhere, as info printing doesn't require any require 
    // filenames for switches we can look for the first argv that doesn't begin with "-"
    unsigned char* possible_filename = NULL;
    int i = 0 ;
    for(i = 0 ; i < argc ; i++){
    	if(argv[i][0]!='-'){
	     possible_filename = argv[i];
	     D("possible_filename at position %d - %s %s\n",i,argv[i],possible_filename);
	     break ;
	 }
    }
    
    
    // this is set to 1 if any action item has been set
    int action_set = 0 ; 
      
    while(argc > 0){
	
	if(!action.filename && (file=fopen(argv[0],"r+b"))){
	    fclose(file);
	    action.filename = argv[0];
	    D("action.filename:%s\n",action.filename);
	    if(!action_set){
		
		// if this is the last token the following token are global action only
		// then print full information
		if(argc == 1 || argv[1][0]!='-' || !only_global_actions(argc-1,argv+1,gaction)  ){ 
		    D("printing full info argc=%d\n",argc);
		    action.kernel	= 1 	;
		    action.header	= 1 	;
		    action.ramdisk	= 1 	;
		    action.second	= 1 	;
		    action.additional	= 1 	;
		}
	    }
		
	    
	}else if(!strlcmp(argv[0],"--kernel") || !strlcmp(argv[0],"-k")){
		
		// we have a kernel setting
		action.kernel = 1;
		action_set = 1 ;
		D("action.kernel:%d\n",action.kernel);
	}
	else if(!strlcmp(argv[0],"--ramdisk") || !strlcmp(argv[0],"-i") || !strlcmp(argv[0],"-r") ){
		
		// we have a ramdisk setting
		action.ramdisk = 1;
		action_set = 1 ;
		D("action.ramdisk:%d\n",action.ramdisk);
	}else if(!strlcmp(argv[0],"--header") || !strlcmp(argv[0],"-h") ){
		
		// we have a ramdisk setting
		action.header = 1;
		action_set = 1 ;
		D("action.header:%d\n",action.header);
	}else if(!strlcmp(argv[0],"--additional") || !strlcmp(argv[0],"-a") ){
		
		// we have a ramdisk setting
		action.additional = 1;
		action_set = 1 ;
		D("action.additional:%d\n",action.additional);
	}
	
	
	argc--; argv++ ;
    }
    // we must have at least a boot image to process
    if(!action.filename){
	    print_program_title();
	    if(!possible_filename){ 
		fprintf(stderr," no file specified!\n\n");
	    }else
		fprintf(stderr," %s - file not found!\n\n",possible_filename);
	    return EINVAL;
    }
    
    
    info_file(&action,gaction);
    return 0;
}