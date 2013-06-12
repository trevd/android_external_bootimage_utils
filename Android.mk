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

ifeq ($(HOST_OS),windows)
	# Fixup windows global LD_FLAGS incase we are using mingw6
	HOST_GLOBAL_LDFLAGS := -Wl,--enable-stdcall-fixup
endif

include $(all-subdir-makefiles)

LOCAL_PATH := $(MAIN_PATH)

	
bootimage_tools_src_files :=  main.c \
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
			  
			  
			 
bootimage_tools_include_dirs := $(LOCAL_PATH) \
				$(LOCAL_PATH)/include \
				$(LOCAL_PATH)/include/libbootimg \
				$(LOCAL_PATH)/liblzop \
				$(LOCAL_PATH)/liblzo/include \
				system/core/mkbootimg
							
bootimage_tools_static_libraries := libbootimage\
									libz \
									liblzop-static \
									liblzo-static \
									liblzma-static

bootimage_tools_module_name := bootimage-utils								  
			
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(bootimage_tools_include_dirs)
					
LOCAL_STATIC_LIBRARIES := $(bootimage_tools_static_libraries)

LOCAL_SRC_FILES := $(bootimage_tools_src_files)

LOCAL_MODULE := $(bootimage_tools_module_name)
 
include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(bootimage_tools_include_dirs)				

LOCAL_STATIC_LIBRARIES := libc $(bootimage_tools_static_libraries)

LOCAL_SRC_FILES := $(bootimage_tools_src_files)

LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_MODULE := $(bootimage_tools_module_name)
 
include $(BUILD_EXECUTABLE)


