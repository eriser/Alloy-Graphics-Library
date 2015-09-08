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

#ifndef ALLOYMESHKDTREE_H_
#define ALLOYMESHKDTREE_H_
#include "AlloyMath.h"

namespace aly {
	struct Mesh;
	static const double ZERO_TOLERANCE = 1E-6;
	static const float3 NO_HIT_PT = float3(std::numeric_limits<float>::infinity());
	class KDBox {
	protected:
		int depth;
		float3 minPoint, maxPoint;
		std::vector<std::shared_ptr<KDBox>> children;
		static const int LEFT = 0, RIGHT = 1, MIDDLE = 2;
	public:
		const bool isLeaf;
		KDBox(int depth = 0) :depth(depth), isLeaf(false) {
			minPoint = float3(0, 0, 0);
			maxPoint = float3(0, 0, 0);
			children = std::vector<std::shared_ptr<KDBox>>();
		}
		KDBox(int depth, bool isLeaf) :depth(depth), isLeaf(isLeaf) {
			minPoint = float3(0, 0, 0);
			maxPoint = float3(0, 0, 0);
			children = std::vector<std::shared_ptr<KDBox>>();
		}
		int getDepth() const {
			return depth;
		}
		void setDepth(int d) {
			depth = d;
		}
		float3 getMin() const {
			return minPoint;
		}
		float3 getMax() const {
			return maxPoint;
		}
		std::vector<std::shared_ptr<KDBox>>& getChildren() {
			return children;
		}
		const std::vector<std::shared_ptr<KDBox>>& getChildren() const {
			return children;
		}
		double volume() const {
			return (maxPoint.z - minPoint.z) * (maxPoint.y - minPoint.y)
				* (maxPoint.x - minPoint.x);
		}
		bool intersects(const KDBox& test) const;
		bool inside(const float3& test) const;
		virtual void update();
		double distanceToBox(const float3& p);
		bool intersectsRay(const float3& org, const float3& dr) const;
		bool intersectsSegment(const float3& org, const float3& end) const;
	};

	class KDBoxEdge {
	protected:
		double val;
		bool start;
		std::shared_ptr<KDBox> box;
	public:
		std::shared_ptr<KDBox>& getBox() {
			return box;
		}
		double getValue() const {
			return val;
		}
		bool isMin() const {
			return start;
		}
		void set(const std::shared_ptr<KDBox>& b, int dim, bool start);
		KDBoxEdge() :val(0), start(true) {
		}
		bool operator <(const KDBoxEdge& e1) {
			if (val == e1.val) {
				if (start && !e1.start) {
					return true;
				}
				else if (!start && e1.start) {
					return false;
				}
				else {
					return true;//equals case
				}
			}
			else {
				return (val < e1.val);
			}
		}
	};
	class KDSegment {
	public:
		float extent;
		float3 direction;
		float3 origin;
		float3 lastIntersect = NO_HIT_PT;
		KDSegment() {
		}
		static KDSegment createFromPoints(const float3& p1, const float3& p2);
		static KDSegment createSegmentFromRay(const float3& p1, const float3& v);
		double distance(KDSegment& seg);
		bool intersectsPoint(const float3& test);
		float3 intersectsPlane(const float3& center, const float3& kNormal);
		void setLastIntersect(const float3& p) {
			lastIntersect = p;
		}
		float3 getLastIntersect() const {
			return lastIntersect;
		}

	};
	class KDTriangle :public KDBox {
	protected:
		float3 pts[3];
		float3 lastIntersect = NO_HIT_PT;
	public:
		float3 getLastIntersect() const {
			return lastIntersect;
		}
		void setLastIntersect(const float3& p) {
			lastIntersect = p;
		}
		KDTriangle(int depth = 0) :KDBox(depth, true) {
		}
		KDTriangle(const float3& pt1, const float3& pt2, const float3& pt3, int depth = 0) :KDBox(depth, true) {
			pts[0] = pt1;
			pts[1] = pt2;
			pts[2] = pt3;
		}
		float3 getNormal() const {
			return normalize(cross(pts[1] - pts[0], pts[2] - pts[0]));
		}
		float getArea() const {
			return 0.5f*length(cross(pts[1] - pts[0], pts[2] - pts[0]));
		}
		float3 getCentroid() {
			return 0.3333333f*(pts[0] + pts[1] + pts[2]);
		}
		float3 fromBary(const double3& b) {
			return float3((float)(pts[0].x * b.x + pts[1].x * b.y + pts[2].x
				* b.z), (float)(pts[0].y * b.x + pts[1].y * b.y + pts[2].y
					* b.z), (float)(pts[0].z * b.x + pts[1].z * b.y + pts[2].z
						* b.z));
		}
		double3 toBary(const float3& p) const;
		float3 intersectionPoint(const float3& p1, const float3& p2);
		float3 intersectionPointRay(const float3& org, const float3& v);
		double distance(const float3& p);
	};

	struct KDBoxDistance {
		std::shared_ptr<KDBox> box;
		double dist;
		KDBoxDistance(const float3& ref, const std::shared_ptr<KDBox>& tri) :box(tri) {
			dist = tri->distanceToBox(ref);
		}

	};
	inline bool operator<(const KDBoxDistance& a, const KDBoxDistance& b) {
		return (a.dist < b.dist);
	}
	class KDTree {
	protected:
		std::shared_ptr<KDBox> root;
		const double intersectCost = 80;
		const double traversalCost = 1;
		const double emptyBonus = 0.2;
		KDTriangle* lastTriangle = nullptr;
		float3 lastPoint = NO_HIT_PT;
		int splitPosition(std::vector<std::shared_ptr<KDBox>>& children, std::vector<KDBoxEdge>& edges, const std::shared_ptr<KDBox>& box);
		void buildTree(const std::shared_ptr<KDBox>& initBox, int maxDepth);
	public:
		std::shared_ptr<KDBox>& getRoot() {
			return root;
		}
		void build(const Mesh& mesh, int maxDepth);
		KDTree(const Mesh& mesh, int maxDepth) {
			build(mesh, maxDepth);
		}
		KDTree() {
		}
		double intersectRayDistance(float3 p1, float3 v);
		double intersectSegmentDistance(float3 p1, float3 p2);
		double signedDistance(float3 r);
		double distance(float3 pt);
		double distance(float3 r, float3 v);
	};
}
#endif