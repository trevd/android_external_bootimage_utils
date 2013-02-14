#ifndef _PROGAM_H_
#define _PROGAM_H_

#include "getopt.h"

#include <limits.h>	

int unpack_boot_image_file();
int pack_boot_image_file();
int list_boot_image_info();
int extract_boot_image_file();

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__) 
	#include <windows.h>
#define lstat stat
#define DEFAULT_BOARD_NAME "board.txt"
#define DEFAULT_HEADER_NAME "header.txt"
#define DEFAULT_CMDLINE_NAME "cmdline.txt"
#define DEFAULT_PAGESIZE_NAME "pagesize.txt"
#define DEFAULT_HEADER_NAME_LENGTH 10
#else
#define DEFAULT_HEADER_NAME "header"
#define DEFAULT_CMDLINE_NAME "cmdline"
#define DEFAULT_PAGESIZE_NAME "pagesize"
#define DEFAULT_BOARD_NAME "board"
#endif



// Specified the switches that are allow for each action type
// Most Switches have the same meaning between action types

static struct option pack_global_options[] = {
	{"help", optional_argument,0,(int)NULL},   
	{"log",  optional_argument,0,(int)NULL},  
	 {0, 0, 0, 0} 
};
static struct option pack_long_options[] =
	 {
	   /* These options don't set a flag.
		  We distinguish them by their indices. */
	   {"boot-image",  required_argument,0, 'i'},                              
	   {"ramdisk",  required_argument,       0, 'r'},
	   {"kernel",  required_argument, 0, 'k'},
	   {"cmdline", required_argument,0,'c' },
		{"board-name", required_argument,0,'n' },
		{"base-address", required_argument,0,'b' },
		{"pagesize", required_argument,0,'p' },
		{"ramdisk-directory", required_argument,0,'d' },
		{"second", required_argument,0,'s' },
	   {0, 0, 0, 0}
	 };	
	 
static struct option unpack_long_options[] = {
			
               {"boot-image",  required_argument,0, 'i'},
               {"all",			no_argument,0,'a'},
               {"ramdisk-name",  optional_argument,0, 'r'},
               {"ramdisk-cpio",  no_argument,0, (int)NULL},
               {"ramdisk-archive",  no_argument,0, 'x'},
               {"ramdisk-directory",  no_argument,0, 'd'},
               {"kernel",  optional_argument, 0, 'k'},
               {"output-dir",  required_argument, 0, 'o'},
               {"cmdline",    optional_argument, 0, 'c'},
               {"name",    optional_argument, 0, 'b'},
               {"second",    optional_argument, 0, 's'},
               {"header",    optional_argument, 0, 'h'},
               {"pagesize",  required_argument, 0, 's'},
               {0, 0, 0, 0}
             };
             
static struct option extract_long_options[] = {
               {"input",  required_argument,0, 'i'},
               {"target",  required_argument,0, 't'},
               {"source",  required_argument, 0, 's'},
               {"",  required_argument, 0, 's'},
               {0, 0, 0, 0}
             };
static struct option update_long_options[] = {
               {"kernel",  required_argument,0, 'i'},
               {"filename",  required_argument,0, 'f'},
               {"output-dir",  required_argument, 0, 'o'},
               {0, 0, 0, 0}
             };
 static struct option list_long_options[] = {
			
               {"input",  required_argument,0, 'i'},
               {"filename",  required_argument,0, 'r'},
               {"kernel",  optional_argument, 0, 'k'},
               {"output-dir",  required_argument, 0, 'o'},
               {"cmdline",    optional_argument, 0, 'c'},
               {"board",    optional_argument, 0, 'b'},
               {"second",    optional_argument, 0, 's'},
               {"header",    optional_argument, 0, 'h'},
               {0, 0, 0, 0}
             };


typedef struct {
	char *image_filename ;
	char *kernel_name;
	char *ramdisk_name;
	char *ramdisk_directory_name;
	char *ramdisk_cpio_name;
	char *ramdisk_archive_name;
	char *page_size_filename;
	char *logfile;
	char *cmdline;
	char *second;
	char *board;
	char *header;
	char *output;
	char *filename;
	int filename_length;
	char *target;
	int target_length;
	char *source;
	int source_length;
	char *log_filename;
	int page_size;
	unsigned base;	
    unsigned kernel_offset;
    unsigned kernel_page_count;
    unsigned ramdisk_offset ;
    unsigned second_offset;  
    unsigned second_page_count ;
    unsigned tags_offset;
} optionvalues_t ;
optionvalues_t option_values;

#define DEFAULT_PAGE_SIZE 2048
#define DEFAULT_PAGE_SIZE_NAME "pagesize"
#define DEFAULT_BASE_ADDRESS 0x10000000
#define DEFAULT_RAMDISK_ADDRESS 0x01000000+DEFAULT_BASE_ADDRESS
#define DEFAULT_KERNEL_ADDRESS 0x00008000+DEFAULT_BASE_ADDRESS
#define DEFAULT_SECOND_ADDRESS 0x00f00000+DEFAULT_BASE_ADDRESS
#define DEFAULT_TAGS_ADDRESS 0x00000100+DEFAULT_BASE_ADDRESS
#define DEFAULT_RAMDISK_NAME "ramdisk"
#define DEFAULT_RAMDISK_NAME_LENGTH 7
#define DEFAULT_RAMDISK_DIRECTORY_NAME "root"
#define DEFAULT_RAMDISK_DIRECTORY_NAME_LENGTH 4
#define DEFAULT_RAMDISK_CPIO_NAME "initramfs.cpio"
#define DEFAULT_RAMDISK_CPIO_GZIP_NAME "initramfs.cpio.gz"
#define DEFAULT_RAMDISK_CPIO_LZOP_NAME "initramfs.cpio.lzo"
#define DEFAULT_KERNEL_NAME "kernel"
#define DEFAULT_KERNEL_NAME_LENGTH 6
#define DEFAULT_SECOND_NAME "second"
#define DEFAULT_LOG
#define OPTIONS_ACTION_UNPACK "i:rl:xfpkcbsdho:a"
#define OPTIONS_ACTION_PACK  "kp:dr:c:s:i:"
#define OPTIONS_ACTION_LIST  "i:"
#define OPTIONS_ACTION_EXTRACT "i:t:s:"
#define OPTIONS_ACTION_REMOVE "rfiv"
#define OPTIONS_ACTION_ADD "rfiv"
#define OPTIONS_ACTION_UPDATE "rfiv"

enum bitwise_parameters { 
	IMAGE = 0x1, 
	KERNEL = 0x2,
	RAMDISK_ARCHIVE=0x4,
	RAMDISK_CPIO =0x8,
	RAMDISK=0x10,
	RAMDISK_DIRECTORY = 0x20 ,
	SECOND=0x40,
	CMDLINE=0x80,
	CMDLINE_STRING=0x100,		
	BOARD=0x200,
	HEADER=0x400,
	PAGESIZE=0x800,
	SOURCE=0x1000,
	TARGET=0x2000,
	LOGSTDOUT=0x4000,
	NOLOGFILE=0x8000,
	FILENAME=0x10000,
	PAGESIZE_STRING=0x20000,
	RAMDISK_FULL = RAMDISK_DIRECTORY | RAMDISK_CPIO | RAMDISK_ARCHIVE,
	ALL=KERNEL | RAMDISK_DIRECTORY |  HEADER,
	OUTPUT=0x1000000
}  params ;


#define HAS_ALL					(params & ALL)
#define HAS_NOLOGFILE			(params & NOLOGFILE)
#define HAS_LOGSTDOUT			(params & LOGSTDOUT)
#define HAS_TARGET 				(params & TARGET)
#define HAS_SOURCE 				(params & SOURCE)
#define HAS_FILENAME 			(params & FILENAME)
#define HAS_SECOND	 			(params & SECOND)
#define HAS_PAGESIZE 			(params & PAGESIZE)
#define HAS_PAGESIZE_STRING		(params & PAGESIZE_STRING)
#define HAS_CMDLINE	 			(params & CMDLINE)
#define HAS_CMDLINE_STRING		(params & CMDLINE_STRING)
#define HAS_BOARD		 		(params & BOARD)
#define HAS_HEADER	 			(params & HEADER)
#define HAS_IMAGE 				(params & IMAGE)
#define HAS_OUTPUT 				(params & OUTPUT)
#define HAS_KERNEL 				(params & KERNEL)
#define HAS_RAMDISK 			(params & RAMDISK)
#define HAS_RAMDISK_ARCHIVE		(params & RAMDISK_ARCHIVE)
#define HAS_RAMDISK_DIRECTORY 	(params & RAMDISK_DIRECTORY)
#define HAS_RAMDISK_CPIO 		(params & RAMDISK_CPIO)
#define HAS_RAMDISK_FULL 		(params & RAMDISK_FULL)

#define SET_ALL					(params | ALL)
#define SET_NOLOGFILE			(params | NOLOGFILE)
#define SET_LOGSTDOUT			(params | LOGSTDOUT)
#define SET_TARGET 				(params | TARGET)
#define SET_SOURCE 				(params | SOURCE)
#define SET_FILENAME 			(params | FILENAME)
#define SET_SECOND	 			(params | SECOND)
#define SET_PAGESIZE 			(params | PAGESIZE)
#define SET_PAGESIZE_STRING		(params | PAGESIZE_STRING)
#define SET_CMDLINE	 			(params | CMDLINE)
#define SET_CMDLINE_STRING		(params | CMDLINE_STRING)
#define SET_BOARD		 		(params | BOARD)
#define SET_HEADER	 			(params | HEADER)
#define SET_IMAGE 				(params | IMAGE)
#define SET_OUTPUT 				(params | OUTPUT)
#define SET_KERNEL 				(params | KERNEL)
#define SET_RAMDISK 			(params | RAMDISK)
#define SET_RAMDISK_ARCHIVE		(params | RAMDISK_ARCHIVE)
#define SET_RAMDISK_DIRECTORY 	(params | RAMDISK_DIRECTORY)
#define SET_RAMDISK_CPIO 		(params | RAMDISK_CPIO)
#define SET_RAMDISK_FULL 		(params | RAMDISK_FULL)


int log_write(const char *format, ...);
typedef enum  _program_actions_t {  
	NOT_SET=0,	UNPACK=1 , PACK=2 , LIST=3 , EXTRACT=4,ADD=5 , REMOVE=6 , UPDATE=7 } program_actions_t ;

typedef struct _program_options_t  {
	const char*	stringopts;
	const struct option* options;
	program_actions_t action ;
	int (*function_name_p)();
} program_options_t; 
		
static program_options_t program_options[] ={
		{ NULL,NULL,NOT_SET,NULL},
		{OPTIONS_ACTION_UNPACK,unpack_long_options,UNPACK ,unpack_boot_image_file},
		{OPTIONS_ACTION_PACK,pack_long_options,PACK ,pack_boot_image_file},		 
			{OPTIONS_ACTION_LIST,list_long_options,LIST ,list_boot_image_info},
					{OPTIONS_ACTION_EXTRACT,extract_long_options,EXTRACT,extract_boot_image_file },
					{OPTIONS_ACTION_ADD,NULL,ADD ,NULL},
					{OPTIONS_ACTION_REMOVE,NULL,REMOVE ,NULL},
					{OPTIONS_ACTION_UPDATE,NULL,UPDATE ,NULL}
			};
program_options_t program_option;
	
#define ACTION_UNPACK (program_option.action==UNPACK )
#define ACTION_EXTRACT (program_option.action==EXTRACT )
#define ACTION_PACK (program_option.action==PACK )
#define ACTION_LIST (program_option.action==LIST )
#define ACTION_ADD (program_option.action==ADD )
#define ACTION_REMOVE (program_option.action==REMOVE )
#define ACTION_UPDATE (program_option.action==UPDATE )
	
#define GET_OPT_LONG_FUNCTION getopt_long(argc, argv,program_option.stringopts, program_option.options, &option_index);
#define MEMORY_BUFFER_SIZE 8192*1024

#endif



