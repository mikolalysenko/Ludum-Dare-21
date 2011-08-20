#include <stddef.h>
#include <iostream>
#include <cmath>
#include <cassert>
#include <Eigen/Core>
#include <GL/glfw.h>
#include <mesh/mesh.h>
#include "solid.h"

using namespace std;
using namespace Eigen;
using namespace Mesh;

//Initialize solid
void Solid::init() {
	display_list = glGenLists(1);
}

//Reset solid
void Solid::reset() {
	mesh.clear();
}

//Rebuilds the display list
void Solid::build_display_lists() {
	//Grab buffers
	const Vertex *vbuffer;
	const int *ibuffer;
	int vcount, icount;
	mesh.get_buffers(
		&vbuffer,
		&vcount,
		&ibuffer,
		&icount);
		
	//Regenerate display list
	glNewList(display_list, GL_COMPILE);
	glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), vbuffer);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &vbuffer[0].normal);
	glColorPointer(3, GL_FLOAT, sizeof(Vertex), &vbuffer[0].color);	
	glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_INT, ibuffer);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
    glEndList();	
}

//Draws a solid
void Solid::draw() {
    glCallList(display_list);
}

