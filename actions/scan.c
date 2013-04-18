#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <utils.h>
#include <bootimg.h>
#include <actions.h>	

#define SCAN_TYPE_QUICK 0
#define SCAN_TYPE_FULL  1
#define SCAN_TYPE_USER_DEFINED_SIZE  2

#define SCAN_SIZE_QUICK 8092
#define SCAN_SIZE_FULL  0

#define REPORT_LEVEL_BOOT_IMAGE 0
#define REPORT_LEVEL_BOOT_IMAGE_TYPE 1
#define REPORT_LEVEL_BOOT_IMAGE_VERSION 2

typedef struct scan_action scan_action;

/*  */

struct scan_action{
	
	char *		target_directory	;
	char *		target_filename		;
	char **		target_wildcards	;
	char *  	path_depth	 	;
	unsigned	scan_size		;
	int	  	scan_type		; 
	int	  	report_level		;
	
};
int check_for_android_magic(FILE* filefp,unsigned read_size){
    
    errno = 0;
    unsigned char* data = calloc(read_size,sizeof(char));
    if(data == 0) {
	errno = ENOMEM; 
    
    }else{
	if(fread(data, 1,read_size, filefp) != read_size){
	    if(!feof(filefp))
		errno = ferror(filefp);
	}else{
	    unsigned char* magic = find_in_memory(data,read_size,BOOT_MAGIC,BOOT_MAGIC_SIZE);
	    if(!magic)
		errno = ENODATA ;
	}
	free(data);
    }
    
    // fprintf(stderr,"read_item_from_disk data %p\n",data);
    //clearerr(filefp);
    fclose(filefp);
    return errno;
    
}

int scan_for_boot_image(scan_action* action){
    
    if(action->target_filename){
	fprintf(stderr,"scan_for_boot_image in target_filename:%s\n",action->target_filename);
	FILE* filefp = fopen(action->target_filename,"rb");
	if(!filefp) return errno;
	
	if(action->scan_type==SCAN_TYPE_QUICK){
	    if(!check_for_android_magic(filefp,action->scan_size)){
		fprintf(stderr,"boot image not found :%s\n",action->target_filename);
	    }else{
	    	fprintf(stderr,"boot image found in :%s\n",action->target_filename);
	    }
	}
    }
    
    
    return 0;
}

int process_scan_action(unsigned argc,char ** argv,global_action* gaction){

    scan_action action;
    action.target_directory 	= NULL 	;
    action.target_filename 	= NULL 	;
    action.scan_size		= SCAN_SIZE_QUICK ;
    action.scan_type		= SCAN_TYPE_QUICK ;
    action.report_level		= REPORT_LEVEL_BOOT_IMAGE ;
    errno = 0 ;
    //action.target_directory 	= NULL 	;
    
    if(argc > 0){
	struct stat target_stat ;
	
	
	if(stat(argv[0],&target_stat) == -1){
	    return errno; 
	}
	
	if(S_ISDIR(target_stat.st_mode)){
	    action.target_directory = argv[0];
	}else if (S_ISBLK(target_stat.st_mode)){
	   action.target_filename = argv[0];
	}
    }
    int return_value = scan_for_boot_image(&action);
    if(return_value)
	fprintf(stderr,"scan_for_boot_image failed with error:%d %s\n",return_value,strerror(return_value));
	
	
    
    return return_value ;
    
}