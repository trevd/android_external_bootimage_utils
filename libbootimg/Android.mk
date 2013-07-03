#
# Android.mk - Android make file for libbootimage
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

libbootimg_src_files := \
			../../../system/core/libmincrypt/sha.c \
			bootimage.c \
			compression.c \
			file.c \
			kernel.c \
			md5.c \
			ramdisk.c \
			utils.c 	



			
libbootimg_include_dirs := $(LOCAL_PATH)/../include \
				$(LOCAL_PATH)/../include/libbootimg \
				$(LOCAL_PATH)/../liblzop \
				system/core/mkbootimg \
				system/core/include/mincrypt \
				external/zlib \
				external/bzip2 \
				$(LOCAL_PATH)/../easylzma/easylzma-0.0.8/include \
				$(LOCAL_PATH)/../liblzma/api \
				$(LOCAL_PATH)/../liblzo/include

##
## Hack to work around difference in the android_filesystem_config.h
##
$(info "CM_VERSION $(CM_VERSION)")

ifneq ( $(CM_VERSION) , )
# Building in AOSP
	libbootimg_include_dirs += $(LOCAL_PATH)/cm
else
	libbootimg_include_dirs += system/core/include/private
	
endif

				
libbootimg_static_libraries := 	libz \
				libbz \
				liblzop-static \
				liblzo-static \
				libxz-static \
				liblzma-static
				
								
libbootimg_module_name := libbootimage

ifeq ($(HOST_OS),windows)
	libbootimg_src_files += utils_windows.c
endif

ifeq ($(HOST_OS),linux)
	libbootimg_src_files += utils_linux.c
endif 


include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(libbootimg_include_dirs)

LOCAL_SRC_FILES := $(libbootimg_src_files)

LOCAL_STATIC_LIBRARIES := $(libbootimg_static_libraries)

LOCAL_MODULE := $(libbootimg_module_name)
 
include $(BUILD_HOST_STATIC_LIBRARY)


ifneq ($(HOST_OS),windows)


include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(libbootimg_include_dirs)

LOCAL_SRC_FILES := $(libbootimg_src_files)

LOCAL_STATIC_LIBRARIES := $(libbootimg_static_libraries)

LOCAL_MODULE := $(libbootimg_module_name)

include $(BUILD_STATIC_LIBRARY)


endif


###### Bootimage Loading Test ########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(libbootimg_include_dirs)

$(info "INC : $(LOCAL_C_INCLUDES) LP: $(LOCAL_PATH)")


LOCAL_SRC_FILES := tests/example_test_harness.c

LOCAL_STATIC_LIBRARIES := libbootimage

LOCAL_MODULE := example-test-harness
 
include $(BUILD_HOST_EXECUTABLE)
