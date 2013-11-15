#
# Android.mk - Android make file for bootimage-utils module
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
 
 
 
MAIN_PATH:= $(call my-dir)

LOCAL_PATH := $(MAIN_PATH)
			  
			  
			 
bootimage_tools_include_dirs := $(LOCAL_PATH) \
				$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/include/libbootimg \
				$(LOCAL_PATH)/liblzop \
				$(LOCAL_PATH)/liblzo/include \
				$(LOCAL_PATH)/lzma-9.22/C \
				$(LOCAL_PATH)/lzma-9.22/C/Util/Lzma \
				system/core/mkbootimg
						  

###### Bootimage Loading Test ########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(libbootimg_include_dirs)

LOCAL_SRC_FILES := tests/bootimage_api_example1.c 

LOCAL_STATIC_LIBRARIES := libbootimage

LOCAL_MODULE := bootimage_api_example1
 
include $(BUILD_HOST_EXECUTABLE)
