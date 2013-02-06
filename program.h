#include "getopt.h"
static struct option pack_long_options[] =
             {
               /* These options don't set a flag.
                  We distinguish them by their indices. */
               {"boot-immage",  required_argument,0, 'i'},                              
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
			
               {"boot-immage",  required_argument,0, 'i'},
               {"all",			no_argument,0,'a'},
               {"ramdisk",  optional_argument,0, 'r'},
               {"ramdisk-cpio",  no_argument,0, 'p'},
               {"ramdisk-archive",  no_argument,0, 'x'},
               {"ramdisk-directory",  no_argument,0, 'd'},
               {"kernel",  optional_argument, 0, 'k'},
               {"output-dir",  required_argument, 0, 'o'},
               {"cmdline",    optional_argument, 0, 'c'},
               {"board",    optional_argument, 0, 'b'},
               {"second",    optional_argument, 0, 's'},
               {"header",    optional_argument, 0, 'h'},
               {0, 0, 0, 0}
             };
             
static struct option extract_long_options[] = {
               {"input",  required_argument,0, 'i'},
               {"filename",  required_argument,0, 'f'},
               {"output-dir",  required_argument, 0, 'o'},
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
	char *ramdisk;
	char *cmdline;
	char *second;
	char *board;
	char *header;
	char *output;
	char *filename;
	int filename_length;
	int page_size;
	unsigned long base_address;
}  optionvalues ;
optionvalues option_values;

#define DEFAULT_RAMDISK_NAME "ramdisk"
#define DEFAULT_KERNEL_NAME "zImage"
#define DEFAULT_HEADER_NAME "info"


enum parameters { 
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
	RAMDISK_FULL = RAMDISK_DIRECTORY | RAMDISK_CPIO | RAMDISK_ARCHIVE,
	ALL=KERNEL | RAMDISK_FULL | CMDLINE | SECOND | BOARD | HEADER,
	OUTPUT=0x10000
} params;

#define HAS_FILENAME 			(params & FILENAME)
#define HAS_SECOND	 			(params & SECOND)
#define HAS_PAGESIZE 			(params & PAGESIZE)
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


#define MEMORY_BUFFER_SIZE 8192*1024




