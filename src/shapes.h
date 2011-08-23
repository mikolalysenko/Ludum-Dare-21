float torus(Vector3f const& v, float radius, float ringrad2)
{
	float val = radius - sqrt(v[0] * v[0] + v[1] * v[1]);
	return -(val * val + v[2] * v[2] - ringrad2);
}

float sphere(Vector3f const& v, float rad2)
{
	return rad2 - v.dot(v);
}

float cylinder(Vector3f const& v, Vector3f const& axis, float rad2) {
	return rad2 - (v - axis*v.dot(axis)).squaredNorm();
}

float cone(Vector3f const& v, 
	Vector3f const& axis, 
	float rad2) {
	
	float a = axis.dot(v);
	float b = (v - axis * a).squaredNorm();

	return a*a*rad2 - b;
}


float bicone(Vector3f const& v, 
	Vector3f const& axis, 
	float rad2, 
	float len) {
	
	float a = axis.dot(v);
	float b = (v - axis * a).squaredNorm();

	a = len - abs(a);
	if(a < 0) {
		return -1e10;
	}

	return a*a*rad2 - b;
}


float max_list(vector<float> const& l) {
	float m = -1e20;
	for(int i=0; i<l.size(); ++i) {
		m = max(m, l[i]);
	}
	return m;
}


Vector4f color_interp(vector<Vector4f, Eigen::aligned_allocator<Vector4f> > const& colors, float t) {
	if(t < 0)
		return colors.front();

	for(int i=1; i<colors.size(); ++i) {
		
		if(t >= colors[i][3]) {
			t -= colors[i][3];		
			continue;
		}
		
		t /= colors[i][3];
		return (1.-t)*colors[i-1] + t*colors[i];
	}
	
	return colors.back();
}


