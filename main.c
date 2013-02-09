#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "bootimg_utils.h"
#include "program.h"
#include "file.h"
int vasprintf(char **strp, const char *fmt, va_list ap);
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
	log_write("main:program_option=%d\n",program_option.action);
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
int main(int argc, char **argv)
{ 
	
	if(argc<2) { return print_usage(); }
	
	params = SET_LOGSTDOUT ;
	params = SET_NOLOGFILE;
	
	int option_index =-1, option_return =-2, argument_count = argc, settings =0; 
	
	program_options_t program_option=get_program_option(argc, argv);
	
	if(program_option.action==NULL)
	{
		fprintf(stderr,"No Action Set! You Lazy Bastard, Lets see if I can help you out\n");
		fprintf(stderr,"SELECT FUZZY FUZZY FUZZY ANALYSIS MODE\ninteractive or JMFIW? [JFMIW]\n");
		
		exit(0);
	}else{
		if(argc==2)
			print_question(argv[1]); 
	 }
		
		
	log_write("main:params=[%08x]\n",params);
	log_write("main:stringopts=%s\n",program_option.stringopts);
	log_write("main:longopts=%p\n",program_option.options);
	option_return = GET_OPT_LONG_FUNCTION;
	while (option_return != -1){
		switch(option_return){
			case 'a':{ 
				log_write("main:all_switch_specified\n");
				option_values.ramdisk_directory=DEFAULT_RAMDISK_DIRECTORY_NAME;
				log_write("main:all_switch:ramdisk_directory_name=%s\n",option_values.ramdisk_directory);
				option_values.kernel=DEFAULT_KERNEL_NAME;
				log_write("main:all_switch:kernel_filename=%s\n",option_values.kernel);
				option_values.header=DEFAULT_HEADER_NAME;
				log_write("main:all_switch:header_filename=%s\n",option_values.header);
				params = SET_RAMDISK_DIRECTORY ;
				log_write("main:params=[%08x]\n",params);
				params = SET_KERNEL;
				log_write("main:params=[%08x]\n",params);
				params = SET_HEADER ;
				log_write("main:params=[%08x]\n",params);
				 break;	}
		case 'x':{ 
			log_write("main:ramdisk-archive_switch_specified\n");
			params = SET_RAMDISK_ARCHIVE ; break;	}
		case 'd':{ 
			log_write("main:ramdisk-directory_switch_specified\n");
			params = SET_RAMDISK_DIRECTORY ; break;}
		case 'p':{ 
			log_write("pagesize_switch_specified\n");
			params = SET_PAGESIZE	 ;  
			break; }
		case 'i':{
			log_write("main:image_switch_specified=%s %p\n",optarg,optarg);
			 params = SET_IMAGE;
			 if(check_file_exists(optarg,CHECK_FAIL_EXIT)){
				log_write("main:image_file_found=%s %p %p\n",optarg,optarg,option_values.image);
				option_values.image=optarg;
				log_write("main:image_file_found=%s %p %p %s\n",optarg,optarg,option_values.image,optarg);
			 }
			 
			 break;
		}
		case 'r':{
			 params= SET_RAMDISK ;
			 option_values.ramdisk_name = malloc(PATH_MAX); 
			 if(!(argv[optind]) || argv[optind][0]=='-'){
				 strncpy(option_values.ramdisk_name,"default_ramdisk",strlen("default_ramdisk"));
				 break;
			 }				
			 if(argv[optind]){
				 memcpy(option_values.ramdisk_name,argv[optind],PATH_MAX);
			 }
			 break;
		}
		case 'k':{
			 params = SET_KERNEL ;
			 option_values.kernel= malloc(PATH_MAX); 
			 if(!(argv[optind]) || (argv[optind][0]=='-')){
				 memcpy(option_values.kernel,"default_kernel",strlen("default_kernel"));
				 break;
			 }				
			 if(argv[optind]){
				 memcpy(option_values.kernel,argv[optind],PATH_MAX);
			 }
			 break;
		}
		case 'c':{
			 params = SET_CMDLINE ;
			 option_values.cmdline = malloc(PATH_MAX); 
			 if(!(argv[optind]) || (argv[optind][0]=='-')){
				 strncpy(option_values.cmdline,"default_cmdline",strlen("default_cmdline")+1);
				 strncat(option_values.cmdline,DEFAULT_FILE_EXT,DEFAULT_FILE_EXT_LEN);
				 //fprintf(stderr,"%s\n",option_values.cmdline);
				 break;
			 }				
			 if(argv[optind]){
				 memcpy(option_values.cmdline,argv[optind],PATH_MAX);
			 }
			 //fprintf(stderr,"option_values.cmdline  %s %s\n",option_values.cmdline ,argv[optind]);
			 break;
		}
		case 'b':{
			 params = SET_BOARD ;
			 option_values.board = malloc(PATH_MAX); 
			 if(!(argv[optind]) || (argv[optind][0]=='-')){
				 memcpy(option_values.board,"default_board",strlen("default_board"));
				 break;
			 }				
			 if(argv[optind]){
				 memcpy(option_values.board,argv[optind],PATH_MAX);
			 }
			 break;
		}
		case 's':{
			 if(program_option.action==UNPACK){
				 params = SET_SECOND ;
				 option_values.second= malloc(PATH_MAX); 
				 if(!(argv[optind]) || (argv[optind][0]=='-')){
					 memcpy(option_values.second,"default_second",strlen("default_second"));
					 break;
				 }				
				 if(argv[optind]){
					 memcpy(option_values.second,argv[optind],PATH_MAX);
				 }
				 break;
			}else{
				params = SET_SOURCE ;
				option_values.source = optarg;
				option_values.source_length = strlen(optarg);
				break;
				}
			break;
			}
				 
		case 't':{ 
				params = SET_TARGET ;
				option_values.target = optarg;
				option_values.target_length = strlen(optarg);
				break;
			}
		
		case 'h':{
			params = SET_HEADER ;
			//option_values.header= malloc(PATH_MAX); 
			if(!(argv[optind]) || (argv[optind][0]=='-')){
				 memcpy(option_values.header,DEFAULT_HEADER_NAME, DEFAULT_HEADER_NAME_LENGTH);
				 break;
			}				
			if(argv[optind]){
			 memcpy(option_values.header,argv[optind],PATH_MAX);
			}
			break;
		}	
		case 'o':{
			 params = SET_OUTPUT ;
			 if(strlen(optarg)>0){
				option_values.output= malloc(PATH_MAX); 
				memcpy(option_values.output,optarg,PATH_MAX);
			 }
			 break;
		}
		default:	break;
	}
	option_return =  GET_OPT_LONG_FUNCTION
		
	} // end while	
	//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s\n",opterr,optind,option_index ,optopt,optarg);
	if(!optopt){
		if(!HAS_IMAGE){		
			return print_usage();
		}
		
		if(!HAS_RAMDISK){
			if(HAS_RAMDISK_ARCHIVE || HAS_RAMDISK_CPIO || HAS_RAMDISK_DIRECTORY)
			{
					option_values.ramdisk_name = malloc(PATH_MAX); 
					memcpy(option_values.ramdisk_name,"default_ramdisk",strlen("default_ramdisk"));
			}
		}
		//fprintf(stderr,"output : %s\n",option_values.output);	
		int sum = (*program_option.function_name_p)();
	}
	exit(0);
		
	return 0;
}

