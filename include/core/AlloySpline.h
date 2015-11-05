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
#ifndef ALLOYSPLINE_H_
#define ALLOYSPLINE_H_
#include "tinysplinecpp.h"
#include "AlloyVector.h"
namespace aly {
	enum class SplineType {
		Open = TS_OPENED,
		Clamp = TS_CLAMPED,
		None = TS_NONE,
	};
	bool SANITY_CHECK_BSPLINE();
	template<int C> class BSpline : public Vector<float, C> {
	protected:
		std::unique_ptr<TsBSpline> spline;
	public:

		size_t getDegree() const {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			return spline->deg();
		}
		size_t getOrder() const {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			return spline->order();
		}
		size_t getDimension() const {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			return spline->dim();
		}
		size_t getNumControlPoints() const {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			return spline->nCtrlp();
		}
		size_t getNumKnots() const {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			return spline->nKnots();
		}
		void setBuckle(const float b) {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			spline->buckle(b);
		}
		void insertKnot(const size_t index, const float u) {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			spline->insertKnot(u, index);
		}
		void splitAt(const float u) {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			spline->split(u);
		}
		TsDeBoorNet& evaluate(const float u) const {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			return spline->evaluate(u);
		}
		void convertToBeziers() {
			if (spline.get() == nullptr) {
				throw std::runtime_error("Spline not intialized. call build() first.");
			}
			spline->toBeziers();
		}
		void build(const SplineType& type = SplineType::Open, int deg = 3) {
			spline = std::unique_ptr<TsBSpline>(new TsBSpline(ptr(),data.size(),3));
			spline->setDeg(deg);
			spline->setupKnots(static_cast<tsBSplineType>(type));
		}
		BSpline(size_t sz) :Vector(sz) {
		}
		BSpline() :Vector(){
		}
		BSpline(const BSpline& spline) :Vector(spline.data) {
		}
		BSpline(const Vector<float, C>& V,const SplineType& type, int deg) :Vector(V) {
			build(type, deg);
		}
	};
	typedef BSpline<2> BSpline2f;
	typedef BSpline<3> BSpline3f;
}

#endif