#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "program.h"
#include "file.h"
#include "help.h"


int log_write(const char *format, ...)
{
	int result;
	char *str = NULL;
	FILE* file;
	
	va_list args;
	va_start(args, format);
	result = vasprintf(&str, format, args);
	if(result == -1)
		return 0;
	va_end(args);
	if(option_values.log_stdout){
		fprintf(stderr,str,NULL);
	}
	
	if(!option_values.log_filename){
		free(str);
		return 0;
	}
	
	file = fopen(option_values.log_filename,"a");
	if(!file) {
		fprintf(stderr, "Couldn't open file %s; %s\n", option_values.log_filename, strerror(errno));
		free(str);
		return 0;
	}
	
	if(fwrite(str,strlen(str),1,file) != 1) {
		fprintf(stderr, "Could not write line to %s; %s\n", option_values.log_filename, strerror(errno));
		free(str);
		fclose(file);
		return 0;
	}
	
	free(str);
	fclose(file);
	
	return 1;
}
int strlcmp(const char *s1, const char *s2){
	
	if(!s1 || !s2 )
		return -99;
	
	size_t string_one_length=strlen(s1);
	size_t string_two_length=strlen(s2);
	size_t compare_length = string_one_length > string_two_length ?
								string_one_length : string_two_length;
	
	return strncmp(s1,s2,compare_length); 
								
}
program_options_t get_program_option(int argc, char **argv){
	
	program_options_t program_option=program_options[NOT_SET];
	//log_write("main:program_option=%d\n",program_option.action);
	if(!strlcmp(argv[1],"unpack"))
			return program_options[UNPACK];
	
	if(!strlcmp(argv[1],"remove"))
			return program_options[REMOVE];
			
	if(!strlcmp(argv[1],"extract")){
			return program_options[EXTRACT];
	}
	if(!strlcmp(argv[1],"list")){
			return program_options[LIST];
	}
	if(!strlcmp(argv[1],"pack")){
			return program_options[PACK];
	}
	if(!strlcmp(argv[1],"update")){
			return program_options[UPDATE];
	}
	return program_options[NOT_SET];
}
char * set_program_switch(char * default_value,char **argv){
	//log_write("main:set_program_switch:default %s -", default_value);	
	if(!(argv[optind]) || argv[optind][0]=='-'){
		//log_write("using default argv[optind]=%s\n",  argv[optind]);	
		return default_value;
	}else if(argv[optind]){
		// log_write("using argv[optind]=%s\n",  argv[optind]);	
		return argv[optind];
	}
	log_write("using SHIT!!! argv[optind]=%s\n",  argv[optind]);	
	return (char*)NULL;
}
int check_for_lazy_image(char * test_string){
	
	if(test_string[0]=='-')
		return 0;
		
	if(check_file_exists(test_string,CHECK_FAIL_OK)){
		option_values.image_filename=test_string;
	//	log_write("check_for_lazy_image:%s\n",test_string);	
		return 1;
	}
	return 0;
	//log_write("check_for_lazy_image:%s\n",test_string);	
	
}
int check_required_parameters(){
	switch(program_option.action){
			case UNPACK:{
					if(!option_values.image_filename){ // Image file is not set look for a valid filename 
						//log_write("main:unpack no image set\n");		
						exit(0);
					}
					if(!option_values.output_directory_name){
						//log_write("main:unpack no output set\n");	
						option_values.output_directory_name = malloc(PATH_MAX); 
						getcwd(option_values.output_directory_name,PATH_MAX);
						
					}
					break;
				}
			case LIST:{ 
					break;
				}
			case PACK:{
				if(!option_values.page_size) 
					option_values.page_size=DEFAULT_PAGE_SIZE;
					
				if(!option_values.image_filename){ // Image file is not set look for a valid filename 
					log_write("main:pack no image set\n");		
					exit(0);
				}
				if(!option_values.kernel_filename){ // Image file is not set look for a valid filename 
						log_write("main:pack no kernel set %s\n", option_values.kernel_filename);		
						exit(0);
				}
				break;
			}
			case UPDATE:{
				if(!option_values.kernel_filename){
					
				}
				break ; 
			}
			case EXTRACT:{
					if(!option_values.image_filename){ // Image file is not set look for a valid filename 
						log_write("main:extract no image set\n");		
						exit(0);
					}
					if(!option_values.source_filename){
						log_write("main:extract no source file set\n");		
						exit(0);
					}
					if(!option_values.target_filename){
						log_write("main:extract no target file set - using source\n");		
						option_values.target_filename=option_values.source_filename;
					}	
				
			}
			default:
					break;
		}
		return 0;
}
int main(int argc, char **argv){ 
	
	
	if(argc==1){
		print_main_usage();}	
	
	//fprintf(stderr,"%d %s\n",argc,argv[1]);
	check_for_help_call(argc, argv);
	
	

	
	option_values.log_stdout=1;
	int option_index =-1, option_return =-2, argument_count = argc,  settings =0; 
	program_option=get_program_option(argc, argv);
	
	
	if(program_option.action==NOT_SET)
	{
		//fprintf(stderr,"No Action Set! Lets see if I can help you out\n");
		//fprintf(stderr,"SELECT FUZZY FUZZY FUZZY ANALYSIS MODE\ninteractive or JFMIW? [JFMIW]\n");
		if(check_for_lazy_image(argv[1])){
			//fprintf(stderr,"Existing File Found at poistion 1! Try Unpack Mode\n");
			program_option=program_options[UNPACK];
			option_values.ramdisk_directory_name=DEFAULT_RAMDISK_DIRECTORY_NAME;
			option_values.kernel_filename=DEFAULT_KERNEL_NAME;
			option_values.header_filename=DEFAULT_HEADER_NAME;
		}else{
			exit(0);
		}
	}else{
		if(argc==2)
			print_question(argv[1]);
		if( ACTION_UNPACK || ACTION_EXTRACT || ACTION_LIST || ACTION_UPDATE  )
			check_for_lazy_image(argv[2]);
	 }
	//log_write("%s - %p Yes\n",option_values.image_filename,option_values.image_filename); 
	//log_write("%s %p\n",option_values.kernel_filename,option_values.kernel_filename); 
	option_return = GET_OPT_LONG_FUNCTION;
	while (option_return != -1){
		switch(option_return){
			case 'x':{ 	log_write("ramdisk_archive\n");   
						option_values.ramdisk_archive_filename=set_program_switch(DEFAULT_RAMDISK_CPIO_GZIP_NAME, argv);	break; }
			case 'd':{ 	log_write("ramdisk_dir\n");   option_values.ramdisk_directory_name	=	set_program_switch(DEFAULT_RAMDISK_DIRECTORY_NAME,argv);	break;	}
			case 'r':{ 	option_values.ramdisk_name				=	set_program_switch(DEFAULT_RAMDISK_NAME,argv); 						break;	}
			case 'k':{  //log_write("kernel_filename\n"); 
						option_values.kernel_filename			=	set_program_switch(DEFAULT_KERNEL_NAME,argv);  						break; 	}
			case 'c':{ 	option_values.cmdline_filename			=	set_program_switch(DEFAULT_CMDLINE_NAME,argv);  					break; 	}
			case 'b':{	option_values.board_filename			=	set_program_switch(DEFAULT_BOARD_NAME,argv);  						break; 	}
			case 'h':{	option_values.header_filename			=	set_program_switch(DEFAULT_HEADER_NAME,argv);  						break; 	}
			case 's':{
				 if(ACTION_UNPACK){
					 option_values.second_filename					=	set_program_switch(DEFAULT_SECOND_NAME,argv);	 break;
				}else{
					log_write("optarg:%s\n",optarg);
					option_values.source_filename = optarg;
					}
				break;
				}
			case 'p':{ 
				if(ACTION_UNPACK){	
					option_values.page_size_filename		=	set_program_switch(DEFAULT_PAGE_SIZE_NAME,argv); 					break; 	
				}else if(ACTION_PACK){
					char *endptr ; char *str ;
					str = argv[optind];
					errno=0;
					long val = strtol(str, &endptr, 10);
					if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0) || (endptr == str) ) {
						log_write("invalid_page_size\n");
					}else{
						option_values.page_size =val;
						log_write("Page Size:%d\n",option_values.page_size);
					}
				}break;            
           }
			case 't':{ 
				option_values.target_filename = optarg;
				break;
			}
			case 'a':{ 
				option_values.ramdisk_directory_name=DEFAULT_RAMDISK_DIRECTORY_NAME;
				option_values.kernel_filename=DEFAULT_KERNEL_NAME;
				option_values.header_filename=DEFAULT_HEADER_NAME;
				 break;	}
			case 'i':{
				//log_write("bootimage_filename:");
				if(ACTION_PACK){
					if(!(optarg) || optarg[0]=='-'){
						//log_write("main:pack no image set %s\n",argv[optind]);		
						exit(0);
					}else{
						if(!option_values.image_filename){
							option_values.image_filename=optarg;
							//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s bootimage_filename:%s ",opterr,optind,option_index ,optopt,optarg,option_values.image_filename);
						}
						
					}
							
				}else{
					// log_write("o=%s\n",optarg);
					if(check_file_exists(optarg,CHECK_FAIL_EXIT))
							option_values.image_filename=optarg;
				}
				log_write("\n");
				//log_write("%s %p\n",option_values.image_filename,option_values.image_filename); 
			 break;
		}
		case 'o':{
			 option_values.output_directory_name=optarg;
		
			break;
		}
		default:	break;
	}
	option_return =  GET_OPT_LONG_FUNCTION
		//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s\n",opterr,optind,option_index ,optopt,optarg);
	} // end while	
	//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s\n",opterr,optind,option_index ,optopt,optarg);
	
	// We've Processed all the options now lets see if we have everything we need
	if(!optopt){
		check_required_parameters();

		int ret =(*program_option.function_name_p)();
		fprintf(stderr,"Done\n"	);
		
	}
	exit(0);
		
	return 0;
}

