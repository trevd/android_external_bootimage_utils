LOCAL_PATH := $(call my-dir)

libbootimage_src_files := \
					lib/api/bootimage.c \
					lib/api/bootimage_extract.c \
					lib/private/bootimage.c \
					lib/private/checks.c \
					lib/private/utils.c \




ifeq ($(HOST_OS),windows)
	include $(CLEAR_VARS)
	LOCAL_MODULE := libbootimage-windows
	LOCAL_MODULE_TAGS := optional
	LOCAL_SRC_FILES := windows/mman.c
	LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/windows
	LOCAL_C_INCLUDES := $(LOCAL_PATH)/windows
	include $(BUILD_HOST_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := libbootimage
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -D_GNU_SOURCE -fvisibility=hidden
ifeq ($(HOST_OS),windows)
	LOCAL_STATIC_LIBRARIES := libbootimage-windows
endif
LOCAL_SHARED_LIBRARIES := libarchive
LOCAL_SRC_FILES := $(libbootimage_src_files)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/lib
include $(BUILD_HOST_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := bootimage-utils-test1
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := test1.c
LOCAL_SHARED_LIBRARIES := libbootimage
LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib
include $(BUILD_HOST_EXECUTABLE)


ifneq ($(SDK_ONLY),true)

include $(CLEAR_VARS)
LOCAL_MODULE := libbootimage
LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := libarchive
LOCAL_SRC_FILES := $(libbootimage_src_files)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/lib
include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := bootimage-utils-test1
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := test1.c
LOCAL_SHARED_LIBRARIES := libbootimage
include $(BUILD_EXECUTABLE)

endif
