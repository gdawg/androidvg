LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := libopenvg
LOCAL_CFLAGS    := -O3
LOCAL_SRC_FILES := \
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
LOCAL_LDLIBS    := -lGLESv1_CM -llog


include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE    := libopenvg-tiger
LOCAL_CFLAGS    := -O3
LOCAL_SRC_FILES := test.c test_tiger_paths.c test_tiger.c test_dash.c

LOCAL_LDLIBS    := -lGLESv1_CM -llog
LOCAL_STATIC_LIBRARIES := libopenvg

include $(BUILD_SHARED_LIBRARY)

