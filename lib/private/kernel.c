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

/* */

__LIBBOOTIMAGE_PRIVATE_API__ static int bootimage_kernel_set_compressed_data_offset(struct bootimage* bi)
{
	int i = 0 ;


	for(i = 1 ; i <= KERNEL_COMPRESSION_TYPE_MAX ; i++){

		bi->compressed_kernel_offset = memmem(bi->kernel,bi->header->kernel_size,kernel_type[i].magic,kernel_type[i].magic_size);
		if ( bi->compressed_kernel_offset != NULL ){
			bi->compressed_kernel_type = &kernel_type[i] ;
			bi->compressed_kernel_size = bi->header->kernel_size - ( bi->compressed_kernel_offset -bi->kernel);
			D("bi->compressed_kernel_offset=%p bi->compressed_kernel_type.compression_type=%d",bi->compressed_kernel_offset,bi->compressed_kernel_type->compression_type);
			break ;
		}
	}
	if( bi->compressed_kernel_offset == NULL ) {
		return -1 ;
	}
	return 0;
}

__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_kernel_decompress(struct bootimage* bi)
{
	/* */
	if ( check_bootimage_kernel(bi) == -1 ){
		return -1;
	}
	if ( bootimage_kernel_set_compressed_data_offset(bi) == -1 ){
		return -1 ;
	}


	struct archive *a = NULL ;
	if ( check_archive_read_memory(&a,bi->compressed_kernel_offset,bi->compressed_kernel_size ) == -1 ){
		return -1;
	}

	D("archive_compression_name=%s",archive_compression_name(a)) ;

	struct archive_entry *entry = NULL;

	if ( archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		   uint64_t size = archive_entry_size(entry);
		   if ( size <= 0 ){
			   size = UNCOMPRESSED_KERNEL_SIZE_32MB ;
		   }
		   bi->uncompressed_kernel = calloc(size,sizeof(char));
		   if ( bi->uncompressed_kernel == NULL ){
			   archive_read_free(a);
			   return -1;
		   }
		   bi->uncompressed_kernel_size = archive_read_data(a,bi->uncompressed_kernel,size);
		   if ( bi->uncompressed_kernel_size <= 0 ){
			   free(bi->uncompressed_kernel) ;
			   archive_read_free(a);
			   return -1;

		   }
		   D("bi->uncompressed_kernel_size=%llu",bi->uncompressed_kernel_size) ;
	}
	archive_read_free(a);

	bi->kernel_version_string = memmem(bi->uncompressed_kernel,bi->uncompressed_kernel_size, KERNEL_VERSION_STRING,KERNEL_VERSION_STRING_SIZE);
	if ( bi->kernel_version_string == NULL ){
		D("kstring is null");
		return -1;
	}
	bi->kernel_version_string_length = paranoid_strnlen(bi->kernel_version_string,256);
	if ( bi->kernel_version_string_length <= 0 ) {
		return -1 ;
	}
	D("bi->kernel_version_string_length len %d",bi->kernel_version_string_length);

	return 0;
}
