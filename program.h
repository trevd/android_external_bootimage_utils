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
int strlcmp(const char *s1, const char *s2);
#define ARRAYSIZE(a) ((int)(sizeof(a) / sizeof(*(a))))

typedef struct _boot_image
{
	boot_img_hdr header; 
	unsigned magic_offset;
	size_t boot_image_filesize;
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

typedef enum  _program_actions_enum {  NOT_SET,	UNPACK , PACK , LIST , EXTRACT,ADD , REMOVE , UPDATE } program_actions_emum ;

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
#define DEFAULT_OUTPUT_DIRECTORY_NAME "."
#define DEFAULT_LOG
#define OPTIONS_ACTION_UNPACK "i:r:l:C:x:f:p:k:c:n:s:d:h:o:a"
#define OPTIONS_ACTION_PACK  "kp:dr:c:s:i:"
#define OPTIONS_ACTION_LIST  "i:"
#define OPTIONS_ACTION_EXTRACT "i:t:s:"
#define OPTIONS_ACTION_REMOVE "i:t:"
#define OPTIONS_ACTION_ADD "i:t:s:"
#define OPTIONS_ACTION_UPDATE "i:t:s:k:"

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
	program_actions_emum action;
} optionvalues_t ;
optionvalues_t option_values;

// Specified the switches that are allow for each action type
// Most Switches have the same meaning between action types
typedef enum  _option_type_enum {  NO_ARG=1, REQ_INT_ARG, REQ_STR_ARG, OPT_INT_ARG, OPT_STR_ARG, DEF_INT_ARG, DEF_STR_ARG
									 } option_type_enum ;
typedef struct _opt {
		option_type_enum option_type;
		const char* long_name;
		const char* short_char;
		const char* default_string;
		int default_value;
		void * dest_ptr;
				
} program_option_t ;

static program_option_t unpack_options[]={ 
	 { REQ_STR_ARG, "boot-image","i",NULL,0,&option_values.image_filename},
	 { DEF_STR_ARG, "ramdisk-cpio","C",DEFAULT_RAMDISK_CPIO_NAME,0,&option_values.ramdisk_cpio_filename},
	 { DEF_STR_ARG, "ramdisk-archive","x",DEFAULT_RAMDISK_CPIO_GZIP_NAME,0,&option_values.ramdisk_archive_filename},
	 { DEF_STR_ARG, "ramdisk-directory","d",DEFAULT_RAMDISK_DIRECTORY_NAME,0,&option_values.ramdisk_directory_name},
	 { DEF_STR_ARG, "kernel","k",DEFAULT_KERNEL_NAME,0,&option_values.kernel_filename},
	 { DEF_STR_ARG, "cmdline","c",DEFAULT_CMDLINE_NAME,0,&option_values.cmdline_filename},
	 { DEF_STR_ARG, "name","n",DEFAULT_BOARD_NAME,0,&option_values.board_filename},
	 { DEF_STR_ARG, "header","h",DEFAULT_HEADER_NAME,0,&option_values.header_filename},
	 { DEF_STR_ARG, "second","s",DEFAULT_SECOND_NAME,0,&option_values.second_filename},
	 { DEF_STR_ARG, "pagesize","p",DEFAULT_PAGESIZE_NAME,0,&option_values.page_size_filename},
	 { DEF_STR_ARG, "output-directory","o",DEFAULT_OUTPUT_DIRECTORY_NAME,0,&option_values.output_directory_name},
	 {0, 0, 0, 0, 0,0}
};


static struct option pack_global_options[] = {
	{"help", optional_argument,0,(int)NULL,(const char*)NULL},   
	{"log",  optional_argument,0,(int)NULL,(char*)NULL},  
	 {0, 0, 0, 0, 0}
};

static struct option unpack_long_options[] = {
			
               {"boot-image",  required_argument,0, 'i',(char*)NULL},
               {"all",			no_argument, 0,'a',(char*)NULL},
               {"ramdisk",  no_argument,0, 'r',(char*)NULL},
               {"ramdisk-cpio",  required_argument,0, 'C',DEFAULT_RAMDISK_CPIO_NAME},
               {"ramdisk-archive",  required_argument,0, 'x',DEFAULT_RAMDISK_CPIO_GZIP_NAME},
               {"ramdisk-directory",  required_argument,0, 'd',DEFAULT_RAMDISK_DIRECTORY_NAME},
               {"kernel",  required_argument, 0, 'k',DEFAULT_KERNEL_NAME},
               {"output-dir",  required_argument, 0, 'o',(char*)NULL},
               {"cmdline",    required_argument, 0, 'c',DEFAULT_CMDLINE_NAME},
               {"name",    required_argument, 0, 'n',DEFAULT_BOARD_NAME},
               {"second",    required_argument, 0, 's',DEFAULT_SECOND_NAME},
               {"header",    required_argument, 0, 'h',DEFAULT_HEADER_NAME},
               {"pagesize",  required_argument, 0, 'p',DEFAULT_PAGESIZE_NAME},
               {0, 0, 0, 0, 0}
             };

static struct option pack_long_options[] =
	 {
	   /* These options don't set a flag.
		  We distinguish them by their indices. */
	   {"boot-image",  required_argument,0, 'i',(char*)NULL} ,                        
	  {"ramdisk",  no_argument,0, 'r',(char*)NULL},
               {"ramdisk-cpio",  required_argument,0, 'C',DEFAULT_RAMDISK_CPIO_NAME},
               {"ramdisk-archive",  required_argument,0, 'x',DEFAULT_RAMDISK_CPIO_GZIP_NAME},
               {"ramdisk-directory",  required_argument,0, 'd',DEFAULT_RAMDISK_DIRECTORY_NAME},
               {"kernel",  required_argument, 0, 'k',DEFAULT_KERNEL_NAME},
             {"cmdline",    required_argument, 0, 'c',DEFAULT_CMDLINE_NAME},
               {"name",    required_argument, 0, 'n',DEFAULT_BOARD_NAME},
               {"second",    required_argument, 0, 's',DEFAULT_SECOND_NAME},
               {"header",    required_argument, 0, 'h',DEFAULT_HEADER_NAME},
        {"pagesize",  required_argument, 0, 'p',DEFAULT_PAGESIZE_NAME},
		{"base-address", required_argument,0,'b',NULL },
	   {0, 0, 0, 0, 0}
	 };	
	 

             
static struct option extract_long_options[] = {
               {"boot-image",  required_argument,0, 'i',NULL},
               {"target",  required_argument,0, 't',NULL},
               {"source",  required_argument, 0, 's',NULL},
               {0, 0, 0, 0, 0}
             };
static struct option update_long_options[] = {
               {"boot-image",  required_argument,0, 'i',NULL},
               {"target",  required_argument,0, 't',NULL},
               {"source",  required_argument, 0, 's',NULL},
               {"kernel",  required_argument,0, 'k',NULL},
               {"cmdline",  required_argument,0, 'c',NULL},
               {0, 0, 0, 0, 0}
             };
 static struct option list_long_options[] = {
			
               {"boot-image",  required_argument,0, 'i',NULL},
               {0, 0, 0, 0, 0}
             };




typedef struct _program_options_t  {
	const char*	stringopts;
	const struct option* options;
	program_actions_emum action ;
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

	
#define ACTION_UNPACK (option_values.action==UNPACK )
#define ACTION_EXTRACT (option_values.action==EXTRACT )
#define ACTION_PACK (option_values.action==PACK )
#define ACTION_LIST (option_values.action==LIST )
#define ACTION_ADD (option_values.action==ADD )
#define ACTION_REMOVE (option_values.action==REMOVE )
#define ACTION_UPDATE (option_values.action==UPDATE )
	

#define MEMORY_BUFFER_SIZE (const size_t)8192*1024

#endif



