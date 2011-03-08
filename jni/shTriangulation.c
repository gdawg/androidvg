/*
 * Copyright (c) 2010 Shuo-Hung Chen, Hsiao-Mei Lin
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
#include "shTriangulation.h"


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

void shDrawPolygon(SHPath *p, GLenum mode)
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
		free(vertices);
		start += size;
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}


/*-----------------------------------------------------------
 * Draw a single rectangle with floating point vertices
 *-----------------------------------------------------------*/
void shDrawQuad2f( GLfloat x0, GLfloat y0, 
			  GLfloat x1, GLfloat y1, 
			  GLfloat x2, GLfloat y2,
			  GLfloat x3, GLfloat y3 ) 
{
#ifdef ANDROIDVG
	GLfloat v[4][2];
	v[0][0] = x0; v[0][1] = y0;
	v[1][0] = x1; v[1][1] = y1;
	v[2][0] = x2; v[2][1] = y2;
	v[3][0] = x3; v[3][1] = y3;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, v); 
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); 
	glDisableClientState(GL_VERTEX_ARRAY);
#else
	glBegin(GL_QUADS);
	glVertex2f(x0, y0);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
#endif
}
