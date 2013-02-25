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
int strstrlcmp(const char *s1, const char *s2,size_t s2_len ){
	
	if(!s1 || !s2 || s2_len<0 )
		return 99;
	
	size_t string_one_length=strlen(s1);
	size_t string_two_length=s2_len;
	size_t compare_length = string_one_length > string_two_length ?
								string_one_length : string_two_length;
	int ret=  memcmp(s1,s2,compare_length); 
	return ret;
								
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
int check_for_lazy_image(char * test_string,const program_actions_emum action){
	
	if(test_string[0]=='-')
		return 0;
	
	if(action == PACK){
		option_values.image_filename=test_string;
		return 1;
	}
	if(check_file_exists(test_string)){
		option_values.image_filename=test_string;
		return 1;
	}
	return 0;	
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
				command_lines_switches->is_set=1;
				*(char **)command_lines_switches->dest_ptr=(*argv[0]);  /* argument value is ok. assign the deference the dest_ptr so we can assign */
				(*argv)++; 												/* a value to the option_value member directly and move to the next argument */
				break ; }
			case DEF_STR_ARG:{ 
				(*argv)++; // argument comes with a default value move the argument pointer to be the next arg along
				if((!(*argv[0])) || (*argv[0][0])=='-'){ // next args appears to be a switch, use the default value instead
					*(const char **)command_lines_switches->dest_ptr =command_lines_switches->default_string;
					command_lines_switches->is_set=1;
				}else{ // assign the argument value as normal
					*(char **)command_lines_switches->dest_ptr=(*argv[0]);
					command_lines_switches->is_set=1;
					(*argv)++;
				}
				break;
			}
		default: break;		 
	}
			
	return 0;
}
char* parse_file_or_string(char *filename,char* value ,char* default_value, size_t  max_size){
	
	size_t length=0; char *return_value = ""; 
	if(value) {
		length = strlen(value);
	}else{
		if(check_file_exists(filename)) { // found the start of a direct command line look for the end
			return_value = (char*) load_file(filename,&length);
		}else{
			if(strlcmp(filename,default_value)){ 
				// File doesn't exist and the text is not default assume direct input
				length= strlen(filename); 
				return_value = filename;
			}				
		}
	}
	// santize for new line
	int counter=0;
	for(counter=0 ; counter < length ; counter++){
		if((counter+1<length) && ( (return_value[counter]=='\r') && (return_value[counter+1]=='\n'))){
			return_value[counter]='\0'; break;
		}
		if(return_value[counter]=='\n'){
			return_value[counter]='\0'; 
			break;		
		}
	}
	length  = strlen(return_value);
	if((length) > max_size){
		fprintf(stderr,"input exceeds allowed maximum size\ninput length = %d maximum size allowed = %d",length ,max_size);
		exit(1);
	};
		 
	return return_value;	
}
int check_required_parameters(const program_actions_emum action){

	if(!option_values.image_filename){
			fprintf(stderr,"image file name not set\n"); exit(0);
	}
	
	if(action!=PACK){
		if(!check_file_exists(option_values.image_filename)){
			fprintf(stderr,"boot image file %s does not exist\n",option_values.image_filename);
			exit(0);
		}
		if((check_file_exists(option_values.image_filename)) && (access(option_values.image_filename,R_OK))){
			fprintf(stderr,"cannot open file boot image %s\n",option_values.image_filename);
			exit(1);}
	}
	
	errno=0; int got1=0;
	switch(action){
			case UNPACK:{
					if((option_values.output_directory_name) && (check_directory_exists(option_values.output_directory_name))){
							
							fprintf(stderr,"the directory %s already exists - overwrite existing files\n",option_values.output_directory_name);
					}else{
							option_values.output_directory_name = malloc(PATH_MAX); 
							getcwd(option_values.output_directory_name,PATH_MAX);
					}
					
					break;
				}
			case PACK:{
				if(!option_values.page_size) option_values.page_size=DEFAULT_PAGE_SIZE;
				if((option_values.board_filename) || (option_values.board_name)  )	
					option_values.cmdline_text =parse_file_or_string(option_values.board_filename,option_values.board_name,DEFAULT_BOARD_NAME,BOOT_NAME_SIZE);	
				if((option_values.cmdline_filename)  || (option_values.cmdline_text)  )	
					option_values.cmdline_text =parse_file_or_string(option_values.cmdline_filename,option_values.cmdline_text,DEFAULT_CMDLINE_NAME,BOOT_ARGS_SIZE);	
				
				if(!option_values.kernel_filename){ // Image file is not set look for a valid filename 
						fprintf(stderr,"no kernel filename set, you must supply a valid linux kernel file to create boot images\n");
						exit(1);
				}  
				fprintf(stderr,"option_values.ramdisk_directory_name=%s\n",option_values.ramdisk_directory_name);
				//if(option_values.ramdisk_directory_name  && (!check_directory_exists(option_values.ramdisk_directory_name))
				if(!check_file_exists(option_values.ramdisk_archive_filename)){
					if(!check_directory_exists(option_values.ramdisk_directory_name)){
						if(!check_file_exists(option_values.ramdisk_cpio_filename)){ 
							fprintf(stderr,"no ramdisk file or directory set, you must supply a valid ramdisk source to create boot images\n");
							exit(1);
						}
					}
				}
				break;
			}
			case UPDATE:{
				if((option_values.board_filename) || (option_values.board_name)  )	
					option_values.board_name =parse_file_or_string(option_values.board_filename,option_values.board_name,DEFAULT_BOARD_NAME,BOOT_NAME_SIZE);	
				if((option_values.cmdline_filename)  || (option_values.cmdline_text)  )	
					option_values.cmdline_text =parse_file_or_string(option_values.cmdline_filename,option_values.cmdline_text,DEFAULT_CMDLINE_NAME,BOOT_ARGS_SIZE);	
				
				if((option_values.kernel_filename) ||  (!check_file_exists(option_values.ramdisk_archive_filename))){ 
						fprintf(stderr,"kernel file %s not found\n",option_values.kernel_filename);
				}
				
				
				fprintf(stderr,"cmdline_filename %s not found\n",option_values.cmdline_filename);
				if((option_values.source_filename) && (!check_file_exists(option_values.source_filename))){
						fprintf(stderr,"source file %s not found\n",option_values.source_filename);
						exit(0); }
				if(!option_values.target_filename){
						fprintf(stderr,"target_filename not specified, source file name will be used\n");
						}
				break ; 
			}
			case EXTRACT:{
					option_values.source_length=1 ;
					if(!option_values.source_filename){
						
						fprintf(stderr,"nothing to extract, you must set one or more sourcefiles\n");
						exit(0);
					}
					if(!option_values.target_filename){
						log_write("main:extract no target file set - using source\n");		
						
						option_values.target_filename=option_values.source_filename;
					}
					if (strchr(option_values.source_filename,',')!=NULL){
						option_values.target_filename=NULL;
		
						int counter = 0 , original_length = strlen(option_values.source_filename);
						for(counter =0 ; counter < original_length;counter++){
								if(option_values.source_filename[counter]==','){
									option_values.source_filename[counter]='\0';
									option_values.source_length += 1;
							}
						}
	
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
	return 0;
	
}
int main(int argc, char **argv){ 
	

	if(argc==1){ print_main_usage();}	
	
	
	//fprintf(stderr,"%d %s\n",argc,argv[1]);
	check_for_help_call(argc, argv);
		
	option_values.log_stdout=1;
	 
	program_options_t program_options=get_program_options(argv[1]);
	argc-- ; argv++ ;
	if(argc==1){ print_main_usage();}	
	
	if(!(option_values.action=program_options.action)) { print_main_usage();}	
	argc-- ; argv++ ;
	fprintf(stderr,"argv[0]=%s\n",argv[0]);
	if(check_for_lazy_image(argv[0],program_options.action)){	
		switch(program_options.action){
		 case LIST:{
			int ret =(*program_options.action_function_p)();
			exit(0);
			break;
			}
		case UNPACK: {
			argc-- ; argv++ ;
			if(argc==0){ 
				fprintf(stderr,"no more args\n");
				exit(0);

			}break;
		}
		case EXTRACT: { // Extract implied order is Image,Source,Target
			 argc-- ; argv++ ;
			 if(argc>0){
				fprintf(stderr,"argv[0]=%s\n",argv[0]);
				if(argv[0][0]!='-'){ 
					option_values.source_filename=argv[0];
					check_required_parameters(program_options.action);
					int ret =(*program_options.action_function_p)();
					exit(0);
				}
			}else
				fprintf(stderr,"no more args\n");
			
			break;}
		case UPDATE: {
			argc-- ; argv++ ;
			if(argc>0){
				fprintf(stderr,"argv[0]=%s\n",argv[0]);
				if(argv[0][0]!='-'){ 
					option_values.source_filename=argv[0];
				
				}else{ break;}
				
				if(argc==1){
					option_values.target_filename=argv[0];
				}
				check_required_parameters(program_options.action);
				int ret =(*program_options.action_function_p)();
				exit(0);
			}else{
				fprintf(stderr,"no more args\n"); 
			}break ; 
		}
		case PACK:{
				fprintf(stderr,"pack\n"); 
			argc-- ; argv++ ; break;
		}
		default:{
			fprintf(stderr,"default\n"); 
			argc-- ; argv++ ; break;
			}
		}
	}
			
	fprintf(stderr,"argv[0]=%s\n",argv[0]);
	parse_command_line_switches(argv,program_options);
	int ret =(*program_options.action_function_p)();
	
	fprintf(stderr,"Done\n"	);
	
	exit(0);
		
	return 0;
}

