#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <archive.h>
#include <archive_entry.h>

#include "api.h"
#include "file.h"
#include "bootimage.h"
#include "internal_api.h"


/* Linux Arm Kernel Magic number */
#define KERNEL_ARM_ZIMAGE_MAGIC "\x18\x28\x6F\x01"
#define KERNEL_ARM_ZIMAGE_MAGIC_SIZE 4 
#define KERNEL_ARM_ZIMAGE_MAGIC_OFFSET 0x24 
#define KERNEL_ARM_ZIMAGE_START_ADDRESS_OFFSET 0x28
#define KERNEL_ARM_ZIMAGE_END_ADDRESS_OFFSET 0x2C


/* Kernel Compression Magics and Magic Sizes */
#define KERNEL_COMPRESSION_MAGIC_LZ4 "\x02\x21"
#define KERNEL_COMPRESSION_MAGIC_LZ4_SIZE 2
#define KERNEL_COMPRESSION_MAGIC_GZIP "\x1F\x8B\x08"
#define KERNEL_COMPRESSION_MAGIC_GZIP_SIZE 3
#define KERNEL_COMPRESSION_MAGIC_LZOP "\x89\x4C\x5A"
#define KERNEL_COMPRESSION_MAGIC_LZOP_SIZE 3
#define KERNEL_COMPRESSION_MAGIC_BZIP2 "\x42\x5A\x68" /* B Z h */
#define KERNEL_COMPRESSION_MAGIC_BZIP2_SIZE 3
#define KERNEL_COMPRESSION_MAGIC_XZ "\xFD\x37\x7A\x58\x5A\x00" /* \xFD 7z X Z \x00 */
#define KERNEL_COMPRESSION_MAGIC_XZ_SIZE 6
#define KERNEL_COMPRESSION_MAGIC_LZMA "\x5D\x00\x00\x00"
#define KERNEL_COMPRESSION_MAGIC_LZMA_SIZE 4

/* We have an Hard Limit of 32MB for storing an uncompressed kernel */
#define UNCOMPRESSED_KERNEL_SIZE_32MB ( (1024 * 1024 ) *32 )

#define KERNEL_VERSION_STRING "Linux version"
#define KERNEL_VERSION_STRING_SIZE 13
#define KERNEL_VERSION_STRING_MAX_SIZE 256


/* 
 * struct bootimage* bootimage_read_initialize() 
 *   
 */ 
__LIBBOOTIMAGE_PUBLIC_API__  struct bootimage* bootimage_read_initialize()
{
	fprintf(stderr,"%s:%s():\n",__FILE__,__FUNCTION__);
	fprintf(stderr,"sizeof(struct bootimage)=%lu [ 0x%x ]\n",sizeof(struct bootimage),sizeof(struct bootimage));
	fprintf(stderr,"sizeof(struct bootimage_header)=%lu [ 0x%x ]\n",sizeof(struct bootimage_header),sizeof(struct bootimage_header));
	/* Allocate and zero memory to store the bootimage struct 
	   This will contain metadata for a loaded bootimage
	 */
	struct bootimage* bi = calloc(1,sizeof(struct bootimage));
	
	return bi ;
		
}


int bootimage_file_read(struct bootimage* bi,const char* file_name)
{
	
	if ( check_bootimage_structure(bi) == -1){
		return -1;
	}
	
	if( check_bootimage_file_name(file_name) == -1 ){
		return -1;
	}
	
	if( check_bootimage_file_stat_size(bi,file_name) == -1 ){
		return -1;
	}
	
		
	
	if( bootimage_read_filename_internal(bi,file_name) == -1 ){
		return -1 ; 
	}
	return 0 ;
	
}



int bootimage_read_kernel(struct bootimage* bi)
{
	/* Validate inputs */
	if ( check_bootimage_structure(bi) == -1){
		return -1;
	}
	
	
	unsigned char* ke = memmem(bi->kernel,bi->header->kernel_size,KERNEL_COMPRESSION_MAGIC_GZIP,KERNEL_COMPRESSION_MAGIC_GZIP_SIZE);
	if ( ke == NULL ) {
		printf("Kernel Not Ok KE = NULL\n");
		return -1;
	}
	unsigned ko = bi->header->kernel_size-(ke-bi->kernel); 
	printf("ko=%d\n",ko);
	
	struct archive *a = NULL;
	struct archive_entry *entry = NULL;
		const struct stat* sb = NULL;
	int r = 0 ; 
	int64_t entry_size;
	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_raw(a);
	
	
	r = archive_read_open_memory(a, ke, ko);
	if (r != ARCHIVE_OK){
		printf("Kernel Not Ok\n");
	}
	bi->uncompressed_kernel = calloc(UNCOMPRESSED_KERNEL_SIZE_32MB,sizeof(char)); 
	
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
			printf("%p %s\n",entry,archive_entry_pathname(entry));
		sb = archive_entry_stat(entry);
		 printf("File type:                ");

           switch (sb->st_mode & S_IFMT) {
           case S_IFBLK:  printf("block device\n");            break;
           case S_IFCHR:  printf("character device\n");        break;
           case S_IFDIR:  printf("directory\n");               break;
           case S_IFIFO:  printf("FIFO/pipe\n");               break;
           case S_IFLNK:  printf("symlink\n");                 break;
           case S_IFREG:  printf("regular file\n");            break;
           case S_IFSOCK: printf("socket\n");                  break;
           default:       printf("unknown?\n");                break;
           }

           printf("I-node number:            %ld\n", (long) sb->st_ino);

           printf("Mode:                     %lo (octal)\n",
                   (unsigned long) sb->st_mode);

           printf("Link count:               %ld\n", (long) sb->st_nlink);
           printf("Ownership:                UID=%ld   GID=%ld\n",
                   (long) sb->st_uid, (long) sb->st_gid);

           printf("Preferred I/O block size: %ld bytes\n",
                   (long) sb->st_blksize);
           printf("File size:                %lld bytes\n",
                   (long long) sb->st_size);
           printf("Blocks allocated:         %lld\n",
                   (long long) sb->st_blocks);

           printf("Last status change:       %s", ctime(&sb->st_ctime));
           printf("Last file access:         %s", ctime(&sb->st_atime));
           printf("Last file modification:   %s", ctime(&sb->st_mtime));

		printf("Uncompressed File Length:   %ld\n",archive_read_data(a,bi->uncompressed_kernel,UNCOMPRESSED_KERNEL_SIZE_32MB));  // Note 2

	}
	
	r = archive_read_free(a);  // Note 3
	
	return 0 ; 	
	
	
	
	
}
int bootimage_read_ramdisk(struct bootimage* bi)
{
	/* Validate inputs */
	if ( check_bootimage_structure(bi) == -1){
		return -1;
	}
	
	struct archive *a = NULL;
	struct archive_entry *entry = NULL;
	const struct stat* sb = NULL;
	int r = 0 ; 
	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
	printf("bi->header->ramdisk_size=%lu\n",bi->header->ramdisk_size);
	r = archive_read_open_memory(a, bi->ramdisk, bi->header->ramdisk_size);
	if (r != ARCHIVE_OK){
		return -1;
	}
	
	
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		printf("%p %s\n",entry,archive_entry_pathname(entry));
		sb = archive_entry_stat(entry);
		 printf("File type:                ");

           switch (sb->st_mode & S_IFMT) {
           case S_IFBLK:  printf("block device\n");            break;
           case S_IFCHR:  printf("character device\n");        break;
           case S_IFDIR:  printf("directory\n");               break;
           case S_IFIFO:  printf("FIFO/pipe\n");               break;
           case S_IFLNK:  printf("symlink\n");                 break;
           case S_IFREG:  printf("regular file\n");            break;
           case S_IFSOCK: printf("socket\n");                  break;
           default:       printf("unknown?\n");                break;
           }

           printf("I-node number:            %ld\n", (long) sb->st_ino);

           printf("Mode:                     %lo (octal)\n",
                   (unsigned long) sb->st_mode);

           printf("Link count:               %ld\n", (long) sb->st_nlink);
           printf("Ownership:                UID=%ld   GID=%ld\n",
                   (long) sb->st_uid, (long) sb->st_gid);

           printf("Preferred I/O block size: %ld bytes\n",
                   (long) sb->st_blksize);
           printf("File size:                %lld bytes\n",
                   (long long) sb->st_size);
           printf("Blocks allocated:         %lld\n",
                   (long long) sb->st_blocks);

           printf("Last status change:       %s", ctime(&sb->st_ctime));
           printf("Last file access:         %s", ctime(&sb->st_atime));
           printf("Last file modification:   %s", ctime(&sb->st_mtime));

		 archive_read_data_skip(a);  // Note 2
	}
	r = archive_read_free(a);  // Note 3
	
	return 0 ; 	
}

__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_read_free(struct bootimage** bip)
{
	struct bootimage* bi = bip[0] ;
	//printf(" Bootimage Structure bi [ %p ]\n", bi);
	if ( check_bootimage_structure(bip[0]) == -1){
		return -1;
	}
	if ( ( bi->kernel != bi->uncompressed_kernel ) && ( bi->uncompressed_kernel != NULL ) ){
		printf("Freeing bi->uncompressed_kernel [ %p ]\n", bi->uncompressed_kernel);
		free(bi->uncompressed_kernel) ;
		bi->uncompressed_kernel = NULL ; 
	}
	if ( ( bi->start != NULL ) && ( bi->stat.st_size > 0 ) ){
		printf("Unmapping Bootimage Data bi->start [ %p ]\n", bi->start);
		munmap(bi->start,bi->stat.st_size);
		bi->start = NULL ; 
		bi->stat.st_size = 0 ;
		
		
	}
	if( bi ){
		printf("Freeing Bootimage Structure bi [ %p ]\n", bi);
		free(bip[0]);
		bip[0] = NULL ;
	}
	return 0;
	
}
