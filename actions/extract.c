
// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <string.h>	

// libbootimage headers
#include <utils.h>
#include <bootimage.h>

// internal program headers
#include <actions.h>
#include <program.h>

#define DEFAULT_BOOTIMAGE_NAME "boot"

typedef struct extract_action extract_action;

struct extract_action{
	
	char *		filename			;
	char *  	header_filename 		;
	char *  	kernel_filename 		;
	char ** 	ramdisk_filenames 		;
	char *  	ramdisk_cpioname 		;
	char *  	ramdisk_imagename 		;
	char * 		ramdisk_directory 		;
	char *		second_filename			;
	char * 		output_directory 		;
	unsigned	ramdisk_filenames_count		;
	char* 	current_working_directory	;
};

int extract_ramdisk_file(extract_action* action, global_action* gaction, ramdisk_image* rimage){
    
    // extract a single file from the ramdisk 
    unsigned entry_index = 0 ; unsigned filename_index = 0 ; 
    
    // search the ramdisk
    for (filename_index = 0 ; filename_index < action->ramdisk_filenames_count ; filename_index ++){
	
	for (entry_index = 0 ; entry_index < rimage->entry_count ; entry_index ++){
	    
	    if(!strlcmp((char*)rimage->entries[entry_index]->name_addr,action->ramdisk_filenames[filename_index])){
		
		D("\%s\n",(char*)rimage->entries[entry_index]->name_addr);
		//FILE* ramdiskfile_fp = fopen(action->ramdisk_filenames[filename_index],"w+b");
		if(write_item_to_disk_extended(rimage->entries[entry_index]->data_addr,rimage->entries[entry_index]->data_size,
		    rimage->entries[entry_index]->mode,action->ramdisk_filenames[filename_index],rimage->entries[entry_index]->name_size)){
		    
		    fprintf(stderr,"error writing %s %d %s\n",action->ramdisk_filenames[filename_index],errno,strerror(errno));
		}
		break;
	    }
	}
    }
    return 0;
}
/* extract_ramdisk_image - expects rimage to be a prointer to the start of a cpio archive*/
int extract_ramdisk_image(extract_action* action, global_action* gaction,ramdisk_image* rimage){
    
    D("rimage_size %d\n",rimage->size);
    
    // save the compress cpio file to disk
    if(action->ramdisk_cpioname){
	   
	if(write_item_to_disk(rimage->start_addr,rimage->size,33188,action->ramdisk_cpioname)){
	    fprintf(stderr,"error writing cpio archive%s %d %s\n",action->ramdisk_cpioname,errno,strerror(errno));
	}else{
	    fprintf(stderr," cpio archive extracted to \"%s\"\n",action->ramdisk_cpioname);
	}
	//fprintf(stderr,"\n");
	errno = 0;
	    
    }
    
    // extract and save the cpio file contents to disc  
    if(action->ramdisk_directory){
	if(save_ramdisk_entries_to_disk(rimage,action->ramdisk_directory)){
	 fprintf(stderr," error unpacking cpio archive to %s %d %s\n",action->ramdisk_directory,errno,strerror(errno));
	}else{
	    fprintf(stderr," cpio archive unpacked to \"%s\"\n",action->ramdisk_directory);
	}
	//fprintf(stderr,"\n");
	errno = 0;
    }
    
    return 0 ; 
}

int extract_bootimage(extract_action* action, global_action* gaction,boot_image* bimage){
    
    errno = 0;
    int return_value=0;
    
    print_program_title();
    
    // Write the header contents
    fprintf(stderr," Extracting boot image components from \"%s\"",action->filename);
    if(action->output_directory)
	fprintf(stderr," to \"%s\"",action->output_directory);
    fprintf(stderr,"\n");
    
    errno = 0;
    if(action->header_filename){
	fprintf(stderr," Header");
	if(bimage->header_size != sizeof(boot_img_hdr)){
	    errno = EINVAL ; 
	    fprintf(stderr," error boot image header is corrupt\n");
	}
	if(write_boot_image_header_to_disk(action->header_filename,bimage)){
	    fprintf(stderr," error writing %s %d %s\n",action->header_filename,errno,strerror(errno));
	}else{
	    fprintf(stderr," extracted to \"%s\"\n",action->header_filename);
	}
	//fprintf(stderr,"\n");
	errno = 0;
    }
     
    // Write the kernel file to disk
    if(action->kernel_filename){
	fprintf(stderr," Kernel");
	if(write_item_to_disk(bimage->kernel_addr,bimage->header->kernel_size,33188,action->kernel_filename)){
	    fprintf(stderr," error writing %s %d %s\n",action->kernel_filename,errno,strerror(errno));
	}else{
	    fprintf(stderr," extracted to \"%s\"\n",action->kernel_filename);
	}
	//fprintf(stderr,"\n");
	errno = 0;
    }
    
    // Write the second file to disk
    if(action->second_filename && bimage->header->second_size > 0){
	fprintf(stderr," Second");
	if(write_item_to_disk(bimage->second_addr,bimage->header->second_size,33188,action->second_filename)){
		fprintf(stderr," error writing %s %d %s\n",action->second_filename,errno,strerror(errno));
	}else{
	    fprintf(stderr," extracted to \"%s\"\n",action->second_filename);
	}
	//fprintf(stderr,"\n");
	errno = 0;
    }
    
    // Write the compressed ramdisk to disk
    if(action->ramdisk_imagename){
	fprintf(stderr," Ramdisk");
	if(write_item_to_disk(bimage->ramdisk_addr,bimage->header->ramdisk_size,33188,action->ramdisk_imagename)){
	    fprintf(stderr, "error writing archive %s %d %s\n",action->ramdisk_imagename,errno,strerror(errno));
	}else{
	    fprintf(stderr," archive extracted to \"%s\"\n",action->ramdisk_imagename);
	}
	//fprintf(stderr,"\n");
	errno = 0;
    }
    
    // check to see if we need to do any additional extraction on
    // the compressed ramdisk image
    if(action->ramdisk_cpioname || action->ramdisk_directory || action->ramdisk_filenames_count > 0){
	
	ramdisk_image rimage; 
	return_value = load_ramdisk_image_from_archive_memory(bimage->ramdisk_addr,bimage->header->ramdisk_size,&rimage);
	if(return_value != 0){
	    if(rimage.start_addr != NULL  ) {
		free(rimage.start_addr);
	    }
	    return return_value;
	}
	extract_ramdisk_image(action,gaction,&rimage);
	free(rimage.start_addr);
    }
    return 0;
    
}
/* extract_file - determines the file type we are dealing with and branches
		  accordingly */
int extract_file(extract_action* action, global_action* gaction){

    
    int return_value=0;
    unsigned action_size=0;     
    
    unsigned char* action_data = read_item_from_disk(action->filename , &action_size);
    if(!action_data && errno){
	//  file too large error. no point in contining
	print_program_title();
	fprintf(stderr," Cannot process \"%s\" - error : %d %s\n\n",action->filename,errno,strerror(errno));
	return 0;    
    }

    if(action->output_directory){
	D("action->output_directory:%s\n",action->output_directory);
	mkdir_and_parents(action->output_directory,0755);
	chdir((char*)action->output_directory);
    }
    
    boot_image bimage;
    if(!(return_value=load_boot_image_from_memory(action_data,action_size,&bimage))){
	return_value = extract_bootimage(action, gaction,&bimage);
	free(bimage.start_addr); 
	fprintf(stderr,"\n");
	return return_value;   
    
    }else{
	if(bimage.start_addr != NULL  ) free(bimage.start_addr);
    }
    
    ramdisk_image rimage;
    return_value = load_ramdisk_image_from_archive_memory(action_data,action_size,&rimage);
    
    if(!return_value){
	fprintf(stderr,"Extracting Ramdisk components from \"%s\"\n",action->filename);
	D("load_ramdisk_image_from_archive_memory returns:%d\n",rimage.entry_count); 
	return_value = extract_ramdisk_image(action,gaction, &rimage);
	fprintf(stderr,"\n");
	free(rimage.start_addr); 
	return return_value;
    }
    
    // The target file was a cpio file. we can only step down to
    if(!load_ramdisk_image_from_cpio_memory(action_data,action_size,&rimage)){
	fprintf(stderr,"Extracting Ramdisk components from \"%s\"\n",action->filename);
	D("cpio file rimage.entry_count=%d\n",rimage.entry_count);
	return_value = extract_ramdisk_image(action,gaction, &rimage);
	fprintf(stderr,"\n");
	free(rimage.start_addr); 
	return return_value;
    }
    
    return print_program_error_file_type_not_recognized(action->filename);
}

// process_extract_action - parse the command line switches
// although this code is repetitive we will favour readability
// over codesize ...... Ask me in 3 months time whether it was
// a good idea.
int process_extract_action(unsigned argc,char ** argv,global_action* gaction){
	
    // Initialize the action struct with NULL values
    extract_action action;
    action.filename 	= NULL 	;
    action.header_filename  	= NULL 	;
    action.kernel_filename  	= NULL 	;
    action.ramdisk_filenames  	= NULL 	;
    action.ramdisk_cpioname  	= NULL 	;
    action.ramdisk_imagename  	= NULL 	;
    action.ramdisk_directory  	= NULL 	;
    action.second_filename  	= NULL 	;
    action.output_directory  	= NULL 	;
    action.ramdisk_filenames_count	= 0	;
    action.current_working_directory = calloc(PATH_MAX,sizeof(char)); 
    
    getcwd(action.current_working_directory,PATH_MAX);
    D("current_working_directory:%s\n",action.current_working_directory);
    
        // work out a possible file name just in case we need it for 
    // error reporting , the possible filename should be at position zero 
    // but it maybe elsewhere, as info printing doesn't require any require 
    // filenames for switches we can look for the first argv that doesn't begin with "-"
    char* possible_filename = NULL;
    unsigned i = 0 ;
    for(i = 0 ; i < argc ; i++){
    	if(argv[i][0]!='-'){
	     possible_filename = argv[i];
	     D("possible_filename at position %d - %s %s\n",i,argv[i],possible_filename);
	     break ;
	 }
    }
    
    FILE*file;
      
    while(argc > 0){
	    
	// check for a valid file name
	if(!action.filename && (file=fopen(argv[0],"r+b"))){
		
		fclose(file);
		action.filename = argv[0];
		D("action.filename:%s\n",action.filename);
		// set full extract if this is the last token 
		// or if the next token is NOT a switch. 
		
		if(argc == 1 || ( argv[1][0]=='-' && argv[1][1]=='o') ||argv[1][0]!='-'){ 
		    D("extract all\n");
		    action.header_filename 	= (char*)"header";
		    action.kernel_filename 	= (char*)"kernel";
		    action.ramdisk_cpioname 	= (char*)"ramdisk.cpio";
		    action.ramdisk_imagename 	= (char*)"ramdisk.img";
		    action.ramdisk_directory 	= (char*)"ramdisk";
		    action.second_filename 	= (char*)"second";
		    // do we have an impiled, output directory
		    if (argv[1] && argv[1][0]!='-') {
			action.output_directory = argv[1];
		    }

		}
		
		
	}else if((!strlcmp(argv[0],"--header") || !strlcmp(argv[0],"-h")) && !action.header_filename){
		
		// we have an header, do we have a filename
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || argv[1][0]=='-'){
			action.header_filename = "header";
		}else{
			action.header_filename = argv[1];
			--argc; ++argv;
		}
		D("action.header_filename:%s\n",action.header_filename);
		
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
		D("action.kernel_filename:%s\n",action.kernel_filename);
		
	}else if(!strlcmp(argv[0],"--cpio") || !strlcmp(argv[0],"-C")) {
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.ramdisk_cpioname = "ramdisk.cpio";
		}else{
			action.ramdisk_cpioname = argv[1];
			--argc; ++argv;
		}
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
		D("action.ramdisk_directory:%s\n",action.ramdisk_directory);
		
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
		D("action.ramdisk_imagename:%s\n",action.ramdisk_imagename);
	
	}else if(!strlcmp(argv[0],"--output") || !strlcmp(argv[0],"-o")) {
		
		// the output directory. if this does not exist then
		// we will attempt to create it.
		
		// if it does exists we will offer to either wipe or 
		// append.
		
		// use the default filename if this is the last token
		// or if the next token is a switch
		if(argc == 1 || (argv[1][0]=='-')){
			action.output_directory = NULL;
		}else{
			action.output_directory = argv[1];
			--argc; ++argv;
		}

	    D("action.output_directory:%s\n",action.output_directory);
		
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
		D("action.second_filename:%s\n",action.second_filename);
		
	}else if(!strlcmp(argv[0],"--files") || !strlcmp(argv[0],"-f")) {
		
		// ramdisk files. This is a variable length char array
		// containing a list of file to extract from the ramdisk
				
		// if this is the last token or if the next token is a switch
		// we need to create an array with 1 entry 
	    
		// work out how much memory is required
		unsigned targc = 0 ; 
		for(targc=0; targc < argc-1 ; targc++ ){
		    fprintf(stderr,"argv[%d] %s\n",targc,argv[targc]);
		    if(argv[targc+1] && argv[targc+1][0]=='-')
		      break;
		    else
			action.ramdisk_filenames_count++;		
			
		}
		D("action.ramdisk_filenames_count %d argc %d\n",action.ramdisk_filenames_count,argc);
		// allocate the memory and assign null to the end of the array
		action.ramdisk_filenames = calloc(action.ramdisk_filenames_count,sizeof(unsigned char*)) ;
		// populate the array with the values 
		for(targc =0 ; targc < action.ramdisk_filenames_count; targc++) {
		    argc--; argv++;
		    action.ramdisk_filenames[targc] = argv[0]; 
		    D("action.ramdisk_filenames[%d]:%s\n",targc,action.ramdisk_filenames[targc] );
		}
	}
	argc--; argv++ ;
    }
	
    // we must have at least a boot image to process
    
    if(!action.filename) 
	return print_program_error_file_name_not_found(action.filename);
    
    extract_file(&action,gaction);
       
    return 0;
}
