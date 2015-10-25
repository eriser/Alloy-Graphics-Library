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
class PointLocator2D {
protected:
	libkdtree::KDTree<2, float2i> locator;
	size_t indexCount = 0;
public:
	static const float2i NO_POINT_FOUND;
	PointLocator2D() {

	}
	void clear() {
		indexCount = 0;
		locator.clear();
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
class PointLocator3D {
protected:
	libkdtree::KDTree<3, float3i> locator;
	int64_t indexCount = 0;
public:
	static const float3i NO_POINT_FOUND;
	PointLocator3D() {

	}
	void insert(const float3i& pt);
	int64_t insert(const float3& pt);
	void insert(const std::vector<float3>& pts);
	void insert(const std::vector<float3i>& ptsi);
	void clear() {
		indexCount = 0;
		locator.clear();
	}
	float3i closestPoint(float3 query, float maxDistance) const;
	float3i closestPoint(float3 query) const;
	float3i closestPointExact(float3i query) const;
	void findNearest(float3 query, float maxDisatnce, std::vector<float3i>& pts) const;
};
}
#endif