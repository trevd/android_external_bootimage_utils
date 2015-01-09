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
 * file : lib/api/bootimage.c
 *
 */
#define TRACE_TAG TRACE_API_BOOTIMAGE

#include <stdio.h>
#include <string.h>

#include <private/api.h>


__LIBBOOTIMAGE_PUBLIC_API__ struct bootimage* bootimage_initialize(){

	/* Allocate and zero memory to store the bootimage struct
	   This will contain metadata for a loaded bootimage
	 */
	trace_init();
	struct bootimage* bi = calloc(1,sizeof(struct bootimage));
	D("bi=%p",bi);
	return bi ;

}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_free(struct bootimage** bip){

	D("bip=%p",bip);
	if ( bip == NULL ){
		errno = EINVAL ;
		return -1;
	}
	D("bip=%p",bip);
	if ( check_bootimage_structure(bip[0]) == -1){
		return -1;
	}

	if ( ( bip[0]->kernel != bip[0]->uncompressed_kernel ) && ( bip[0]->uncompressed_kernel != NULL ) ){
		D("freeing uncompressed_kernel=%p",bip[0]->uncompressed_kernel);
		free(bip[0]->uncompressed_kernel) ;
		bip[0]->uncompressed_kernel = NULL ;
	}

	if ( ( bip[0]->start != NULL ) && ( bip[0]->stat.st_size > 0 ) ){
		D("unmapping boot image structures=%p",bip[0]->start);
		/* munmap(bip[0]->start,bip[0]->stat.st_size); */
		bip[0]->start = NULL ;
		bip[0]->stat.st_size = 0 ;

	}
	if( bip[0] != NULL ){
		free(bip[0]);
		bip[0] = NULL ;
	}

	return 0;
}

__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_read(struct bootimage* bi,const char* file_name)
{

	/* Call  */
	if ( bi == NULL ){
		errno = EINVAL ;
		return -1;
	}
	D("bip=%p",bi);
	if ( utils_read_all(file_name,&bi->start,&bi->stat) ){
		return -1 ;

	}
	if ( bootimage_set_magic_address(bi) == -1){
		return -1;
	}
	if( bootimage_set_sections(bi) == -1 ){
		return -1;
	}
	return 0;
}
