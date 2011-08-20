#include <stddef.h>
#include <iostream>
#include <cmath>
#include <cassert>
#include <Eigen/Core>
#include <GL/glfw.h>
#include <mesh/mesh.h>
#include "btBulletDynamicsCommon.h"
#include "solid.h"

using namespace std;
using namespace Eigen;
using namespace Mesh;

//Rebuilds the display list
void Solid::setup_data() {
	//Grab buffers
	const Vertex *vbuffer;
	const int *ibuffer;
	int vcount, icount;
	mesh.get_buffers(
		&vbuffer,
		&vcount,
		&ibuffer,
		&icount);
		
	//Generate display list
	display_list = glGenLists(1);
	glNewList(display_list, GL_COMPILE);
	glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vbuffer[0].position);
	glNormalPointer(GL_FLOAT, sizeof(Vertex), &vbuffer[0].normal);
	glColorPointer(3, GL_FLOAT, sizeof(Vertex), &vbuffer[0].color);	
	glDrawElements(GL_TRIANGLES, icount, GL_UNSIGNED_INT, ibuffer);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
    glEndList();	

	//Update collision data
	auto index_array = new btTriangleIndexVertexArray(
		mesh.triangles().size(),
		ibuffer,
		3*sizeof(int),
		mesh.vertices().size(),
		(btScalar*)(void*)(&vbuffer[0].position),
		sizeof(Vertex));
	collision_shape = new btBvhTriangleMeshShape(index_array, true, true);
	
	//Update mass
	mass = 0.0;
	float J = 1.0 / (scale[0]*scale[1]*scale[2]);
	for(int i=0; i<resolution[0]*resolution[1]*resolution[2]; ++i) {
		if(data[i].density > 0) {
			mass += J * data[i].density;
		}
	}
}

//Draws a solid
void Solid::draw() {
    glCallList(display_list);
}

