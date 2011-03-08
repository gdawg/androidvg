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

#include "openvg.h"
#include "androidvg_openvg_VG11.h"

#include <jni.h>
#include <android/log.h>
#include <stdlib.h>

extern "C" {
	////////////////    VG-Env ////////////////////////////////////////////////////////////////////////////////
  JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_nativeClassInit( JNIEnv* env , jobject obj);
	JNIEXPORT jboolean JNICALL Java_com_example_tigervg_VG11_vgCreateContextSH( JNIEnv * env, jobject obj, 
															jint width, jint height );
  JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgResizeSurfaceSH( JNIEnv * env, jobject obj, 
	                            jint width, jint height );
  JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgDestroyContextSH( JNIEnv * env, jobject obj);

	////////////////    VG-SetUp //////////////////////////////////////////////////////////////////////////////
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgFlush( JNIEnv* env, jobject obj );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgSetf( JNIEnv* env, jobject obj, 
	                            jint type, jfloat value );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgSeti( JNIEnv* env, jobject obj, 
	                            jint type, jint value );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgSetParameterfv( JNIEnv* env, jobject obj, 
	                            jlong object, jint paramType, jint count, jobject values);
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgLoadIdentity( JNIEnv* env, jobject obj);
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgTranslate( JNIEnv* env, jobject obj, 
	                            jfloat tx, jfloat ty );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgScale( JNIEnv* env, jobject obj, 
	                            jfloat sx, jfloat sy );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgRotate( JNIEnv* env, jobject obj, 
	                            jfloat angle );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgClear( JNIEnv* env, jobject obj, 
	                            jint x, jint y, jint width, jint height );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgSetfv( JNIEnv* env, jobject obj, 
		                          jint type, jint count, jobject values );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgTranslate( JNIEnv* env, jobject obj,
	                            jfloat tx, jfloat ty);
	////////////////    VG-Paths //////////////////////////////////////////////////////////////////////////////
	JNIEXPORT jlong JNICALL Java_com_example_tigervg_VG11_vgCreatePath( JNIEnv* env, jobject obj, 
	                            jint pathFormat, jint datatype, jfloat scale, jfloat bias, 
															jint segmentCapacityHint, jint coorrdCapacityHint, jint capabilities );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgClearPath( JNIEnv* env, jobject obj, 
	                            jlong pathHandle, jint capabilities );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgDestroyPath( JNIEnv* env, jobject obj, 
	                            jlong pathHandle );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgAppendPathData( JNIEnv* env, jobject obj, 
	                            jlong dstPathHandle, jint numSegments, jobject pathSegments, 
															jobject pathData );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgTransformPath( JNIEnv* env, jobject obj, 
	                            jlong dstPathHandle, jlong srcPathHandle );
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgDrawPath( JNIEnv* env, jobject obj, 
	                            jlong pathHandle, jint paintModes );
	JNIEXPORT VGboolean JNICALL Java_com_example_tigervg_VG11_vgInterpolatePath( JNIEnv* env, jobject obj, 
	                            jlong dstPath, jlong startPath, jlong endPath, jfloat amount);
	////////////////    VG-Paints /////////////////////////////////////////////////////////////////////////////
	JNIEXPORT jlong JNICALL Java_com_example_tigervg_VG11_vgCreatePaint( JNIEnv* env, jobject obj);
	JNIEXPORT jint JNICALL Java_com_example_tigervg_VG11_vgSetPaint( JNIEnv* env, jobject obj, 
	                            jlong paintHandle, jint paintModes );

}

static int initialized =0;
static const char *classPathName = "androidvg/openvg/VG11";
static jclass nioAccessClass;
static jclass bufferClass;
static jclass OOMEClass;
static jclass UOEClass;
static jclass IAEClass;
static jclass AIOOBEClass;
static jmethodID getBasePointerID;
static jmethodID getBaseArrayID;
static jmethodID getBaseArrayOffsetID;
static jfieldID positionID;
static jfieldID limitID;
static jfieldID elementSizeShiftID;


/* Cache method IDs each time the class is loaded. */
static void
nativeClassInitBuffer(JNIEnv *env){
  LOGD("nativeClassInitBuffer.--- 1 ---");
  jclass nioAccessClassLocal = env->FindClass("java/nio/NIOAccess");
	nioAccessClass = (jclass) env->NewGlobalRef(nioAccessClassLocal);
	jclass bufferClassLocal = env->FindClass("java/nio/Buffer");
	bufferClass = (jclass) env->NewGlobalRef(bufferClassLocal);
  LOGD("nativeClassInitBuffer.--- 2 ---");
	getBasePointerID = env->GetStaticMethodID(nioAccessClass, "getBasePointer", "(Ljava/nio/Buffer;)J");
  LOGD("nativeClassInitBuffer.--- 3 ---");
	getBaseArrayID = env->GetStaticMethodID(nioAccessClass, "getBaseArray", "(Ljava/nio/Buffer;)Ljava/lang/Object;");
  LOGD("nativeClassInitBuffer.--- 4 ---");
	getBaseArrayOffsetID = env->GetStaticMethodID(nioAccessClass, "getBaseArrayOffset", "(Ljava/nio/Buffer;)I");
  LOGD("nativeClassInitBuffer.--- 5 ---");
	positionID = env->GetFieldID(bufferClass, "position", "I");
  LOGD("nativeClassInitBuffer.--- 6 ---");
	limitID = env->GetFieldID(bufferClass, "limit", "I");
  LOGD("nativeClassInitBuffer.--- 7 ---");
	elementSizeShiftID = env->GetFieldID(bufferClass, "_elementSizeShift", "I");
  LOGD("nativeClassInitBuffer.--- 8 ---");
}

static void *
getPointer(JNIEnv *env, jobject buffer, jarray *array, jint *remaining){
	jint position;
	jint limit;
	jint elementSizeShift;
	jlong pointer;
	jint offset;
	void *data;

	nativeClassInitBuffer( env );
	position = env->GetIntField(buffer, positionID);
	limit = env->GetIntField(buffer, limitID);
	elementSizeShift = env->GetIntField(buffer, elementSizeShiftID);
	*remaining = (limit - position) << elementSizeShift;
	pointer = env->CallStaticLongMethod(nioAccessClass,  getBasePointerID, buffer);

	*array = (jarray) env->CallStaticObjectMethod(nioAccessClass, getBaseArrayID, buffer);
	offset = env->CallStaticIntMethod(nioAccessClass, getBaseArrayOffsetID, buffer);

	data = env->GetPrimitiveArrayCritical( *array, 0);
	return (void *) ((char*) data + offset );
	
}

static void *
releasePointer(JNIEnv *env, jarray array, void *data, jboolean commit){

	env->ReleasePrimitiveArrayCritical( array, data, commit? 0: JNI_ABORT);
}


static void
nativeClassInit(JNIEnv *env)
{
  LOGD("nativeClassInit.--- 1 ---");
  nativeClassInitBuffer(env);
  LOGD("nativeClassInit.--- 2 ---");
  jclass IAEClassLocal = env->FindClass("java/lang/IllegalArgumentException");
  jclass OOMEClassLocal = env->FindClass("java/lang/OutOfMemoryError");
  jclass UOEClassLocal = env->FindClass("java/lang/UnsupportedOperationException");
  jclass AIOOBEClassLocal = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");

  IAEClass = (jclass) env->NewGlobalRef(IAEClassLocal);
  OOMEClass = (jclass) env->NewGlobalRef(OOMEClassLocal);
  UOEClass = (jclass) env->NewGlobalRef(UOEClassLocal);
  AIOOBEClass = (jclass) env->NewGlobalRef(AIOOBEClassLocal);
  LOGD("nativeClassInit.--- done. ---");
}


////////////////////////////////////////////////////////////////////////////////////////
	
	/* Function Prototypes */
	

	//// C function:: VGErrorCode vgGetError(void);
	//JNIEXPORT XXX JNICALLVGErrorCode vgGetError();

	// C function:: void vgFlush(void);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgFlush(
	  JNIEnv* env, jobject obj ) {
		vgFlush();
	} 

	//// C function:: void vgFinish(void);
	//JNIEXPORT jint JNICALL vgFinish();

	/* Getters and Setters */
	// C function:: void vgSetf (VGParamType type, VGfloat value);
	JNIEXPORT jint JNICALL 
	Java_com_example_tigervg_VG11_vgSetf( 
	  JNIEnv* env, jobject obj, 
		jint type, jfloat value ){
	  
		vgSetf (
		(VGParamType) type, 
		(VGfloat) value
		);		
	}

	// C function:: void vgSeti (VGParamType type, VGint value);
	JNIEXPORT jint JNICALL 
	Java_com_example_tigervg_VG11_vgSeti( 
	  JNIEnv* env, jobject obj, 
		jint type, jint value ){
	  
		vgSeti (
		(VGParamType) type, 
		(VGint) value
		);		
	}

	//// C function:: void vgSetfv(VGParamType type, VGint count, const VGfloat *
	//// values);
	//JNIEXPORT jint JNICALL vgSetfv(VGParamType type, int count,
			//float[] values, int offset);

	// C function:: void vgSetfv(VGParamType type, VGint count, const VGfloat *
	// values);
	JNIEXPORT jint JNICALL 
	Java_com_example_tigervg_VG11_vgSetfv( 
	  JNIEnv* env, jobject obj, 
		jint type, jint count, jobject values ){

		jarray _array = (jarray) 0;
		jint _remaining;
		char *data = (char*) 0;
		jint vgSetfv_ret = FALSE;

		data = (char*)getPointer(env, values, &_array, &_remaining);
	  if (_remaining < 0){
			env->ThrowNew(AIOOBEClass, "In vgSetfv():: Empty Buffer -- _remaining < 0.");
		}

			vgSetfv(
			(VGParamType) type, 
			(VGint) count, 
			(const VGfloat *) data
			);
		
		vgSetfv_ret = SUCCESS;

	exit:
			if ( _array ){
				releasePointer(env, _array, data, JNI_FALSE);
			}
			return vgSetfv_ret;
	}

	//// C function:: void vgSetiv(VGParamType type, VGint count, const VGint *
	//// values);
	//JNIEXPORT jint JNICALL vgSetiv(VGParamType type, int count,
			//int[] values, int offset);

	//// C function:: void vgSetiv(VGParamType type, VGint count, const VGint *
	//// values);
	//JNIEXPORT jint JNICALL vgSetiv(VGParamType type, int count,
			//java.nio.IntBuffer values);

	//// C function:: VGfloat vgGetf(VGParamType type);
	//JNIEXPORT XXX JNICALLfloat vgGetf(VGParamType type);

	//// C function:: VGint vgGeti(VGParamType type);
	//JNIEXPORT jint JNICALL vgGeti(VGParamType type);

	//// C function:: VGint vgGetVectorSize(VGParamType type);
	//JNIEXPORT jint JNICALL vgGetVectorSize(VGParamType type);

	//// C function:: void vgGetfv(VGParamType type, VGint count, VGfloat * values);
	//JNIEXPORT jint JNICALL vgGetfv(VGParamType type, int count,
			//float[] values, int offset);

	//// C function:: void vgGetfv(VGParamType type, VGint count, VGfloat * values);
	//JNIEXPORT jint JNICALL vgGetfv(VGParamType type, int count,
			//java.nio.FloatBuffer values);

	//// C function:: void vgGetiv(VGParamType type, VGint count, VGint * values);
	//JNIEXPORT jint JNICALL vgGetiv(VGParamType type, int count,
			//int[] values, int offset);

	//// C function:: void vgGetiv(VGParamType type, VGint count, VGint * values);
	//JNIEXPORT jint JNICALL vgGetiv(VGParamType type, int count,
			//java.nio.IntBuffer values);

	//// C function:: void vgSetParameterf(VGHandle object, VGint paramType, VGfloat
	//// value);
	//JNIEXPORT jint JNICALL vgSetParameterf(VGHandle object, int paramType,
			//float value);

	//// C function:: void vgSetParameteri(VGHandle object, VGint paramType, VGint
	//// value);
	//JNIEXPORT jint JNICALL vgSetParameteri(VGHandle object, int paramType,
			//int value);

	//// C function:: void vgSetParameterfv(VGHandle object, VGint paramType, VGint
	//// count, const VGfloat * values);
	//JNIEXPORT jint JNICALL vgSetParameterfv(VGHandle object, int paramType,
			//int count, float[] values, int offset);

	// C function:: void vgSetParameterfv(VGHandle object, VGint paramType, VGint
	// count, const VGfloat * values);
	JNIEXPORT jint JNICALL 
	Java_com_example_tigervg_VG11_vgSetParameterfv(
	  JNIEnv* env, jobject obj, 
	  jlong object, jint paramType, jint count, jobject values){
		
		jarray _array = (jarray) 0;
		jint _remaining;
		char *data = (char*) 0;
		jint vgSetParameterfv_ret = FALSE;

		data = (char*)getPointer(env, values, &_array, &_remaining);
	  if (_remaining < 0){
			env->ThrowNew(AIOOBEClass, "In vgSetfv():: Empty Buffer -- _remaining < 0.");
		}

			vgSetParameterfv(
			(VGHandle) object, 
			(VGint) paramType, 
			(VGint) count, 
			(const VGfloat *) data
			);
		
		vgSetParameterfv_ret = SUCCESS;

	exit:
			if ( _array ){
				releasePointer(env, _array, data, JNI_FALSE);
			}
			return vgSetParameterfv_ret;



	}

	//// C function:: void vgSetParameteriv(VGHandle object, VGint paramType, VGint
	//// count, const VGint * values);
	//JNIEXPORT jint JNICALL vgSetParameteriv(VGHandle object, int paramType,
			//int count, int[] values, int offset);

	//// C function:: void vgSetParameteriv(VGHandle object, VGint paramType, VGint
	//// count, const VGint * values);
	//JNIEXPORT jint JNICALL vgSetParameteriv(VGHandle object, int paramType,
			//int count, java.nio.IntBuffer values);

	//// C function:: VGfloat vgGetParameterf(VGHandle object, VGint paramType);
	//JNIEXPORT XXX JNICALLfloat vgGetParameterf(VGHandle object, int paramType);

	//// C function:: VGint vgGetParameteri(VGHandle object, VGint paramType);
	//JNIEXPORT jint JNICALL vgGetParameteri(VGHandle object, int paramType);

	//// C function:: VGint vgGetParameterVectorSize(VGHandle object, VGint
	//// paramType);
	//JNIEXPORT jint JNICALL vgGetParameterVectorSize(VGHandle object,
			//int paramType);

	//// C function:: void vgGetParameterfv(VGHandle object, VGint paramType, VGint
	//// count, VGfloat * values);
	//JNIEXPORT jint JNICALL vgGetParameterfv(VGHandle object, int paramType,
			//int count, float[] values, int offset);

	//// C function:: void vgGetParameterfv(VGHandle object, VGint paramType, VGint
	//// count, VGfloat * values);
	//JNIEXPORT jint JNICALL vgGetParameterfv(VGHandle object, int paramType,
			//int count, java.nio.FloatBuffer values);

	//// C function:: void vgGetParameteriv(VGHandle object, VGint paramType, VGint
	//// count, VGint * values);
	//JNIEXPORT jint JNICALL vgGetParameteriv(VGHandle object, int paramType,
			//int count, int[] values, int offset);

	//// C function:: void vgGetParameteriv(VGHandle object, VGint paramType, VGint
	//// count, VGint * values);
	//JNIEXPORT jint JNICALL vgGetParameteriv(VGHandle object, int paramType,
			//int count, java.nio.IntBuffer values);

	/* Matrix Manipulation */
	// C function:: void vgLoadIdentity(void);
	JNIEXPORT jint JNICALL 
	Java_com_example_tigervg_VG11_vgLoadIdentity( 
	  JNIEnv* env, jobject obj) {

		  vgLoadIdentity();
	}

	//// C function:: void vgLoadMatrix(const VGfloat * m);
	//JNIEXPORT jint JNICALL vgLoadMatrix(float[] m, int offset);

	//// C function:: void vgLoadMatrix(const VGfloat * m);
	//JNIEXPORT jint JNICALL vgLoadMatrix(java.nio.FloatBuffer m);

	//// C function:: void vgGetMatrix(VGfloat * m);
	//JNIEXPORT jint JNICALL vgGetMatrix(float[] m, int offset);

	//// C function:: void vgGetMatrix(VGfloat * m);
	//JNIEXPORT jint JNICALL vgGetMatrix(java.nio.FloatBuffer m);

	//// C function:: void vgMultMatrix(const VGfloat * m);
	//JNIEXPORT jint JNICALL vgMultMatrix(float[] m, int offset);

	//// C function:: void vgMultMatrix(const VGfloat * m);
	//JNIEXPORT jint JNICALL vgMultMatrix(java.nio.FloatBuffer m);

	// C function:: void vgTranslate(VGfloat tx, VGfloat ty);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgTranslate( 
	  JNIEnv* env, jobject obj,
	  jfloat tx, jfloat ty){
	  
		vgTranslate(
		(VGfloat) tx, 
		(VGfloat) ty
		);
	}

	// C function:: void vgScale(VGfloat sx, VGfloat sy);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgScale(
	  JNIEnv* env, jobject obj,
	  jfloat sx, jfloat sy){
			
			vgScale(
			(VGfloat) sx, 
			(VGfloat) sy
			);
	}

	//// C function:: void vgShear(VGfloat shx, VGfloat shy);
	//JNIEXPORT jint JNICALL vgShear(float shx, float shy);

	// C function:: void vgRotate(VGfloat angle);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgRotate(
	  JNIEnv* env, jobject obj,
	  jfloat angle){
		  
			vgRotate(
		  (VGfloat) angle
		  );
	}

	//[> Masking and Clearing <]
	//// C function:: void vgMask(VGImage mask, VGMaskOperation operation, VGint x,
	//// VGint y, VGint width, VGint height);
	//JNIEXPORT jint JNICALL vgMask(VGImage mask, VGMaskOperation operation,
			//int x, int y, int width, int height);

	// C function:: void vgClear(VGint x, VGint y, VGint width, VGint height);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgClear(
	  JNIEnv* env, jobject obj,
	  jint x, jint y, jint width, jint height){
	    
			vgClear(
			(VGint) x, 
			(VGint) y, 
			(VGint) width, 
			(VGint) height
			);
	}

	  

	/* Paths */
	// C function:: VGPath vgCreatePath(VGint pathFormat, VGPathDatatype datatype,
	//              VGfloat scale, VGfloat bias, VGint segmentCapacityHint, 
	//              VGint coordCapacityHint, VGbitfield capabilities);
	JNIEXPORT jlong JNICALL
	Java_com_example_tigervg_VG11_vgCreatePath( 
		JNIEnv* env, jobject obj, 
		jint pathFormat, jint datatype, jfloat scale, jfloat bias, jint segmentCapacityHint, 
		jint coordCapacityHint, jint capabilities) {
		LOGD("In the vgCreate Path");
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

	// C function:: void vgClearPath(VGPath path, VGbitfield capabilities);
	JNIEXPORT jint JNICALL vgClearPath(long path, int capabilities);

	// C function:: void vgDestroyPath(VGPath path);
	JNIEXPORT jint JNICALL vgDestroyPath(long path);

	//// C function:: void vgRemovePathCapabilities(VGPath path, VGbitfield
	//// capabilities);
	//JNIEXPORT jint JNICALL vgRemovePathCapabilities(VGPath path,
			//int capabilities);

	//// C function:: VGbitfield vgGetPathCapabilities(VGPath path);
	//JNIEXPORT jint JNICALL vgGetPathCapabilities(VGPath path);

	//// C function:: void vgAppendPath(VGPath dstPath, VGPath srcPath);
	//JNIEXPORT jint JNICALL vgAppendPath(VGPath dstPath, VGPath srcPath);

	//// C function:: void vgAppendPathData(VGPath dstPath, VGint numSegments,
	////              const VGubyte * pathSegments, const void * pathData);
	//JNIEXPORT jint JNICALL vgAppendPathData(VGPath dstPath, int numSegments,
			//byte[] pathSegments, int offset_pathSegments,
			//java.nio.Buffer pathData);

	// C function:: void vgAppendPathData(VGPath dstPath, VGint numSegments, 
	//              const VGubyte * pathSegments, const void * pathData);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgAppendPathData( 
		JNIEnv * env, jobject obj, 
		jlong dstPathHandle, jint numSegments, jobject pathSegments, jobject pathData) {
			
		vgAppendPathData(
			(VGPath) dstPathHandle, 
			(VGint) numSegments, 
			(const VGubyte *) pathSegments, 
			(const void *) pathData
		);
	}

	//// C function:: void vgModifyPathCoords(VGPath dstPath, VGint startIndex,
	//// VGint numSegments, const void * pathData);
	//JNIEXPORT jint JNICALL vgModifyPathCoords(VGPath dstPath,
			//int startIndex, int numSegments, java.nio.Buffer pathData);

	// C function:: void vgTransformPath(VGPath dstPath, VGPath srcPath);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgTransformPath( 
		JNIEnv* env, jobject obj, 
		jlong dstPath, jlong srcPath){
	
	    vgTransformPath(
			(VGPath) dstPath, 
			(VGPath) srcPath
			);
	}

	// C function:: VGboolean vgInterpolatePath(VGPath dstPath, VGPath startPath,
	// VGPath endPath, VGfloat amount);
	JNIEXPORT VGboolean JNICALL  
	Java_com_example_tigervg_VG11_vgInterpolatePath(
		JNIEnv* env, jobject obj, 
	  jlong dstPath, jlong startPath, jlong endPath, jfloat amount){
	
	  return (VGboolean) vgInterpolatePath(
		(VGPath) dstPath, 
		(VGPath) startPath, 
		(VGPath) endPath, 
		(VGfloat) amount
		);
		
	}

	//// C function:: VGfloat vgPathLength(VGPath path, VGint startSegment, VGint
	//// numSegments);
	//JNIEXPORT XXX JNICALLfloat vgPathLength(VGPath path, int startSegment,
			//int numSegments);

	//// C function:: void vgPointAlongPath(VGPath path, VGint startSegment, VGint
	//// numSegments, VGfloat distance,
	//// VGfloat * x, VGfloat * y, VGfloat * tangentX, VGfloat * tangentY);
	//JNIEXPORT jint JNICALL vgPointAlongPath(VGPath path, int startSegment,
			//int numSegments, float distance, float[] x, int offsetX, float[] y,
			//int offsetY, float[] tangentX, int offset_tanX, float[] tangentY,
			//int offset_tanY);

	//// C function:: void vgPointAlongPath(VGPath path, VGint startSegment, VGint
	//// numSegments, VGfloat distance,
	//// VGfloat * x, VGfloat * y, VGfloat * tangentX, VGfloat * tangentY);
	//JNIEXPORT jint JNICALL vgPointAlongPath(VGPath path, int startSegment,
			//int numSegments, float distance, java.nio.FloatBuffer x,
			//java.nio.FloatBuffer y, java.nio.FloatBuffer tangentX,
			//java.nio.FloatBuffer tangentY);

	//// C function:: void vgPathBounds(VGPath path, VGfloat * minX, VGfloat * minY,
	//// VGfloat * width, VGfloat * height);
	//JNIEXPORT jint JNICALL vgPathBounds(VGPath path, float[] minX,
			//int offset_minX, float[] minY, int offset_minY, float[] width,
			//int offset_width, float[] height, int offset_height);

	//// C function:: void vgPathBounds(VGPath path, VGfloat * minX, VGfloat * minY,
	//// VGfloat * width, VGfloat * height);
	//JNIEXPORT jint JNICALL vgPathBounds(VGPath path,
			//java.nio.FloatBuffer minX, java.nio.FloatBuffer minY,
			//java.nio.FloatBuffer width, java.nio.FloatBuffer height);

	//// C function:: void vgPathTransformedBounds(VGPath path, VGfloat * minX,
	//// VGfloat * minY, VGfloat * width, VGfloat * height);
	//JNIEXPORT jint JNICALL vgPathTransformedBounds(VGPath path,
			//float[] minX, int offset_minX, float[] minY, int offset_minY,
			//float[] width, int offset_width, float[] height, int offset_height);

	//// C function:: void vgPathTransformedBounds(VGPath path, VGfloat * minX,
	//// VGfloat * minY, VGfloat * width, VGfloat * height);
	//JNIEXPORT jint JNICALL vgPathTransformedBounds(VGPath path,
			//java.nio.FloatBuffer minX, java.nio.FloatBuffer minY,
			//java.nio.FloatBuffer width, java.nio.FloatBuffer height);

	// C function:: void vgDrawPath(VGPath path, VGbitfield paintModes);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgDrawPath(
		JNIEnv * env, jobject obj, 
	  jlong path, jint paintModes ){
	
	    vgDrawPath(
		  (VGPath) path, 
		  (VGbitfield) paintModes
		  );
  }
	/* Paint */
	// C function:: VGPaint vgCreatePaint(void);
	JNIEXPORT jlong JNICALL
	Java_com_example_tigervg_VG11_vgCreatePaint(
		JNIEnv * env, jobject obj){
	  
			return (long) vgCreatePaint();
	}

	//// C function:: void vgDestroyPaint(VGPaint paint);
	//JNIEXPORT jint JNICALL vgDestroyPaint(VGPaint paint);

	// C function:: void vgSetPaint(VGPaint paint, VGbitfield paintModes);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgSetPaint(
		JNIEnv * env, jobject obj, 
	  long paint, int paintModes){
	  
			vgSetPaint(
			(VGPaint) paint, 
			(VGbitfield) paintModes
			);
	}

	//// C function:: VGPaint vgGetPaint(VGPaintMode paintMode);
	//JNIEXPORT XXX JNICALLVGPaint vgGetPaint(VGPaintMode paintMode);

	//// C function:: void vgSetColor(VGPaint paint, VGuint rgba);
	//JNIEXPORT jint JNICALL vgSetColor(VGPaint paint, int rgba);

	//// C function:: VGuint vgGetColor(VGPaint paint);
	//JNIEXPORT jint JNICALL vgGetColor(VGPaint paint);

	//// C function:: void vgPaintPattern(VGPaint paint, VGImage pattern);
	//JNIEXPORT jint JNICALL vgPaintPattern(VGPaint paint, VGImage pattern);

	//[> Images <]
	//// C function:: VGImage vgCreateImage(VGImageFormat format, VGint width,
	//// VGint height, VGbitfield allowedQuality);
	//JNIEXPORT XXX JNICALLVGImage vgCreateImage(VGImageFormat format, int width,
			//int height, int allowedQuality);

	//// C function:: void vgDestroyImage(VGImage image);
	//JNIEXPORT jint JNICALL vgDestroyImage(VGImage image);

	//// C function:: void vgClearImage(VGImage image, VGint x, VGint y, VGint
	//// width, VGint height);
	//JNIEXPORT jint JNICALL vgClearImage(VGImage image, int x, int y,
			//int width, int height);

	//// C function:: void vgImageSubData(VGImage image,
	//// const void * data, VGint dataStride,
	//// VGImageFormat dataFormat,
	//// VGint x, VGint y, VGint width, VGint height);
	//JNIEXPORT jint JNICALL vgImageSubData(VGImage image,
			//java.nio.Buffer data, int dataStride, VGImageFormat dataFormat,
			//int x, int y, int width, int height);

	//// C function:: void vgGetImageSubData(VGImage image,
	//// void * data, VGint dataStride,
	//// VGImageFormat dataFormat,
	//// VGint x, VGint y,
	//// VGint width, VGint height);
	//JNIEXPORT jint JNICALL vgGetImageSubData(VGImage image,
			//java.nio.Buffer data, int dataStride, VGImageFormat dataFormat,
			//int x, int y, int width, int height);

	//// C function:: VGImage vgChildImage(VGImage parent,
	//// VGint x, VGint y, VGint width, VGint height);
	//JNIEXPORT XXX JNICALLVGImage vgChildImage(VGImage parent, int x, int y,
			//int width, int height);

	//// C function:: VGImage vgGetParent(VGImage image);
	//JNIEXPORT XXX JNICALLVGImage vgGetParent(VGImage image);

	//// C function:: void vgCopyImage(VGImage dst, VGint dx, VGint dy,
	//// VGImage src, VGint sx, VGint sy,
	//// VGint width, VGint height,
	//// VGboolean dither);
	//JNIEXPORT jint JNICALL vgCopyImage(VGImage dst, int dx, int dy,
			//VGImage src, int sx, int sy, int width, int height, VGboolean dither);

	//// C function:: void vgDrawImage(VGImage image);
	//JNIEXPORT jint JNICALL vgDrawImage(VGImage image);

	//// C function:: void vgSetPixels(VGint dx, VGint dy,
	//// VGImage src, VGint sx, VGint sy,
	//// VGint width, VGint height);
	//JNIEXPORT jint JNICALL vgSetPixels(int dx, int dy, VGImage src, int sx,
			//int sy, int width, int height);

	//// C function:: void vgWritePixels(const void * data, VGint dataStride,
	//// VGImageFormat dataFormat,
	//// VGint dx, VGint dy,
	//// VGint width, VGint height);
	//JNIEXPORT jint JNICALL vgWritePixels(java.nio.Buffer data,
			//int dataStride, VGImageFormat dataFormat, int dx, int dy,
			//int width, int height);

	//// C function:: void vgGetPixels(VGImage dst, VGint dx, VGint dy, VGint sx,
	//// VGint sy, VGint width, VGint height);
	//JNIEXPORT jint JNICALL vgGetPixels(
			//VGImage dst, 
			//int dx, 
			//int dy, 
			//int sx,
			//int sy, 
			//int width, 
			//int height
	//);

	//// C function:: void vgReadPixels(void * data, VGint dataStride, VGImageFormat
	//// dataFormat,
	//// VGint sx, VGint sy, VGint width, VGint height);
	//JNIEXPORT jint JNICALL vgReadPixels(java.nio.Buffer data,
			//int dataStride, VGImageFormat dataFormat, int sx, int sy,
			//int width, int height);

	//// C function:: void vgCopyPixels(VGint dx, VGint dy, VGint sx, VGint sy,
	//// VGint width, VGint height);
	//JNIEXPORT jint JNICALL vgCopyPixels(int dx, int dy, int sx, int sy,
			//int width, int height);

	//[> Image Filters <]
	//// C function:: void vgColorMatrix(VGImage dst, VGImage src, const VGfloat *
	//// matrix);
	//JNIEXPORT jint JNICALL vgColorMatrix(VGImage dst, VGImage src,
			//float[] matrix, int offset);

	//// C function:: void vgColorMatrix(VGImage dst, VGImage src, const VGfloat *
	//// matrix);
	//JNIEXPORT jint JNICALL vgColorMatrix(VGImage dst, VGImage src,
			//java.nio.FloatBuffer matrix);

	//// C function:: void vgConvolve(VGImage dst, VGImage src, VGint kernelWidth,
	//// VGint kernelHeight,
	//// VGint shiftX, VGint shiftY, const VGshort * kernel, VGfloat scale,
	//// VGfloat bias, VGTilingMode tilingMode);
	//JNIEXPORT jint JNICALL vgConvolve(VGImage dst, VGImage src,
			//int kernelWidth, int kernelHeight, int shiftX, int shiftY,
			//short[] kernel, int offset_kernel, float scale, float bias,
			//VGTilingMode tilingMode);

	//// C function:: void vgConvolve(VGImage dst, VGImage src, VGint kernelWidth,
	//// VGint kernelHeight,
	//// VGint shiftX, VGint shiftY, const VGshort * kernel, VGfloat scale,
	//// VGfloat bias, VGTilingMode tilingMode);
	//JNIEXPORT jint JNICALL vgConvolve(VGImage dst, VGImage src,
			//int kernelWidth, int kernelHeight, int shiftX, int shiftY,
			//java.nio.ShortBuffer kernel, float scale, float bias,
			//VGTilingMode tilingMode);

	//// C function:: void vgSeparableConvolve(VGImage dst, VGImage src, VGint
	//// kernelWidth,
	//// VGint kernelHeight, VGint shiftX, VGint shiftY, const VGshort * kernelX,
	//// const VGshort * kernelY, VGfloat scale, VGfloat bias, VGTilingMode
	//// tilingMode);
	//JNIEXPORT jint JNICALL vgSeparableConvolve(VGImage dst, VGImage src,
			//int kernelWidth, int kernelHeight, int shiftX, int shiftY,
			//short[] kernelX, int offset_kernelX, short[] kernelY,
			//int offset_kernelY, float scale, float bias, VGTilingMode tilingMode);

	//// C function:: void vgSeparableConvolve(VGImage dst, VGImage src, 
	////                                     VGint kernelWidth, VGint kernelHeight, 
	////                                     VGint shiftX, VGint shiftY, const VGshort * kernelX,
	////                                     const VGshort * kernelY, VGfloat scale, 
	////                                     VGfloat bias, VGTilingMode tilingMode);
	//JNIEXPORT jint JNICALL vgSeparableConvolve(VGImage dst, VGImage src,
			//int kernelWidth, 
			//int kernelHeight, 
			//int shiftX, 
			//int shiftY,
			//java.nio.ShortBuffer kernelX, 
			//java.nio.ShortBuffer kernelY,
			//float scale, 
			//float bias, 
			//VGTilingMode tilingMode
	//);

	//// C function:: void vgGaussianBlur(VGImage dst, VGImage src, VGfloat
	//// stdDeviationX,
	//// VGfloat stdDeviationY, VGTilingMode tilingMode);
	//JNIEXPORT jint JNICALL vgGaussianBlur(VGImage dst, VGImage src,
			//float stdDeviationX, float stdDeviationY, VGTilingMode tilingMode);

	//// C function:: void vgLookup(VGImage dst, VGImage src, const VGubyte *
	//// redLUT, const VGubyte * greenLUT,
	//// const VGubyte * blueLUT, const VGubyte * alphaLUT, VGboolean
	//// outputLinear, VGboolean outputPremultiplied);
	//JNIEXPORT jint JNICALL vgLookup(VGImage dst, VGImage src, byte[] redLUT,
			//int offset_redLUT, byte[] greenLUT, int offset_greenLUT,
			//byte[] blueLUT, int offset_blueLUT, byte[] alphaLUT,
			//int offset_alphaLUT, VGboolean outputLinear,
			//VGboolean outputPremultiplied);

	//// C function:: void vgLookup(VGImage dst, VGImage src, const VGubyte * redLUT, 
	////                          const VGubyte * greenLUT, const VGubyte * blueLUT, 
	////						    const VGubyte * alphaLUT, VGboolean outputLinear, 
	////							VGboolean outputPremultiplied);
	//JNIEXPORT jint JNICALL vgLookup(VGImage dst, VGImage src,
			//java.nio.ByteBuffer redLUT, java.nio.ByteBuffer greenLUT,
			//java.nio.ByteBuffer blueLUT, java.nio.ByteBuffer alphaLUT,
			//VGboolean outputLinear, VGboolean outputPremultiplied);

	//// C function:: void vgLookupSingle(VGImage dst, VGImage src, const VGuint *
	//// 								  lookupTable, VGImageChannel sourceChannel,
	//// 								  VGboolean outputLinear, VGboolean outputPremultiplied);
	//JNIEXPORT jint JNICALL vgLookupSingle(VGImage dst, VGImage src,
			//int[] lookupTable, int offset_lookupTable,
			//VGImageChannel sourceChannel, VGboolean outputLinear,
			//VGboolean outputPremultiplied);

	//// C function:: void vgLookupSingle(VGImage dst, VGImage src, const VGuint *
	//// 								  lookupTable, VGImageChannel sourceChannel,
	////                                VGboolean outputLinear, VGboolean outputPremultiplied);
	//JNIEXPORT jint JNICALL vgLookupSingle(VGImage dst, VGImage src,
			//java.nio.IntBuffer lookupTable, VGImageChannel sourceChannel,
			//VGboolean outputLinear, VGboolean outputPremultiplied);

	//[> Hardware Queries <]
	//// C function:: VGHardwareQueryResult vgHardwareQuery(VGHardwareQueryType key, VGint setting);
	//JNIEXPORT XXX JNICALL VGHardwareQueryResult vgHardwareQuery(
			//VGHardwareQueryType key, int setting);

	//[> Renderer and Extension Information <]
	//// C function:: const VGubyte * vgGetString(VGStringID name);
	//JNIEXPORT XXX JNICALL String vgGetString(VGStringID name);

	/* Extensions */

	#define OVG_SH_blend_src_out 1
	#define OVG_SH_blend_dst_out 1
	#define OVG_SH_blend_src_atop 1
	#define OVG_SH_blend_dst_atop 1

	//// C function:: VGboolean vgCreateContextSH(VGint width, VGint height);
	JNIEXPORT jboolean JNICALL  
	Java_com_example_tigervg_VG11_vgCreateContextSH(
		JNIEnv * env, jobject obj, 
	  jint width, jint height ){
	    
			nativeClassInit(env);

	    return (jboolean) vgCreateContextSH(
			(VGint) width, 
			(VGint) height
			);
		  // LOGD("vgCreateContextSH -- done. -- width=[%d], height=[%d]", width, height);
		  // return SUCCESS;
  }

	// C function:: void vgResizeSurfaceSH(VGint width, VGint height);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgResizeSurfaceSH(
		JNIEnv* env, jobject obj, 
	  jint width, jint height){
	
	  vgResizeSurfaceSH(
		(VGint) width, 
		(VGint) height
		);
	}

	// C function:: void vgDestroyContextSH(void);
	JNIEXPORT jint JNICALL
	Java_com_example_tigervg_VG11_vgDestroyContextSH(
	  JNIEnv* env, jobject obj){
	  
		vgDestroyContextSH();
	}
	
  JNIEXPORT jint JNICALL 
	Java_com_example_tigervg_VG11_nativeClassInit( 
	  JNIEnv* env , jobject obj) {

    LOGD("VG11-CPP-wrapper::nativeClassInit(): done!!");
	  nativeClassInit(env);
  }

	//static {
	//	nativeClassInit();
	//}
