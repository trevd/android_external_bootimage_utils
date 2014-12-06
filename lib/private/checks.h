#ifndef _a4d5d442_7d15_11e4_9366_5404a601fa9d
#define _a4d5d442_7d15_11e4_9366_5404a601fa9d
#include "api.h"
__LIBBOOTIMAGE_HIDDEN__ inline int check_bootimage_structure(struct bootimage* bi);
__LIBBOOTIMAGE_HIDDEN__ inline int check_bootimage_file_stat_size(struct bootimage* bi ,const char* file_name);
__LIBBOOTIMAGE_HIDDEN__ inline int check_bootimage_file_name(const char* file_name);
__LIBBOOTIMAGE_HIDDEN__ inline static int check_ramdisk_entryname(const char* entry_name);
__LIBBOOTIMAGE_HIDDEN__ inline static int check_bootimage_ramdisk(struct bootimage* bi);
#endif
