
// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <string.h>	

// internal program headers
#include <actions.h>
#include <utils.h>

int check_program_name(unsigned argc,char ** argv, global_action* action){

    // switch multicall on, we can turn it back of if we don't find one
    // cheeky logic, saves on code.
    action->multicall = 1 ;
    
    // Note : When adding new action make sure single verb words are checked
    // last, otherwise incorrect values maybe set    
    if(strstr(action->program_name,"-info"))
	action->process_action = ACTION_INFO;
    else if (strstr(action->program_name,"-extract-kernel") )
	action->process_action = ACTION_EXTRACT_KERNEL;
    else if (strstr(action->program_name,"-extract-ramdisk") )
	action->process_action = ACTION_EXTRACT_RAMDISK;
    else if (strstr(action->program_name,"-extract-header") )
	action->process_action = ACTION_EXTRACT_HEADER;
    else if (strstr(action->program_name,"-extract") )
	action->process_action = ACTION_EXTRACT;
    else if (strstr(action->program_name,"-create-ramdisk") )
	action->process_action = ACTION_CREATE_RAMDISK;
    else if (strstr(action->program_name,"-create"))
	action->process_action = ACTION_CREATE_BOOT_IMAGE;
    else if (strstr(action->program_name,"-update-properties"))
	action->process_action = ACTION_UPDATE_PROPERTIES;
    else if (strstr(action->program_name,"-update-files"))
	action->process_action = ACTION_UPDATE_FILES;
    else if (strstr(action->program_name,"-update-kernel"))
	action->process_action = ACTION_UPDATE_KERNEL;
    else if (strstr(action->program_name,"-update-ramdisk"))
	action->process_action = ACTION_UPDATE_RAMDISK;
    else if (strstr(action->program_name,"-update"))
	action->process_action = ACTION_UPDATE;
    else if (strstr(action->program_name,"-scan")) 
	action->process_action = ACTION_SCAN;
    else if (strstr(action->program_name,"-copy-kernel"))
	action->process_action = ACTION_COPY_KERNEL;
    else if (strstr(action->program_name,"-copy-ramdisk"))
	action->process_action = ACTION_COPY_RAMDISK;
    else {
	// not a multicall, check the next argument for the process action
	action->multicall = 0 ;
	argc -- ; argv ++ ;
	if(!argc) return -1 ;
	
	if(!strlcmp(argv[0],"extract") || !strlcmp(argv[0],"x"))
	    action->process_action = ACTION_EXTRACT;
	if(!strlcmp(argv[0],"info") || !strlcmp(argv[0],"i"))
	    action->process_action = ACTION_INFO;
	if(!strlcmp(argv[0],"update") || !strlcmp(argv[0],"u"))
	    action->process_action = ACTION_UPDATE;
	if(!strlcmp(argv[0],"scan") || !strlcmp(argv[0],"s"))
	    action->process_action = ACTION_SCAN;
	if(!strlcmp(argv[0],"create") || !strlcmp(argv[0],"c"))
	    action->process_action = ACTION_CREATE_BOOT_IMAGE;
	if(!strlcmp(argv[0],"create-ramdisk") || !strlcmp(argv[0],"r"))
	    action->process_action = ACTION_CREATE_RAMDISK;
	if(!strlcmp(argv[0],"copy-ramdisk"))
	    action->process_action = ACTION_COPY_RAMDISK;
	if(!strlcmp(argv[0],"copy-kernel"))
	    action->process_action = ACTION_COPY_KERNEL;
	if(!strlcmp(argv[0],"update-kernel"))
	    action->process_action = ACTION_UPDATE_KERNEL;
	if(!strlcmp(argv[0],"update-ramdisk"))
	    action->process_action = ACTION_UPDATE_RAMDISK;
	
	
    }
    return 0;

}

int init_global_action(unsigned argc,char ** argv, global_action* action){
    
    errno = 0;
    if(!action){
	errno = EINVAL ;
	return errno;
    }
    action->debug = 0;
    action->log = 0;
    action->verbose = 0 ;
    action->multicall = 0 ;
    action->program_name = argv[0] ;
    action->process_action = ACTION_NONE ; 
    check_program_name(argc,argv,action);
    return 0;
}

//  only_global_actions will check the argument list for parameters other than global actions
//  returns 0 if the list contains only global actions
//  returns 1 if the list contains extra parameters
int only_global_actions(unsigned argc,char ** argv,global_action* action){
    
    errno = 0;
    D("only_global_actions: debug=%d\n",action->debug);
    int return_value = 0;
    while(argc > 0){
       D("only_global_actions argv[0]=%s\n",argv[0]);
	if(!(argv[0],"--verbose") || !strlcmp(argv[0],"--log")){
	   return_value = 0 ;
	}else{
	    return_value =1 ;
	    break;
	}
	argc--; argv++;
    }
    D("only_global_actions returning %d\n",return_value);
    return return_value;
}

int process_global_action(unsigned argc,char ** argv,global_action* action){
    
    errno = 0;
    if(!action){
	errno = EINVAL ;
	return errno;
    }
    if(!action) init_global_action(argc,argv,action);
   
    while(argc > 0){
		
	if(getenv("BITDEBUG")){
	    action->debug = 1 ;
	    // initialize debug printing for libbootimage
	    init_debug();
	    D("debug output enabled action->debug=%d\n", action->debug) ;
	}else if(!strlcmp(argv[0],"--verbose")){
	    action->verbose = 1 ;
	}
	else if(!strlcmp(argv[0],"--log")){
	    action->log = 1 ;
	}
	argc--; argv++;
	 
    }
    
    return 0;
}