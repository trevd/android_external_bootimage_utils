#
# Android.mk
# 
# Copyright 2013 Trevor Drake <trevd1234@gmail.com>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301, USA.
# 
# 
#

LOCAL_PATH:= $(call my-dir)

src_files := bootimage.c \
			compression.c \
			kernel.c \
			ramdisk.c\
			utils.c \
			md5.c \
			file.c \
			../../../system/core/libmincrypt/sha.c
			
			
include_dirs := external/bootimage-tools/include \
				external/bootimage-tools/include/libbootimg \
				external/bootimage-tools/liblzop \
				system/core/mkbootimg \
				system/core/include/mincrypt \
				external/zlib \
				external/xz/src/liblzma/api \
				external/lzo/include

ifeq ($(HOST_OS),windows)
	src_files += utils_windows.c
endif

ifeq ($(HOST_OS),linux)
	src_files += utils_linux.c
endif 

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz liblzop-static liblzo-static liblzma-static

LOCAL_MODULE := libbootimage
 
include $(BUILD_HOST_STATIC_LIBRARY)

include $(CLEAR_VARS)




LOCAL_C_INCLUDES := $(include_dirs)		

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz liblzop-static liblzo-static liblzma-static

LOCAL_MODULE := libbootimage
 
#include $(BUILD_HOST_SHARED_LIBRARY)
ifneq ($(HOST_OS),windows)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz liblzop-static liblzo-static liblzma-static

LOCAL_MODULE := libbootimage
 
include $(BUILD_STATIC_LIBRARY)
endif
#### Shared library
ifneq ($(HOST_OS),windows)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz liblzop-static liblzo-static liblzma-static
LOCAL_MODULE := libbootimage
 
include $(BUILD_SHARED_LIBRARY)

#include $(LOCAL_PATH)/test.mk
endif



###### Bootimage Loading Test ########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)

$(info "INC : $(LOCAL_C_INCLUDES) LP: $(LOCAL_PATH)")


LOCAL_SRC_FILES := tests/load-unknown-file.c

LOCAL_STATIC_LIBRARIES := 	libbootimage

LOCAL_MODULE := load-unknown-file-test
 
#include $(BUILD_HOST_EXECUTABLE)
