
#include "bootimg.h"
#include "file.h"
#include <stdarg.h>
#include "program.h"
int create_bootfs(char* root_path,char* filename);
int extract_bootfs(char* filename);
void file_compress(char  *file, char  *mode);
char * file_uncompress(char*ramdisk_fn);


#define ARRAYSIZE(a) ((int)(sizeof(a) / sizeof(*(a))))

typedef struct _boot_block
{
	size_t content_size;
	byte_p content_data;
	size_t padding_size;
	byte_p padding_data;
	
} boot_block ;

int usage(void);
static void die(const char *why, ...);
static int DEBUG_ON = 0;
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
	void * kernel_data;
	void * ramdisk_data;
	void * second_stage_data;
	
} boot_image_t ;


char *strrev(char *str);
int print_boot_image_info(boot_image_t ,FILE*);
void *memmem1(const void *haystack, size_t haystack_len,
			  const void *needle,  size_t needle_len);