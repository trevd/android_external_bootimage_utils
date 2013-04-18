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
#include <compression.h>
typedef struct create_ramdisk_action create_ramdisk_action;

struct create_ramdisk_action{
    char*	ramdisk_imagename ;
    char*	ramdisk_directory ;
    char*	ramdisk_cpioname ;
    int 	compression_type ;
};
int create_ramdisk(create_ramdisk_action* action){
    
    unsigned char* cpio_data = NULL ;
    unsigned cpio_ramdisk_size = 0;     
    
    if(action->ramdisk_directory)
	cpio_data = pack_ramdisk_directory(action->ramdisk_directory,&cpio_ramdisk_size) ;
    else if(action->ramdisk_cpioname)
	cpio_data = read_item_from_disk(action->ramdisk_cpioname,&cpio_ramdisk_size);

    if(cpio_data){
	unsigned char* ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
	unsigned ramdisk_size = 0; 
	switch(action->compression_type){
	    case RAMDISK_COMPRESSION_GZIP:	ramdisk_size=compress_gzip_memory(cpio_data,cpio_ramdisk_size,ramdisk_data,cpio_ramdisk_size); break;
	    case RAMDISK_COMPRESSION_LZO:	ramdisk_size=compress_lzo_memory(cpio_data,cpio_ramdisk_size,ramdisk_data,cpio_ramdisk_size); break;
	    default:	break;
	}
	D("ramdisk_size=%u\n",ramdisk_size);
	write_item_to_disk(ramdisk_data,ramdisk_size,33188,action->ramdisk_imagename);
	free(ramdisk_data);
	free(cpio_data);
	D("ramdisk_directory:%s\n",action->ramdisk_directory);
	
    }
    
	
    return 0;
    
}
int process_create_ramdisk_action(unsigned argc,char ** argv,global_action* gaction){
    

    
    create_ramdisk_action action;
    action.ramdisk_imagename 	= NULL 	;
    action.ramdisk_directory	= NULL 	;
    action.ramdisk_cpioname	= NULL 	;
    action.compression_type 	= RAMDISK_COMPRESSION_GZIP ;
    FILE*file; int ramdisk_set = 0;
    while(argc > 0){
	
	if(!action.ramdisk_imagename){
	    action.ramdisk_imagename = argv[0];
	    D("action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
	}else if (!strlcmp(argv[0],"--type") || !strlcmp(argv[0],"-t")) {
		
		if(argc == 1 || argv[1][0]=='-'){
		    action.compression_type = RAMDISK_COMPRESSION_GZIP;
		}else{
		   action.compression_type = int_ramdisk_compression(argv[1]);
		    --argc; ++argv;
		}
		D("action.compression_type:%s\n",str_ramdisk_compression(action.compression_type));
	    
	}else if (!ramdisk_set){

	    if(!strlcmp(argv[0],"--cpio") || !strlcmp(argv[0],"-C")) {
		    
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
		    action.ramdisk_cpioname = "ramdisk.cpio";
		}else{
		    action.ramdisk_cpioname = argv[1];
		    --argc; ++argv;
		}
		ramdisk_set = 1 ;
		D("action.ramdisk_cpioname:%s\n",action.ramdisk_cpioname);
		
	    }else if(!strlcmp(argv[0],"--directory") || !strlcmp(argv[0],"-d")) {
		    
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
		    action.ramdisk_directory = "ramdisk";
		}else{
		    action.ramdisk_directory = argv[1];
		    --argc; ++argv;
		}
		ramdisk_set = 1 ;
		D("action.ramdisk_directory:%s\n",action.ramdisk_directory);
		    
	    }
	}
	--argc; ++argv;
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