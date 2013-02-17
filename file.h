#ifndef _BOOTIMAGE_FILE_H_
#define _BOOTIMAGE_FILE_H_
#include <stdarg.h>
#include <limits.h>
#define CHECK_FAIL_EXIT 1
#define CHECK_FAIL_OK 0
#define WINDOWS_EOL "\r\n"
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__) 
#include <windows.h>
#define mkdir(fn,mode) _mkdir(fn);
#define S_IFLNK	 0120000
#define S_IFSOCK 0140000
#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_IWGRP 00020
#define CONVERT_LINE_ENDINGS 1==1
#define EOL WINDOWS_EOL
int symlink(char *symlink_src,char *filename);
ssize_t readlink(const char *path, char *buf, size_t bufsiz);
#else
#define EOL "\n"
#define CONVERT_LINE_ENDINGS 0==1
#endif

int vasprintf(char **strp, const char *fmt, va_list ap);

typedef enum _file_info_enum { FILE_NO=0 , FILE_YES=1,FILE_ALL=2 } file_info_enum ;
typedef unsigned char** byte_pp ;
typedef unsigned char* byte_p ;
typedef unsigned char byte ;
int is_path_directory(char *dname);
int check_directory_exists(char *dname, int exitonfailure);
int check_file_exists(char *fname, int exitonfailure);
int check_magic(const char *filepath);
char *remove_file_extension(char* filename);
byte_p load_file_to_size(const char *filepath, unsigned size );
file_info_enum is_android_boot_image_file(const char *filepath);
//file_info_enum is_sony_boot_image_file(const char *filepath);
//file_info_enum is_samsung_boot_zimage_file(const char *filepath);
//file_info_enum is_linux_kernal_zimage_file(const char *filepath);
//file_info_enum is_lzop_file(const char *filepath);
file_info_enum is_cpio_file(const char *filepath);
file_info_enum is_gzip_file(const char *filepath);
file_info_enum is_ascii_text(const byte_p stream, const size_t size);
file_info_enum is_windows_text(const byte_p stream, const size_t size);
file_info_enum confirm_file_replace(const char *source_filename,const char *target_filename);
file_info_enum find_in_file(const char *filepath,byte_pp magic	);
int write_single_line_to_file(const char *filepath, const char *output_buffer,unsigned size);
int read_file_to_size(const char *filepath, unsigned size , unsigned char *output_buffer);
int write_to_file_mode(unsigned char *data_in, size_t output_size,char * output_filename, mode_t mode);
int write_to_file(unsigned char *data_in, size_t output_size,char * output_filename);
byte_p load_file_from_offset(const char *filepath,int offset,size_t *file_size);
long read_file( const char *fn, unsigned char *output,unsigned long *output_size);
byte_p load_file(const char *fn, size_t *file_size);
byte_p find_in_memory(const byte_p haystack, size_t haystack_len, const void *needle,  size_t needle_len);
size_t unix_to_dos(byte_p output_buffer, const byte_p input_buffer);
size_t dos_to_unix(byte_p output_buffer, const byte_p input_buffer);

#endif
