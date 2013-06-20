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
	
LOCAL_PATH :=  $(call my-dir)

liblzop_src_files := liblzop.c lzop_support.c

include_dirs := $(LOCAL_PATH) \
		$(LOCAL_PATH)/../liblzo/include
										
include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(liblzop_src_files)

LOCAL_C_INCLUDES := $(include_dirs)

LOCAL_STATIC_LIBRARIES :=  liblzo-static

LOCAL_MODULE := liblzop-static
 
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(liblzop_src_files)

LOCAL_C_INCLUDES := $(include_dirs)

LOCAL_STATIC_LIBRARIES :=  liblzo-static

LOCAL_MODULE := liblzop-static
 
include $(BUILD_HOST_STATIC_LIBRARY)


