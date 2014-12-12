/*
 * Copyright (C) 2014 Trevor Drake
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * file : lib/private/bootimage.c
 * This file is part of the INTERNAL api for the bootimage utils project
 *
 */
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <api/errors.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#include <private/bootimage.h>

__LIBBOOTIMAGE_PRIVATE_API__  static uint32_t calculate_padding(size_t section_size, uint32_t page_size);


__LIBBOOTIMAGE_PRIVATE_API__  static uint32_t calculate_padding(size_t section_size, uint32_t page_size)
{

    errno = 0 ;  // clear the errno
    if(page_size == 0 || section_size == 0){
        //D("Calculate Padding Error %d\n",errno);
        errno = EINVAL ;
        return  0 ;
    }
    uint32_t pagemask = page_size - 1;
    uint32_t padding_size = page_size - (section_size & pagemask);
    if(padding_size == page_size) padding_size = 0 ;
    //D("Calculate Padding Returns %d\n",padding_size);
    return padding_size ;
}

__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_mmap_file(struct bootimage* bi,const char* file_name)
{

	fprintf(stdout,"bi->stat.st_size=%lu\n",bi->stat.st_size );
	/* Open the file as read only, read for mmapping */
	int bifd = open(file_name,O_RDONLY);
	if(bifd < 0 ){
		/* Could not open file. errno should be set already so return -1 */
		return -1;
	}
	/* mmap the full file into memory */
	bi->start = mmap(NULL,bi->stat.st_size,PROT_READ,MAP_PRIVATE,bifd,0);

	/* stash the mmap error number */
	int ierrno = errno ;

	/* even if mmap failed we still need/want to close the file */
	if( close(bifd) == -1 ){
		/* failed to close the file. is this fatal? Not in the overall
		   scheme of things. According to the documentaton close should
		   not be retried */
		fprintf(stdout,"close failed for file %d\n",bifd);
	}

	/* mmap failed. We should exit */
	if( bi->start == MAP_FAILED ) {
		bi->start == NULL ;
		errno = ierrno ;
		return -1 ;
	}
	return 0 ;
}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_magic_address(struct bootimage* bi)
{
	/* Look for the ANDROID! boot magic in the mapped file area */
	bi->header = memmem(bi->start,bi->stat.st_size,BOOT_MAGIC,BOOT_MAGIC_SIZE);
	if( bi->header == NULL ){
		/* Boot magic not found unmap the mapped file */
		if ( munmap(bi->start,bi->stat.st_size) == -1 ){
			/* cannot unmap the file return -1 and leave errno intact */
			return -1;
		};
		/* set the error to no magic and return */
		errno = EBINOMAGIC ;
		return -1 ;
	}
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_header_section(struct bootimage* bi)
{
	/* Store the size of the header struct for convience */
	bi->header_size = sizeof(struct bootimage_header) ;
	/* Work out the padding for header section of the bootimage file */
	bi->header_padding = calculate_padding(bi->header_size,bi->header->page_size);
	if ( bi->header_padding == -1 ) {
		/* Header padding could not be calculated unmap and exit */
		if ( munmap(bi->start,bi->stat.st_size) == -1 ){
			/* cannot unmap the file return -1 and leave errno intact */
			return -1;
		};
		/* set the error and return */
		errno = EBIBADPADHEAD ;
		return -1 ;
	}
	errno = EBIOK;
	return 0;

}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_kernel_section(struct bootimage* bi)
{
	/* Set the kernel address offset within the mmapped area.
	   We could just use header offset + page_size as the header size should never
	   span multiple pages ... but that is just not a safe assumption to make.

	   We must also use the address of the first entry of the header structure
	   we are pointing to ( i.e magic ) instead of the outer bi->header address
	   Although these pointers have the same value doing pointer arithmetic on
	   bi->header results in traversing the bootimage stricture rather than the
	   mmapped area of which bi->header->magic[0] points at
	 */
	bi->kernel = bi->header->magic + bi->header_size + bi->header_padding ;

	/* Work out the padding for kernel section of the bootimage file */
	bi->kernel_padding = calculate_padding(bi->header->kernel_size ,bi->header->page_size);
	if ( bi->kernel_padding == -1 ) {
		/* Header padding could not be calculate unmap and exit */
		if ( munmap(bi->start,bi->stat.st_size) == -1 ){
			/* cannot unmap the file return -1 and leave errno intact */
			return -1;
		};
		/* set the error and return */
		errno = EBIBADPADKERNEL ;
		return -1 ;
	}

	errno = EBIOK;
	return 0;

}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_ramdisk_section(struct bootimage* bi)
{
	/* Set the ramdisk address offset within the mmapped area */
	bi->ramdisk = bi->kernel + bi->header->kernel_size + bi->kernel_padding ;

	/* Work out the padding for ramdisk section of the bootimage file */
	bi->ramdisk_padding = calculate_padding(bi->header->ramdisk_size ,bi->header->page_size);
	if ( bi->ramdisk_padding == -1 ) {
		/* Ramdisk padding could not be calculate unmap and exit */
		if ( munmap(bi->start,bi->stat.st_size) == -1 ){
			/* cannot unmap the file return -1 and leave errno intact */
			return -1;
		};
		/* set the error and return */
		errno = EBIBADPADRAMDISK ;
		return -1 ;
	}
	errno = EBIOK;
	return 0;

}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_second_section(struct bootimage* bi)
{

	/* Set the second bootloader address offset within the mmapped area
	   We must first check the second size in the header. If it is zero
	   then we are not using the second bootloader section and the checks
	   can be skipped
	 */
	if ( bi->header->second_size > 0 ){
		bi->second = bi->ramdisk + bi->header->ramdisk_size + bi->ramdisk_padding ;

		/* Work out the padding for ramdisk section of the bootimage file */
		bi->second_padding = calculate_padding(bi->header->second_size,bi->header->page_size);
		if ( bi->second_padding == -1 ) {
			/* Second padding could not be calculate unmap and exit */
			if ( munmap(bi->start,bi->stat.st_size) == -1 ){
				/* cannot unmap the file return -1 and leave errno intact */
				return -1;
			};
			/* set the error and return */
			errno = EBIBADPADSECOND ;
			return -1 ;
		}

	}
	errno = EBIOK;
	return 0;

}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_sections(struct bootimage* bi)
{

	if ( bootimage_set_header_section(bi) == -1 ){
		return -1 ;
	}
	if ( bootimage_set_kernel_section(bi) == -1 ){
		return -1 ;
	}
	if ( bootimage_set_ramdisk_section(bi) == -1 ){
		return -1 ;
	}
	if ( bootimage_set_second_section(bi) == -1 ){
		return -1 ;
	}
	return 0;
}
__LIBBOOTIMAGE_PRIVATE_API__  int bootimage_file_read_magic(struct bootimage* bi,const char* file_name)
{
	if ( check_bootimage_structure(bi) == -1){
		fprintf(stderr,"bootimage_file_read check_bootimage_structure failed [ %p ]\n", bi);
		return -1;
	}

	if( check_bootimage_file_name(file_name) == -1 ){
		fprintf(stderr,"bootimage_file_read check_bootimage_file_name failed [ %p ]\n", bi);
		return -1;
	}

	if( check_bootimage_file_stat_size(bi,file_name) == -1 ){
		return -1;
	}
	if( bootimage_mmap_file(bi,file_name) == -1 ){
		return -1;
	}
	if( bootimage_set_magic_address(bi) == -1 ){
		return -1;
	}
	return 0;
}
