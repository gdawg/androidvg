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
int monotone_check(SHVertex *vertices, int nvertices)
{
	int i, j, k;
	int leftmost_vertex = 0;
	int rightmost_vertex = 0;
	SHVector2 leftmost, rightmost, tmp;
	leftmost.x = vertices[0].point.x;
	rightmost.x = vertices[0].point.x;
	
	//Find the leftmost & rightmost vertices in the path
	for(i = 1; i < nvertices; i++){
		if(vertices[i].point.x < leftmost.x) {
			leftmost.x = vertices[i].point.x;
			leftmost_vertex = i;
		}
		else if(vertices[i].point.x > rightmost.x) {
			rightmost.x = vertices[i].point.x;
			rightmost_vertex = i;
		}
	}

	i = leftmost_vertex;
	tmp.x = vertices[i--].point.x;
	while(i != rightmost_vertex){
		if(vertices[i].point.x <= tmp.x)
			return 0;
		tmp.x = vertices[i].point.x;
		i = (i + nvertices -1) % nvertices;
	} 

	i = leftmost_vertex;
        tmp.x = vertices[i++].point.x;
	while(i != rightmost_vertex){

		if(vertices[i].point.x <= tmp.x)
                        return 0;
        tmp.x = vertices[i].point.x;
        i = (i + 1) % nvertices;
    }
	return 1;
}
 
 int LeftOnRight(SHVector2 a, SHVector2 b, SHVector2 c )
{
	float area = a.x*b.y - a.y*b.x +
		     a.y*c.x - a.x*c.y +
		     b.x*c.y - c.x*b.y;
	if(area>0)		return 0;//left
	else if (area=0)	return 2;//on the line
	else 			return 1;//right
}
 
 void shMonoTriangulation(SHVertex *vertices, int nvertices, SHVector2 *triangles, int ntriangles)
{
	//LOGD("Monotone polygon triangulation");
	// * Not finish yet
	int i, j, k;
	int leftmost_vertex = 0;
	int rightmost_vertex = 0;
	SHVector2 leftmost, rightmost, privious, next;
	leftmost.x = vertices[0].point.x;
	rightmost.x = vertices[0].point.x;
	
	//Find the leftmost & rightmost vertices in the path
	for(i = 1; i < nvertices; i++){
		if(vertices[i].point.x < leftmost.x) {
			leftmost.x = vertices[i].point.x;
			leftmost_vertex = i;
		}
		else if(vertices[i].point.x > rightmost.x) {
			rightmost.x = vertices[i].point.x;
			rightmost_vertex = i;
		}
	}

	int *chain = (int*)malloc(nvertices * sizeof(int));
	next = vertices[(leftmost_vertex + 1) % nvertices].point;
	privious = vertices[(leftmost_vertex + nvertices -1) % nvertices].point;
	int clockwise = next.y > privious.y? 1: 0;
	
	// 0:upper chain  1:lower chain  2:start  3:end
	for(i=0; i<nvertices; i++){
		chain[i] = 0;
	}
	i = leftmost_vertex;
	while(i != rightmost_vertex){
		chain[i] = 1;
		if(clockwise)	i = (i + nvertices -1) % nvertices;
		else		i = (i + 1) % nvertices;
	} 
	chain[leftmost_vertex] = 2;
	chain[rightmost_vertex] = 3;
	
	int *sort = (int*)malloc(nvertices * sizeof(int));
	sort[0] = leftmost_vertex;
	int p1, p2;

	p1 = (leftmost_vertex+1) % nvertices;
	p2 = (leftmost_vertex + nvertices -1) % nvertices;
	i = 1;
	while(i<nvertices){
		if(vertices[p1].point.x < vertices[p2].point.x){
			sort[i] = p1;
			p1 = (p1==nvertices -1)? 0: p1+1;
		}
		else{
			sort[i] = p2;
			p2 = (p2==0)? nvertices-1: p2-1;
		}
		i++;
	}

	//starting tiangulation
	//int *tri = triangles;
	j = 0;
	int *stack = (int*)malloc(nvertices * sizeof(int));
	int top, current;
	SHVector2 v1, v2, v3;
	stack[0] = sort[0];
	stack[1] = sort[1];
	top = 1;
	for(i=2; i<nvertices; i++){

		current = sort[i];
		if(chain[current] != chain[ stack[top] ]){
			k = stack[top];
			while(top!=0){
				triangles[j*3] = vertices[current].point;
				triangles[j*3 + 1] = vertices[stack[top]].point;
				triangles[j*3 + 2] = vertices[stack[top-1]].point;
				top--;
				j++;
			}
			stack[0] = k;
			stack[1] = current;
			top = 1;
		}
		else{
			v1 = vertices[stack[top-1]].point;
			v2 = vertices[stack[top]].point;
			v3 = vertices[current].point;
			
			if( (chain[ stack[top] ]==0&& LeftOnRight(v1, v2, v3)==0)  ||  (chain[stack[top]]==1&& LeftOnRight(v1, v2, v3)==1)){
				stack[top+1] = current;
				top++;
			}
			else if(LeftOnRight(v1, v2, v3)==2){
                           	stack[top+1] = current;
	                     	top++;
			}
			else{
				do{
					
					triangles[j*3] = vertices[current].point;
					triangles[j*3 + 1] = vertices[stack[top]].point;
					triangles[j*3 + 2] = vertices[stack[top-1]].point;
					top--;
					j++;

					if(top==0)	break;

					v1 = vertices[stack[top-1]].point;
					v2 = vertices[stack[top]].point;
				}while( !((chain[ stack[top] ]==0&& LeftOnRight(v1, v2, v3)==0)  ||  (chain[stack[top]]==1&& LeftOnRight(v1, v2, v3)==1)) );
				stack[top+1] = current;
				top++;
			}
		}
	}
	free(chain);
	free(sort);
	free(stack);
}
  
void shTriangulation(SHVertex *vertices, int nvertices, SHVector2 *triangles, int ntriangles)
{
	//LOGD("Ear-clipping triangulation");
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
		shMonoTriangulation(vertices, size, temp, ntris);
		/*
		if(monotone_check(vertices, size)){
			shMonoTriangulation(vertices, size, temp, ntris);
		}
		else{
			shTriangulation(vertices, size, temp, ntris);
		}*/
		// Draw the triangluated subsegment
		glVertexPointer(2, GL_FLOAT, sizeof(SHVector2), temp);
		glDrawArrays(GL_TRIANGLES, 0, 3 * ntris);
		free(temp);
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
