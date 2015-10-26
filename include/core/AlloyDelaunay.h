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
#ifndef ALLOYDELAUNAY_H
#define ALLOYDELAUNAY_H
#include "AlloyMath.h"
#include "AlloyVector.h"
#include <iostream>
namespace aly {
	void Triangulate(std::vector<float2>& pxyz, std::vector<uint3>& v);
	bool CircumCircle(float, float, float, float, float, float, float, float, float&, float&, float&);
	void MakeDelaunay(const std::vector<float2>& vertexes, std::vector<uint3>& output);
	inline void MakeDelaunay(const Vector2f& vertexes, std::vector<uint3>& output) {
		MakeDelaunay(vertexes.data, output);
	}
	inline void MakeDelaunay(const Vector2f& vertexes, Vector3ui& output) {
		MakeDelaunay(vertexes.data, output.data);
	}
}
#endif
