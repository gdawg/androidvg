LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES  := \
	shExtensions.c\
	shArrays.c\
	shVectors.c\
	shPath.c\
	shImage.c\
	shPaint.c\
	shGeometry.c\
	shPipeline.c\
	shParams.c\
	shContext.c\
	shVgu.c \
	androidvg_openvg_VG11.c
	
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include
	
LOCAL_LDLIBS     := -lGLESv1_CM -llog

LOCAL_CFLAGS     := -O3

LOCAL_MODULE     := libopenvg

include $(BUILD_STATIC_LIBRARY)



include $(CLEAR_VARS)

LOCAL_SRC_FILES  := test.c test_dash.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include

LOCAL_LDLIBS     := -lGLESv1_CM -llog

LOCAL_CFLAGS     := -O3

LOCAL_STATIC_LIBRARIES := libopenvg

LOCAL_MODULE     := libopenvg-star

include $(BUILD_SHARED_LIBRARY)

