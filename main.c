
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
	
	size_t compare_length = s1_len > s2_len ?
								s1_len : s2_len;
	int ret=  memcmp(s1,s2,compare_length); 
	return ret;
								
}
program_options_t get_program_options(char *program_action){
	
	
	if(!strlcmp(program_action,"remove") || !strlcmp(program_action,"r")) return program_options[REMOVE];
			
	if(!strlcmp(program_action,"extract")  || !strlcmp(program_action,"x"))	return program_options[EXTRACT];
	
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
	help_main();
	return 0;	
}
void split_file_list(){
	if(option_values.file_list){
		option_values.file_list_count=1 ;
		if (strchr(option_values.file_list,',')!=NULL){
			int counter = 0 , original_length = strlen(option_values.file_list);
			for(counter =0 ; counter < original_length;counter++){
				if(option_values.file_list[counter]==','){
					option_values.file_list[counter]='\0';
					option_values.file_list_count += 1;
				}
			}
		}
		//log_write("option_values.file_list_count=%d\n",option_values.file_list_count);		
	}
}
char* parse_file_or_string(char *filename,char* default_value, size_t  max_size){
	//fprintf(stderr,"parse_file_or_string %s\n",filename);
	if(!filename) return "\0";
	size_t length=0; char *return_value = "\0"; 
	
		if(check_file_exists(filename)) { // found the start of a direct command line look for the end
			return_value = (char*) load_file(filename,&length);
		}else{
			if(strlcmp(filename,default_value)){ // File doesn't exist and the text is not default assume direct input
				length= strlen(filename); 
				return_value = filename;
			}				
		}

	// santize for new line
	unsigned int counter=0;
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
	
	if(action!=CREATE){
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
				
				
				
			case CREATE:{
				if(!option_values.page_size) option_values.page_size=DEFAULT_PAGE_SIZE;
					option_values.board_name =parse_file_or_string(option_values.board_filename,DEFAULT_BOARD_NAME,BOOT_NAME_SIZE);	
					option_values.cmdline_text =parse_file_or_string(option_values.cmdline_filename,DEFAULT_CMDLINE_NAME,BOOT_ARGS_SIZE);	
				
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
				if(option_values.board_filename)	
					option_values.board_name =parse_file_or_string(option_values.board_filename,DEFAULT_BOARD_NAME,BOOT_NAME_SIZE);	
				if((option_values.cmdline_filename)  )	
					option_values.cmdline_text =parse_file_or_string(option_values.cmdline_filename,DEFAULT_CMDLINE_NAME,BOOT_ARGS_SIZE);
						
				//fprintf(stderr,"option_values.cmdline_text %s\n",option_values.cmdline_filename);
				if((option_values.kernel_filename) ||  (!check_file_exists(option_values.ramdisk_archive_filename))){ 
						fprintf(stderr,"kernel file %s not found\n",option_values.kernel_filename);
				}
				split_file_list();

				if(!check_file_exists(option_values.source_filename))	{
					fprintf(stderr,"source file %s not found\n",option_values.source_filename);
				}else{ 
					fprintf(stderr,"source file name not specified\n");
				}
				
				if(!option_values.target_filename){
						fprintf(stderr,"target_filename not specified, source file name will be used\n");
						}
				break ; 
			}
			case EXTRACT:{
				fprintf(stderr,"check extract\n");
					if(option_values.kernel_filename){ // Image file is not set look for a valid filename 
						fprintf(stderr,"kernel filename=%s\n",option_values.kernel_filename);
					}
					//if(option_values.file_list)  
					if((option_values.output_directory_name) && (check_directory_exists(option_values.output_directory_name))){
							
							fprintf(stderr,"the directory %s already exists - overwrite existing files\n",option_values.output_directory_name);
					}else{
							option_values.output_directory_name = malloc(PATH_MAX); 
							getcwd(option_values.output_directory_name,PATH_MAX);
					}
					
					
					
					if(!option_values.source_filename){
							if(!option_values.target_filename){
						log_write("main:extract no target file set - using source\n");		
						option_values.target_filename=option_values.source_filename;
						}
								//fprintf(stderr,"nothing to extract, you must set one or more sourcefiles\n");
					}
					split_file_list();
					break;
			}
			default:
					break;
		}
		
		return 0;
}
int parse_command_line_switches(char **argv,program_options_t program_options){
	
	
	command_line_switch_t* switches_start = program_options.command_line_switches;
	while(argv[0]){
		//fprintf(stderr,"argv[0]=%s\n",argv[0]);
		char* arg = strrchr(argv[0],'-')==NULL ? argv[0] : strrchr(argv[0],'-')+1;
		while(program_options.command_line_switches){
			if((!strlcmp(program_options.command_line_switches->short_char,arg)) || (!strlcmp(program_options.command_line_switches->long_name,arg))){
				//fprintf(stderr,"match argv[0]=%s short\n",arg);
				switch(program_options.command_line_switches->argument_type){
					case REQ_STR_ARG:{ 
						// cast the void * to a double char pointer ( or a pointer to a pointer
						char** dest = (char**)program_options.command_line_switches->dest_ptr;
						if((!argv[1]) || (argv[1][0]=='-') ){
							fprintf(stderr,"invalid argument for switch '%s'  \n",arg); 
							exit(0);
						}else{
							(*dest)=argv[1];
						}	
						argv++;
						break;
					}
					case DEF_STR_ARG:{ 
						char** dest = (char**)program_options.command_line_switches->dest_ptr;
						if((!argv[1]) || (argv[1][0]=='-') ){
							(*dest)=program_options.command_line_switches->default_string;
						}else{
							argv++;
							(*dest)=argv[0];
							//fprintf(stderr,"argv[1]=%s 1%s 2%s\n",option_values.cmdline_filename,dest[0],argv[0]);
							
						}
						break;	
					}
				}
			break ;
			} //endif	
			if(program_options.command_line_switches->argument_type==0){
				fprintf(stderr,"unknown command line switch '%s'\n",arg);
				exit(0);
			}
			program_options.command_line_switches++;
		} //endwhile inner
		program_options.command_line_switches=switches_start;
		argv++;
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
			 
			 if((*argv)){
				
				if((*argv)[0][0]!='-'){ 
					option_values.source_filename=(*argv)[0];
					check_required_parameters(program_options->action);
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
				check_required_parameters(program_options->action);
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
	
	
	//fprintf(stderr,"%d %s\n",argc,argv[1]);
	check_for_help_call(argc, argv);
		
	option_values.log_stdout=1;
	 
	program_options_t program_options=get_program_options(argv[1]);
	argc-- ; argv++ ;
	if(argc==1){(*program_options.help_function_p)();}	
	argv++ ;
	
	try_implicit_mode(&argv,&program_options);
	

	parse_command_line_switches(argv,program_options);
	check_required_parameters(program_options.action);
	int ret =(*program_options.action_function_p)();
	
	fprintf(stderr,"Done\n"	);
	
	exit(0);
		
	return 0;
}

