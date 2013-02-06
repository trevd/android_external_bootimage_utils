#include <limits.h>
#define CHECK_FAIL_EXIT 1
#define CHECK_FAIL_OK 0
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__) 
#include <windows.h>
#define mkdir(fn,mode) _mkdir(fn);
#define S_IFLNK	 0120000
#define S_IFSOCK 0140000
#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_IWGRP 00020
#define CONVERT_LINE_ENDINGS 1=1
int symlink(char *symlink_src,char *filename){ return 0; }
ssize_t readlink(const char *path, char *buf, size_t bufsiz) { return 0; }
#else
#define CONVERT_LINE_ENDINGS 0=1
#endif

#define MAGIC_GZIP 0x08088B1F
#define MAGIC_GZIP_NONAME 0x00088B1F
#define MAGIC_CPIO_ASCII "070701"
#define MAGIC_CPIO_SIZE 7

static const mode_t defualt_file_mode = 0666 ;
static const mode_t defualt_directory_mode = 0755 ;
int is_path_directory(char *dname);
int check_directory_exists(char *dname, int exitonfailure);
int check_file_exists(char *fname, int exitonfailure);
int check_magic(const char *filepath);
char *remove_file_extension(char* filename);
int is_cpio_file(const char *filepath);
int is_gzip_file(const char *filepath);
int write_single_line_to_file(const char *filepath, const char *output_buffer,unsigned size);
int read_file_to_size(const char *filepath, unsigned size , unsigned char *output_buffer);
int write_to_file_mode(unsigned char *data_in, unsigned output_size,char * output_filename, mode_t mode);
int write_to_file(unsigned char *data_in, unsigned output_size,char * output_filename);
unsigned char *load_file_from_offset(const char *filepath,int offset,unsigned long *file_size);
long read_file( const char *fn, unsigned char *output,unsigned long *output_size);
unsigned char *load_file(const char *fn, unsigned long *file_size);
