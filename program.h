#ifndef _PROGAM_H_
#define _PROGAM_H_



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
#include <limits.h>	
#include "getopt.h"
#include <bootimg.h>
#include "file.h"

int unpack_boot_image_file();
int pack_boot_image_file();
int list_boot_image_info();
int extract_boot_image_file();
int update_boot_image_file();
int log_write(const char *format, ...);

#define ARRAYSIZE(a) ((int)(sizeof(a) / sizeof(*(a))))
typedef struct _boot_block
{
	size_t content_size;
	byte_p content_data;
	size_t padding_size;
	byte_p padding_data;
	
} boot_block ;

typedef struct _boot_image
{
	boot_img_hdr header; 
	unsigned magic_offset;
	unsigned boot_image_filesize;
	unsigned kernel_page_count;
	unsigned kernel_offset;
	unsigned ramdisk_page_count;
	unsigned ramdisk_offset;
	unsigned second_page_count;
	unsigned second_offset;
	byte_p header_data_start;
	byte_p kernel_data_start;
	byte_p ramdisk_data_start;
	byte_p second_data_start;
	
} boot_image_t ;



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
	 

             
static struct option extract_long_options[] = {
               {"input",  required_argument,0, 'i'},
               {"target",  required_argument,0, 't'},
               {"source",  required_argument, 0, 's'},
               {0, 0, 0, 0}
             };
static struct option update_long_options[] = {
               {"input",  required_argument,0, 'i'},
               {"target",  required_argument,0, 't'},
               {"source",  required_argument, 0, 's'},
               {"kernel",  optional_argument,0, 'k'},
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
	char *kernel_filename;
	char *ramdisk_name;
	char *ramdisk_directory_name;
	char *ramdisk_cpio_filename;
	char *ramdisk_archive_filename;
	char *page_size_filename;
	char *header_filename;
	char *second_filename;
	char *cmdline_filename;
	char *output_directory_name;
	char *board_filename;
	char *target_filename;
	char *source_filename;
	char *log_filename;
	int page_size;
	int log_stdout;
} optionvalues_t ;
optionvalues_t option_values;

static struct option unpack_long_options[] = {
			
               {"boot-image",  required_argument,0, 'i'},
               {"all",			no_argument, 0,'a'},
               {"ramdisk-name",  optional_argument,0, 'r'},
               {"ramdisk-cpio",  optional_argument,0, (int)NULL},
               {"ramdisk-archive",  optional_argument,0, 'x'},
               {"ramdisk-directory",  optional_argument,0, 'd'},
               {"kernel",  optional_argument, 0, 'k'},
               {"output-dir",  required_argument, 0, 'o'},
               {"cmdline",    optional_argument, 0, 'c'},
               {"name",    optional_argument, 0, 'b'},
               {"second",    optional_argument, 0, 's'},
               {"header",    optional_argument, 0, 'h'},
               {"pagesize",  required_argument, 0, 's'},
               {0, 0, 0, 0}
             };

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
#define OPTIONS_ACTION_UPDATE "i:t:s:k"




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
					{OPTIONS_ACTION_UPDATE,update_long_options,UPDATE ,update_boot_image_file}
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



