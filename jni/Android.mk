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
	shTriangulation.c\
	androidvg_openvg_VG11.c
LOCAL_LDLIBS    := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)
