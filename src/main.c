#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <biutils.h>

enum program_actions_enum {
	ACTION_EXTRACT,
	ACTION_EXTRACT_KERNEL,
	ACTION_EXTRACT_RAMDISK,
	ACTION_EXTRACT_BOOTIMAGE,
	ACTION_EXTRACT_BOOTIMAGE_KERNEL,
	ACTION_EXTRACT_BOOTIMAGE_FILE,
	ACTION_EXTRACT_BOOTIMAGE_FILES,
	ACTION_PRINT,
	ACTION_PRINT_HEADER,
	ACTION_PRINT_KERNEL,
	ACTION_PRINT_RAMDISK,
	ACTION_PRINT_RAMDISK_LIST,
	ACTION_PRINT_BOOTIMAGE,
	ACTION_PRINT_BOOTIMAGE_HEADER,
	ACTION_PRINT_BOOTIMAGE_KERNEL,
	ACTION_PRINT_BOOTIMAGE_RAMDISK,
	ACTION_INFO
};
#define ACTION_COUNT 17
static struct program_actions_t {
		int index ;
		char* name ;
		int name_length ;
}  program_actions[ACTION_COUNT] = {
	{ACTION_EXTRACT_BOOTIMAGE_KERNEL,"extract-bootimage-kernel",24 },
	{ACTION_EXTRACT_BOOTIMAGE_FILE,"extract-bootimage-file",22 },
	{ACTION_EXTRACT_BOOTIMAGE_FILES,"extract-bootimage-files",23 },
	{ACTION_EXTRACT_BOOTIMAGE,"extract-bootimage",17 },
	{ACTION_EXTRACT_RAMDISK,"extract-ramdisk",15 },
	{ACTION_EXTRACT_KERNEL,"extract-kernel",14 },
	{ACTION_EXTRACT,"extract",7 },
	{ACTION_PRINT_HEADER,"print-header",12 },
	{ACTION_PRINT_KERNEL,"print-kernel",12 },
	{ACTION_PRINT_RAMDISK,"print-ramdisk",13 },
	{ACTION_PRINT_RAMDISK_LIST,"print-ramdisk-list",18 },
	{ACTION_PRINT_BOOTIMAGE,"print-bootimage",15 },
	{ACTION_PRINT_BOOTIMAGE_HEADER,"print-bootimage-header",22 },
	{ACTION_PRINT_BOOTIMAGE_KERNEL,"print-bootimage-kernel",22 },
	{ACTION_PRINT_BOOTIMAGE_RAMDISK,"print-bootimage-ramdisk",23 },
	{ACTION_PRINT,"print",5 },
	{ACTION_INFO,"info",4 },

};
int get_action(char* argv){

	int i = 0 ;
	int found = 0 ;
	for ( i = 0 ; i < ACTION_COUNT ; i++) {
		//int j = 0 ;
		//fprintf(stdout,"program_actions[i].name=%s\n",program_actions[i].name);
		//for ( j = 0 ; j < argc ; j++ ) {
			size_t len = utils_sanitize_string(argv,PATH_MAX);
			//fprintf(stdout,"len=%d\n",len);
			if (len == SIZE_MAX ){
				//fprintf(stdout,"returning =%d\n",len);
				return -1;
			}
			//fprintf(stdout,"Found %d argv[j]=%s program_actions[i].name=%s\n",i,argv[j],program_actions[i].name);
			unsigned char* action = utils_memmem(argv,len,program_actions[i].name,program_actions[i].name_length);
			if ( action != NULL ){
				fprintf(stdout,"Found %d\n",i);
				found = 1 ;
				break ;
			}


	}
	if ( found == 0 ){
		return -1;
	}
	//fprintf(stdout,"Found %d\n",i);
	return program_actions[i].index ;

}


int main(int argc , char** argv){

	/* Sanity Check argc first. Never trust user input not even in main */
	if ( argc <= 0 ){
		return 0 ;
	}
	if ( argc == 1 ){
		fprintf(stdout,"Help\n");
	}
	int action = get_action(argv[0]);
	fprintf(stdout,"argv[0]=%s action=%d action=%s\n",argv[0],action ,program_actions[action].name);
	if ( action == -1 ){
		argv++;
		argc--;
		action = get_action(argv[0]);
		fprintf(stdout,"Second argv[0]=%s action=%d action=%s\n",argv[0],action ,program_actions[action].name);
	}
	fprintf(stdout,"action=%d %d\n",action,ACTION_EXTRACT_BOOTIMAGE_KERNEL);
	if ( action == ACTION_EXTRACT_BOOTIMAGE_KERNEL ) {
		fprintf(stdout,"argv[1]=%s\n",argv[1]);
		if ( argc == 2 ){

			bootimage_file_extract_kernel(argv[1],NULL);
		} else if ( argc == 3 ){
			bootimage_file_extract_kernel(argv[1],argv[2]);
		}
	}
	/*struct bootimage_utils* biu = bootimage_utils_initialize();
	bootimage_utils_file_read(biu,argv[1]);
	bootimage_utils_free(&biu);

	fprintf(stdout,"biu=%p\n",biu);
*/
	return 0 ;

}
