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

#define GET_OPT_LONG_FUNCTION getopt_long(argc, argv,program_option.stringopts, program_option.options, &long_option_index);

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
program_options_t get_program_options(char *program_action){
	
	if(!strlcmp(program_action,"unpack")) return program_options[UNPACK];
	
	if(!strlcmp(program_action,"remove")) return program_options[REMOVE];
			
	if(!strlcmp(program_action,"extract"))	return program_options[EXTRACT];
	
	if(!strlcmp(program_action,"list")) return program_options[LIST];
	
	if(!strlcmp(program_action,"pack")) return program_options[PACK];
	
	if(!strlcmp(program_action,"update")) return program_options[UPDATE];

	return program_options[NOT_SET];
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
int parse_command_line_switch_arguments(char*** argv, command_line_switches_p command_lines_switches ){

	// Are we dealing with a short or long command
	char *current_arg = (*argv[0]); const char * compare_string;
	int check_long = ((current_arg[0]=='-') && (current_arg[1]=='-')) ? 2 : 1 ; 
	while( command_lines_switches->argument_type ){
		compare_string = check_long > 1 ? command_lines_switches->long_name :command_lines_switches->short_char ;
		if (!strlcmp(current_arg+check_long,compare_string)) break;
		command_lines_switches++;		
	}
	// Option not found return a pointer to the empty End of Array struct
	// and move the argument along
	if(!command_lines_switches->argument_type) { 
		fprintf(stderr,"unknown command line switch '%s'\n",current_arg+check_long); 
		exit(0); 
	} 
	switch(command_lines_switches->argument_type){
		case REQ_STR_ARG:{ 
			    // required argument, move the argument pointer to be the next arg along
				(*argv)++;
				if((!(*argv[0])) || (*argv[0][0])=='-'){ // invalid argument, bin it.
				fprintf(stderr,"invalid argument for switch '%s'  \n",compare_string); exit(0); }
				// argument value is ok. assign the deference the dest_ptr so we can assign 
				// a value to the option_value member directly

				*(char **)command_lines_switches->dest_ptr=(*argv[0]);
				// move to the next argument
				(*argv)++;
				break ; }
			case DEF_STR_ARG:{ 
				// argument comes with a default value
				// move the argument pointer to be the next arg along
				(*argv)++;
				if((!(*argv[0])) || (*argv[0][0])=='-'){ // invalid argument, assign the default
					*(const char **)command_lines_switches->dest_ptr =command_lines_switches->default_string;
				}else{ // assign the argument value as normal
					*(char **)command_lines_switches->dest_ptr=(*argv[0]);
					// move to the next argument
					(*argv)++;
				}
				break;
			}
		default: break;		 
	}
			
	return 0;
}


int check_required_parameters(const program_actions_emum action){
	
	if(!option_values.image_filename){
			fprintf(stderr,"image file name not set\n"); exit(0);
	}
	if(action!=PACK){
		check_file_exists(option_values.image_filename,CHECK_FAIL_EXIT);
	}
	
	
	switch(action){
			case UNPACK:{
					// set output directory to working if not set
					if(option_values.output_directory_name){
						if(check_directory_exists(option_values.output_directory_name,CHECK_FAIL_OK)){
							fprintf(stderr,"the directory %s already exists - overwrite existing files\n",option_values.output_directory_name);
						}
					else
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
					
				if(!option_values.kernel_filename){ // Image file is not set look for a valid filename 
						fprintf(stderr,"missing kernel filename");
						exit(0);
				}else {  
					check_file_exists(option_values.kernel_filename,CHECK_FAIL_EXIT);
				}
				break;
			}
			case UPDATE:{
				if(!option_values.kernel_filename){
					
				}
				break ; 
			}
			case EXTRACT:{
				
					if(!option_values.source_filename){
						log_write("main:extract no source file set\n");		
						exit(0);
					}
					if(!option_values.target_filename){
						//log_write("main:extract no target file set - using source\n");		
						option_values.target_filename=option_values.source_filename;
					}
						
				
			}
			default:
					break;
		}
		return 0;
}
int parse_command_line_switches(char **argv,program_options_t program_options){
	
	
	while(argv[0]){
		parse_command_line_switch_arguments( &argv, program_options.command_line_switches);				
	}	
	check_required_parameters(program_options.action);
	//fprintf(stderr,"done:\n");
	/*fprintf(stderr,"image:%s kernel:%s dir:%s arc:%s ps:%s cl:%s cpio:%s out:%s sec:%s b:%s h:%s\n",
		option_values.image_filename, option_values.kernel_filename, option_values.ramdisk_directory_name,
		option_values.ramdisk_archive_filename,option_values.page_size_filename,option_values.cmdline_filename,
		option_values.ramdisk_cpio_filename,option_values.output_directory_name,option_values.second_filename,
		option_values.board_filename,option_values.header_filename); 
	*/
	return 0;
	
}
int main(int argc, char **argv){ 
	
	//if(is_cpio_file(argv[1]))		printf("Yes io\n");
	//else printf("no io\n");	
	//if(is_gzip_file(argv[1]))		printf("Yes\n");
	//else printf("no\n");	
	//if(is_android_boot_image_file(argv[1]))		printf("Yes\n");
	//else printf("no\n");	
		
	
	
	if(argc==1){
		print_main_usage();}	
	
	//fprintf(stderr,"%d %s\n",argc,argv[1]);
	check_for_help_call(argc, argv);
		
	option_values.log_stdout=1;
	int long_option_index =-1, option_return =-2, argument_count = argc,  settings =0; 
	program_options_t program_options=get_program_options(argv[1]);
	option_values.action=program_options.action;
	parse_command_line_switches(argv+2,program_options);
	int ret =(*program_options.action_function_p)();
	fprintf(stderr,"Done\n"	);
	
	
	/*if(program_options.action==NOT_SET)
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
	opterr=0;
	option_return = GET_OPT_LONG_FUNCTION;
	while (option_return != -1){
		switch(option_return){
			case 'x':{ 	log_write("ramdisk_archive\n");   
						option_values.ramdisk_archive_filename=set_program_switch(DEFAULT_RAMDISK_CPIO_GZIP_NAME, argv,long_option_index,argc);	break; }
			case 'd':{ 	log_write("ramdisk_dir\n");   option_values.ramdisk_directory_name	=	set_program_switch(DEFAULT_RAMDISK_DIRECTORY_NAME,argv,long_option_index,argc);	break; }
			case 'r':{ 	option_values.ramdisk_name				=	set_program_switch(DEFAULT_RAMDISK_NAME,argv,long_option_index,argc);	break; }
			case 'k':{  //log_write("kernel_filename\n"); 
						option_values.kernel_filename			=	set_program_switch(DEFAULT_KERNEL_NAME,argv,long_option_index,argc);				break; 	}
			case 'c':{ 	option_values.cmdline_filename			=	set_program_switch(DEFAULT_CMDLINE_NAME,argv,long_option_index,argc);			break; 	}
			case 'b':{	option_values.board_filename			=	set_program_switch(DEFAULT_BOARD_NAME,argv,long_option_index,argc);				break; 	}
			case 'h':{	option_values.header_filename			=	set_program_switch(DEFAULT_HEADER_NAME,argv,long_option_index,argc);				break; 	}
			case 's':{
				 if(ACTION_UNPACK){
					 option_values.second_filename					=	set_program_switch(DEFAULT_SECOND_NAME,argv,long_option_index,argc);	 break;
				}else{
			
					option_values.source_filename = optarg;
					}
				break;
				}
			case 'p':{ 
				if(ACTION_UNPACK){	
					option_values.page_size_filename		=	set_program_switch(DEFAULT_PAGE_SIZE_NAME,argv,long_option_index,argc); 					break; 	
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
				if(!(optarg) || optarg[0]=='-'){
					fprintf(stderr,"opterr:%d optind:%d long_option_index:%d optopt:%d optarg:%s\n",opterr,optind,long_option_index ,optopt,optarg);
					if(long_option_index==-1)
						log_write("option requires an argument -- '%c'\n",option_return);			
					else
						log_write("option '%s' requires an argument\n",program_option.options[long_option_index].name);	
						
					exit(0);
				}else{
					if(ACTION_PACK){
						option_values.image_filename=optarg;
					}else{
						if(check_file_exists(optarg,CHECK_FAIL_EXIT))
							option_values.image_filename=optarg;
					}
				}
				//log_write("\n");
			//log_write("%s %p\n",option_values.image_filename,option_values.image_filename); 
				break;
			}
		case 'o':{
			 option_values.output_directory_name=optarg;
		
			break;
		}
		default:	break;
	}
	fprintf(stderr,"argc:%d opterr:%d optind:%d long_option_index:%d optopt:%d optarg:%s  argv:%s\n",argc,opterr,optind,long_option_index ,optopt,optarg,argv[optind]);
	option_return =  GET_OPT_LONG_FUNCTION
		//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s\n",opterr,optind,option_index ,optopt,optarg);
	} // end while	
	//fprintf(stderr,"opterr:%d optind:%d option_index:%d optopt:%d optarg:%s\n",opterr,optind,option_index ,optopt,optarg);
	
	// We've Processed all the options now lets see if we have everything we need
	if(!optopt){
		check_required_parameters(program_option.action);

		int ret =(*program_option.function_name_p)();
		fprintf(stderr,"Done\n"	);
		
	}*/
	exit(0);
		
	return 0;
}

