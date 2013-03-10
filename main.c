
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


int log_write(const char *format, ...)
{
	return 0;
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
int strstrlcmp(const char *s1,size_t s1_len, const char *s2,size_t s2_len ){
	
	if(!s1 || !s2 || (int)s2_len<0  || (int)s1_len<0 )
		return 99;
	
	size_t compare_length = s1_len > s2_len ? s1_len : s2_len;
	
	int ret=  memcmp(s1,s2,compare_length); 
	return ret;
								
}
int setup_required_defaults(){
	  
	option_values.base_address   = DEFAULT_BASE_ADDRESS;
	option_values.kernel_offset  = DEFAULT_KERNEL_OFFSET ;
    option_values.ramdisk_offset = DEFAULT_RAMDISK_OFFSET;
    option_values.second_offset  = DEFAULT_SECOND_OFFSET ;
    option_values.tags_offset    = DEFAULT_TAGS_OFFSET   ;
    option_values.page_size		 = DEFAULT_PAGE_SIZE;
 
    return 0;
    
}

program_options_t get_program_options(char *program_action){
	
	
	if(!strlcmp(program_action,"remove") || !strlcmp(program_action,"r")) return program_options[REMOVE];
			
	if(!strlcmp(program_action,"extract")  || !strlcmp(program_action,"x") || !strlcmp(program_action,"unpack")   )	return program_options[EXTRACT];
	
	if(!strlcmp(program_action,"list")  || !strlcmp(program_action,"l")) return program_options[LIST];
	
	if(!strlcmp(program_action,"pack")  || !strlcmp(program_action,"p")) return program_options[CREATE];
	
	if(!strlcmp(program_action,"update")  || !strlcmp(program_action,"u")) return program_options[UPDATE];

	help_main();
	exit(0);
	
}
int check_for_lazy_image(char * test_string,const program_actions_emum action){
	
	if(test_string[0]=='-'){
		//log_write("check_for_lazy_image:argument switch found\n");	
		return 0;
	}
	if(action == CREATE){
		//fprintf(stderr,"check_for_lazy_image:creating new image:%s\n",test_string);	
		option_values.image_filename=test_string;
		return 1;
	}
	if(check_file_exists(test_string)){
		option_values.image_filename=test_string;
		//log_write("check_for_lazy_image:found:%s\n",test_string);	
		return 1;
	}
	help_error_no_boot(test_string);
	return 0;	
}
char* is_switch(char* test){
	if(!test)
		return NULL;	
	if((strlen(test) < 2) || (test[0]!='-'))
		return NULL;
	// We've already establish that this is a switch so just check the type
	if(test[1]=='-')
		return test+2;
	else
		return test+1;
}
int parse_value_or_error_exists(char ***argv,void* command_line_switch_p){
	command_line_switch_t* command_line_switch = command_line_switch_p;
	parse_value_or_error(argv,command_line_switch_p);
	if(!check_file_exists((*argv)[0])){
		help_error_arg_file((*argv)--,command_line_switch->long_name);
	}
	return 0;
}
int parse_value_or_error(char ***argv,void* command_line_switch_p){
	command_line_switch_t* command_line_switch = command_line_switch_p;
	char** dest = command_line_switch->dest_ptr;
	// fprintf(stderr,"value %s\n",(*argv)[0]);
	 if((!(*argv)[0]) || ((*argv)[0][0]=='-')){
		help_error_arg_invalid((*argv)--,command_line_switch->long_name);
	}else{
		//fprintf(stderr,"setting value %s\n",(*argv)[0]);
		(*dest)=(*argv)[0];
	}
	return 0;
}
int parse_no_value_arg(char ***argv,void* command_line_switch_p){
	
	command_line_switch_t* command_line_switch = command_line_switch_p;
	
	int** dest = command_line_switch->dest_ptr;
	(*dest)=1;
	fprintf(stderr,"parse_no_value_arg %s\n",command_line_switch->long_name);
	argv[0]--;
	return 0;
}
int parse_value_or_default_directory_exists(char ***argv,void* command_line_switch_p){
	//fprintf(stderr,"parse_value_or_default_directory_exists %s\n",(*argv)[0]);
	command_line_switch_t* command_line_switch = command_line_switch_p;
	char** dest = command_line_switch->dest_ptr;
	 //fprintf(stderr,"value %s\n",(*argv)[0]);
	 if((!(*argv)[0]) || ((*argv)[0][0]=='-')){
		//fprintf(stderr,"setting default %s\n",command_line_switch->default_string);
	 	(*dest)=(char*)command_line_switch->default_string;
	 	argv[0]--;
	}else{
		//fprintf(stderr,"setting value %s\n",(*argv)[0]);
		(*dest)=(*argv)[0];
	
	}
	if(!check_directory_exists((*dest))){		
		help_error_arg_file((*argv)--,command_line_switch->long_name);
	}
	return 0;
}
int parse_value_or_default_exists(char ***argv,void* command_line_switch_p){
	//fprintf(stderr,"parse_value_or_default_exists %s\n",(*argv)[0]);
	command_line_switch_t* command_line_switch = command_line_switch_p;
	char** dest = command_line_switch->dest_ptr;
	// fprintf(stderr,"value %s\n",(*argv)[0]);
	 if((!(*argv)[0]) || ((*argv)[0][0]=='-')){
		//fprintf(stderr,"setting default %s\n",command_line_switch->default_string);
	 	(*dest)=(char*)command_line_switch->default_string;
	 	argv[0]--;
	}else{
		//fprintf(stderr,"setting value %s\n",(*argv)[0]);
		(*dest)=(*argv)[0];
	
	}
	if(!check_file_exists((*dest))){		
		help_error_arg_file((*argv)--,command_line_switch->long_name);
	}
	return 0;
}
int parse_value_int_or_default(char ***argv,void* command_line_switch_p){
	//fprintf(stderr,"parse_value_or_default %s\n",(*argv)[0]);
	command_line_switch_t* command_line_switch = command_line_switch_p;
	int** dest = command_line_switch->dest_ptr;
	 if((!(*argv)[0]) || ((*argv)[0][0]=='-')){
		argv[0]--;
		(*dest)=&command_line_switch->default_value;
	}else{
		(*dest)=atoll((*argv)[0]);
	
	}
	return 0;
}
int parse_value_or_default(char ***argv,void* command_line_switch_p){
	//fprintf(stderr,"parse_value_or_default %s\n",(*argv)[0]);
	command_line_switch_t* command_line_switch = command_line_switch_p;
	char** dest = command_line_switch->dest_ptr;
	// fprintf(stderr,"value %s\n",(*argv)[0]);
	 if((!(*argv)[0]) || ((*argv)[0][0]=='-')){
		//fprintf(stderr,"setting default %s\n",command_line_switch->default_string);
		argv[0]--;
	 	(*dest)=(char*)command_line_switch->default_string;
	}else{
		//fprintf(stderr,"setting value %s\n",(*argv)[0]);
		(*dest)=(*argv)[0];
	
	}
	return 0;
}
char* set_value_or_error(char* value,command_line_switch_t* command_line_switch){
	char** dest = command_line_switch->dest_ptr;
	 if((!value) || (value[0]=='-')){
	 	fprintf(stderr,"invalid argument for switch %s\n",command_line_switch->long_name);
	 	exit(0);
	}else{
		(*dest)=value;
	}
	return (*dest);
}
int parse_file_or_int(char ***argv, void* command_line_switch_p){
	command_line_switch_t* command_line_switch = command_line_switch_p;
	//fprintf(stderr,"parse_file_or_int %s\n",(*argv)[0]);

	char *test_value=(*argv)[0]; 
	size_t default_value=command_line_switch->default_value;
	if(!test_value) return 0;
	size_t length=0;
	unsigned int counter=0;
	for(counter=0 ; counter < length ; counter++){
		if((counter+1<length) && ( (test_value[counter]=='\r') && (test_value[counter+1]=='\n'))){
			test_value[counter]='\0'; break;
		}
		if(test_value[counter]=='\n'){
			test_value[counter]='\0'; 
			break;		
		}
	}
	if(check_file_exists(test_value)) { 
		byte_p file_contents = load_file(test_value,&length);		
		byte_p* dest = command_line_switch->dest_ptr;
		(**dest)=atoi(file_contents);
	
		}else{
			if(strlcmp(test_value,command_line_switch->default_string)){ 
				int** dest = command_line_switch->dest_ptr;
				(**dest)=atoi(test_value);
			}				
		}
	

	//if((length) >max_size){
		//fprintf(stderr,"input exceeds allowed maximum size\ninput length = %d maximum size allowed = %d",length ,max_size);
		//exit(1);
	//};
	return 0;	 
		
}
int parse_file_or_string(char ***argv, void* command_line_switch_p){
	command_line_switch_t* command_line_switch = command_line_switch_p;
	fprintf(stderr,"parse_file_or_string %s\n",(*argv)[0]);

	char *test_value=(*argv)[0]; 
	size_t max_size=command_line_switch->default_value;
	if(!test_value) return 0;
	size_t length=0;
	unsigned int counter=0;
	for(counter=0 ; counter < length ; counter++){
		if((counter+1<length) && ( (test_value[counter]=='\r') && (test_value[counter+1]=='\n'))){
			test_value[counter]='\0'; break;
		}
		if(test_value[counter]=='\n'){
			test_value[counter]='\0'; 
			break;		
		}
	}
	if(check_file_exists(test_value)) { 
		char* file_contents = load_file(test_value,&length);		
		char** dest = command_line_switch->dest_ptr;
		(*dest)=file_contents;
	
		}else{
			if(strlcmp(test_value,command_line_switch->default_string)){ 
				char** dest = command_line_switch->dest_ptr;
				(*dest)=test_value;
				length= strlen(test_value); 
			}				
		}

	if((length) >max_size){
		fprintf(stderr,"input exceeds allowed maximum size\ninput length = %d maximum size allowed = %d",length ,max_size);
		exit(1);
	};
	return 0;	 
		
}
int parse_file_list(char ***argv, void* command_line_switch_p) {
	command_line_switch_t* command_line_switch = command_line_switch_p;
	
	fprintf(stderr,"parse file list\n");
	option_values.file_list=malloc(PATH_MAX);
	int counter=0;
	while((*argv)[0]){
		
		if(is_switch((*argv)[0])){
			break;
		}
		option_values.file_list[counter]=(*argv)[0];
		(*argv)++;counter++;
	}
	if(!counter){
		help_error_arg_file_list(option_values.file_list);
	}
	if(!(*argv)[0]) argv--;
	option_values.file_list[counter]=NULL;
	//fprintf(stderr,"parse file list:option_values.file_list_count %d\n",option_values.file_list_count);
	return 0 ;	
}
int parse_property_list(char ***argv, void* command_line_switch_p) {
	command_line_switch_t* command_line_switch = command_line_switch_p;
	fprintf(stderr,"parse property list\n");
	option_values.property_list=malloc(PATH_MAX);
	int counter=0;
	while((*argv)[0]){
		
		if(is_switch((*argv)[0])){
			break;
		}
		option_values.property_list[counter]=(*argv)[0];
		(*argv)++;counter++;
	}
	if(!counter){
		help_error_arg_file_list(option_values.property_list);

	}
	if(!(*argv)[0]) argv--;
	option_values.property_list[++counter]=NULL;
	fprintf(stderr,"parse file list:option_values.property_list_count %d\n",counter);
	return 0 ;	
}

int parse_file_list_exists(char ***argv, void* command_line_switch_p) {
	command_line_switch_t* command_line_switch = command_line_switch_p;
	fprintf(stderr,"parse file list exists\n");
	option_values.file_list=malloc(PATH_MAX);
	int counter=0;
	while((*argv)[0]){
		
		if(is_switch((*argv)[0])){
			break;
		}
		option_values.file_list[counter]=(*argv)[0];
		fprintf(stderr,"parse file list exists:%s: %s\n",(*argv)[0],option_values.file_list[counter]);
		(*argv)++;counter++;
	}
	if(!counter){
		help_error_arg_file_list(option_values.file_list);
	}
	if(!(*argv)[0]) argv--;

	option_values.file_list[++counter]=NULL;
	fprintf(stderr,"parse file list:option_values.file_list_count %s %s\n",option_values.file_list[3],option_values.file_list[2]);
	return 0 ;	
}
int parse_command_line_switches(char ***argv,program_options_t program_options){
	
	
	command_line_switch_t* switches_start = program_options.command_line_switches;
	while((*argv)[0]){
		char* switch_string = is_switch(*argv[0]);
	
		if(!switch_string){
			fprintf(stderr,"unknown command line switch '%s' %s\n",*argv[0],switch_string);
			exit(0);
		}
		
		//fprintf(stderr,"argv[0]=%s\n",*argv[0]);
		while(program_options.command_line_switches){
			if((!strlcmp(program_options.command_line_switches->short_char,switch_string)) || (!strlcmp(program_options.command_line_switches->long_name,switch_string))){
				if(program_options.command_line_switches->parser_function_p){
					//fprintf(stderr,"argv[0]=%s\n",*argv[0]);
					(*argv)++;
					(*program_options.command_line_switches->parser_function_p)(argv,program_options.command_line_switches);
					//fprintf(stderr,"inner argv[0]=%s\n",(*argv)[0]);
					break;
				}
			}else {
				
			}
			//fprintf(stderr,"inner argv[0]=%s\n",(*argv)[0]);
			program_options.command_line_switches++;
		} //endwhile inner
		program_options.command_line_switches=switches_start;
		//fprintf(stderr,"outer argv[0]=%s\n",(*argv)[0]);
		if(!(*argv)[0]){ break; }// fprintf(stderr,"break\n"); break;}
		(*argv)++;
		//fprintf(stderr,"outer argv[0]=%s\n",(*argv)[0]);
		
	}
	//fprintf(stderr,"done loop\n");
	
	return 0;
	
}
int try_implicit_mode(char ***argv,program_options_p program_options){

	//fprintf(stderr,"argv[0]=%s\n",(*argv)[0]);
	if(check_for_lazy_image((*argv)[0],program_options->action)){	
		switch(program_options->action){
		 case LIST:{
			(*argv)++;
			if(!(*argv)[0]){
				option_values.list_ramdisk =  option_values.list_section=  option_values.list_kernel= option_values.list_kernel_version= option_values.list_header = 1 ;
				int ret =(*program_options->action_function_p)();
				exit(0);
			}else
				option_values.list_ramdisk =  option_values.list_kernel= option_values.list_kernel_version= option_values.list_header = 0 ;
			
			break;
			}
		case EXTRACT: { // Extract implied order is Image,Source,Target
			 (*argv)++ ;
			 if((*argv)[0]){
				if((*argv)[0][0]!='-'){ 
					option_values.source_filename=(*argv)[0];
					int ret =(*program_options->action_function_p)();
					exit(0);
				}
			}else{
				fprintf(stderr,"no more args here %s\n",(*argv)[0]);
			}
			break;}
		case UPDATE: {
			(*argv)++ ;
			if((*argv)){
				//fprintf(stderr,"argv[0]=%s\n",(*argv)[0]);
				if((*argv)[0][0]!='-'){ 
					option_values.source_filename=(*argv)[0];
				}else{ 
					break;
				}
				if((*argv)){ // target and source are the same name
					option_values.target_filename=(*argv)[0];
				}
				
				int ret =(*program_options->action_function_p)();
				exit(0);
			}else{
				fprintf(stderr,"no more args updare\n"); 
			}break ; 
		}
		case CREATE:{
			//	fprintf(stderr,"pack\n"); 
			(*argv)++ ; break;
		}
		default:{
			fprintf(stderr,"default\n"); 
			(*argv)++ ; break;
			}
		}
	}
	return 0;
}
int main(int argc, char **argv){ 
	

	if(argc==1){ help_main();}	
		
	
	check_for_help_call(argc, argv);
	option_values.log_stdout=1;
	option_values.argument_count=argc;
	program_options_t program_options=get_program_options(argv[1]);
	argc-- ; argv++ ;
	
	if(argc==1){(*program_options.help_function_p)();}	
	(*program_options.setup_function_p)();
	argv++ ;
	try_implicit_mode(&argv,&program_options);
	
	parse_command_line_switches(&argv,program_options);
	int ret =(*program_options.action_function_p)();
		
	exit(0);
		
	return 0;
}

