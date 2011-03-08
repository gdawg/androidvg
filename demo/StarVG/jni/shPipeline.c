/*
 * Copyright (c) 2007 Ivan Leben
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library in the file COPYING;
 * if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#define VG_API_EXPORT
#include "openvg.h"
#include "shDefs.h"
#include "shExtensions.h"
#include "shContext.h"
#include "shPath.h"
#include "shImage.h"
#include "shGeometry.h"
#include "shPaint.h"

void shPremultiplyFramebuffer()
{
  /* Multiply target color with its own alpha */
  glBlendFunc(GL_ZERO, GL_DST_ALPHA);
}

void shUnpremultiplyFramebuffer()
{
  /* TODO: hmmmm..... any idea? */
}

void updateBlendingStateGL(VGContext *c, int alphaIsOne)
{
  /* Most common drawing mode (SRC_OVER with alpha=1)
     as well as SRC is optimized by turning OpenGL
     blending off. In other cases its turned on. */
  
  switch (c->blendMode)
  {
  case VG_BLEND_SRC:
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND); break;

  case VG_BLEND_SRC_IN:
    glBlendFunc(GL_DST_ALPHA, GL_ZERO);
    glEnable(GL_BLEND); break;

  case VG_BLEND_DST_IN:
    glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
    glEnable(GL_BLEND); break;
    
  case VG_BLEND_SRC_OUT_SH:
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
    glEnable(GL_BLEND); break;

  case VG_BLEND_DST_OUT_SH:
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND); break;

  case VG_BLEND_SRC_ATOP_SH:
    glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND); break;

  case VG_BLEND_DST_ATOP_SH:
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA);
    glEnable(GL_BLEND); break;

  case VG_BLEND_DST_OVER:
    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
    glEnable(GL_BLEND); break;

  case VG_BLEND_SRC_OVER: default:
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (alphaIsOne) glDisable(GL_BLEND);
    else glEnable(GL_BLEND); break;
  };
}

/*-----------------------------------------------------------
 * Draws the triangles representing the stroke of a path.
 *-----------------------------------------------------------*/

static void shDrawStroke(SHPath *p)
{
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, p->stroke.items);
  glDrawArrays(GL_TRIANGLES, 0, p->stroke.size);
  glDisableClientState(GL_VERTEX_ARRAY);
}

/*-----------------------------------------------------------
 * Draws the subdivided vertices in the OpenGL mode given
 * (this could be VG_TRIANGLE_FAN or VG_LINE_STRIP).
 *-----------------------------------------------------------*/

static void shDrawVertices(SHPath *p, GLenum mode)
{
  int start = 0;
  int size = 0;
  /* We separate vertex arrays by contours to properly
     handle the fill modes */
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, sizeof(SHVertex), p->vertices.items);
  while (start < p->vertices.size) {
    size = p->vertices.items[start].flags;
    glDrawArrays(mode, start, size);
    start += size;
  }
  glDisableClientState(GL_VERTEX_ARRAY);
}

/*-----------------------------------------------------------
 * Triangulate a cancave polygon 
 *-----------------------------------------------------------*/

void shTriangulation(SHVertex *vertices, int nvertices, SHVector2 *triangles, int ntriangles)
{
	SHVector2 current, left, right, internal;
	SHVector2 s1, s2, s3;
	int *plunk;
	int i, j, k;
	int cross;
	plunk = (int *) malloc(nvertices * sizeof(int));
	for (i = 0; i < nvertices; i++) {
		plunk[i] = 0;
	}
	for (i = 0; i < ntriangles; i++) {
		// Find an ear triangle and plunk it.
		for (j = 0; j < nvertices; j++) {
			if (plunk[j] == 1) {
				continue;
			}
			// Find 1st point of the triangle
			current.x = vertices[j].point.x;
			current.y = vertices[j].point.y;
			// Find 2nd point of the triangle
			k = j;
			do {
				if (k == nvertices) {
					k = 0;
				} else {
					k++;
				}
				if (plunk[k] == 0) {
					right.x = vertices[k].point.x;
					right.y = vertices[k].point.y;
					break;
				}
			} while (k != j);
			// Find 3rd point of the triangle
			k = j;
			do {
				if (k == -1) {
					k = nvertices - 1;
				} else {
					k--;
				}
				if (plunk[k] == 0) {
					left.x = vertices[k].point.x;
					left.y = vertices[k].point.y;
					break;
				}
			} while (k != j);
			// Test triangle(left, current, right) form an
			// ear triangle or not
			internal.x = (left.x + right.x) /2;
			internal.y = (left.y + right.y) /2;
			for (k = 0, cross = 0; k < nvertices; k++) {
				// Pick a segment, (s1, s2)
				s1.x = vertices[k].point.x;
				s1.y = vertices[k].point.y;
				if ((k + 1) == nvertices) {
					s2.x = vertices[0].point.x;
					s2.y = vertices[0].point.y;
				} else {
					s2.x = vertices[k + 1].point.x;
					s2.y = vertices[k + 1].point.y;
				}
				if (((s1.y > internal.y) && (s2.y < internal.y))
					|| ((s1.y < internal.y) && (s2.y > internal.y))){
					s3.y = internal.y;
					s3.x = (s3.y - s1.y) * (s1.x - s2.x) / (s1.y - s2.y) + s1.x;
					if (s3.x >= internal.x) {
						cross++;
					}
				}
			}
			if ((cross % 2) == 1) {
				// Internal(x,y) is the real internal point
				// We find one ear triangle 
				// Plunk it and assign to ouput triangle array
				plunk[j] = 1;	
				triangles[i * 3].x = current.x;
				triangles[i * 3].y = current.y;
				triangles[i * 3 + 1].x = left.x;
				triangles[i * 3 + 1].y = left.y;
				triangles[i * 3 + 2].x = right.x;
				triangles[i * 3 + 2].y = right.y;
				break;
			}
		}
	}
	free(plunk);
}

/*-----------------------------------------------------------
 * Draws the subdivided vertices to from a polygon in the OpenGL mode given
 * (this could be VG_TRIANGLE_FAN or VG_LINE_STRIP).
 *-----------------------------------------------------------*/

static void shDrawPolygon(SHPath *p, GLenum mode)
{
	int start = 0;
	int size = 0;
	SHVector2 *temp;
	SHfloat tx, ty;
	SHVector2 min, max;
	int i;
	int ntris; // number of triangles
	SHVertex *vertices;

	glEnableClientState(GL_VERTEX_ARRAY);
	while (start < p->vertices.size) {
		size = p->vertices.items[start].flags;
		ntris = size - 2; // number of triangles is the number of vertices - 2
		temp = (SHVector2 *) malloc(3 * ntris * sizeof(SHVector2));
		// Triangluate the current subsegment
		vertices = (SHVertex *) malloc(size * sizeof(SHVertex));
		for (i = 0; i < size; i++) {
			vertices[i].point.x = p->vertices.items[start + i].point.x;
			vertices[i].point.y = p->vertices.items[start + i].point.y;
		}
		shTriangulation(vertices, size, temp, ntris);
		// Draw the triangluated subsegment
		glVertexPointer(2, GL_FLOAT, sizeof(SHVector2), temp);
		glDrawArrays(GL_TRIANGLES, 0, 3 * ntris);
		free(temp);
		start += size;
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

/*-------------------------------------------------------------
 * Draw a single quad that covers the bounding box of a path
 *-------------------------------------------------------------*/

static void shDrawBoundBox(VGContext *c, SHPath *p, VGPaintMode mode)
{
	SHfloat K = 1.0f;
	if (mode == VG_STROKE_PATH) {
		K = SH_CEIL(c->strokeMiterLimit * c->strokeLineWidth) + 1.0f;
	}
#ifdef ANDROIDVG
	GLfloat v[6][2];
	v[0][0] = p->min.x-K; v[0][1] = p->min.y-K;
	v[1][0] = p->max.x+K; v[1][1] = p->min.y-K;
	v[2][0] = p->max.x+K; v[2][1] = p->max.y+K;
	v[3][0] = p->min.x-K; v[3][1] = p->min.y-K;
	v[4][0] = p->max.x+K; v[4][1] = p->max.y+K;
	v[5][0] = p->min.x-K; v[5][1] = p->max.y+K;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, v); 
	glDrawArrays(GL_TRIANGLES, 0, 6); 
	glDisableClientState(GL_VERTEX_ARRAY);
#else
	glBegin(GL_QUADS);
	glVertex2f(p->min.x-K, p->min.y-K);
	glVertex2f(p->max.x+K, p->min.y-K);
	glVertex2f(p->max.x+K, p->max.y+K);
	glVertex2f(p->min.x-K, p->max.y+K);
	glEnd();
#endif
}

/*--------------------------------------------------------------
 * Constructs & draws colored OpenGL primitives that cover the
 * given bounding box to represent the currently selected
 * stroke or fill paint
 *--------------------------------------------------------------*/

static void shDrawPaintMesh(VGContext *c, SHVector2 *min, SHVector2 *max,
                            VGPaintMode mode, GLenum texUnit)
{
  SHPaint *p;
  SHVector2 pmin, pmax;
  SHfloat K = 1.0f;
#ifdef ANDROIDVG
	SHColor *color;
	GLfloat v[6][2];
#endif
  
  /* Pick the right paint */
  if (mode == VG_FILL_PATH) {
    p = (c->fillPaint ? c->fillPaint : &c->defaultPaint);
  }else if (mode == VG_STROKE_PATH) {
    p = (c->strokePaint ? c->strokePaint : &c->defaultPaint);
    K = SH_CEIL(c->strokeMiterLimit * c->strokeLineWidth) + 1.0f;
  }
  
  /* We want to be sure to cover every pixel of this path so better
     take a pixel more than leave some out (multisampling is tricky). */
  SET2V(pmin, (*min)); SUB2(pmin, K,K);
  SET2V(pmax, (*max)); ADD2(pmax, K,K);

  /* Construct appropriate OpenGL primitives so as
     to fill the stencil mask with select paint */

  switch (p->type) {
  case VG_PAINT_TYPE_LINEAR_GRADIENT:
    shDrawLinearGradientMesh(p, min, max, mode, texUnit);
    break;

  case VG_PAINT_TYPE_RADIAL_GRADIENT:
    shDrawRadialGradientMesh(p, min, max, mode, texUnit);
    break;
    
  case VG_PAINT_TYPE_PATTERN:
    if (p->pattern != VG_INVALID_HANDLE) {
      shDrawPatternMesh(p, min, max, mode, texUnit);
      break;
    }/* else behave as a color paint */
  
  case VG_PAINT_TYPE_COLOR:
#ifdef ANDROIDVG
	v[0][0] = pmin.x; v[0][1] = pmin.y;
	v[1][0] = pmax.x; v[1][1] = pmin.y;
	v[2][0] = pmax.x; v[2][1] = pmax.y;
	v[3][0] = pmin.x; v[3][1] = pmin.y;
	v[4][0] = pmax.x; v[4][1] = pmax.y;
	v[5][0] = pmin.x; v[5][1] = pmax.y;
	color = &p->color;
	glColor4f(color->r, color->g, color->b, color->a);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, v); 
	glDrawArrays(GL_TRIANGLES, 0, 6); 
	glDisableClientState(GL_VERTEX_ARRAY);
#else
	glColor4fv((GLfloat*)&p->color);
	glBegin(GL_QUADS);
	glVertex2f(pmin.x, pmin.y);
	glVertex2f(pmax.x, pmin.y);
	glVertex2f(pmax.x, pmax.y);
	glVertex2f(pmin.x, pmax.y);
	glEnd();
#endif
    break;
  }
}

/*-----------------------------------------------------------
 * Tessellates / strokes the path and draws it according to
 * VGContext state.
 *-----------------------------------------------------------*/

VG_API_CALL void vgDrawPath(VGPath path, VGbitfield paintModes)
{
  SHPath *p;
  SHMatrix3x3 mi;
  SHfloat mgl[16];
  SHPaint *fill, *stroke;
  SHRectangle *rect;

  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(!shIsValidPath(context, path),
                   VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(paintModes & (~(VG_STROKE_PATH | VG_FILL_PATH)),
                   VG_ILLEGAL_ARGUMENT_ERROR, VG_NO_RETVAL);

  /* Check whether scissoring is enabled and scissor
     rectangle is valid */
  if (context->scissoring == VG_TRUE) {
    rect = &context->scissor.items[0];
    if (context->scissor.size == 0) VG_RETURN( VG_NO_RETVAL );
    if (rect->w <= 0.0f || rect->h <= 0.0f) VG_RETURN( VG_NO_RETVAL );
    glScissor( (GLint)rect->x, (GLint)rect->y, (GLint)rect->w, (GLint)rect->h );
    glEnable( GL_SCISSOR_TEST );
  }
  
  p = (SHPath*)path;
  
  /* If user-to-surface matrix invertible tessellate in
     surface space for better path resolution */
  if (shInvertMatrix(&context->pathTransform, &mi)) {
    shFlattenPath(p, 1);
    shTransformVertices(&mi, p);
  }else shFlattenPath(p, 0);
  shFindBoundbox(p);
  
  /* TODO: Turn antialiasing on/off */
  glDisable(GL_LINE_SMOOTH);
#ifndef ANDROIDVG
  glDisable(GL_POLYGON_SMOOTH);
#endif
  glEnable(GL_MULTISAMPLE);
  
  /* Pick paint if available or default*/
  fill = (context->fillPaint ? context->fillPaint : &context->defaultPaint);
  stroke = (context->strokePaint ? context->strokePaint : &context->defaultPaint);
  
  /* Apply transformation */
  shMatrixToGL(&context->pathTransform, mgl);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf(mgl);
  
  if (paintModes & VG_FILL_PATH) {
#ifdef ANDROIDVG
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthRangef(0, 1);
	glDepthFunc(GL_ALWAYS);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//	glColor4f(1,0,0,1);
//	shDrawVertices(p, GL_TRIANGLE_FAN);
	shDrawPolygon(p, GL_TRIANGLE_FAN);

	// Setup blending
	updateBlendingStateGL(context,
		fill->type == VG_PAINT_TYPE_COLOR &&
		fill->color.a == 1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthFunc(GL_EQUAL);
	shDrawPaintMesh(context, &p->min, &p->max, VG_FILL_PATH, GL_TEXTURE0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_MULTISAMPLE);
	glDisable(GL_BLEND);
#else
    /* Tesselate into stencil */
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    shDrawVertices(p, GL_TRIANGLE_FAN);
    /* Setup blending */
    updateBlendingStateGL(context,
                          fill->type == VG_PAINT_TYPE_COLOR &&
                          fill->color.a == 1.0f);
    /* Draw paint where stencil odd */
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    shDrawPaintMesh(context, &p->min, &p->max, VG_FILL_PATH, GL_TEXTURE0);
    /* Clear stencil for sure */
    /* TODO: Is there any way to do this safely along
       with the paint generation pass?? */
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    shDrawBoundBox(context, p, VG_FILL_PATH);
    /* Reset state */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
#endif 
  }
  
  /* TODO: Turn antialiasing on/off */
  glDisable(GL_LINE_SMOOTH);
#ifndef ANDROIDVG
	glDisable(GL_POLYGON_SMOOTH);
#endif
  glEnable(GL_MULTISAMPLE);
  
  if ((paintModes & VG_STROKE_PATH) && context->strokeLineWidth > 0.0f) {
    
    if (1) {/*context->strokeLineWidth > 1.0f) {*/
	    //shc

      /* Generate stroke triangles in path space */
      shVector2ArrayClear(&p->stroke);
      shStrokePath(context, p);
#ifdef ANDROIDVG
	glClearDepthf(1.0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthRangef(0, 1);
	glDepthFunc(GL_ALWAYS);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	shDrawStroke(p);

	// Setup blending 
	updateBlendingStateGL(context,
	stroke->type == VG_PAINT_TYPE_COLOR &&
	stroke->color.a == 1.0f);

	glDepthFunc(GL_EQUAL);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	shDrawPaintMesh(context, &p->min, &p->max, VG_STROKE_PATH, GL_TEXTURE0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_MULTISAMPLE);
	glDisable(GL_BLEND);

#else
      /* Stroke into stencil */
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_NOTEQUAL, 1, 1);
      glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      shDrawStroke(p);

      /* Setup blending */
      updateBlendingStateGL(context,
                            stroke->type == VG_PAINT_TYPE_COLOR &&
                            stroke->color.a == 1.0f);

      /* Draw paint where stencil odd */
      glStencilFunc(GL_EQUAL, 1, 1);
      glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      shDrawPaintMesh(context, &p->min, &p->max, VG_STROKE_PATH, GL_TEXTURE0);
      
      /* Clear stencil for sure */
      glDisable(GL_BLEND);
      glDisable(GL_MULTISAMPLE);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      shDrawBoundBox(context, p, VG_STROKE_PATH);
      
      /* Reset state */
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glDisable(GL_STENCIL_TEST);
      glDisable(GL_BLEND);
#endif 
    }else{
      
      /* Simulate thin stroke by alpha */
      SHColor c = stroke->color;
      if (context->strokeLineWidth < 1.0f)
        c.a *= context->strokeLineWidth;
      
      /* Draw contour as a line */
      glDisable(GL_MULTISAMPLE);
      glEnable(GL_BLEND);
      glEnable(GL_LINE_SMOOTH);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glColor4f(c.r, c.g, c.b, c.a);
      shDrawVertices(p, GL_LINE_STRIP);
      glDisable(GL_BLEND);
      glDisable(GL_LINE_SMOOTH);
    }
  }
  
  
  glPopMatrix();
  
  if (context->scissoring == VG_TRUE)
    glDisable( GL_SCISSOR_TEST );

  VG_RETURN(VG_NO_RETVAL);
}

VG_API_CALL void vgDrawImage(VGImage image)
{
  SHImage *i;
  SHfloat mgl[16];
  SHfloat texGenS[4] = {0,0,0,0};
  SHfloat texGenT[4] = {0,0,0,0};
  SHPaint *fill;
  SHVector2 min, max;
  SHRectangle *rect;
  
  VG_GETCONTEXT(VG_NO_RETVAL);
  
  VG_RETURN_ERR_IF(!shIsValidImage(context, image),
                   VG_BAD_HANDLE_ERROR, VG_NO_RETVAL);

  /* TODO: check if image is current render target */
  
  /* Check whether scissoring is enabled and scissor
     rectangle is valid */
  if (context->scissoring == VG_TRUE) {
    rect = &context->scissor.items[0];
    if (context->scissor.size == 0) VG_RETURN( VG_NO_RETVAL );
    if (rect->w <= 0.0f || rect->h <= 0.0f) VG_RETURN( VG_NO_RETVAL );
    glScissor( (GLint)rect->x, (GLint)rect->y, (GLint)rect->w, (GLint)rect->h );
    glEnable( GL_SCISSOR_TEST );
  }
  
  /* Apply image-user-to-surface transformation */
  i = (SHImage*)image;
  shMatrixToGL(&context->imageTransform, mgl);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf(mgl);
  
  /* Clamp to edge for proper filtering, modulate for multiply mode */
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, i->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  /* Adjust antialiasing to settings */
  if (context->imageQuality == VG_IMAGE_QUALITY_NONANTIALIASED) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glDisable(GL_MULTISAMPLE);
  }else{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glEnable(GL_MULTISAMPLE);
  }
  
  /* Generate image texture coords automatically */
  texGenS[0] = 1.0f / i->texwidth;
  texGenT[1] = 1.0f / i->texheight;

#ifdef ANDROIDVG
  LOGD("vgDrawImage(): glTexGeni failed!\n");
//shc	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//shc	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
//shc	glTexGenfv(GL_S, GL_OBJECT_PLANE, texGenS);
//shc	glTexGenfv(GL_T, GL_OBJECT_PLANE, texGenT);
//shc	glEnable(GL_TEXTURE_GEN_S);
//shc	glEnable(GL_TEXTURE_GEN_T);
#else
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, texGenS);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, texGenT);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
#endif
  /* Pick fill paint */
  fill = (context->fillPaint ? context->fillPaint : &context->defaultPaint);
  
  /* Use paint color when multiplying with a color-paint */
  if (context->imageMode == VG_DRAW_IMAGE_MULTIPLY &&
      fill->type == VG_PAINT_TYPE_COLOR) {
#ifdef ANDROIDVG
      glColor4f((fill->color).r, (fill->color).g, (fill->color).b, (fill->color).a);
#else
      glColor4fv((GLfloat*)&fill->color);
#endif
  } else { 
	  glColor4f(1,1,1,1); 
  }
  
  
  /* Check image drawing mode */
  if (context->imageMode == VG_DRAW_IMAGE_MULTIPLY &&
      fill->type != VG_PAINT_TYPE_COLOR) {
    
    /* Draw image quad into stencil */
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

#ifdef ANDROIDVG
    LOGD("vgDrawImage(): draw GL_QUADS failed!\n");
/*shc
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(i->width, 0);
    glVertex2i(i->width, i->height);
    glVertex2i(0, i->height);
    glEnd();
shc*/
#else
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(i->width, 0);
    glVertex2i(i->width, i->height);
    glVertex2i(0, i->height);
    glEnd();
#endif

    /* Setup blending */
    updateBlendingStateGL(context, 0);
    
    /* Draw gradient mesh where stencil 1*/
    glEnable(GL_TEXTURE_2D);
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_ZERO,GL_ZERO,GL_ZERO);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    
    SET2(min,0,0);
    SET2(max, (SHfloat)i->width, (SHfloat)i->height);
    if (fill->type == VG_PAINT_TYPE_RADIAL_GRADIENT) {
      shDrawRadialGradientMesh(fill, &min, &max, VG_FILL_PATH, GL_TEXTURE1);
    }else if (fill->type == VG_PAINT_TYPE_LINEAR_GRADIENT) {
      shDrawLinearGradientMesh(fill, &min, &max, VG_FILL_PATH, GL_TEXTURE1);
    }else if (fill->type == VG_PAINT_TYPE_PATTERN) {
      shDrawPatternMesh(fill, &min, &max, VG_FILL_PATH, GL_TEXTURE1); }
    
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_STENCIL_TEST);
    
  }else if (context->imageMode == VG_DRAW_IMAGE_STENCIL) {
    
    
  }else{/* Either normal mode or multiplying with a color-paint */
    
    /* Setup blending */
    updateBlendingStateGL(context, 0);

    /* Draw textured quad */
    glEnable(GL_TEXTURE_2D);
#ifdef ANDROIDVG
    LOGD("vgDrawImage(): draw GL_QUADS failed! in else\n");
/*shc
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(i->width, 0);
    glVertex2i(i->width, i->height);
    glVertex2i(0, i->height);
    glEnd();
shc*/
#else
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(i->width, 0);
    glVertex2i(i->width, i->height);
    glVertex2i(0, i->height);
    glEnd();
#endif
    glDisable(GL_TEXTURE_2D);
  }
  
#ifndef ANDROIDVG 
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
#endif
  glPopMatrix();

  if (context->scissoring == VG_TRUE)
    glDisable( GL_SCISSOR_TEST );
  
  VG_RETURN(VG_NO_RETVAL);
}
