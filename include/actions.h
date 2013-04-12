
#ifndef _ac16cc24_978f_11e2_8e27_5404a601fa9d
#define _ac16cc24_978f_11e2_8e27_5404a601fa9d

typedef struct global_action global_action;

struct global_action{
	
	int	debug ;
	int  	log ;
	char *	log_filename;
	int 	verbose ;
};

int process_extract_action(int argc,char ** argv);
int process_create_action(int argc,char ** argv);
int process_update_action(int argc,char ** argv);
int process_info_action(int argc,char ** argv);
int process_scan_action(int argc,char ** argv);
int process_copy_kernel_action(int argc,char ** argv);
int process_create_kernel_action(int argc,char ** argv);



#endif
