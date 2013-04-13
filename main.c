#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>	
#include <utils.h>
#include <actions.h>

int get_action(int argc,char ** argv,global_action* gaction){

	D("argv[0]=%s\n",argv[0]);
	

	
	if(!strlcmp(argv[0],"extract") || !strlcmp(argv[0],"x") || strstr(argv[0],"-extract") ){
		process_extract_action(--argc,++argv,gaction);
	}else if(!strlcmp(argv[0],"update") || !strlcmp(argv[0],"u") || strstr(argv[0],"-update") ) {
		process_update_action(--argc,++argv,gaction);
	}else if(!strlcmp(argv[0],"info") || !strlcmp(argv[0],"i") || strstr(argv[0],"-info") ) {
		process_info_action(--argc,++argv,gaction);
	}else if(!strlcmp(argv[0],"create-boot") || !strlcmp(argv[0],"c") || strstr(argv[0],"-create") ) {
		process_create_action(--argc,++argv,gaction);
	}else if(!strlcmp(argv[0],"scan") || !strlcmp(argv[0],"s") || strstr(argv[0],"-scan") ) {
		process_scan_action(--argc,++argv,gaction);
	}else if(!strlcmp(argv[0],"create-ramdisk") || !strlcmp(argv[0],"r") ) {
		process_create_ramdisk_action(--argc,++argv,gaction);
	}
	else if(!strlcmp(argv[0],"copy-kernel") ) {
		process_copy_kernel_action(--argc,++argv,gaction);
	}
	//fprintf(stderr,"get_action\n");
	return  0 ;

}

int main(int argc,char ** argv){

	if(argc==1){
		return 0;
	}
	// look for global actions first as this effects output
    global_action gaction ;
    process_global_action(argc,argv, &gaction) ;

    D("gaction.debug:%d argc:%d\n",gaction.debug,argc);
    if(gaction.debug){
		int i = 0 ;
		for(i = 0 ; i < argc ; i ++ ){
			D("argv[%d]=%s\n",i,argv[i]);
		}
	}
	
	// was this a multicall, if not then move the arg pointer along
	if(!(strstr(argv[0],"-info") || strstr(argv[0],"-extract") 
			|| strstr(argv[0],"-create") || strstr(argv[0],"-update") 
			|| strstr(argv[0],"-scan") ) ){
		
		// not a multicall
		if(!strlcmp(argv[1],"-help") || !strlcmp(argv[1],"-h")){
			 //standard help requested 
			 D("Printing Standard Help\n");
		 }
		
		--argc ; ++argv	;
		
	}else{
		gaction.multicall = 1 ;
	} 
	
	get_action(argc , argv,&gaction );
		
	return 0;

}
