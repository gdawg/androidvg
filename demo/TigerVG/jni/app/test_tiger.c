/*
 * Copyright (C) 2010 Shuo-Hung Chen, Hsiao-Mei Lin
 * 
 * The AndroidVG Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifdef USE_GL
#undef USE_GL
#endif

#include "test.h"
#include <ctype.h>


extern const VGint     pathCount;
extern const VGint     commandCounts[];
extern const VGubyte*  commandArrays[];
extern const VGfloat*  dataArrays[];
extern const VGfloat*  styleArrays[];

VGPath *tigerPaths = NULL;
VGPaint tigerStroke;
VGPaint tigerFill;

VGfloat sx=1.0f, sy=1.0f;
VGfloat tx=1.0f, ty=1.0f;
VGfloat ang = 0.0f;
int animate = 1;
char mode = 'z';

VGfloat startX = 0.0f;
VGfloat startY = 0.0f;
VGfloat clickX = 0.0f;
VGfloat clickY = 0.0f;

const char commands[] =
  "Click & drag mouse to change\n"
  "value for current mode\n\n"
  "H - this help\n"
  "Z - zoom mode\n"
  "P - pan mode\n"
  "SPACE - animation pause\\play\n";

//Display functions
void display(float interval)
{
  int i;
  const VGfloat *style;
  VGfloat clearColor[] = {1,1,1,1};
  
  if (animate) {
    ang += interval * 360 * 0.1f;
    if (ang > 360) ang -= 360;
  }
  
  vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
  vgClear(0,0,testWidth(),testHeight());
  
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgLoadIdentity();
  vgTranslate(testWidth()/2 + tx,testHeight()/2 + ty);
  vgScale(sx, sy);
  vgRotate(ang);
  
  for (i=0; i<pathCount; ++i) {
    
    style = styleArrays[i];
    vgSetParameterfv(tigerStroke, VG_PAINT_COLOR, 4, &style[0]);
    vgSetParameterfv(tigerFill, VG_PAINT_COLOR, 4, &style[4]);
    vgSetf(VG_STROKE_LINE_WIDTH, style[8]);
    vgDrawPath(tigerPaths[i], (VGint)style[9]); // Bingo!!, Draw it!! 
  }
  vgFlush();
}

void updateOverlayString()
{
  switch (mode) {
  case 'z':
    testOverlayString("Zoom Mode"); break;
  case 'p':
    testOverlayString("Pan Mode"); break;
  }
}

void drag(int x, int y)
{
  VGfloat dx, dy;
  y = testHeight() - y;
  dx = x - clickX;
  dy = y - clickY;
  
  switch (mode) {
  case 'z':
    sx = startY + dy * 0.01;
    sy = sx;
    break;
  case 'p':
    tx = startX + dx;
    ty = startY + dy;
    break;
  }
  
  updateOverlayString();
}

void click(int button, int state, int x, int y)
{
  y = testHeight() - y;
  clickX = x; clickY = y;
  
  switch (mode) {
  case 'z':
    startY = sx;
    break;
  case 'p':
    startX = tx;
    startY = ty;
    break;
  }
}

void key(unsigned char code, int x, int y)
{
  switch (tolower(code)) {
  case 'z':
  case 'p':
    break;
    
  case ' ':
    /* Toggle animation */
    animate = !animate;
    testOverlayString("%s\n", animate ? "Play" : "Pause");
    return;
    
  case 'h':
    /* Show help */
    testOverlayString(commands);
    return;
    
  default:
    return;
  }
  
  /* Switch mode */
  mode = tolower(code);
  updateOverlayString();
}

void loadTiger()
{
  int i;
  VGPath temp;
  
  temp = testCreatePath();  
  tigerPaths = (VGPath*)malloc(pathCount * sizeof(VGPath));
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
  vgTranslate(-100,100);
  vgScale(1,-1);
  
  for (i=0; i<pathCount; ++i) {
    
    vgClearPath(temp, VG_PATH_CAPABILITY_ALL);
    vgAppendPathData(temp, commandCounts[i],
                     commandArrays[i], dataArrays[i]);
    
    tigerPaths[i] = testCreatePath();
    vgTransformPath(tigerPaths[i], temp);
  }
  
  tigerStroke = vgCreatePaint();
  tigerFill = vgCreatePaint();
  vgSetPaint(tigerStroke, VG_STROKE_PATH);
  vgSetPaint(tigerFill, VG_FILL_PATH);
  vgLoadIdentity();
  vgDestroyPath(temp);
}

void cleanup()
{
  free(tigerPaths);
}

int tiger()
{
  int argc;
  char **argv;
  //Initial windows(glut) and some gl projection
  testInit(argc, argv, 320 ,480, "ShivaVG: Tiger SVG Test");

#ifdef USE_GL
  //callbacks[type] = func
  testCallback(TEST_CALLBACK_CLEANUP, (CallbackFunc)cleanup);
  testCallback(TEST_CALLBACK_DISPLAY, (CallbackFunc)display);
  testCallback(TEST_CALLBACK_KEY, (CallbackFunc)key);
  testCallback(TEST_CALLBACK_BUTTON, (CallbackFunc)click);
  testCallback(TEST_CALLBACK_DRAG, (CallbackFunc)drag);
#endif //USE_GL
  
  time_t stime, etime;
  double tpp;
  double fps;

	loadTiger();
	display(0.05);

#ifdef USE_GL 
  testOverlayString("Press H for a list of commands");
  testRun();
#endif //USE_GL

  return EXIT_SUCCESS;
}



jint Java_com_example_tigervg_DemoRenderer_nativeStart( JNIEnv*  env, jobject  this, jint width, jint height) 
{
	int argc;
	char **argv;
	LOGD("Before testInit....");
	testInit(argc, argv, width ,height, "ShivaVG: Tiger SVG Test");
	LOGD("After testInit....");
	loadTiger();
	return 0; 
}

jint Java_com_example_tigervg_DemoRenderer_nativeDisplay( JNIEnv*  env, jobject  this) 
{
	display(0.05);
	return 0; 
}


JNIEXPORT void JNICALL
Java_com_example_startvg_StarVG_nativeDestroy(JNIEnv* env, jobject this)
{
	testCleanup();
}

JNIEXPORT void JNICALL
Java_com_example_startvg_StarVG_nativeChmod(JNIEnv* env, jobject this)
{
	animate=(animate+1)%2;
}








