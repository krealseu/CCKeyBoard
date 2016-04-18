# A simple test for the minimal standard C++ library
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := keyboard
LOCAL_SRC_FILES := main.cpp cckeyboard.cpp
LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE
LOCAL_LDLIBS    := -llog
include $(BUILD_EXECUTABLE)
