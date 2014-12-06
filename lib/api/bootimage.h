#ifndef _40589ef6_7d28_11e4_9dc9_5404a601fa9d
#define _40589ef6_7d28_11e4_9dc9_5404a601fa9d

struct bootimage ;

#define __LIBBOOTIMAGE_PUBLIC_API__  __attribute__((visibility("default"))) 

__LIBBOOTIMAGE_PUBLIC_API__ struct bootimage* bootimage_read_initialize();
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_read_free(struct bootimage** bi);

#endif
