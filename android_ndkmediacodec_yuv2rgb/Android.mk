#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS) 
  
LOCAL_SRC_FILES := yuv2rgb.cpp mediacodec.cpp 

LOCAL_LDLIBS += -lOpenMAXAL -lmediandk
      
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libbinder \
    libui   \
    libgui \
    libstagefright_foundation  
      
LOCAL_MODULE := mediacodec_test

LOCAL_CFLAGS += -DARM_NEON_ENABLE -mfpu=neon -flax-vector-conversions

LOCAL_C_INCLUDES := \
        frameworks/av/include/ndk
  
LOCAL_CLANG := true
  
include $(BUILD_EXECUTABLE)  
