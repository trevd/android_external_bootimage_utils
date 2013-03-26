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

###### Bootimage Loading Test ########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  

LOCAL_SRC_FILES := tests/loading.c

LOCAL_STATIC_LIBRARIES := 	libbootimage

LOCAL_MODULE := bootimage-loading-test
 
include $(BUILD_HOST_EXECUTABLE)

###### Bootimage Saving Test ###########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  

LOCAL_SRC_FILES := tests/saving.c

LOCAL_STATIC_LIBRARIES = libbootimage libz

LOCAL_MODULE := bootimage-saving-test
 
include $(BUILD_HOST_EXECUTABLE)

###### Unpack Kernel Test ############

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  

LOCAL_SRC_FILES := tests/unpack_kernel.c

LOCAL_STATIC_LIBRARIES := libbootimage libz

LOCAL_MODULE := bootimage-unpack-kernel-test
 
include $(BUILD_HOST_EXECUTABLE)


###### Unpack Ramdisk Test ###########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  

LOCAL_SRC_FILES := tests/unpack_ramdisk.c

LOCAL_STATIC_LIBRARIES = libbootimage libz

LOCAL_MODULE := bootimage-unpack-ramdisk-test
 
include $(BUILD_HOST_EXECUTABLE)

###### Print Ramdisk File Test ###########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  

LOCAL_SRC_FILES := tests/print_ramdisk_file.c

LOCAL_STATIC_LIBRARIES = libbootimage libz

LOCAL_MODULE := bootimage-print-ramdisk-file-test
 
include $(BUILD_HOST_EXECUTABLE)