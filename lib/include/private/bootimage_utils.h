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
 * file : lib/include/private/bootimage_utils.h
 *
 */

#ifndef _5203c50e_9290_11e4_981e_5404a601fa9d
#define _5203c50e_9290_11e4_981e_5404a601fa9d

#define BOOTIMAGE_UTILS_FILENAME_UPDATE_ZIP "update.zip"
#define BOOTIMAGE_UTILS_FILENAME_BOOT_IMG "boot.img"
#define BOOTIMAGE_UTILS_FILENAME_RECOVERY_IMG "recovery.img"
#define BOOTIMAGE_UTILS_FILENAME_KERNEL "kernel"
#define BOOTIMAGE_UTILS_FILENAME_BZIMAGE "bzImage"
#define BOOTIMAGE_UTILS_FILENAME_RAMDISK_CPIO_GZ "ramdisk.cpio.gz"
#define BOOTIMAGE_UTILS_FILENAME_RAMDISK_IMG "ramdisk.img"
#define BOOTIMAGE_UTILS_FILENAME_RECOVERY_RAMDISK_CPIO_GZ "ramdisk-recovery.cpio.gz"
#define BOOTIMAGE_UTILS_FILENAME_RECOVERY_RAMDISK_IMG "ramdisk-recovery.img"


#define BOOTIMAGE_UTILS_FILE_EXTENSION_ZIP ".zip"
#define BOOTIMAGE_UTILS_FILE_EXTENSION_TGZ ".tgz"
#define BOOTIMAGE_UTILS_FILE_EXTENSION_IMG ".img"

enum bootimage_utils_filetype
{
	BOOTIMAGE_UTILS_FILETYPE_UNKNOWN = 0,
	BOOTIMAGE_UTILS_FILETYPE_NEXUS_FACTORY_IMAGE,
	BOOTIMAGE_UTILS_FILETYPE_UPDATE_ZIP,
	BOOTIMAGE_UTILS_FILETYPE_OTA_UPDATE_ZIP,
	BOOTIMAGE_UTILS_FILETYPE_STANDARD_BOOT_IMAGE,
	BOOTIMAGE_UTILS_FILETYPE_OEM_BOOT_IMAGE,
	BOOTIMAGE_UTILS_FILETYPE_STANDARD_RECOVERY_IMAGE,
	BOOTIMAGE_UTILS_FILETYPE_OEM_RECOVERY_IMAGE,
	BOOTIMAGE_UTILS_FILETYPE_STANDARD_RAMDISK,
	BOOTIMAGE_UTILS_FILETYPE_RECOVERY_RAMDISK,
	BOOTIMAGE_UTILS_FILETYPE_COMPRESSED_KERNEL,
	BOOTIMAGE_UTILS_FILETYPE_MAX = BOOTIMAGE_UTILS_FILETYPE_COMPRESSED_KERNEL
};

struct bootimage_utils {
	int filetype ;
	struct stat stat ;
	char* file_name ;
	char* data ;
	struct bootimage bootimage ;

};



#endif
