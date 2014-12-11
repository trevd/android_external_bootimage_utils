#ifndef _b4cab7f2_80b0_11e4_a8e8_5404a601fa9d
#define _b4cab7f2_80b0_11e4_a8e8_5404a601fa9d
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
__LIBBOOTIMAGE_PRIVATE_API__  DIR* mkdir_and_parents_umask(const char *path,unsigned mode, mode_t umask);
__LIBBOOTIMAGE_PRIVATE_API__  DIR* mkdir_and_parents(const char *path,unsigned mode);
#endif
