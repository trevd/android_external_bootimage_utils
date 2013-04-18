
#ifndef _ac16cc24_978f_11e2_8e27_5404a601fa9d
#define _ac16cc24_978f_11e2_8e27_5404a601fa9d

#define ACTION_UNKNOWN 				-1
#define ACTION_NONE					0
#define ACTION_INFO					1
#define ACTION_UPDATE				2
#define ACTION_UPDATE_KERNEL		3
#define ACTION_UPDATE_RAMDISK		4
#define ACTION_UPDATE_PROPERTIES	5
#define ACTION_UPDATE_FILES			6
#define ACTION_EXTRACT				7
#define ACTION_EXTRACT_KERNEL		8
#define ACTION_EXTRACT_RAMDISK		9
#define ACTION_EXTRACT_HEADER		10
#define ACTION_SCAN					11
#define ACTION_COPY_KERNEL			12
#define ACTION_COPY_RAMDISK			13
#define ACTION_CREATE_BOOT_IMAGE	14
#define ACTION_CREATE_RAMDISK 		15
#define ACTION_CREATE_KERNEL 		16



typedef struct global_action global_action;

struct global_action{
	
	int		debug ;
	int  	log ;
	char *	log_filename;
	char *	program_name;
	int 	verbose ;
	int 	multicall ;
	int		process_action;
	int		current_working_directory;
};

int init_global_action(unsigned argc,char ** argv,global_action* action);
int only_global_actions(unsigned argc,char ** argv,global_action* action);

int process_global_action(unsigned argc,char ** argv,global_action* action);

int process_extract_action(unsigned argc,char ** argv,global_action* gaction);

int process_update_action(unsigned argc,char ** argv,global_action* gaction);

int process_info_action(unsigned argc,char ** argv,global_action* gaction);

int process_scan_action(unsigned argc,char ** argv,global_action* gaction);

int process_copy_kernel_action(unsigned argc,char ** argv,global_action* gaction);
int process_copy_ramdisk_action(unsigned argc,char ** argv,global_action* gaction);

int process_create_action(unsigned argc,char ** argv,global_action* gaction);
int process_create_kernel_action(unsigned argc,char ** argv,global_action* gaction);
int process_create_ramdisk_action(unsigned argc,char ** argv,global_action* gaction);





#endif
