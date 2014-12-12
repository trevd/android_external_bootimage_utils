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
 * file : lib/private/kernel.c
 *
 */

#define  TRACE_TAG   TRACE_PRIVATE_KERNEL
#include <string.h>
#include <private/api.h>


__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_kernel_set_compressed_data_offset(struct bootimage* bi)
{
	int i = 0 ;


	for(i = 1 ; i <= KERNEL_COMPRESSION_TYPE_MAX ; i++){

		bi->compressed_kernel_offset = memmem(bi->kernel,bi->header->kernel_size,kernel_type[i].magic,kernel_type[i].magic_size);
		if ( bi->compressed_kernel_offset != NULL ){
			bi->compressed_kernel_type = &kernel_type[i] ;
			D("bi->compressed_kernel_offset=%p bi->compressed_kernel_type.compression_type=%d",bi->compressed_kernel_offset,bi->compressed_kernel_type->compression_type);
			break ;
		}
	}
	return 0;
}

__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_kernel_decompress(struct bootimage* bi)
{
	if ( check_bootimage_kernel(bi) == -1 ){
		return -1;
	}
	bootimage_kernel_set_compressed_data_offset(bi);
	uint64_t size = bi->header->kernel_size - ( bi->compressed_kernel_offset -bi->kernel);
	D("compressed kernel size=%llu",size);
	struct archive *a = NULL ;
	check_archive_read_memory(&a,bi->compressed_kernel_offset,size);
	D("a=%p",a);
	D("archive_compression_name=%s",archive_compression_name(a)) ;
	 struct archive_entry *entry = NULL;

     if ( archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		   uint64_t size = archive_entry_size(entry);
		   bi->uncompressed_kernel = calloc(UNCOMPRESSED_KERNEL_SIZE_32MB,sizeof(char));
		   bi->uncompressed_kernel_size = archive_read_data(a,bi->uncompressed_kernel,UNCOMPRESSED_KERNEL_SIZE_32MB);

		   D("bi->uncompressed_kernel_size=%llu",bi->uncompressed_kernel_size) ;
	 }

	archive_read_free(a);
	return 0;
}
