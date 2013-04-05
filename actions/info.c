
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
// internal program headers
#include <actions.h>
#include <utils.h>
#include <bootimage.h>

typedef struct info_action info_action;

struct info_action{
	
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
    char ** 	property_names 	;
    unsigned property_count ;
};

int info_boot_image(info_action* action){
    
    errno = 0 ;
    boot_image bimage ;
    char* current_working_directory = NULL; 
    getcwd(current_working_directory,PATH_MAX);
    int return_value = load_boot_image(action->bootimage_filename,&bimage);
    if(return_value != 0){
        if(bimage.start_addr != NULL  ) free(bimage.start_addr);
        return return_value;
    }
    
    print_boot_image_info(&bimage);
    
    kernel_image kimage;
    return_value = load_kernel_image(bimage.kernel_addr,bimage.kernel_size,&kimage);
    if(return_value != 0){
        if(bimage.start_addr != NULL  ) free(bimage.start_addr);
        return return_value;
    }
    
    print_kernel_info(&kimage);
    
    ramdisk_image rimage;
    return_value = load_ramdisk_image(bimage.ramdisk_addr,bimage.ramdisk_size,&rimage);
    
    print_ramdisk_info(&rimage);
    
    free(rimage.start_addr);
    free(kimage.start_addr);
    free(bimage.start_addr);
    
    return 0 ;
}

int process_info_action(int argc,char ** argv){
    
    info_action action;
    action.bootimage_filename 	= NULL 	;
    FILE*file; int ramdisk_set = 0;
      
    if(argc > 0){
	    
	if(!action.bootimage_filename && (file=fopen(argv[0],"r+b"))){
	    
	    fclose(file);
	    action.bootimage_filename = argv[0];
	    fprintf(stderr,"action.bootimage_filename:%s\n",action.bootimage_filename);
	}
    }
    // we must have at least a boot image to process
    if(!action.bootimage_filename){
	    fprintf(stderr,"no boot image:%s\n",action.bootimage_filename);
	    return EINVAL;
    }
    info_boot_image(&action);
    return 0;
}