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
 * file : lib/include/private/bootimage.h
 *
 */

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
 * file : lib/private/bootimage.h
 * This file is part of the INTERNAL api for the bootimage utils project
 *
 */


#ifndef _a373d2ec_79e2_11e4_a4f4_5404a601fa9d
#define _a373d2ec_79e2_11e4_a4f4_5404a601fa9d
#include <stdint.h>
#include <sys/stat.h>
#include <private/api.h>

#define DEFAULT_NAME_KERNEL "kernel"
#define DEFAULT_NAME_KERNEL_RAMDISK_DIRECTORY "kernel-ramdisk"
#define DEFAULT_NAME_KERNEL_RAMDISK_ARCHIVE "kernel-ramdisk.img"
#define DEFAULT_NAME_RAMDISK_DIRECTORY "ramdisk"
#define DEFAULT_NAME_RAMDISK_ARCHIVE "ramdisk.img"
#define DEFAULT_NAME_HEADER "header"
#define DEFAULT_NAME_HEADER_BLOCK "header.bin"



/* This is the codeaurora ( caf ) version of the boot_img_hdr structure
 * This is modified to "handle" kernel device trees see
 * https://www.codeaurora.org/cgit/quic/la/platform/system/core/commit/?h=lp&id=27d21ae19ba72a66a6094aa3ffd995e3979211a8
 * for more info .
 */
#define BOOT_MAGIC "ANDROID!"
#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512
#define BOOT_EXTRA_ARGS_SIZE 1024

struct bootimage_header
{
    unsigned char magic[BOOT_MAGIC_SIZE];

    uint32_t kernel_size;  /* size in bytes */
    uint32_t kernel_addr;  /* physical load addr */

    uint32_t ramdisk_size; /* size in bytes */
    uint32_t ramdisk_addr; /* physical load addr */

    uint32_t second_size;  /* size in bytes */
    uint32_t second_addr;  /* physical load addr */

    uint32_t tags_addr;    /* physical addr for kernel tags */
    uint32_t page_size;    /* flash page size we assume */
    uint32_t dt_size;      /* device tree in bytes */
    uint32_t unused;       /* future expansion: should be 0 */

    unsigned char name[BOOT_NAME_SIZE]; /* asciiz product name */

    unsigned char cmdline[BOOT_ARGS_SIZE];

    unsigned id[8]; /* timestamp / checksum / sha1 / etc */

    /* Supplemental command line data; kept here to maintain
     * binary compatibility with older versions of mkbootimg */
    unsigned char extra_cmdline[BOOT_EXTRA_ARGS_SIZE];
};


/*
** +-----------------+
** | boot header     | 1 page
** +-----------------+
** | kernel          | n pages
** +-----------------+
** | ramdisk         | m pages
** +-----------------+
** | second stage    | o pages
** +-----------------+
** | device tree     | p pages
** +-----------------+
**
** n = (kernel_size + page_size - 1) / page_size
** m = (ramdisk_size + page_size - 1) / page_size
** o = (second_size + page_size - 1) / page_size
** p = (dt_size + page_size - 1) / page_size
**
** 0. all entities are page_size aligned in flash
** 1. kernel and ramdisk are required (size != 0)
** 2. second is optional (second_size == 0 -> no second)
** 3. load each element (kernel, ramdisk, second) at
**    the specified physical address (kernel_addr, etc)
** 4. prepare tags at tag_addr.  kernel_args[] is
**    appended to the kernel commandline in the tags.
** 5. r0 = 0, r1 = MACHINE_TYPE, r2 = tags_addr
** 6. if second_size != 0: jump to second_addr
**    else: jump to kernel_addr
*/

enum page_size {
	PAGE_SIZE_2048= 2048 ,
	PAGE_SIZE_MIN =  PAGE_SIZE_2048,
	PAGE_SIZE_4096 = 4096 ,
	PAGE_SIZE_8192 = 8192 ,
	PAGE_SIZE_16384 = 16384 ,
	PAGE_SIZE_32768 = 32768  ,
	PAGE_SIZE_65536 = 65536 ,
	PAGE_SIZE_131072 = 131072 ,
	PAGE_SIZE_262144 = 262144 ,
	PAGE_SIZE_MAX = PAGE_SIZE_262144

};

struct bootimage
{

	unsigned char* start  ; /*  pointer to the start of the image file in memory
                               This is often the same as value as the header.
                               However some boot images have additional data before
                               The header magic ( e.g HTC ) */


	struct bootimage_header* header ; /* pointer to the start of the boot image header
							   identified with the ANDROID! magic */

	unsigned char* kernel ;
	/* pointer to the start of the kernel data.
	   This is normally locate one page after the header and
	   is usually compressed. If the kernel is uncompressed then
	   the uncompressed_kernel and kernel members will be the same */

	unsigned char* ramdisk ; /* pointer to the start of the ramdisk data. This is
							    on the next page boundary after the kernel data  */
	unsigned char* second ; /* pointer to the start of the second bootloader data. */

	struct stat stat ; /* The file size of the bootimage. other sizes are found in the
						 bootimage header*/

	off_t header_size; /* Header size stores the sizeof(bootimage_header) structure
						  for convient easy access */

	uint32_t header_padding; /* the number of bytes required to align the
								header to the next page boundary */
	uint32_t kernel_padding;
	uint32_t ramdisk_padding;
	uint32_t second_padding;

	unsigned char* uncompressed_kernel ; /* pointer to the uncompressed kernel data */
	uint32_t uncompressed_kernel_size;  /* size in bytes */


 };

__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_mmap_file(struct bootimage* bi,const char* file_name);
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_magic_address(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_header_section(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_kernel_section(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_ramdisk_section(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_second_section(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_set_sections(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_file_read_magic(struct bootimage* bi,const char* file_name);
#endif
