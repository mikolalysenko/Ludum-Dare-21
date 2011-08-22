struct SpikeballModelFunc {
	Cell operator()(Eigen::Vector3f v) const {
		using namespace Eigen;
		Cell result;
		result.density = 
			max_list({
			bicone(v, Vector3f(1, 0, 0), 0.2, 2),
			bicone(v, Vector3f(0, 1, 0), 0.2, 2),
			bicone(v, Vector3f(0, 0, 1), 0.2, 2)});
		result.friction = 0;
		return result;
	}
};

struct SpikeballStyleFunc {
	Vertex operator()(Eigen::Vector3f const& v) const {
		using namespace Eigen;
		
		Vertex result;
		result.position = v;
		
		float t = sin(v.norm());
		result.color = Vector3f(1, 0.1, 0.2)*t + (1.-t)*Vector3f(0.8,0.7,0.2);
		return result;
	}
};

Solid* make_spikeball() {
	SpikeballModelFunc func;
	SpikeballStyleFunc style;
	auto model = new Solid(
		Vector3i(16, 16, 16),
		Vector3f(-2, -2, -2),
		Vector3f( 2,  2,  2));
	setup_solid(*model, func, style);
	
	return model;
}


