#ifndef _a7808ef4_8179_11e4_991e_5404a601fa9d
#define _a7808ef4_8179_11e4_991e_5404a601fa9d
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <archive.h>
#include <archive_entry.h>
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all_memory( char* data , uint64_t data_size, DIR* target);
__LIBBOOTIMAGE_PRIVATE_API__  int archive_extract_all(struct archive *a,DIR* target);
#endif
