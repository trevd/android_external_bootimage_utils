
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

int init_global_action(global_action* action){
    
    errno = 0;
    if(!action){
	errno = EINVAL ;
	return errno;
    }
    action->debug = 0;
    action->log = 0;
    action->verbose = 0 ;
    return 0;
}

int process_global_action(int argc,char ** argv,global_action* action){
    
    errno = 0;
    if(!action){
	errno = EINVAL ;
	return errno;
    }
    
    
    if(argc > 0){
		
	if(!strlcmp(argv[0],"--debug")){
	    action->debug = 1 ;
	}else if(!strlcmp(argv[0],"--verbose")){
	    action->verbose = 1 ;
	}
	else if(!strlcmp(argv[0],"--log")){
	    action->log = 1 ;
	}
	 
    }
    
    return 0;
}