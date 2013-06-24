LOCAL_PATH:= $(call my-dir)

lzma_src_files := \
	C/Alloc.c \
	C/7zFile.c \
	C/7zStream.c \
	C/LzFind.c \
	C/LzmaDec.c \
	C/LzmaEnc.c \

		
lzma_include_dirs += $(LOCAL_PATH) \
			$(LOCAL_PATH)/C \
			 $(LOCAL_PATH)/C/Util/Lzma


# ========================================================
# lzma - Lzma Utility 
# ========================================================

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(lzma_src_files) C/Util/Lzma/LzmaUtil.c
LOCAL_C_INCLUDES += $(lzma_include_dirs)

LOCAL_CFLAGS += \
	-D_7ZIP_ST \
	-DHAVE_CONFIG_H \
	-std=c99

LOCAL_MODULE:= lzma

include $(BUILD_EXECUTABLE)

# ========================================================
# lzma - Lzma Utility For The Host
# ========================================================

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(lzma_src_files) C/Util/Lzma/LzmaUtil.c
LOCAL_C_INCLUDES += $(lzma_include_dirs)

LOCAL_CFLAGS += \
	-D_7ZIP_ST \
	-DHAVE_CONFIG_H \
	-std=c99

LOCAL_MODULE:= lzma

include $(BUILD_HOST_EXECUTABLE)



# ========================================================
# liblzma - Lzma Static Library for the host
# ========================================================

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(lzma_src_files)
LOCAL_C_INCLUDES += $(lzma_include_dirs)

LOCAL_CFLAGS += \
	-D_7ZIP_ST \
	-DHAVE_CONFIG_H \
	-std=c99

LOCAL_MODULE:= liblzma

include $(BUILD_HOST_LIBRARY)



# ========================================================
# liblzma-static.a - static library for host
# ========================================================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(lzma_src_files)
LOCAL_C_INCLUDES += $(lzma_include_dirs)

LOCAL_CFLAGS += \
	-D_7ZIP_ST \
	-DHAVE_CONFIG_H \
	-std=c99

ifeq ($(HOST_OS),windows)
	LOCAL_CFLAGS += -DHAVE_WINDOWS
endif

LOCAL_MODULE:= liblzma-static

include $(BUILD_HOST_STATIC_LIBRARY)

# ========================================================
# liblzma-static.a - static library for target
# ========================================================
include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(lzma_src_files)
LOCAL_C_INCLUDES += $(lzma_include_dirs)

LOCAL_CFLAGS += \
	-D_7ZIP_ST \
	-DHAVE_CONFIG_H \
	-std=c99

ifeq ($(HOST_OS),windows)
	LOCAL_CFLAGS += -DHAVE_WINDOWS
endif

LOCAL_MODULE:= liblzma-static

include $(BUILD_STATIC_LIBRARY)

