#include <stddef.h>
#include <iostream>
#include <cmath>
#include <cassert>
#include <Eigen/Core>
#include <GL/glfw.h>
#include <mesh/mesh.h>

#include "solid.h"
#include "surface_coordinate.h"

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

IntrinsicCoordinate Solid::random_point() {

	if(mesh.triangles().size() == 0) {
		return IntrinsicCoordinate(-1, Vector3f(0,0,0), NULL);
	}

	int rnd_tri = rand() % mesh.triangles().size();
	auto tr = mesh.triangle(rnd_tri);
	
	float a = drand48();
	float b = (1. - a) * drand48();
	float c = 1. - a - b;
	
	auto p = mesh.vertex(tr.v[0]).position*a + 
			 mesh.vertex(tr.v[1]).position*b +
			 mesh.vertex(tr.v[2]).position*c;
	
	return IntrinsicCoordinate(rnd_tri, p, this);
}


IntrinsicCoordinate Solid::closest_point(Eigen::Vector3f const& p) {

	IntrinsicCoordinate closest_coord(-1, p, NULL);
	float d = 1e20;
	
	for(int i=mesh.triangles().size()-1; i>=0; --i) {
		IntrinsicCoordinate tmp(i, p, this);
		
		//Get closest point
		auto verts = tmp.triangle_vertices();
		Vector3f du, dv, n;
		tmp.tangent_space(du, dv, n);
		auto mu = IntrinsicCoordinate::barycentric(p, n, du, dv, verts[0]);
		IntrinsicCoordinate::clamp_barycentric(mu);
		tmp.position = mu[0] * verts[0] + mu[1] * verts[1] + mu[2] * verts[2];
		
		//Check distance
		float nd = (tmp.position - p).norm();
		if(nd < d) {
			d = nd;
			closest_coord = tmp;
		}
	}

	return closest_coord;
}

