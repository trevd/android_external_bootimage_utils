#ifndef _3de6f27e_7794_11e4_83a8_5404a601fa9d
#define _3de6f27e_7794_11e4_83a8_5404a601fa9d

#define EBINULL 10000
#define EBIFSIZE 10001
#define EBIFNAME 10002
#define EBIFACCESS 10003
#define EBINOMAGIC 10004
#define EBIBADPADHEAD 10005
#define EBIBADPADKERNEL 10006
#define EBIBADPADRAMDISK 10007
#define EBIBADPADSECOND 10008
#define EBISTAT 10009
#define EBIRDENTRY 10010
#define EBIRDENTRYLENGTH 10011
#define EBIRDMEM 10012
#define EBIHEADMEM 10013
#define EBIRDMEMSIZE 10014

#define CPIO_FILE_NAME_MAX 1024




/* 
	bootimage_read_filename(struct bootimage* bi,const char* name);
	struct bootimage* bi - pointer to a bootimage structure
	const char* name - pointer to null terminated string containing
					   a filename of the boot image file to open

	Returns : Zero - On success. -1 On Failure with errno set.
 
 */
__LIBBOOTIMAGE_PUBLIC_API__ int bootimage_read_filename(struct bootimage* bi,const char* file_name);

int bootimage_read_ramdisk(struct bootimage* bi);
int bootimage_read_ramdisk_entry(struct bootimage* bi);
int bootimage_read_kernel(struct bootimage* bi);

int bootimage_extract_ramdisk_entry(struct bootimage* bi,const char* entry_name);
__LIBBOOTIMAGE_PUBLIC_API__ int bootimage_extract_ramdisk_entry_from_file(struct bootimage* bi,const char* file_name,const char* entry_name);

#endif
