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

// Debug Functionality. use the init_debug() function to turn on debug messages

extern int   utils_debug;

void init_debug() ;

#define D(  ...) \
        if (utils_debug){ \
            fprintf(stderr, "DEBUG: %s::%s():", __FILE__, __FUNCTION__); \
            fprintf(stderr,  __VA_ARGS__ ); \
        }

    unsigned char *find_in_memory(unsigned char *haystack, unsigned haystack_len, char* needle, unsigned needle_len); 
    unsigned char *find_in_memory_start_at(unsigned char *haystack, unsigned haystack_len,unsigned char *haystack_offset, char* needle, unsigned needle_len);
    unsigned long  get_long_from_hex_field(char * header_field_value);
    int mkdir_and_parents(const char *path,unsigned mode);
    unsigned strlcmp(const  char *s1, const  char *s2);
    unsigned strulcmp(const unsigned char *s1, const unsigned char *s2);
    int symlink_os(const char *source, size_t size,const char *path);
    int readlink_os(const char *path, char *buf, size_t bufsiz);
    unsigned long write_item_to_disk_extended(unsigned char *data,unsigned data_size,unsigned mode,char* name,unsigned name_size);
    unsigned long write_item_to_disk(unsigned char *data,unsigned data_size,unsigned mode,char* name);
    unsigned char* read_item_from_disk(const char *name, unsigned* data_size);
    unsigned char* read_from_block_device(const char *name, unsigned* data_size);
    char* get_md5_sum(unsigned char* data ,unsigned size) ;
    int is_md5_match(unsigned char* data_a ,unsigned size_a,unsigned char* data_b ,unsigned size_b);
#endif
