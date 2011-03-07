/*
** Copyright (c) 2010 Shuo-Hung Chen, Hsiao-Mei Lin
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library in the file COPYING;
** if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
**/

#include <jni.h>
#include "openvg.h"

// VGboolean vgCreateContextSH(VGint width, VGint height);
jint
java_androidvg_openvg_VG11_vgCreateContextSH__II
  ( JNIEnv * _env, jobject _this, jint width, jint height) {
    vgCreateContextSH(
      (VGint) width,
      (VGint) height
    );
}

// void vgResizeSurfaceSH(VGint width, VGint height);
void
java_androidvg_openvg_VG11_vgResizeSurfaceSH__II
  ( JNIEnv * _env, jobject _this, jint width, jint height) {
    vgResizeSurfaceSH(
      (VGint) width,
      (VGint) height
    );
}

// void vgDestroyContextSH(void);
void
java_androidvg_openvg_VG11_vgDestroyContextSH__
  ( JNIEnv * _env, jobject _this) {
    vgDestroyContextSH();
}

// VGPath vgCreatePath(VGint pathFormat, VGPathDatatype datatype, VGfloat scale, VGfloat bias, VGint segmentCapacityHint, VGint coordCapacityHint, VGbitfield capabilities);
void
java_androidvg_openvg_VG11_vgCreatePath__IIFFIII
  ( JNIEnv * _env, jobject _this, jint pathFormat, jint datatype, jfloat scale, jfloat bias, jint segmentCapacityHint, jint coordCapacityHint, jint capabilities) {
    vgCreatePath(
	(VGint) pathFormat,
	(VGPathDatatype) datatype,
	(VGfloat) scale,
	(VGfloat) bias,
	(VGint) segmentCapacityHint,
	(VGint) coordCapacityHint,
	(VGbitfield) capabilities
    );
}

// void vgAppendPathData(VGPath dstPath, VGint numSegments, const VGubyte * pathSegments, const void * pathData);
void
java_androidvg_openvg_VG11_vgAppendPathData__
	( JNIEnv * _env, jobject _this) {

}
