# Copyright (C) 2010 Shuo-Hung Chen, Hsiao-Mei Lin, Yi-Cheng Chen
# The AndroidVG Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#//////////////////////////////////////////////////////////////////////
# The OpenVG library will be built statically.
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
	androidvg_openvg_VG11.cpp
LOCAL_LDLIBS    := -lGLESv1_CM -llog
include $(BUILD_STATIC_LIBRARY)

#//////////////////////////////////////////////////////////////////////
# The openvg-star lib depends on and include the libopenvg.
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)
LOCAL_MODULE    := libopenvg-star
LOCAL_CFLAGS    := -O3 -Werror
LOCAL_SRC_FILES := \
	app/test.c\
	app/test_dash.c
LOCAL_LDLIBS    := -lGLESv1_CM -llog
LOCAL_STATIC_LIBRARIES := libopenvg
include $(BUILD_SHARED_LIBRARY)




