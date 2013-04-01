#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <utils.h>
#include <actions.h>




int get_action(int argc,char ** argv){

	if(!strlcmp(argv[0],"extract") || !strlcmp(argv[0],"x")) {
		process_extract_action(--argc,++argv);
	}else if(!strlcmp(argv[0],"update") || !strlcmp(argv[0],"u")) {
		process_update_action(--argc,++argv);
	}else if(!strlcmp(argv[0],"info") || !strlcmp(argv[0],"i")) {
		process_info_action(--argc,++argv);
	}
	return  0 ;

}

int main(int argc,char ** argv){

	if(argc==1){
	
	}
	get_action(--argc,++argv);
	return 0;

}
