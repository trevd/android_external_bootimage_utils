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

typedef struct info_action info_action;

struct info_action{
	
	char *		filename;
	int	  	header 	;
	int	  	kernel 	;
	int	 	ramdisk ;
	int		second  ;
};
int info_ramdisk(ramdisk_image* rimage){
    
    
    //print_kernel_info(kimage);
    return 0;
    
}

int info_boot_image(info_action* action,boot_image* bimage){
       
    
    if(action->header){
	print_boot_image_info(bimage);
    }
  
    
    if(action->kernel){
        kernel_image kimage;
	if(!load_kernel_image_from_memory(bimage->kernel_addr,bimage->header->kernel_size,&kimage)){
	    print_kernel_info(&kimage);
	    if(kimage.start_addr != NULL  )  free(kimage.start_addr);
	}
    }
    if(action->ramdisk){
	ramdisk_image rimage;
	if(!load_ramdisk_image_from_archive_memory(bimage->ramdisk_addr,bimage->header->ramdisk_size,&rimage)){
	    print_ramdisk_info(&rimage);
	    free(rimage.start_addr);
	}
    }
    return 0 ;
}
int info_file(info_action* action){

    char* current_working_directory = NULL; 
    errno = 0 ; 
    int return_value=0;
    unsigned action_size;     
    getcwd(current_working_directory,PATH_MAX);
    
    unsigned char* action_data = read_item_from_disk(action->filename , &action_size);
    fprintf(stderr,"info_file: %s %u\n",action->filename,action_size);
    boot_image bimage;
    if(!(return_value=load_boot_image_from_memory(action_data,action_size,&bimage))){
	return_value = info_boot_image(action, &bimage);
	 if(bimage.start_addr != NULL ) free(bimage.start_addr); 
	return return_value;   
    
    }else{
	
	if(bimage.start_addr != NULL  ) free(bimage.start_addr);
    }


    kernel_image kimage;
    errno = 0 ; 
    if(!(return_value = load_kernel_image_from_memory(action_data,action_size,&kimage))){
	fprintf(stderr,"load_kernel_image_from_memory returns:%d\n", return_value); 
	return_value = print_kernel_info(&kimage);
	if(kimage.start_addr != NULL  )  free(kimage.start_addr);
        return return_value;
    }
    
    
    ramdisk_image rimage;
    return_value = load_ramdisk_image_from_archive_memory(action_data,action_size,&rimage);
    
    if(!return_value){
	fprintf(stderr,"load_ramdisk_image_from_archive_memory returns:%d\n",rimage.entry_count); 
	return_value = info_ramdisk(&rimage);
	free(rimage.start_addr); 
	return return_value;
    }
    
     if(!load_ramdisk_image_from_cpio_memory(action_data,action_size,&rimage)){
	return_value = info_ramdisk(&rimage);
	free(rimage.start_addr); 
	return return_value;
    }
    fprintf(stderr,"file type not a recognized\n");
    return 0;
}

int process_info_action(int argc,char ** argv){
    
    info_action action;
    action.filename 	= NULL 	;
    action.kernel	= 0 	;
    action.header	= 0 	;
    action.ramdisk	= 0 	;
    action.second	= 0 	;
    FILE*file; int ramdisk_set = 0;
      
    while(argc > 0){
	
	if(!action.filename && (file=fopen(argv[0],"r+b"))){
	    fclose(file);
	    action.filename = argv[0];
	    fprintf(stderr,"action.filename:%s\n",action.filename);
	    if(argc == 1 ||argv[1][0]!='-'){ 
		fprintf(stderr,"info all\n");
		 action.kernel	= 1 	;
		 action.header	= 1 	;
		 action.ramdisk	= 1 	;
		 action.second	= 1 	;
		}
	    
	}else if(!strlcmp(argv[0],"--kernel") || !strlcmp(argv[0],"-k")){
		
		// we have a kernel setting
		action.kernel = 1;
	    
		fprintf(stderr,"action.kernel:%d\n",action.kernel);
	}
	else if(!strlcmp(argv[0],"--ramdisk") || !strlcmp(argv[0],"-i") || !strlcmp(argv[0],"-r") ){
		
		// we have a ramdisk setting
		action.ramdisk = 1;
	    
		fprintf(stderr,"action.ramdisk:%d\n",action.ramdisk);
	}
	argc--; argv++ ;
    }
    // we must have at least a boot image to process
    if(!action.filename){
	    fprintf(stderr,"no filename\n");
	    return EINVAL;
    }
    
    
    info_file(&action);
    return 0;
}