#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "bootimg_utils.h"
#include "program.h"
#include "file.h"

#define BOOT_IMAGE_UTILITIES_TITLE "Android Boot Image Utilities "
#define BOOT_IMAGE_UTILITIES_VERSION "0.01b"
#define BOOT_IMAGE_UTILITIES_DESCRIPTION ""
#define BOOT_IMAGE_UTILITIES_PROGRAM_NAME "bootutils"
#define BOOT_IMAGE_UTILITIES_FULL_TITLE "%s%s\n",BOOT_IMAGE_UTILITIES_TITLE,BOOT_IMAGE_UTILITIES_TITLE
#define PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE fprintf(stderr,BOOT_IMAGE_UTILITIES_FULL_TITLE);
char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}
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
	if(HAS_LOGSTDOUT)
		fprintf(stderr,str);
	
	if(HAS_NOLOGFILE){
		free(str);
		return 0;
	}
	
	file = fopen(option_values.filename,"a");
	if(!file) {
		fprintf(stderr, "Couldn't open file %s; %s\n", option_values.filename, strerror(errno));
		free(str);
		return 0;
	}
	
	if(fwrite(str,strlen(str),1,file) != 1) {
		fprintf(stderr, "Could not write line to %s; %s\n", option_values.filename, strerror(errno));
		free(str);
		fclose(file);
		return 0;
	}
	
	free(str);
	fclose(file);
	
	return 1;
}
int help(void){
	return 0;
}
void print_usage_detail(){
}
void print_usage_header(){
	PRINT_BOOT_IMAGE_UTILITIES_FULL_TITLE
}
int print_usage(){
	
	print_usage_header();
	print_usage_detail();
	return 0;
}

void print_question(char *action){
	print_usage_header();	
	fprintf(stderr,"%s what?\n",action);
	print_usage_detail();
	exit(0);
}	
#define USEAGE "1346 		245"
program_options_t get_program_option(int argc, char **argv){
	
	program_options_t program_option=program_options[NOT_SET];
	//log_write("main:program_option=%d\n",program_option.action);
	int compare_length = strlen(argv[1]) > 6 ? strlen(argv[1]) : 6;
	if(!strncmp(argv[1],"unpack",compare_length))
			return program_options[UNPACK];
	
	if(!strncmp(argv[1],"remove",compare_length))
			return program_options[REMOVE];
			
	compare_length = strlen(argv[1]) > 7 ? strlen(argv[1]) : 7;
	if(!strncmp(argv[1],"extract",compare_length)){
			return program_options[EXTRACT];
	}
	compare_length = strlen(argv[1]) > 4 ? strlen(argv[1]) : 4;
	if(!strncmp(argv[1],"list",compare_length)){
			return program_options[LIST];
	}
	if(!strncmp(argv[1],"pack",compare_length)){
			return program_options[PACK];
	}
	return program_options[NOT_SET];
}
char * set_program_switch(int param,char * default_value,char **argv){
	params = param ;
	if(!(argv[optind]) || argv[optind][0]=='-'){
		return default_value;
	}else if(argv[optind]){
		return argv[optind];
	}
	return (char*)NULL;
}
int check_for_lazy_image(char * test_string){
	
	if(test_string[0]=='-')
		return 0;
		
	if(check_file_exists(test_string,CHECK_FAIL_OK)){
		params = SET_IMAGE;
		option_values.image_filename=test_string;
		return 1;
	}
	return 0;
	log_write("check_for_lazy_image:%s\n",test_string);	
	
}
int main(int argc, char **argv){ 	
	if(argc<2) { return print_usage(); }
	
	params = SET_LOGSTDOUT;
	params = SET_NOLOGFILE;
	
	int option_index =-1, option_return =-2, argument_count = argc,  settings =0; 
	program_option=get_program_option(argc, argv);
	
	
	if(program_option.action==NULL)
	{
		fprintf(stderr,"No Action Set! Lets see if I can help you out\n");
		fprintf(stderr,"SELECT FUZZY FUZZY FUZZY ANALYSIS MODE\ninteractive or JFMIW? [JFMIW]\n");
		if(check_for_lazy_image(argv[1])){
			fprintf(stderr,"Existing File Found at poistion 1! Try Unpack Mode\n");
			program_option=program_options[UNPACK];
			option_values.ramdisk_directory_name=DEFAULT_RAMDISK_DIRECTORY_NAME;
			option_values.kernel_name=DEFAULT_KERNEL_NAME;
			option_values.header=DEFAULT_HEADER_NAME;
			params = SET_ALL ;
		}else{
			exit(0);
		}
	}else{
		if(argc==2)
			print_question(argv[1]);
		if( ACTION_UNPACK || ACTION_EXTRACT || ACTION_LIST )
			check_for_lazy_image(argv[2]);
	 }

	option_return = GET_OPT_LONG_FUNCTION;
	while (option_return != -1){
		switch(option_return){
			case 'x':{ 	option_values.ramdisk_archive_name 		= 	set_program_switch( SET_RAMDISK_ARCHIVE,DEFAULT_RAMDISK_CPIO_GZIP_NAME, argv);	break; }
			case 'd':{ 	option_values.ramdisk_directory_name	=	set_program_switch(SET_RAMDISK_DIRECTORY,DEFAULT_RAMDISK_DIRECTORY_NAME,argv);	break;	}
			case 'r':{ 	option_values.ramdisk_name				=	set_program_switch(SET_RAMDISK,DEFAULT_RAMDISK_NAME,argv); 						break;	}
			case 'k':{  option_values.kernel_name				=	set_program_switch(SET_KERNEL,DEFAULT_KERNEL_NAME,argv);  						break; 	}
			case 'c':{ 	option_values.cmdline					=	set_program_switch(SET_CMDLINE,DEFAULT_CMDLINE_NAME,argv);  					break; 	}
			case 'b':{	option_values.board						=	set_program_switch(SET_BOARD,DEFAULT_BOARD_NAME,argv);  						break; 	}
			case 'h':{	option_values.header					=	set_program_switch(SET_HEADER,DEFAULT_HEADER_NAME,argv);  						break; 	}
			case 's':{
				 if(ACTION_UNPACK){
					 option_values.second					=	set_program_switch(SET_SECOND,DEFAULT_SECOND_NAME,argv);	 break;
				}else{
					params = SET_SOURCE ;
					option_values.source = optarg;
					break;
					}
				break;
				}
			case 'p':{ 
				if(ACTION_UNPACK){	
					option_values.page_size_filename		=	set_program_switch(SET_PAGESIZE,DEFAULT_PAGE_SIZE_NAME,argv); 					break; 	
				}else if(ACTION_PACK){
					char *endptr ; char *str ;
					str = argv[optind];
					errno=0;
					long val = strtol(str, &endptr, 10);
					if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0) || (endptr == str) ) {
						log_write("invalid_page_size\n");
					}else{
						params = SET_PAGESIZE_STRING;
						option_values.page_size =val;
						log_write("Page Size:%d\n",option_values.page_size);
					}
				}             
           }
			case 't':{ 
				params = SET_TARGET ;
				option_values.target = optarg;
				break;
			}
			case 'a':{ 
				option_values.ramdisk_directory_name=DEFAULT_RAMDISK_DIRECTORY_NAME;
				option_values.kernel_name=DEFAULT_KERNEL_NAME;
				option_values.header=DEFAULT_HEADER_NAME;
				params = SET_ALL ;
				 break;	}
			case 'i':{
			 params = SET_IMAGE;
			 if(ACTION_PACK){
				option_values.image_filename=optarg;
			 }else{
				 if(check_file_exists(optarg,CHECK_FAIL_EXIT))
					option_values.image_filename=optarg;
				}
		 
			 break;
		}
		case 'o':{
			 params = SET_OUTPUT ;
			 option_values.output=optarg;
		
			break;
		}
		default:	break;
	}
	option_return =  GET_OPT_LONG_FUNCTION
		
	} // end while	
	//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s\n",opterr,optind,option_index ,optopt,optarg);
	
	// We've Processed all the options now lets see if we have everything we need
	if(!optopt){
		switch(program_option.action){
			case UNPACK:{
					if(!HAS_IMAGE){ // Image file is not set look for a valid filename 
						log_write("main:unpack no image set\n");		
						exit(0);
					}
					if(!HAS_OUTPUT){
						log_write("main:unpack no output set\n");	
						params = SET_OUTPUT ;
						option_values.output = malloc(PATH_MAX); 
						getcwd(option_values.output,PATH_MAX);
						
					}
					break;
				}
			case LIST:{ 
					log_write("main:list\n");
					break;
				}
			case PACK:{
				if(!HAS_PAGESIZE_STRING){
					option_values.page_size=DEFAULT_PAGE_SIZE;
					log_write("main:pack no image set\n");		
				}
				if(!HAS_IMAGE){ // Image file is not set look for a valid filename 
					log_write("main:pack no image set\n");		
					exit(0);
				}
				if(!HAS_IMAGE){ // Image file is not set look for a valid filename 
					log_write("main:pack no image set\n");		
					exit(0);
				}
				if(!HAS_KERNEL){ // Image file is not set look for a valid filename 
						log_write("main:unpack no image set\n");		
						exit(0);
				}
				break;
			}
			default:
					break;
		}
		
	
	
		if(!HAS_IMAGE){		
			return print_usage();
		}
		
		if(!HAS_RAMDISK){
			if(HAS_RAMDISK_ARCHIVE || HAS_RAMDISK_CPIO || HAS_RAMDISK_DIRECTORY)
			{
					option_values.ramdisk_name=DEFAULT_RAMDISK_DIRECTORY_NAME;
			
			}
		}
		int ret =(*program_option.function_name_p)();
	}
	exit(0);
		
	return 0;
}

