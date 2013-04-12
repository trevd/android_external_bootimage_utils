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

typedef struct copy_kernel_action copy_kernel_action;

struct copy_kernel_action{
    char*	source ;
    char*	destination ;
    char*	output ;
};
int copy_kernel(copy_kernel_action* action){
    
    
    boot_image bimage_source , bimage_dest;
    errno = 0;
    
    // load the source boot image
    int return_value=load_boot_image_from_file(action->source,&bimage_source);
    if(return_value){
	errno = return_value ;
	if(bimage_source.start_addr != NULL  ) free(bimage_source.start_addr);
	return return_value ;
    }
    
    // load the destination boot image
    errno = 0;
    return_value=load_boot_image_from_file(action->destination,&bimage_dest);
    if(return_value){
	errno = return_value ;
	if(bimage_dest.start_addr != NULL  ) free(bimage_dest.start_addr);
	return return_value ;
    }
    print_boot_image_info(&bimage_dest);
    bimage_dest.kernel_addr = bimage_source.kernel_addr ;
    fprintf(stderr,"bimage_dest.header->kernel_size:%u\n",bimage_dest.header->kernel_size);
    bimage_dest.header->kernel_size = bimage_source.header->kernel_size ;
    fprintf(stderr,"bimage_dest.header->kernel_size:%u\n",bimage_dest.header->kernel_size);
    set_boot_image_padding(&bimage_dest);
    set_boot_image_content_hash(&bimage_dest);
    set_boot_image_offsets(&bimage_dest);
    
    print_boot_image_info(&bimage_dest);
    
    write_boot_image(action->destination,&bimage_dest);
    
    if(bimage_source.start_addr != NULL  ) free(bimage_source.start_addr);
    if(bimage_dest.start_addr != NULL  ) free(bimage_dest.start_addr);
    
    
    // we will create a clean
    
    
    return 0;
    
}
int process_copy_kernel_action(int argc,char ** argv){
    
    copy_kernel_action action;
    action.source 	= NULL 	;
    action.destination 	= NULL 	;
    action.output 	= NULL 	;
    FILE*file; 
    while(argc > 0){
	
	if(argc > 1){
	    if(!action.source && (file=fopen(argv[0],"r+b"))){
		fclose(file);
		action.source = argv[0];
		fprintf(stderr,"action.source:%s\n",action.source);
		argc--; argv++ ;
		
		if(!action.destination && (file=fopen(argv[0],"r+b"))){
		    fclose(file);
		    action.destination = argv[0];
		    fprintf(stderr,"action.destination:%s\n",action.destination);
		   
		}
	    }
	    
	}
	 argc--; argv++ ;
    }
    // we must have a source and destination image to process
    if(!action.source){
	    fprintf(stderr,"no source\n");
	    errno = EINVAL ;
	    return EINVAL;
    }
    // we must have a source and destination image to process
    if(!action.destination){
	    fprintf(stderr,"no destination\n");
	    errno = EINVAL ;
	    return EINVAL;
    }
    copy_kernel(&action);
    return 0;
}