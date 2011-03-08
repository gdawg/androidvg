#ifndef __TEST_H
#define __TEST_H

#include "test.h"
#include <ctype.h>

#include <jni.h>

VGPaint testStroke;
VGPaint testFill;
VGPath testPath;
VGfloat phase=0.0f;

VGint jindex = 0;
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

VGfloat ssx=1.0f, ssy=1.0f;

VGfloat sstartX=0.0f;
VGfloat sstartY=0.0f;
VGfloat sclickX=0.0f;
VGfloat sclickY=0.0f;

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
  vgSetf(VG_STROKE_JOIN_STYLE, joins[jindex]);
  vgSetf(VG_STROKE_CAP_STYLE, caps[cindex]);
  
  vgLoadIdentity();
  vgTranslate(testWidth()/2,testHeight()/2);
  vgScale(3.0 * ssx, 3.0 * ssy);
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

jint Java_com_example_tigervg_DemoRenderer_nativeDashStart( JNIEnv*  env, jobject  this) 
{
	int argc;
	char **argv;
	testInit(argc, argv, 320,480, "ShivaVG: Dash Pattern Test");
	createStar();
	return 0; 
}

jint Java_com_example_tigervg_DemoRenderer_nativeDashDisplay( JNIEnv*  env, jobject  this) 
{
	dash_display(0.05);
	return 0; 
}

#endif /* __TEST_H */
