LOCAL_PATH:= $(call my-dir)

src_files := main.c bootimg.c memmem.c line_endings.c getopt_long.c	getopt.c file.c	ramdisk.c

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../system/core/mkbootimg 
LOCAL_SRC_FILES := $(src_files)
LOCAL_STATIC_LIBRARIES := libc libcutils libmincrypt libz

LOCAL_MODULE := bootimg-tools
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
 
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../system/core/mkbootimg 
LOCAL_SRC_FILES := $(src_files)

LOCAL_STATIC_LIBRARIES := 	libcutils libmincrypt   libz

LOCAL_MODULE := bootimg-tools
 
include $(BUILD_HOST_EXECUTABLE)

