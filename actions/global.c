
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

int init_global_action(char* program_name, global_action* action){
    
    errno = 0;
    if(!action){
	errno = EINVAL ;
	return errno;
    }
    action->debug = 0;
    action->log = 0;
    action->verbose = 0 ;
    action->multicall = 0 ;
    action->program_name = program_name ;
    return 0;
}

//  only_global_actions will check the argument list for parameters other than global actions
//  returns 0 if the list contains only global actions
//  returns 1 if the list contains extra parameters
int only_global_actions(int argc,char ** argv,global_action* action){
    
    errno = 0;
    D("only_global_actions: debug=1\n",action->debug);
    int return_value = 0;
    while(argc > 0){
       D("only_global_actions argv[0]=%s\n",argv[0]);
	if(!strlcmp(argv[0],"--debug") || !strlcmp(argv[0],"--verbose") || !strlcmp(argv[0],"--log")){
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

int process_global_action(int argc,char ** argv,global_action* action){
    
    errno = 0;
    if(!action){
	errno = EINVAL ;
	return errno;
    }
    init_global_action(argv[0],action);
   
    while(argc > 0){
		
	if(!strlcmp(argv[0],"--debug")){
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