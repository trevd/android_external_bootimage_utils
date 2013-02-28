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

typedef enum _file_info_enum { FILE_NO=0 , FILE_YES=1,FILE_ALL=2, FILE_NOT_FOUND=3,FILE_INVALID_TYPE } file_info_enum ;
typedef unsigned char** byte_pp ;
typedef unsigned char* byte_p ;
typedef unsigned char byte ;
typedef unsigned long offset_t ;
static byte magic_cpio_ascii[6] = { 0x30,0x37,0x30,0x37,0x30,0x31 };
static byte magic_linux_zimage[4]={ 0x18,0x28,0x6F,0x01 }; 
static int magic_linux_zimage_offset=0x24; 
int is_path_directory(char *dname);
file_info_enum check_directory_exists(char *dname);
file_info_enum check_file_exists(char *fname);
int check_magic(const char *filepath);
char *remove_file_extension(char* filename);
file_info_enum is_android_boot_image_file(const char *filepath);
//file_info_enum is_sony_boot_image_file(const char *filepath);
//file_info_enum is_samsung_boot_zimage_file(const char *filepath);
file_info_enum is_linux_kernal_zImage_file(const char *filepath);
//file_info_enum is_lzop_file(const char *filepath);
file_info_enum is_cpio_file(const char *filepath);
file_info_enum is_gzip_file(const char *filepath);
file_info_enum is_ascii_text(const byte_p stream, const size_t size);
file_info_enum is_windows_text(const byte_p stream, const size_t size);
file_info_enum confirm_file_replace(const char *source_filename,const char *target_filename);
file_info_enum find_in_file(const char *filepath,byte_pp magic	);
int write_single_line_to_file(const char *filepath, const char *output_buffer,unsigned size);
int write_to_file_mode(unsigned char *data_in, size_t output_size,char * output_filename, mode_t mode);
int write_to_file(unsigned char *data_in, size_t output_size,char * output_filename);
void mkdir_and_parents(const char *path,mode_t mode);

byte_p find_in_memory(const byte_p haystack, size_t haystack_len, const void *needle,  size_t needle_len);
byte_p find_string_in_memory(const byte_p haystack, size_t haystack_len, const char * needle);
byte_p load_file(const char *fn, size_t *file_size);
byte_p load_file_from_offset(const char *filepath,off_t offset,size_t *file_size);


// Line Ending Conversion Functions
size_t unix_to_dos(byte_p output_buffer, const byte_p input_buffer);
size_t dos_to_unix(byte_p output_buffer, const byte_p input_buffer);

#endif
