#ifndef _a4d5d442_7d15_11e4_9366_5404a601fa9d
#define _a4d5d442_7d15_11e4_9366_5404a601fa9d
#include <private/api.h>
__LIBBOOTIMAGE_PRIVATE_API__ int check_output_name(const char* name);
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_structure(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_header(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_file_stat_size(struct bootimage* bi ,const char* file_name);
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_file_name(const char* file_name);
__LIBBOOTIMAGE_PRIVATE_API__ int check_ramdisk_entryname(const char* entry_name);
__LIBBOOTIMAGE_PRIVATE_API__ int check_bootimage_ramdisk(struct bootimage* bi);

#define CPIO_FILE_NAME_MAX 1024
#endif
