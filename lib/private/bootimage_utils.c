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
 * file : lib/private/bootimage_utils.c
 *
 */
#include <private/api.h>

__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_nexus_factory_image(struct bootimage_utils* biu)
{

	struct factory_images* fi =  &factory_image_info[0] ;
	char* bname = utils_basename(biu->file_name);
	D("bname=%s",bname);
	while ( fi->name != NULL ){
		/* D("fi=%p fi->name=%s fi->name_length=%d",fi,fi->name,fi->name_length); */
		if ( strncmp(fi->name,bname,fi->name_length) == FACTORY_IMAGE_STRNCMP_MATCH_FOUND ){
			D("Potential Factory Image File Found file_name=%s",biu->file_name);
			break ;
		}

		fi++;
	}
	if ( fi == NULL ) {
		return -1 ;
	}
	/* For this to be a potentially bona-fida factory image the Gzip identifaction must be at the start of the data */
	char* magic = utils_memmem(biu->data,biu->stat.st_size,FACTORY_IMAGE_MAGIC_GZIP,FACTORY_IMAGE_MAGIC_GZIP_SIZE);
	D("Factory Image Gzip Magic %p : biu-data[0] %p",magic ,biu->data ) ;
	if ( magic != biu->data ){
		return -1 ;
	}

	size_t zip_size ,boot_image_size;
	char* zip_data = archive_extract_entry(biu->data,biu->stat.st_size, fi->zip_name,fi->zip_name_length,&zip_size);



		//	unsigned int uncompressed_size = archive_gzip_get_uncompressed_size(biu->compressed_data,biu->stat.st_size);

			D("Factory Image zip data %p : zip_size %zu",zip_data,zip_size) ;


			 archive_extract_file(zip_data, zip_size,"boot.img",0);
			//D("Factory Image boot_image_data  %p : boot_image_size %zu",boot_image_data,boot_image_size) ;
			//D("Factory Image Gzip Magic %p : biu-data[0] %p",magic ,biu->compressed_data ) ;
		//}

	}

}

__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_factory_image(struct bootimage_utils* biu)
{


}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_update_zip_image(struct bootimage_utils* biu)
{


}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_ota_update_zip_image(struct bootimage_utils* biu)
{


}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_standard_boot_image(struct bootimage_utils* biu)
{

	char* magic = utils_memmem(biu->data,biu->stat.st_size,BOOT_MAGIC,BOOT_MAGIC_SIZE);

	if ( ( magic == NULL ) || ( magic != biu->data ) ){
		/* Boot Image Magic Not Found or Not at the start of the data */
		return -1 ;
	}


	return 0 ;
}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_oem_boot_image(struct bootimage_utils* biu)
{


	char* magic = utils_memmem(biu->data,biu->stat.st_size,BOOT_MAGIC,BOOT_MAGIC_SIZE);

	if ( ( magic == NULL )  ){
		/* Boot Image Magic Not Found */
		return -1 ;
	}

}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_recovery_boot_image(struct bootimage_utils* biu)
{


}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_oem_recovery_image(struct bootimage_utils* biu)
{


}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_standard_ramdisk(struct bootimage_utils* biu)
{


}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_recovery_ramdisk(struct bootimage_utils* biu)
{


}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_factory(struct bootimage_utils* biu)
{


}
__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_is_compressed_kernel(struct bootimage_utils* biu)
{


}


__LIBBOOTIMAGE_PRIVATE_API__ int bootimage_utils_get_filetype(struct bootimage_utils* biu)
{


}


