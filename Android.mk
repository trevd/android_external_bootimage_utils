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
 
MAIN_PATH:= $(call my-dir)
include $(all-subdir-makefiles)

LOCAL_PATH := $(MAIN_PATH)

	
src_files :=  main.c \
			  help.c \
			  program.c \
			  actions/extract.c \
			  actions/create.c \
			  actions/update.c \
			  actions/info.c \
			  actions/scan.c \
			  actions/copy_kernel.c \
			  actions/copy_ramdisk.c \
			  actions/create_ramdisk.c \
			  actions/global.c \
			  actions/install.c \
			  libbootimg/utils.c \
			  
			  
			 
include_dirs := $(LOCAL_PATH) \
				$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/include/libbootimg \
				system/core/mkbootimg
								  
			
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)
					
LOCAL_STATIC_LIBRARIES := libbootimage libz

LOCAL_SRC_FILES := $(src_files)

LOCAL_MODULE := bootimage-utils
 
include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)				

LOCAL_STATIC_LIBRARIES := libbootimage libz libc

LOCAL_SRC_FILES := $(src_files)

LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_MODULE := bootimage-utils
 
include $(BUILD_EXECUTABLE)


