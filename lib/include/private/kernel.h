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
 * file : lib/include/private/kernel.h
 *
 */

#ifndef _b24c0928_81d6_11e4_8336_5404a601fa9d
#define _b24c0928_81d6_11e4_8336_5404a601fa9d
#include <stdio.h>
#include <stdint.h>


#define UNCOMPRESSED_KERNEL_SIZE_32MB ( (1024 * 1024 ) *32 )

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

enum kernel_compression_type {
	KERNEL_COMPRESSION_TYPE_UNKNOWN = 0 ,
	KERNEL_COMPRESSION_TYPE_LZ4,
	KERNEL_COMPRESSION_TYPE_GZIP,
	KERNEL_COMPRESSION_TYPE_LZOP,
	KERNEL_COMPRESSION_TYPE_BZIP2,
	KERNEL_COMPRESSION_TYPE_XZ,
	KERNEL_COMPRESSION_TYPE_LZMA,
	KERNEL_COMPRESSION_TYPE_MAX = KERNEL_COMPRESSION_TYPE_LZMA
};


static struct kernel_type_t  {
	int compression_type ;
	char* magic ;
	uint32_t magic_size ;
} kernel_type[] = {
	{ KERNEL_COMPRESSION_TYPE_LZ4 , KERNEL_COMPRESSION_MAGIC_LZ4 , KERNEL_COMPRESSION_MAGIC_LZ4_SIZE },
	{ KERNEL_COMPRESSION_TYPE_GZIP , KERNEL_COMPRESSION_MAGIC_GZIP , KERNEL_COMPRESSION_MAGIC_GZIP_SIZE },
	{ KERNEL_COMPRESSION_TYPE_LZOP , KERNEL_COMPRESSION_MAGIC_LZOP , KERNEL_COMPRESSION_MAGIC_LZOP_SIZE },
	{ KERNEL_COMPRESSION_TYPE_BZIP2 , KERNEL_COMPRESSION_MAGIC_BZIP2 , KERNEL_COMPRESSION_MAGIC_BZIP2_SIZE },
	{ KERNEL_COMPRESSION_TYPE_XZ , KERNEL_COMPRESSION_MAGIC_XZ , KERNEL_COMPRESSION_MAGIC_XZ_SIZE },
	{ KERNEL_COMPRESSION_TYPE_LZMA , KERNEL_COMPRESSION_MAGIC_LZMA , KERNEL_COMPRESSION_MAGIC_LZMA_SIZE },
	{ KERNEL_COMPRESSION_TYPE_UNKNOWN , NULL , 0 }
};


struct bootimage ;
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_kernel_compression_type(struct bootimage* bi);
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_kernel_decompress(struct bootimage* bi);

#endif
