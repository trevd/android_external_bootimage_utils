LOCAL_PATH := $(call my-dir)

libbootimage_src_files := \
					lib/api/bootimage.c \
					lib/api/bootimage_extract.c \
					lib/api/bootimage_print.c \
					lib/api/bootimage_file_print.c \
					lib/api/bootimage_utils.c \
					lib/private/bootimage.c \
					lib/private/checks.c \
					lib/private/archive.c \
					lib/private/trace.c \
					lib/private/print.c \
					lib/private/kernel.c \
					lib/private/utils.c

libbootimage_c_includes := $(LOCAL_PATH)/lib/include

bootimageutils_src_files := \
					src/main.c



include $(CLEAR_VARS)
LOCAL_MODULE := libbootimage
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -D_GNU_SOURCE -fvisibility=hidden
LOCAL_SRC_FILES := $(libbootimage_src_files)
LOCAL_C_INCLUDES := $(libbootimage_c_includes)
LOCAL_WHOLE_STATIC_LIBRARIES := libarchive
LOCAL_EXPORT_C_INCLUDE_DIRS := $(libbootimage_c_includes)
$(info LOCAL_C_INCLUDES $(LOCAL_C_INCLUDES))
include $(BUILD_HOST_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libbootimage
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -D_GNU_SOURCE -fvisibility=hidden
LOCAL_WHOLE_STATIC_LIBRARIES := libbootimage
LOCAL_EXPORT_C_INCLUDE_DIRS := $(libbootimage_c_includes)
$(info LOCAL_C_INCLUDES $(LOCAL_C_INCLUDES))
include $(BUILD_HOST_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := bootimage-utils
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES :=  $(bootimageutils_src_files)
LOCAL_STATIC_LIBRARIES := libbootimage
LOCAL_C_INCLUDES := $(libbootimage_c_includes)
LOCAL_FORCE_STATIC_EXECUTABLE := true
include $(BUILD_HOST_EXECUTABLE)

# Do not build target binaries if we are not targeting linux
# on the host
ifeq ($(HOST_OS),linux)

include $(CLEAR_VARS)
LOCAL_MODULE := libbootimage
LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := libarchive
LOCAL_SRC_FILES := $(libbootimage_src_files)
LOCAL_C_INCLUDES := $(libbootimage_c_includes)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(libbootimage_c_includes)
include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := bootimage-utils-test1
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := test1.c
LOCAL_SHARED_LIBRARIES := libbootimage
#include $(BUILD_EXECUTABLE)

endif
