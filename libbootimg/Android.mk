LOCAL_PATH:= $(call my-dir)

src_files := bootimage.c \
			compression.c \
			kernel.c \
			ramdisk.c\
			utils.c \
			../../../system/core/libmincrypt/sha.c 

include $(CLEAR_VARS)

ifeq ($(HOST_OS),windows)
	src_files += utils_windows.c
endif

ifeq ($(HOST_OS),linux)
	src_files += utils_linux.c
endif 

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					system/core/mkbootimg \
					system/core/include/mincrypt \
					external/zlib 

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz

LOCAL_MODULE := libbootimage
 
include $(BUILD_HOST_STATIC_LIBRARY)

include $(CLEAR_VARS)

ifeq ($(HOST_OS),windows)
	src_files += utils_windows.c
endif

ifeq ($(HOST_OS),linux)
	src_files += utils_linux.c
endif 

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					system/core/mkbootimg \
					system/core/include/mincrypt \
					external/zlib 

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz

LOCAL_MODULE := libbootimage
 
include $(BUILD_STATIC_LIBRARY)

#include $(LOCAL_PATH)/test.mk

