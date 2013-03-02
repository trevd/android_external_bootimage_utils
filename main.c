
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
		//log_write("check_for_lazy_image:creating new image:%s\n",test_string);	
		option_values.image_filename=test_string;
		return 1;
	}
	if(check_file_exists(test_string)){
		option_values.image_filename=test_string;
		//log_write("check_for_lazy_image:found:%s\n",test_string);	
		return 1;
	}
	help_main_boot();
	return 0;	
}
char* is_switch(char* test){
	if(!test)
		return NULL;	
	if((strlen(test) < 2) || (test[0]!='-'))
		return NULL;
	// We've already establish that this is a switch
	// so just check the type
	if(test[1]=='-')
		return test+2;
	else
		return test+1;
}
int parse_value_or_error_exists(char ***argv,void* command_line_switch_p){return 0;}
int parse_value_or_error(char ***argv,void* command_line_switch_p){return 0;}
int parse_value_or_default_exists(char ***argv,void* command_line_switch_p){return 0;
}
int parse_value_or_default(char ***argv,void* command_line_switch_p){
	command_line_switch_t* command_line_switch = command_line_switch_p;
	char** dest = command_line_switch->dest_ptr;
	 fprintf(stderr,"value %s\n",(*argv)[0]);
	 if((!(*argv)[0]) || ((*argv)[0][0]=='-')){
		fprintf(stderr,"setting default %s\n",command_line_switch->default_string);
		argv[0]--;
	 	(*dest)=command_line_switch->default_string;
	}else{
		fprintf(stderr,"setting value %s\n",(*argv)[0]);
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
int parse_file_or_string(char ***argv, void* command_line_switch_p){
	command_line_switch_t* command_line_switch = command_line_switch_p;
	//fprintf(stderr,"parse_file_or_string %s\n",filename);
	(*argv)++;
	char *test_value=(*argv)[0]; 
	

	
	int max_size=command_line_switch->default_value;
	
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
			char* file_contents = (char*) load_file(test_value,&length);		
			set_value_or_error(file_contents,command_line_switch); 
			
		}else{
			if(strlcmp(test_value,command_line_switch->default_string)){ 
				set_value_or_error(test_value,command_line_switch); 
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
	option_values.file_list_count=0;
	while((*argv)[0]){
		
		if(is_switch((*argv)[0])){
			break;
		}
		option_values.file_list[option_values.file_list_count]=(*argv)[0];
		option_values.file_list_count+=1;
		(*argv)++;
	}
	if(!option_values.file_list_count){
		fprintf(stderr,"no files specified in file list %d\n",option_values.file_list_count);
		free(option_values.file_list);
		exit(0);
	}
	if(!(*argv)[0]) argv--;
	option_values.file_list[option_values.file_list_count]=NULL;
	fprintf(stderr,"parse file list:option_values.file_list_count %d\n",option_values.file_list_count);
	return 0 ;	
}
int parse_command_line_switches(char ***argv,program_options_t program_options){
	
	
	command_line_switch_t* switches_start = program_options.command_line_switches;
	while((*argv)[0]){
		
		char* switch_string = is_switch(*argv[0]);
		//fprintf(stderr,"arg=%s\n",switch_string);
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
					fprintf(stderr,"inner argv[0]=%s\n",(*argv)[0]);
					break;
				}
			}else {
				
			}
			//fprintf(stderr,"inner argv[0]=%s\n",(*argv)[0]);
			program_options.command_line_switches++;
		} //endwhile inner
		program_options.command_line_switches=switches_start;
		fprintf(stderr,"outer argv[0]=%s\n",(*argv)[0]);
		if(!(*argv)[0]) break;
		(*argv)++;
			//fprintf(stderr,"outer argv[0]=%s\n",(*argv)[0]);
		
	}
	//fprintf(stderr,"argv[0]=%s\n",argv[0]);
	
	return 0;
	
}
int try_implicit_mode(char ***argv,program_options_p program_options){

	//fprintf(stderr,"argv[0]=%s\n",(*argv)[0]);
	if(check_for_lazy_image((*argv)[0],program_options->action)){	
		switch(program_options->action){
		 case LIST:{
			int ret =(*program_options->action_function_p)();
			exit(0);
			break;
			}
		case EXTRACT: { // Extract implied order is Image,Source,Target
			 (*argv)++ ;
			 fprintf(stderr,"no more args\n");
			 if((*argv)){
				
				if((*argv)[0][0]!='-'){ 
					option_values.source_filename=(*argv)[0];
					int ret =(*program_options->action_function_p)();
					exit(0);
				}
			}else
				fprintf(stderr,"no more args\n");
			
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
				fprintf(stderr,"no more args\n"); 
			}break ; 
		}
		case CREATE:{
				fprintf(stderr,"pack\n"); 
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
	//fprintf(stderr,"%d %s\n",argc,argv[1]);
	if(argc==1){(*program_options.help_function_p)();}	
	fprintf(stderr,"%d %s\n",argc,argv[1]);
	argv++ ;
	//fprintf(stderr,"%d %s\n",argc,argv[1]);
	try_implicit_mode(&argv,&program_options);
	//fprintf(stderr,"%d %s\n",argc,argv[1]);
	parse_command_line_switches(&argv,program_options);
	int ret =(*program_options.action_function_p)();
	
	fprintf(stderr,"Done\n"	);
	
	exit(0);
		
	return 0;
}

