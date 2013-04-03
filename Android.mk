
MAIN_PATH:= $(call my-dir)
include $(all-subdir-makefiles)

LOCAL_PATH := $(MAIN_PATH)

	
src_files :=  main.c \
			  extract.c \
			  libbootimg/utils.c	
			  #update.c \
			  #info.c \
			  
			
include $(CLEAR_VARS)



LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/libbootimg \
					

LOCAL_STATIC_LIBRARIES := libbootimage libz

LOCAL_SRC_FILES := $(src_files)

LOCAL_MODULE := bootimage-utils
 
include $(BUILD_HOST_EXECUTABLE)

include $(CLEAR_VARS)



LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/libbootimg \
					

LOCAL_STATIC_LIBRARIES := libbootimage libz

LOCAL_SRC_FILES := $(src_files)

LOCAL_MODULE := bootimage-utils
 
include $(BUILD_EXECUTABLE)
