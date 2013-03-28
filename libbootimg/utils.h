#ifndef _cfe5afee_9069_11e2_93f2_5404a601fa9d
#define _cfe5afee_9069_11e2_93f2_5404a601fa9d

#define WINDOWS_EOL "\r\n"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__) 
    #include <utils_windows.h>
#else
    #include <utils_linux.h>
#endif

    unsigned char *find_in_memory(unsigned char *haystack, unsigned haystack_len, char* needle, unsigned needle_len); 
    unsigned char *find_in_memory_start_at(unsigned char *haystack, unsigned haystack_len,unsigned char *haystack_offset, char* needle, unsigned needle_len);
    unsigned long  get_long_from_hex_field(char * header_field_value);
    void mkdir_and_parents(const unsigned char *path,unsigned mode);
    unsigned strlcmp(const  char *s1, const  char *s2);
    unsigned strulcmp(const unsigned char *s1, const unsigned char *s2);
    int symlink_os(const unsigned char *source, unsigned size,const char *path);
    unsigned long write_item_to_disk_extended(char *data,unsigned data_size,unsigned mode,char* name,unsigned name_size);
    unsigned long write_item_to_disk(char *data,unsigned data_size,unsigned mode,char* name);
#endif
