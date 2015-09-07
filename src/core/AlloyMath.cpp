/*
 * Copyright(C) 2014, Blake C. Lucas, Ph.D. (img.science@gmail.com)
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
#include "Eigen/Core.h"
#include "Eigen/Dense.h"
#include "AlloyMath.h"
using namespace std;
using namespace Eigen;
namespace aly {
void SVD(const matrix<float, 3, 3> &A, matrix<float, 3, 3>& U,
		matrix<float, 3, 3>& D, matrix<float, 3, 3>& Vt) {
	Matrix3f m;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			m(i, j) = A(i, j);
		}
	}
	std::cout << "A=\n" << m << std::endl;
	JacobiSVD<Matrix3f> svd(m, ComputeFullU | ComputeFullV);
	auto values = svd.singularValues();
	D = MakeDiagonal(float3(values[0], values[1], values[2]));
	Matrix3f u = svd.matrixU();
	Matrix3f v = svd.matrixV();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			U(i, j) = u(i, j);
			Vt(i, j) = v(j, i);
		}
	}
}
void SVD(const matrix<float, 4, 4> &A, matrix<float, 4, 4>& U,
		matrix<float, 4, 4>& D, matrix<float, 4, 4>& Vt) {
	Matrix4f m;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m(i, j) = A(i, j);
		}
	}
	JacobiSVD<Matrix4f> svd(m, ComputeFullU | ComputeFullV);
	auto values = svd.singularValues();
	D = MakeDiagonal(float4(values[0], values[1], values[2], values[3]));
	Matrix4f u = svd.matrixU();
	Matrix4f v = svd.matrixV();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			U(i, j) = u(i, j);
			Vt(i, j) = v(j, i);
		}
	}
}
}
