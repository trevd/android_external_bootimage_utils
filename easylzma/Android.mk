LOCAL_PATH:= $(call my-dir)

lzma_src_files := \
	src/pavlov/7zCrc.c \
	src/pavlov/LzmaDec.c \
	src/pavlov/Bra.c \
	src/pavlov/LzmaEnc.c \
	src/pavlov/BraIA64.c \
	src/pavlov/LzFind.c \
	src/pavlov/LzmaLib.c \
	src/pavlov/7zStream.c \
	src/pavlov/7zBuf2.c \
	src/pavlov/Bra86.c \
	src/pavlov/7zFile.c \
	src/pavlov/7zBuf.c \
	src/pavlov/Bcj2.c \
	src/pavlov/Alloc.c \
	src/lzma_header.c \
	src/lzip_header.c \
	src/decompress.c \
	src/compress.c \
	src/common_internal.c

		
lzma_include_dirs += $(LOCAL_PATH) \
			$(LOCAL_PATH)/src \
			
			 


# ========================================================
# lzma - Lzma Utility 
# ========================================================

include $(CLEAR_VARS)
#LOCAL_SRC_FILES := $(lzma_src_files) C/Util/Lzma/LzmaUtil.c
LOCAL_C_INCLUDES += $(lzma_include_dirs)

LOCAL_CFLAGS += \
	-D_7ZIP_ST \
	-DHAVE_CONFIG_H \
	-std=c99

LOCAL_MODULE:= lzma

#include $(BUILD_EXECUTABLE)

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

#include $(BUILD_HOST_EXECUTABLE)



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


ifneq ($(HOST_OS),windows)
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

endif
