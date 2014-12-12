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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <api/bootimage.h>

#include <private/checks.h>
#include <private/bootimage.h>

__LIBBOOTIMAGE_PUBLIC_API__ struct bootimage* bootimage_initialize(){

	/* Allocate and zero memory to store the bootimage struct
	   This will contain metadata for a loaded bootimage
	 */
	struct bootimage* bi = calloc(1,sizeof(struct bootimage));

	return bi ;

}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_free(struct bootimage** bip){

	struct bootimage* bi = bip[0] ;
	if ( check_bootimage_structure(bip[0]) == -1){
		return -1;
	}
	if ( ( bi->kernel != bi->uncompressed_kernel ) && ( bi->uncompressed_kernel != NULL ) ){
		fprintf(stdout,"Freeing bi->uncompressed_kernel [ %p ]\n", bi->uncompressed_kernel);
		free(bi->uncompressed_kernel) ;
		bi->uncompressed_kernel = NULL ;
	}
	fprintf(stdout,"bi->stat.st_size [ %u ]\n",  bi->stat.st_size);
	if ( ( bi->start != NULL ) && ( bi->stat.st_size > 0 ) ){
		fprintf(stdout,"Unmapping Bootimage Data bi->start [ %p ]\n", bi->start);

		munmap(bi->start,bi->stat.st_size);
		bi->start = NULL ;
		bi->stat.st_size = 0 ;


	}
	if( bi ){
		fprintf(stdout,"Freeing Bootimage Structure bi [ %p ]\n", bi);
		free(bip[0]);
		bip[0] = NULL ;
	}
	return 0;

	return 0;
}

__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_file_read(struct bootimage* bi,const char* file_name){

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
	if( bootimage_set_sections(bi) == -1 ){
		return -1;
	}
	return 0;
}
