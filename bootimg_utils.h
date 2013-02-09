
#include "bootimg.h"
#include <stdarg.h>
	
int create_bootfs(char* root_path,char* filename);
int extract_bootfs(char* filename);
void file_compress(char  *file, char  *mode);
char * file_uncompress(char*ramdisk_fn);
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__) 
	#include <windows.h>
#define lstat stat

 
#define DEFAULT_FILE_EXT ".txt"
#define DEFAULT_FILE_EXT_LEN 5
#else
#define DEFAULT_FILE_EXT ""
#define DEFAULT_FILE_EXT_LEN 0
#endif


#define ARRAYSIZE(a) ((int)(sizeof(a) / sizeof(*(a))))



int usage(void);
static void die(const char *why, ...);
static int DEBUG_ON = 0;
typedef struct boot_image
{
	boot_img_hdr header;
	unsigned boot_image_filesize;
	unsigned kernel_page_count;
	unsigned kernel_offset;
	unsigned ramdisk_page_count;
	unsigned ramdisk_offset;
	unsigned second_page_count;
	unsigned second_offset;
	void * kernel_data;
	void * ramdisk_data;
	void * second_stage_data;
	
} boot_image_t ;


char *strrev(char *str);

int print_boot_image_info(boot_image_t boot_image);

void *memmem1(const void *haystack, size_t haystack_len,
			  const void *needle,  size_t needle_len);
