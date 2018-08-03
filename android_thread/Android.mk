LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder 

LOCAL_MODULE_TAGS := optional 
LOCAL_SRC_FILES:= test_thread.cpp
LOCAL_MODULE := test_thread

include $(BUILD_EXECUTABLE)
