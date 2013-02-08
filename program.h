#include "getopt.h"
#include <limits.h>	
static struct option pack_global_options[] = {
	{"help", optional_argument,0, NULL},   
	{"log",  optional_argument,0, NULL},  
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
				{"ramdisk-offset", required_argument,0,'d' },
				{"second", required_argument,0,'s' },
				{"output-image", required_argument,0,'o' },
               {0, 0, 0, 0}
             };	

static struct option unpack_long_options[] = {
			
               {"boot-image",  required_argument,0, 'i'},
               {"all",			no_argument,0,'a'},
               {"ramdisk-name",  optional_argument,0, 'r'},
               {"ramdisk-cpio",  no_argument,0, NULL},
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


typedef struct  {
	char *image ;
	char *kernel;
	char *ramdisk_name;
	char *ramdisk_directory;
	char *ramdisk_cpio;
	char *ramdisk_archive;
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
	int page_size;
	unsigned base;
    unsigned kernel_offset;
    unsigned kernel_page_count;
    unsigned ramdisk_offset ;
    unsigned ramdisk_page_count ;
    unsigned second_offset;  
    unsigned second_page_count ;
    unsigned tags_offset;
}  optionvalues ;
optionvalues option_values;

#define DEFAULT_PAGE_SIZE 2048
#define DEFAULT_BASE_ADDRESS 0x10000000
#define DEFAULT_KERNEL_OFFSET 0x00008000
#define DEFAULT_SECOND_OFFSET 0x00f00000
#define DEFAULT_TAGS_OFFSET 0x00000100
#define DEFAULT_RAMDISK_DIRECTORY_NAME "ramdisk"
#define DEFAULT_RAMDISK_DIRECTORY_NAME_LENGTH 7
#define DEFAULT_RAMDISK_CPIO_NAME "ramdisk.cpio"
#define DEFAULT_KERNEL_NAME "kernel"
#define DEFAULT_KERNEL_NAME_LENGTH 6
#define DEFAULT_HEADER_NAME "header"
#define DEFAULT_HEADER_NAME_LENGTH 6
#define DEFAULT_CMDLINE_NAME "cmdline"
#define DEFAULT_PAGESIZE_NAME "pagesize"
#define DEFAULT_SECOND_NAME "second"

enum  parameters { 
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
	FILENAME=0x800,
	SOURCE=0x1000,
	TARGET=0x2000,
	LOGSTDOUT=0x4000,
	NOLOGFILE=0x8000,
	RAMDISK_FULL = RAMDISK_DIRECTORY | RAMDISK_CPIO | RAMDISK_ARCHIVE,
	ALL=KERNEL | RAMDISK_FULL |  HEADER,
	OUTPUT=0x100000000
}  params ;




#define HAS_NOLOGFILE			(params & NOLOGFILE)
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

#define SET_NOLOGFILE			(params | NOLOGFILE)
#define SET_LOGSTDOUT			(params | LOGSTDOUT)
#define SET_TARGET 				(params | TARGET)
#define SET_SOURCE 				(params | SOURCE)
#define SET_FILENAME 			(params | FILENAME)
#define SET_SECOND	 			(params | SECOND)
#define SET_PAGESIZE 			(params | PAGESIZE)
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


#define MEMORY_BUFFER_SIZE 8192*1024
typedef unsigned char* byte_p ;




