#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>	
#include <utils.h>
#include <actions.h>

int get_action(int argc,char ** argv,global_action* gaction){

	D("argv[0]=%s\n",argv[0]);
	switch(gaction->process_action){				
		case ACTION_INFO:				process_info_action(--argc,++argv,gaction);				break;				
		case ACTION_UPDATE:				process_update_action(--argc,++argv,gaction);			break;			
		case ACTION_UPDATE_KERNEL:		break;
		case ACTION_UPDATE_RAMDISK:		break;
		case ACTION_UPDATE_PROPERTIES:	break;
		case ACTION_UPDATE_FILES:		break;		
		case ACTION_EXTRACT:			process_extract_action(--argc,++argv,gaction);			break;			
		case ACTION_EXTRACT_KERNEL:		break;	
		case ACTION_EXTRACT_RAMDISK:	break;	
		case ACTION_EXTRACT_HEADER:		break;	
		case ACTION_SCAN:				process_scan_action(--argc,++argv,gaction);				break;				
		case ACTION_COPY_KERNEL:		process_copy_kernel_action(--argc,++argv,gaction);		break;		
		case ACTION_COPY_RAMDISK:		break;
		case ACTION_CREATE_BOOT_IMAGE:	process_create_action(--argc,++argv,gaction); 			break;
		case ACTION_CREATE_RAMDISK:		process_create_ramdisk_action(--argc,++argv,gaction);	break;
		case ACTION_CREATE_KERNEL:		break;
		default:						break;
		}	 
		
	return  0 ;

}

int main(int argc,char ** argv){

	
	global_action gaction ;
	init_global_action(argc,argv,&gaction);
	if(argc==1){
		
		return 0;
	}
	// look for global actions first as this effects output
    process_global_action(argc,argv, &gaction) ;

    D("gaction.debug:%d argc:%d\n",gaction.debug,argc);
    if(gaction.debug){
		int i = 0 ;
		for(i = 0 ; i < argc ; i ++ ){
			D("argv[%d]=%s\n",i,argv[i]);
		}
	}
	
	// was this a multicall, if not then move the arg pointer along
	if(!gaction.multicall){
		// not a multicall
		if(!strlcmp(argv[1],"-help") || !strlcmp(argv[1],"-h")){
			 //standard help requested 
			 D("Printing Standard Help\n");
		 }
		--argc ; ++argv	;
	}
	
	get_action(argc , argv, &gaction );
		
	return 0;

}
