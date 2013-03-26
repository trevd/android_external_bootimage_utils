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

###### Update Ramdisk File Test ###########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  

LOCAL_SRC_FILES := tests/update_ramdisk_file.c

LOCAL_STATIC_LIBRARIES = libbootimage libz

LOCAL_MODULE := bootimage-update-ramdisk-file-test
 
include $(BUILD_HOST_EXECUTABLE)

###### Update Kernel Test ###########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  

LOCAL_SRC_FILES := tests/update_kernel.c

LOCAL_STATIC_LIBRARIES = libbootimage libz

LOCAL_MODULE := bootimage-update-kernel-test
 
include $(BUILD_HOST_EXECUTABLE)

###### Print Full Details ###########

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)  

LOCAL_SRC_FILES := tests/loading_all_print.c

LOCAL_STATIC_LIBRARIES = libbootimage libz

LOCAL_MODULE := bootimage-print-details-test
 
include $(BUILD_HOST_EXECUTABLE)