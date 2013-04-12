

// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <string.h>	

// libbootimage headers
#include <utils.h>
#include <bootimage.h>
#include <ramdisk.h>
#include <compression.h>

// internal program headers
#include <actions.h>

typedef struct update_action update_action;

struct update_action{
	
	char *		bootimage_filename	;
	char *  	header_filename 	;
	char *  	kernel_filename 	;
	char ** 	ramdisk_filenames 	;
	char *  	ramdisk_cpioname 	;
	char *  	ramdisk_imagename 	;
	unsigned char * ramdisk_directory 	;
	char *		second_filename		;
	char * 		output_filename 	;
	unsigned	ramdisk_filenames_count	;
	char ** 	property_names 	;
	unsigned property_count ;
};

int update_ramdisk_files(update_action* action,ramdisk_image* rimage){
    
    fprintf(stderr,"update_ramdisk_files %d\n",rimage->size);	
    // extract a single file from the ramdisk 
    int entry_index = 0 ; int filename_index = 0 ; 
    for (filename_index = 0 ; filename_index < action->ramdisk_filenames_count ; filename_index ++){
	for (entry_index = 0 ; entry_index < rimage->entry_count ; entry_index ++){
	    if(!strlcmp(rimage->entries[entry_index]->name_addr,action->ramdisk_filenames[filename_index])){
		    fprintf(stderr,"%s\n",rimage->entries[entry_index]->name_addr);
		    rimage->entries[entry_index]->data_addr = read_item_from_disk( rimage->entries[entry_index]->name_size, &rimage->entries[entry_index]->data_size);
		    update_ramdisk_header(rimage->entries[entry_index]->start_addr);
		    
		    
		    //FILE* ramdiskfile_fp = fopen(action->ramdisk_filenames[filename_index],"w+b");
		    if(write_item_to_disk_extended(rimage->entries[entry_index]->data_addr,rimage->entries[entry_index]->data_size,
			rimage->entries[entry_index]->mode,action->ramdisk_filenames[filename_index],rimage->entries[entry_index]->name_size)){
			
			fprintf(stderr,"error writing %s %d %s\n",action->ramdisk_filenames[filename_index],errno,strerror(errno));
		    }
		    break;
		}
	    }
	}
	
    
    return 0 ; 
}

int update_bootimage(update_action* action){
    errno = 0 ;
    boot_image* bimage = calloc(1,sizeof(boot_image));
    
    char* current_working_directory = NULL; 
    getcwd(current_working_directory,PATH_MAX);
    int return_value = load_boot_image_from_file(action->bootimage_filename,bimage);

    fprintf(stderr,"load_boot_image_from_file:%d\n",return_value); 
    if(return_value != 0){
        if(bimage->start_addr != NULL  ) free(bimage->start_addr);
        return return_value;
    }
    
    if(action->kernel_filename){
	fprintf(stderr,"doing action->kernel_filename\n");
	
	bimage->kernel_addr = read_item_from_disk(action->kernel_filename,&bimage->header->kernel_size);
    }
    
    write_boot_image(action->output_filename,bimage);
    
    free(bimage->start_addr) ;
    return 0;
}

/*int update_bootimage1(update_action* action){
    
    errno = 0 ;
    boot_image bimage ;
    
    char* current_working_directory = NULL; 
    getcwd(current_working_directory,PATH_MAX);
    int return_value = load_boot_image_from_file(action->bootimage_filename,&bimage);
    print_boot_image_info(&bimage);
    
    fprintf(stderr,"load_boot_image_from_file:%d\n",return_value); 
    if(return_value != 0){
        if(bimage.start_addr != NULL  ) free(bimage.start_addr);
        return return_value;
    }
    fprintf(stderr,"bimage.start_addr:%p %d %d\n",bimage.start_addr,sizeof(boot_image),sizeof(bimage));
    // setup a new boot_image_struct to receive the modified information
    boot_image bni; 
    //memset(&bni,0,sizeof(boot_image));
    boot_image* bnewimage = &bni;
      fprintf(stderr,"bimage.start_addr:%p %d %d\n",bimage.start_addr,sizeof(bni),sizeof(bnewimage));
    set_boot_image_defaults(bnewimage);
    //print_boot_image_info(&bni);
    copy_boot_image_header_info(&bni,&bimage);
    unsigned new_ramdisk_data;
   
    
    int ramdisk_processed = 0;
    
    if(action->kernel_filename){
	fprintf(stderr,"doing action->kernel_filename %p\n",bnewimage->kernel_size);
	bnewimage->kernel_addr = read_item_from_disk(action->kernel_filename,&bni.kernel_size);
    }else{
	write_item_to_disk(bimage.kernel_addr,bimage.kernel_size,33188,"tk");
	fprintf(stderr,"doing action->kernel_filename not set bimage.kernel_addr :%p %u\n",bnewimage->kernel_addr,bnewimage->kernel_size);
	   fprintf(stderr,"bimage.kernel_addr size:%u %u\n",sizeof(bimage.kernel_addr),sizeof(bnewimage->kernel_addr));
	
	///bnewimage->kernel_addr =  calloc(bimage.kernel_size,sizeof(char));
	//memcpy(bni.kernel_addr , bimage.kernel_addr,bimage.kernel_size);
	bnewimage->kernel_addr = bimage.kernel_addr;
	bni.kernel_size = bimage.kernel_size;
	//	write_item_to_disk(bimage.kernel_addr, bimage.kernel_size,33188,"testkernel");
	fprintf(stderr,"bimage.kernel_addr size:%u %u\n",sizeof(bimage.kernel_addr),sizeof(bnewimage->kernel_addr));
	fprintf(stderr,"doing action->kernel_filename not set bimage.kernel_addr :%p %u\n",bnewimage->kernel_addr,bnewimage->kernel_size);
    }
    print_boot_image_info(&bni);
    if(action->ramdisk_directory){
	fprintf(stderr,"doing action->ramdisk_directory\n");
	unsigned cpio_ramdisk_size = 0; 
	
	
	unsigned char* cpio_data = pack_ramdisk_directory(action->ramdisk_directory,&cpio_ramdisk_size) ;
	if(cpio_data){
	
	    fprintf(stderr,"Packed Ramdisk\n");
	    new_ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
	    bnewimage->ramdisk_size = compress_gzip_memory(cpio_data,cpio_ramdisk_size,new_ramdisk_data,cpio_ramdisk_size);
	    fprintf(stderr,"Compressed Ramdisk\n");
	    bnewimage->ramdisk_addr = new_ramdisk_data;
	    free(cpio_data);
	    ramdisk_processed = 1; 
	}
	    
    }
    
    if(action->ramdisk_cpioname && !ramdisk_processed){
    
	fprintf(stderr,"doing action->ramdisk_cpioname\n");
	unsigned cpio_ramdisk_size = 0; 
	unsigned char* cpio_data =  read_item_from_disk(action->ramdisk_cpioname,&cpio_ramdisk_size);
	
	new_ramdisk_data = calloc(cpio_ramdisk_size,sizeof(char));
	bnewimage->ramdisk_size = compress_gzip_memory(cpio_data,cpio_ramdisk_size,new_ramdisk_data,cpio_ramdisk_size);
	bnewimage->ramdisk_addr = new_ramdisk_data;
	
	free(cpio_data);
	ramdisk_processed = 1;
    }    
    if(action->ramdisk_imagename && !ramdisk_processed){
    
	fprintf(stderr,"doing action->ramdisk_imagename bnewimage->ramdisk_size %u\n",bnewimage->ramdisk_size);
	
	bnewimage->ramdisk_addr =  read_item_from_disk(action->ramdisk_imagename,bnewimage->ramdisk_size);
	if(bnewimage->ramdisk_size == 0 ) bnewimage->ramdisk_addr = NULL;
	
    }
    
    if(action->second_filename){
    
	
	fprintf(stderr,"doing action->second_filename\n");
	 bnewimage->second_addr  =  read_item_from_disk(action->second_filename,bnewimage->second_size);
	if(bimage.second_size == 0 )  bnewimage->second_addr  = NULL;
   
	
    }
    if(action->ramdisk_filenames_count > 0){
	
	fprintf(stderr,"doing action->ramdisk_filenames_count\n");
	ramdisk_image rimage ;
	
	rimage.start_addr=NULL;
	errno = 0 ;
	fprintf(stderr,"load_ramdisk_image_from_archive_memory 1:%d %p\n",bimage.ramdisk_addr); 
	if(!load_ramdisk_image_from_archive_memory(bimage.ramdisk_addr,bimage.ramdisk_size,&rimage)){
	    
	   update_ramdisk_files(action,&rimage); 
	}
	if(rimage.start_addr) free(rimage.start_addr);
	
    }
    if(!ramdisk_processed){
	fprintf(stderr,"ramdisk not processed\n");
	fprintf(stderr,"newimage.ramdisk_addr:%p %u\n",bnewimage->ramdisk_addr,bnewimage->ramdisk_size);
	bnewimage->ramdisk_addr = bimage.ramdisk_addr;
	bnewimage->ramdisk_size = bimage.ramdisk_size;
	fprintf(stderr,"newimage.ramdisk_addr:%p %u\n",bnewimage->ramdisk_addr,bnewimage->ramdisk_size);
    }
    
    if(action->property_names){
	fprintf(stderr,"doing action->property_names\n");
	
	ramdisk_image rimage ;
	if(!load_ramdisk_image_from_archive_memory(bimage.ramdisk_addr,bimage.ramdisk_size,&rimage)){
	    
	    int i = 0;
	    for(i = 0 ; i < rimage.entry_count ; i ++ ){
		if(!strlcmp(rimage.entries[i]->name_addr,"default.prop")){
		    fprintf(stderr,"default prop:\n");
		}
	    }
	    
	    
	    
	    for(i = 0 ; i < action->property_count ; i++){
		
		fprintf(stderr,"prop:%s\n", action->property_names[i]);
	    } 
	    free(rimage.start_addr);
	}
    }
    
    
    fprintf(stderr,"bnewimage->kernel_addr:%p %u\n",bnewimage->kernel_addr,bnewimage->kernel_size);
    fprintf(stderr,"newimage.ramdisk_addr:%p %u\n",bnewimage->ramdisk_addr,bnewimage->ramdisk_size);
    fprintf(stderr,"bnewimage->second_addr:%p\n",bnewimage->second_addr);
     fprintf(stderr,"bimage.start_addr:%p\n",bimage.start_addr);
    
    set_boot_image_padding(bnewimage);
    fprintf(stderr,"set_boot_image_padding:%s\n",bnewimage->magic);
    set_boot_image_offsets(bnewimage);
    fprintf(stderr,"set_boot_image_offsets:%p\n",bnewimage->second_addr);
    //set_boot_image_content_hash(bnewimage); 
    fprintf(stderr,"set_boot_image_padding:%p\n",bnewimage->second_addr);
     
    fprintf(stderr,"write_boot_image failed %s\n",action->output_filename);
   if(write_boot_image(action->output_filename,bnewimage)){
	fprintf(stderr,"write_boot_image failed %d\n",errno);
    }
    
   
    

cleanup_bootimage:
    fprintf(stderr,"kernel_addr free\n");
    if(action->kernel_filename) free(bnewimage->kernel_addr);
    fprintf(stderr,"ramdisk_addr free\n");
    if(ramdisk_processed) free(bnewimage->ramdisk_addr);
    fprintf(stderr,"second_addr free\n");
    if(bnewimage->second_addr) free(bnewimage->second_addr);
    fprintf(stderr,"bimage free\n");
    //free(bimage.start_addr);
    //fprintf(stderr,"bimage free\n");
    return errno;
    
    
    
}*/
int process_update_action(int argc,char ** argv){

	
    // Initialize the action struct with NULL values
    update_action action;
    action.bootimage_filename 	= NULL 	;
    action.header_filename  	= NULL 	;
    action.kernel_filename  	= NULL 	;
    action.ramdisk_filenames  	= NULL 	;
    action.ramdisk_cpioname  	= NULL 	;
    action.ramdisk_imagename  	= NULL 	;
    action.ramdisk_directory  	= NULL 	;
    action.second_filename  	= NULL 	;
    action.output_filename  	= NULL 	;
    action.property_names  	= NULL 	;
    action.ramdisk_filenames_count	= 0	;
    action.property_count = 0 ;

    FILE*file; int ramdisk_set = 0;
      
    while(argc > 0){
	    
	// check for a valid file name
	if(!action.bootimage_filename && (file=fopen(argv[0],"r+b"))){
		
		fclose(file);
		action.bootimage_filename = argv[0];
		fprintf(stderr,"action.bootimage_filename:%s\n",action.bootimage_filename);
		// set full extract if this is the last token 
		// or if the next token is NOT a switch. 
		
		if(argc == 1 || ( argv[1][0]=='-' && argv[1][1]=='o') ||argv[1][0]!='-'){ 
		    fprintf(stderr,"extract all\n");
		    action.header_filename 	= "header";
		    action.kernel_filename 	= "kernel";
		    action.ramdisk_cpioname 	= "ramdisk.cpio";
		    action.ramdisk_imagename 	= "ramdisk.img";
		    action.ramdisk_directory 	= "ramdisk";
		    action.second_filename 	= "second";
		    // do we have an impiled output filename
		    if (argv[1] && argv[1][0]!='-') action.output_filename = argv[1];

		}
		
		
	}else if(!strlcmp(argv[0],"--header") || !strlcmp(argv[0],"-h")){
		
		// we have an header, do we have a filename
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || argv[1][0]=='-'){
			action.header_filename = "header";
		}else{
			action.header_filename = argv[1];
			--argc; ++argv;
		}
		fprintf(stderr,"action.header_filename:%s\n",action.header_filename);
		
	}else if(!strlcmp(argv[0],"--kernel") || !strlcmp(argv[0],"-k")){
		
		// we have a kernel, do we have a filename
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.kernel_filename = "kernel";
		}else{
			action.kernel_filename = argv[1];
			--argc; ++argv;
		}
		fprintf(stderr,"action.kernel_filename:%s\n",action.kernel_filename);
		
	}else if(!strlcmp(argv[0],"--output") || !strlcmp(argv[0],"-o")) {
		
		// the output directory. if this does not exist then
		// we will attempt to create it.
		
		// if it does exists we will offer to either wipe or 
		// append.
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.output_filename = NULL;
		}else{
			action.output_filename = argv[1];
			--argc; ++argv;
		}

	    fprintf(stderr,"action.output_filename:%s\n",action.output_filename);
		
	}else if(!strlcmp(argv[0],"--second") || !strlcmp(argv[0],"-s")) {
		
		// the second bootloader. AFAIK there is only 1 device
		// in existance that
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.second_filename = "second";
		}else{
			action.second_filename = argv[1];
			--argc; ++argv;
		}
		fprintf(stderr,"action.second_filename:%s\n",action.second_filename);
		
	} else if(!strlcmp(argv[0],"--properties") || !strlcmp(argv[0],"-p")) {
		
            // properties. This is a variable length char array
            // containing a list of properties to be modified in the 
            // default.prop
                    
            // if this is the last token or if the next token is a switch
            // we need to create an array with 1 entry 
              fprintf(stderr,"action.property\n");
            // work out how much memory is required
            int targc = 0 ; 
            for(targc=0; targc < argc-1 ; targc++ ){
                fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
                if(argv[targc+1] && argv[targc+1][0]=='-')
                  break;
                else
                action.property_count++;		
                
            }
            fprintf(stderr,"action.property_count %d argc %d\n",action.property_count,argc);
            // allocate the memory and assign null to the end of the array
            action.property_names = calloc(action.property_count,sizeof(unsigned char*)) ;
            // populate the array with the values 
            for(targc =0 ; targc < action.property_count; targc++) {
                argc--; argv++;
                action.property_names[targc] = argv[0]; 
                fprintf(stderr,"action.property_names[%d]:%s\n",targc,action.property_names[targc] );
            }
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
		fprintf(stderr,"action.ramdisk_cpioname:%s\n",action.ramdisk_cpioname);
		
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
		fprintf(stderr,"action.ramdisk_directory:%s\n",action.ramdisk_directory);
		    
	    }else if(!strlcmp(argv[0],"--image") || !strlcmp(argv[0],"-i")) {
		    
		// the ramdisk image as it is in the boot.img
		// this is normally a cpio.gz file but we need to 
		// check that later on.
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
		    action.ramdisk_imagename = "ramdisk.img";
		}else{
		    action.ramdisk_imagename = argv[1];
		    --argc; ++argv;
		}
		ramdisk_set = 1 ;
		fprintf(stderr,"action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
		
	    } else if(!strlcmp(argv[0],"--files") || !strlcmp(argv[0],"-f")) {
		
		// ramdisk files. This is a variable length char array
		// containing a list of file to extract from the ramdisk
			
		// if this is the last token or if the next token is a switch
		// we need to create an array with 1 entry 
		
		// work out how much memory is required
		int targc = 0 ; 
		for(targc=0; targc < argc-1 ; targc++ ){
		    fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
		    if(argv[targc+1] && argv[targc+1][0]=='-')
		      break;
		    else
		    action.ramdisk_filenames_count++;		
		    
		}
		fprintf(stderr,"action.ramdisk_filenames_count %d argc %d\n",action.ramdisk_filenames_count,argc);
		// allocate the memory and assign null to the end of the array
		action.ramdisk_filenames = calloc(action.ramdisk_filenames_count,sizeof(unsigned char*)) ;
		// populate the array with the values 
		for(targc =0 ; targc < action.ramdisk_filenames_count; targc++) {
		    argc--; argv++;
		    action.ramdisk_filenames[targc] = argv[0]; 
		    fprintf(stderr,"action.ramdisk_filenames[%d]:%s\n",targc,action.ramdisk_filenames[targc] );
		}
		ramdisk_set = 1 ;
	    }        
	}
        argc--; argv++ ;
    }
    // we must have at least a boot image to process
    if(!action.bootimage_filename){
	    fprintf(stderr,"no boot image:%s\n",action.bootimage_filename);
	    return EINVAL;
    }
    // output_file not set. use the bootimage filename
    if(!action.output_filename){
	fprintf(stderr,"no output set using bootimage_filename:%s\n",action.bootimage_filename);
	action.output_filename = action.bootimage_filename;
    }
	
    
    update_bootimage(&action);
       
    return 0;
}
