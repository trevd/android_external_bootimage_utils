LOCAL_PATH:= $(call my-dir)

src_files := bootimage.c \
			compression.c \
			kernel.c \
			ramdisk.c\
			utils.c \
			md5.c \
			../../../system/core/libmincrypt/sha.c 
			
include_dirs := $(LOCAL_PATH)/../include \
				$(LOCAL_PATH)/../include/libbootimg \
				system/core/mkbootimg \
				system/core/include/mincrypt \
				external/zlib 

ifeq ($(HOST_OS),windows)
	src_files += utils_windows.c
endif

ifeq ($(HOST_OS),linux)
	src_files += utils_linux.c
endif 



include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz

LOCAL_MODULE := libbootimage
 
include $(BUILD_HOST_STATIC_LIBRARY)

include $(CLEAR_VARS)




LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include \
					system/core/mkbootimg \
					system/core/include/mincrypt \
					external/zlib
					 

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz

LOCAL_MODULE := libbootimage
 
#include $(BUILD_HOST_SHARED_LIBRARY)
ifneq ($(HOST_OS),windows)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz

LOCAL_MODULE := libbootimage
 
include $(BUILD_STATIC_LIBRARY)
endif
#### Shared library
ifneq ($(HOST_OS),windows)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(include_dirs)

LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := libz

LOCAL_MODULE := libbootimage
 
include $(BUILD_SHARED_LIBRARY)

#include $(LOCAL_PATH)/test.mk
endif
