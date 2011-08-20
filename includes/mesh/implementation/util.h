#ifndef MESH_UTIL_H
#define MESH_UTIL_H

#include <cstdlib>
#include <stdint.h>
#include <unordered_map>

#include <Eigen/Core>

#define FP_TOLERANCE 	1e-6

namespace Mesh {
namespace impl {

	struct EmptyStruct {};

	struct ScopedFree {
		ScopedFree() : ptr(NULL) {}
		ScopedFree(int sz) : ptr(malloc(sz)) {}
		ScopedFree(void* ptr_) : ptr(ptr_) {}
		~ScopedFree() {
			if(ptr) free(ptr);
		}
		void* ptr;			
	};
	
	template<typename T> struct ScopedDelete {
		ScopedDelete() : ptr(NULL) {}
		ScopedDelete(T* ptr_) : ptr(ptr_) {}
		~ScopedDelete() {
			if(ptr) delete ptr;
		}
		T* ptr;
	};

	template<typename T> struct ScopedArray {
		ScopedArray() : ptr(NULL) {}
		ScopedArray(int n) : ptr(new T[n]) {}
		ScopedArray(T* ptr_) : ptr(ptr_) {}
		~ScopedArray() {
			if(ptr) delete[] ptr;
		}
		T* ptr;
	};
	
	/**
	 * 3D Z-order spatial hash.
	 */
	template<typename Coord>
	struct ZOrderHash {
		static uint64_t expand(uint64_t x) {
			x &= 0xFFFFFFFFULL;
			
			x = (x | (x << 32)) & 18446462598732906495ULL;  //0b1111111111111111000000000000000000000000000000001111111111111111ULL
			x = (x | (x << 16)) & 71776123339407615ULL;     //0b0000000011111111000000000000000011111111000000000000000011111111ULL
			x = (x | (x << 8))  & 17298045724797235215ULL;	//0b1111000000001111000000001111000000001111000000001111000000001111ULL
			x = (x | (x << 4))  & 3513665537849438403ULL;   //0b0011000011000011000011000011000011000011000011000011000011000011ULL
			x = (x | (x << 2))  & 10540996613548315209ULL;  //0b1001001001001001001001001001001001001001001001001001001001001001ULL
			
			return x;
		}
			
		size_t operator()(const Coord& coord) const {
			return (size_t)(expand(coord[0]) | (expand(coord[1]) << 1) | (expand(coord[2]) << 2));
		}
	};
	
	//Type alias for a spatial grid
	template<typename ValueType> 
	struct SpatialGrid {
		typedef std::unordered_map<
			Eigen::Vector3i,
			ValueType,
			ZOrderHash<Eigen::Vector3i>,
			std::equal_to<Eigen::Vector3i>,
			Eigen::aligned_allocator< std::pair<const Eigen::Vector3i, ValueType> > > type;
	};
	
}; };

#endif

