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
#include <program.h>
#include <bootimage.h>

typedef struct copy_ramdisk_action copy_ramdisk_action;

struct copy_ramdisk_action{
    char*	source ;
    char*	destination ;
    char*	output ;
};
int copy_ramdisk(copy_ramdisk_action* action){
    
    
    boot_image bimage_source , bimage_dest;
    errno = 0;
    print_program_title();
    // load the source boot image
    int return_value=load_boot_image_from_file(action->source,&bimage_source);
    if(errno){
	return_value = errno;
	fprintf(stderr," cannot open file \"%s\" as boot image - error %d - %s\n\n",action->source ,errno , strerror(errno));
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
	fprintf(stderr," cannot get kernel information in \"%s\" as boot image - error %d - %s\n",action->source ,errno , strerror(errno));
	goto fail_hard;
    }
    // load the source kernel details 
    return_value = load_kernel_image_from_memory(bimage_dest.kernel_addr,bimage_dest.header->kernel_size,&kimage_dest);
    if(errno){
	fprintf(stderr," cannot get kernel information in \"%s\" as boot image - error %d - %s\n",action->destination ,errno , strerror(errno));
	goto fail_hard;
    }
    
    // Tell 'em how it's going down
    fprintf(stderr," replacing kernel in %s\n",action->destination);
    print_kernel_info(&kimage_dest);
    fprintf(stderr," with\n");
    print_kernel_info(&kimage_source);        
        
    
   
    
    D("bimage_dest.header->kernel_size:%u\n",bimage_dest.header->kernel_size);
    bimage_dest.header->kernel_size = bimage_source.header->kernel_size ;
    D("bimage_dest.header->kernel_size:%u\n",bimage_dest.header->kernel_size);
    set_boot_image_padding(&bimage_dest);
    set_boot_image_content_hash(&bimage_dest);
    set_boot_image_offsets(&bimage_dest);
    
    //print_boot_image_info(&bimage_dest);
    
    write_boot_image(action->destination,&bimage_dest);
    
fail_hard:
    if(bimage_source.start_addr != NULL  ) free(bimage_source.start_addr);
    if(bimage_dest.start_addr != NULL  ) free(bimage_dest.start_addr);
    
    
    // we will create a clean
    
    
    return 0;
    

    
}
int process_copy_ramdisk_action(int argc,char ** argv,global_action* gaction){
    
    copy_ramdisk_action action;
    action.source 	= NULL 	;
    action.destination 	= NULL 	;
    action.output 	= NULL 	;
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
