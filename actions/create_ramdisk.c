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

typedef struct create_ramdisk_action create_ramdisk_action;

struct create_ramdisk_action{
    char*	ramdisk_imagename ;
    char*	ramdisk_directory ;
    char*	ramdisk_cpioname ;
    int 	compression_type ;
};
int create_ramdisk(create_ramdisk_action* action){
    
    return 0;
    
}
int process_create_ramdisk_action(int argc,char ** argv,global_action* gaction){
    

    
    create_ramdisk_action action;
    action.ramdisk_imagename 	= NULL 	;
    action.ramdisk_directory	= NULL 	;
    action.ramdisk_cpioname	= NULL 	;
    action.compression_type 	= 0 	;
    FILE*file; 
    while(argc > 0){
	
	if(!action.ramdisk_imagename){
	    action.ramdisk_imagename = argv[0];
	    fprintf(stderr,"action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
	}
	argc--; argv++ ;
    }
    // we must have a source and destination image to process
    if(!action.ramdisk_imagename){
	    fprintf(stderr,"no ramdisk_imagename\n");
	    errno = EINVAL ;
	    return EINVAL;
    }
  
    create_ramdisk(&action);
    return 0;
}