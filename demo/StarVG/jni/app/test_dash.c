#include "test.h"
#include <ctype.h>

#include <jni.h>

VGPaint testStroke;
VGPaint testFill;
VGPath testPath;
VGfloat phase=0.0f;

VGint j_index = 0;
VGint j_size = 3;
VGint joins[] = {
  VG_JOIN_MITER,
  VG_JOIN_BEVEL,
  VG_JOIN_ROUND
};

VGint cindex = 2;
VGint csize = 3;
VGint caps[] = {
  VG_CAP_BUTT,
  VG_CAP_SQUARE,
  VG_CAP_ROUND
};

VGfloat px, py;

VGfloat sx=1.0f, sy=1.0f;

VGfloat startX=0.0f;
VGfloat startY=0.0f;
VGfloat clickX=0.0f;
VGfloat clickY=0.0f;

#define MODE_NUM 5
VGbyte modes[MODE_NUM] = {'d', 'x', 'y', 'b', 't'};
VGint  mode_index = 3;
VGbyte mode;

#ifdef __cplusplus
extern "C" {
#endif

void dash_display(float interval)
{
  VGfloat cc[] = {0,0,0,1};
  VGfloat dash[] = {10,15,0,15};

  vgSetfv(VG_CLEAR_COLOR, 4, cc);
  vgClear(0,0,testWidth(),testHeight());
  
  vgSetfv(VG_STROKE_DASH_PATTERN, sizeof(dash)/sizeof(float), dash);
  vgSeti(VG_STROKE_DASH_PHASE_RESET, VG_TRUE);
  vgSetf(VG_STROKE_DASH_PHASE, phase);
  
  vgSetf(VG_STROKE_LINE_WIDTH, 10.0f);
  vgSetf(VG_STROKE_JOIN_STYLE, joins[j_index]);
  vgSetf(VG_STROKE_CAP_STYLE, caps[cindex]);
  
  vgLoadIdentity();
  if(mode != 't') {
	  vgTranslate(testWidth()/2, testHeight()/2);
  } else {
	  vgTranslate(px, py);
  }
  vgScale(3.0 * sx, 3.0 * sy);
  vgDrawPath(testPath, VG_FILL_PATH);
  vgDrawPath(testPath, VG_STROKE_PATH);
}

void createStar()
{
  VGubyte segs[] = {
    VG_MOVE_TO, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL,
    VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL, VG_LINE_TO_REL,
    VG_LINE_TO_REL, VG_LINE_TO_REL, VG_CLOSE_PATH };
  
  VGfloat data[] = {
    0,50, 15,-40, 45,0, -35,-20,
    15,-40, -40,30, -40,-30, 15,40,
    -35,20, 45,0 };
  
  VGfloat cstroke[] = {0.5,0.2,0.8, 0.6};
  VGfloat cfill[] = {0.3,1.0,0.0, 0.6};
  
  testPath = testCreatePath();
  vgAppendPathData(testPath, sizeof(segs), segs, data);
  
  testStroke = vgCreatePaint();
  vgSetParameterfv(testStroke, VG_PAINT_COLOR, 4, cstroke);
  vgSetPaint(testStroke, VG_STROKE_PATH);
  
  testFill = vgCreatePaint();
  vgSetParameterfv(testFill, VG_PAINT_COLOR, 4, cfill);
  vgSetPaint(testFill, VG_FILL_PATH);
}

int dash(int argc, char **argv)
{
  
  createStar();
  
  testOverlayColor(1,1,1,1);
  
  return EXIT_SUCCESS;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* ------------- JNI ------------- */

JNIEXPORT jint JNICALL
Java_com_example_starvg_DemoRenderer_nativeStart(JNIEnv* env, jobject this, jint width, jint height)
{
	int argc;
	char **argv;
	testInit(argc, argv, width, height, "ShivaVG: Dash Pattern Test");
	createStar();
	px = testWidth()/2;
	py = testHeight()/2;
	mode = modes[mode_index];
	return 0;
}

JNIEXPORT void JNICALL
Java_com_example_startvg_StarVG_nativeDestroy(JNIEnv* env, jobject this)
{
	testCleanup();
}

JNIEXPORT void JNICALL
Java_com_example_starvg_StarVG_nativeCycleCapsType(JNIEnv* env, jobject this)
{
    /* Cycle caps type */
    cindex = (cindex+1) % csize;
}

JNIEXPORT void JNICALL
Java_com_example_starvg_StarVG_nativeCycleJoinsType(JNIEnv* env, jobject this)
{
    /* Cycle joins type */
    j_index = (j_index+1) % j_size;
}

JNIEXPORT int JNICALL
Java_com_example_starvg_DemoGLSurfaceView_nativeChangeMode(JNIEnv* env, jobject this)
{
	mode = modes[(mode_index++) % MODE_NUM];
}

JNIEXPORT jint JNICALL
Java_com_example_starvg_DemoRenderer_nativeDisplay(JNIEnv* env, jobject  this)
{
	dash_display(0.05);
	return 0;
}

JNIEXPORT void JNICALL
Java_com_example_starvg_DemoGLSurfaceView_nativeClick(JNIEnv* env, jobject this, jfloat x, jfloat y)
{
	y = testHeight() - y;
	clickX = x; clickY = y;

	switch(mode) {
	case 'd':
		startY = phase;
		break;
	case 'x':
		startX = sx;
		break;
	case 'y':
		startY = sy;
		break;
	case 'b':
		startX = sx;
		startY = sy;
		break;
	case 't':
		startX = px;
		startY = py;
		break;
	}
}

JNIEXPORT void JNICALL
Java_com_example_starvg_DemoGLSurfaceView_nativeDrag(JNIEnv* env, jobject this, jfloat x, jfloat y)
{
	VGfloat dx, dy;
	y = testHeight() - y;
	dx = x - clickX;
	dy = y - clickY;

	switch(mode) {
	case 'd':
		phase = startY + dy * 0.1;
		LOGD("phase: %f", phase);
		break;
	case 'x':
		sx = startX + dx * 0.01;
		LOGD("dx: %f", dx);
		break;
	case 'y':
		sy = startY + dy * 0.01;
		LOGD("dy: %f", dy);
		break;
	case 'b':
		sx = startX + dx * 0.01;
		sy = startY + dy * 0.01;
		LOGD("(dx,dy)=(%f,%f)", dx, dy);
		break;
	case 't':
		px = startX + dx * 0.6;
		py = startY + dy * 0.6;
		LOGD("(px,py)=(%f,%f)", px, py);
		break;
	}
}
