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
    unsigned char*	source ;
    unsigned char*	destination ;
};
int copy_kernel(copy_kernel_action* action){
    
    return 0;
    
}
int process_copy_kernel_action(int argc,char ** argv){
    
    copy_kernel_action action;
    action.source 	= NULL 	;
    action.destination 	= NULL 	;
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
    return 0;
}