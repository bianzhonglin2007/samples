LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder 

LOCAL_MODULE_TAGS := optional 
LOCAL_SRC_FILES:= test.cpp
LOCAL_MODULE := swtest

include $(BUILD_EXECUTABLE)
