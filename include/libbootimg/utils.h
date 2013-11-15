/*
 * utils.h
 * 
 * Copyright 2013 Trevor Drake <trevd1234@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#ifndef _cfe5afee_9069_11e2_93f2_5404a601fa9d
#define _cfe5afee_9069_11e2_93f2_5404a601fa9d

#define WINDOWS_EOL "\r\n"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__) 
    #include <utils_windows.h>
#else
    #include <utils_linux.h>
#endif

#ifndef BOOT_IMAGE_SIZE_MAX
#define BOOT_IMAGE_SIZE_MAX (8192*1024)*4
#endif


#define FILE_TYPE_UNKNOWN 0
#define FILE_TYPE_BINARY 1
#define FILE_TYPE_TEXT_UNIX 2
#define FILE_TYPE_TEXT_WINDOWS 3

extern int utils_debug ;
    
#include <bitapi_error.h>
static int internal_errno = 0 ; 
#define __FILENAME__  (strstr(__FILE__, "libbootimg/") ? strstr(__FILE__, "libbootimg/") : strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define D(  ...) \
        if (utils_debug){ \
            /* save our errno because we don't want debug output to fuck */ \
            internal_errno = errno ; \
            fprintf(stderr, "DBG: %s:%s():",__FILENAME__ , __FUNCTION__); \
            fprintf(stderr,  __VA_ARGS__ ); \
            errno = internal_errno ; \
        }
        
#define DC( ac, av ) \
        if (utils_debug){ \
            fprintf(stderr, "DEBUG: %s::%s():Array Count:%d\n", __FILE__, __FUNCTION__,ac,av); \
            int c = 0 ; \
            for(c = 0 ; c < ac ; c++){ \
                fprintf(stderr, "DEBUG: %s::%s(): %0d %p - %s\n", __FILE__, __FUNCTION__, c ,av[c],av[c]); \
            } \
        }
        
/* NAME
    find_in_memory - locate a substring

 SYNOPSIS
      
    #include <utils.h>

    unsigned char * find_in_memory(unsigned char *haystack, size_t haystacklen,
                    char *needle, size_t needlelen);

 DESCRIPTION
    The find_in_memory() function finds the start of the first occurrence of the substring needle of length needlelen in the memory area haystack of length haystacklen.

 RETURN VALUE
    The find_in_memory() function returns a pointer to the beginning of the substring, or NULL if the substring is not found.
       
    If an error occurs NULL is returned and errno is set appropriately
       
    */
    unsigned char *find_in_memory(unsigned char *haystack, unsigned haystack_len, char* needle, unsigned needle_len); 
    unsigned char *find_in_memory_start_at(unsigned char *haystack, unsigned haystack_len,unsigned char *haystack_offset, char* needle, unsigned needle_len);
    unsigned long  get_long_from_hex_field(char * header_field_value);
    int mkdir_and_parents(const char *path,unsigned mode);
    unsigned strlcmp(const  char *s1, const  char *s2);
    unsigned strulcmp(const unsigned char *s1, const unsigned char *s2);
    unsigned long write_item_to_disk_extended(unsigned char *data,unsigned data_size,unsigned mode,char* name,unsigned name_size);
    unsigned long write_item_to_disk(unsigned char *data,unsigned data_size,unsigned mode,char* name);
    unsigned char* read_item_from_disk(const char *name, unsigned* data_size);
    unsigned char* read_from_block_device(const char *name, unsigned* data_size);
    char* get_md5_sum(unsigned char* data ,unsigned size) ;
    int is_md5_match(unsigned char* data_a ,unsigned size_a,unsigned char* data_b ,unsigned size_b);
    int get_exe_path(char* buffer,size_t buffer_size);
    void convert_uinx_to_out(const char* in,char* out ) ;
    void convert_dos_to_unix(const char* in,char* out) ;
    unsigned get_stream_type(char *stream);
#endif
