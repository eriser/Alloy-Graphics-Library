/*
* Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#ifndef ALLOYPOINTLOCATOR_H_
#define ALLOYPOINTLOCATOR_H_
#include "libkdtree/kdtree.h"
#include "AlloyMath.h"
#include "AlloyVector.h"
#include "nanoflann.h"

namespace aly {
struct float2i : public float2{
		typedef float value_type;
		int64_t index;
		float2i():float2(), index(-1) {
		}
		float2i(const float2i& pt):float2(pt),index(pt.index) {

		}
		float operator[](size_t n) const {
			return (&x)[n];
		}
		float2i(const float2& pos, int64_t index):float2(pos) {
			this->index = index;
		}
		double distance(const float2i &node) const {
			return std::max(std::abs(x - node.x), std::abs(y - node.y));
		}
	};
struct float3i : public float3 {
	typedef float value_type;
	int64_t index;
	float3i() :float3(),index(-1) {
	}
	float3i(const float3i& pt) :float3(pt), index(pt.index) {

	}
	float operator[](size_t n) const {
		return (&x)[n];
	}
	float3i(const float3& pos, int64_t index) :float3(pos) {
		this->index = index;
	}
	double distance(const float3i &node) const {
		return std::max(std::abs(z-node.z),std::max(std::abs(x - node.x), std::abs(y - node.y)));
	}
};
class PointLocator2f {
protected:
	libkdtree::KDTree<2, float2i> locator;
	int64_t indexCount = 0;
public:
	static const float2i NO_POINT_FOUND;
	PointLocator2f() {

	}
	void clear() {
		indexCount = 0;
		locator.clear();
	}
	size_t size() const {
		return locator.size();
	}
	void insert(const float2i& pt);
	int64_t insert(const float2& pt);
	void insert(const std::vector<float2>& pts);
	void insert(const std::vector<float2i>& ptsi);
	float2i closestPoint(float2 query, float maxDistance) const;
	float2i closestPoint(float2 query) const;
	float2i closestPointExact(float2i query) const;
	void findNearest(float2 query, float maxDisatnce, std::vector<float2i>& pts) const;
};
class PointLocator3f {
protected:
	libkdtree::KDTree<3, float3i> locator;
	int64_t indexCount = 0;
public:
	static const float3i NO_POINT_FOUND;
	PointLocator3f() {

	}
	void insert(const float3i& pt);
	int64_t insert(const float3& pt);
	void insert(const std::vector<float3>& pts);
	void insert(const std::vector<float3i>& ptsi);
	void clear() {
		indexCount = 0;
		locator.clear();
	}
	size_t size() const {
		return locator.size();
	}
	float3i closestPoint(float3 query, float maxDistance) const;
	float3i closestPoint(float3 query) const;
	float3i closestPointExact(float3i query) const;
	void findNearest(float3 query, float maxDisatnce, std::vector<float3i>& pts) const;
};
template <class VectorOfVectorsType, typename T = double, int C = -1, class Distance = nanoflann::metric_L2, typename IndexType = size_t> struct KdTreeVectorAdapter
{
	typedef KdTreeVectorAdapter<VectorOfVectorsType, T, C, Distance> self_t;
	typedef typename Distance::template traits<T, self_t>::distance_t metric_t;
	typedef nanoflann::KDTreeSingleIndexAdapter< metric_t, self_t, C, IndexType>  index_t;
	index_t* index; //! The kd-tree index for the user to call its methods as usual with any other FLANN index.
	vec<T, C> minPt;
	vec<T, C> maxPt;
	KdTreeVectorAdapter(const VectorOfVectorsType &mat, const int leaf_max_size = 16) : m_data(mat)
	{
		assert(mat.size() != 0 && mat[0].size() != 0);
		index = new index_t(C, *this, nanoflann::KDTreeSingleIndexAdapterParams(leaf_max_size));
		minPt = mat.min();
		maxPt = mat.max();
		index->buildIndex();
	}

	~KdTreeVectorAdapter() {
		delete index;
	}
	const VectorOfVectorsType &m_data;
	/** Query for the \a num_closest closest points to a given point (entered as query_point[0:dim-1]).
	*  Note that this is a short-cut method for index->findNeighbors().
	*  The user can also call index->... methods as desired.
	* \note nChecks_IGNORED is ignored but kept for compatibility with the original FLANN interface.
	*/
	inline void query(const T *query_point, const size_t num_closest, IndexType *out_indices, T *out_distances_sq, const int nChecks_IGNORED = 10) const
	{
		nanoflann::KNNResultSet<typename VectorOfVectorsType::Scalar, IndexType> resultSet(num_closest);
		resultSet.init(out_indices, out_distances_sq);
		index->findNeighbors(resultSet, query_point, nanoflann::SearchParams());
	}
	const self_t & derived() const {
		return *this;
	}
	self_t & derived() {
		return *this;
	}
	// Must return the number of data points
	inline size_t kdtree_get_point_count() const {
		return m_data.size();
	}
	// Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
	inline T kdtree_distance(const T *p1, const size_t idx_p2, size_t size) const
	{
		T s = 0;
		for (size_t i = 0; i<size; i++) {
			const T d = p1[i] - m_data[idx_p2][i];
			s += d*d;
		}
		return s;
	}
	// Returns the dim'th component of the idx'th point in the class:
	inline T kdtree_get_pt(const size_t idx, int dim) const {
		return m_data[idx][dim];
	}
	template <class BBOX>
	bool kdtree_get_bbox(BBOX & bb) const {
		for (int c = 0;c < C:c++) {
			bb[c].low = minPt[c];
			bb[c].high = maxPt[c];
		}
		return true;
	}
}; // end of KdTreeVectorAdapter
typedef KdTreeVectorAdapter<Vector4f, float, 4, nanoflann::metric_L2, size_t> FLANNLocator4f;
typedef KdTreeVectorAdapter<Vector3f, float, 3, nanoflann::metric_L2, size_t> FLANNLocator3f;
typedef KdTreeVectorAdapter<Vector2f, float, 2, nanoflann::metric_L2, size_t> FLANNLocator2f;
typedef KdTreeVectorAdapter<Vector1f, float, 1, nanoflann::metric_L2, size_t> FLANNLocator1f;

}
#endif