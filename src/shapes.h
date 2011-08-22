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

