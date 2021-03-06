/*
 * Copyright (C) 2015 Trevor Drake
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
 * file : lib/api/bootimage_utils.c
 *
 */
#define  TRACE_TAG   TRACE_API_BOOTIMAGE_UTILS
#include <string.h>
#include <private/api.h>



__LIBBOOTIMAGE_PUBLIC_API__ struct bootimage_utils* bootimage_utils_initialize()
{

	/* Allocate and zero memory to store the bootimage struct
	   This will contain metadata for a loaded bootimage
	 */
	trace_init();
	struct bootimage_utils* bi = calloc(1,sizeof(struct bootimage_utils));
	D("bi=%p",bi);
	return bi ;

}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_utils_free(struct bootimage_utils** biup)
{
	D("biup=%p",biup);
	if ( biup == NULL ){
		errno = EINVAL ;
		return -1;
	}
	if ( check_bootimage_utils_structure(biup[0]) == -1){
		return -1;
	}

	if ( ( biup[0]->data != NULL ) ){
		D("freeing bootimage_utils data=%p",biup[0]->data);
		free(biup[0]->data) ;
		biup[0]->data = NULL ;
	}

	if( biup[0] != NULL ){
		D("freeing bootimage_utils structure=%p",biup[0]);
		free(biup[0]);
		biup[0] = NULL ;
	}

	return 0;

}
__LIBBOOTIMAGE_PUBLIC_API__  int bootimage_utils_file_read(struct bootimage_utils* biu,const char* file_name){

	/* Call  */
	if ( biu == NULL ) {
		errno = EBIUNULL;
		D("biu=%p errno=%d [ EBINULL ]",biu,errno);
		return -1;
	}
	if ( utils_read_all(file_name,&biu->data,&biu->stat) == -1 ){
		return -1;
	}
	biu->file_name = file_name;

	D("biu->data 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",biu->data[0],biu->data[1],biu->data[2],biu->data[3],biu->data[4],biu->data[5]);
	if( check_bootimage_utils_file_type(biu) == -1 ){
		D("file_type check failed");
		return -1;
	}
	D("Return 0");

	return 0;
}
