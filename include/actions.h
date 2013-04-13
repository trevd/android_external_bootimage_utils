
#ifndef _ac16cc24_978f_11e2_8e27_5404a601fa9d
#define _ac16cc24_978f_11e2_8e27_5404a601fa9d

typedef struct global_action global_action;

struct global_action{
	
	int	debug ;
	int  	log ;
	char *	log_filename;
	int 	verbose ;
};
int only_global_actions(int argc,char ** argv,global_action* action);
int process_extract_action(int argc,char ** argv,global_action* gaction);
int process_create_action(int argc,char ** argv,global_action* gaction);
int process_update_action(int argc,char ** argv,global_action* gaction);
int process_info_action(int argc,char ** argv,global_action* gaction);
int process_scan_action(int argc,char ** argv,global_action* gaction);
int process_copy_kernel_action(int argc,char ** argv,global_action* gaction);
int process_create_kernel_action(int argc,char ** argv,global_action* gaction);
int process_create_ramdisk_action(int argc,char ** argv,global_action* gaction);
int process_global_action(int argc,char ** argv,global_action* action);



#endif
