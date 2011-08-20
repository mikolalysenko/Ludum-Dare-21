#ifndef MESH_ATTRIBUTE_H
#define MESH_ATTRIBUTE_H

#include <Eigen/Core>

namespace Mesh {

/// Generic vertex position attributes
template<typename T> struct PositionAttribute {
	Eigen::Vector3f const& get(T const& v) const { return v.position; }
	void set(T& v, Eigen::Vector3f const& p) const { v.position = p; }
};

/// Generic vertex normal attribute
template<typename T> struct NormalAttribute {
	Eigen::Vector3f const& get(T const& v) const { return v.normal; }
	void set(T& v, Eigen::Vector3f const& p) const { v.normal = p; }
};

/// Generic tolerance attribute
template<typename T> struct ToleranceAttribute {
	float get(T const& v) const { return v.tolerance; }
	void set(T& v, float t) const { v.tolerance = t; }
};

/// Special hard coded position attribute for 3d vectors
template<> struct PositionAttribute<Eigen::Vector3f> {
	Eigen::Vector3f const& get(Eigen::Vector3f const& v) const { return v; }
	void set(Eigen::Vector3f& v, Eigen::Vector3f const& p) const { v = p; }
};

};

#endif

