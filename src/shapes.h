float torus(Vector3f const& v, float radius, float ringrad2)
{
	float val = radius - sqrt(v[0] * v[0] + v[1] * v[1]);
	return -(val * val + v[2] * v[2] - ringrad2);
}