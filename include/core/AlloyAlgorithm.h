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
#ifndef ALLOYALGORITGM_H_
#define ALLOYALGORITGM_H_
#include "AlloyMath.h"
#include "AlloyVector.h"
#include "AlloySparseMatrix.h"
namespace aly {
bool SANITY_CHECK_ALGO();
template<class T, int C> void SolveCG(const Vector<T, C>& b,
		const SparseMatrix<T, C>& A, Vector<T, C>& x, int iters = 100,
		T tolerance = 1E-6f) {
	const double ZERO_TOLERANCE = 1E-16;
	vec<double, C> err(0.0);
	size_t N = b.size();
	Vector<T, C> p(N);
	Vector<T, C> Ap(N);
	Multiply(Ap, A, x);
	Vector<T, C> tmp1(N), tmp2(N);
	Vector<T, C>* rcurrent = &tmp1;
	Vector<T, C>* rnext = &tmp2;
	Subtract(*rcurrent, b, Ap);
	p = *rcurrent;
	err = lengthVecSqr(*rcurrent);
	for (int iter = 0; iter < iters; iter++) {
		Multiply(Ap, A, p);
		vec<double, C> denom = dotVec(p, Ap);
		for (int c = 0; c < C; c++) {
			if (std::abs(denom[c]) < ZERO_TOLERANCE) {
				denom[c] = (denom[c] < 0) ? -ZERO_TOLERANCE : ZERO_TOLERANCE;
			}
		}
		vec<double, C> alpha = lengthVecSqr(*rcurrent) / denom;
		ScaleAdd(x, vec<T, C>(alpha), p);
		ScaleSubtract(*rnext, *rcurrent, vec<T, C>(alpha), Ap);
		vec<double, C> err = lengthVecSqr(*rnext);
		if (lengthL1(err) < N * tolerance)
			break;
		denom = lengthVecSqr(*rcurrent);
		for (int c = 0; c < C; c++) {
			if (std::abs(denom[c]) < ZERO_TOLERANCE) {
				denom[c] = (denom[c] < 0) ? -ZERO_TOLERANCE : ZERO_TOLERANCE;
			}
		}
		vec<double, C> beta = err / denom;
		ScaleAdd(p, *rnext, vec<T, C>(beta), p);
		std::swap(rcurrent, rnext);
	}
}
template<class T, int C> void SolveCG(const Vector<T, C>& b,
		const SparseMatrix<T, 1>& A, Vector<T, C>& x, int iters = 100,
		T tolerance = 1E-6f) {
	const double ZERO_TOLERANCE = 1E-16;
	vec<double, C> err(0.0);
	size_t N = b.size();
	Vector<T, C> p(N);
	Vector<T, C> Ap(N);
	Multiply(Ap, A, x);
	Vector<T, C> tmp1(N), tmp2(N);
	Vector<T, C>* rcurrent = &tmp1;
	Vector<T, C>* rnext = &tmp2;
	Subtract(*rcurrent, b, Ap);
	p = *rcurrent;
	err = lengthVecSqr(*rcurrent);
	for (int iter = 0; iter < iters; iter++) {
		Multiply(Ap, A, p);
		vec<double, C> denom = dotVec(p, Ap);
		for (int c = 0; c < C; c++) {
			if (std::abs(denom[c]) < ZERO_TOLERANCE) {
				denom[c] = (denom[c] < 0) ? -ZERO_TOLERANCE : ZERO_TOLERANCE;
			}
		}
		vec<double, C> alpha = lengthVecSqr(*rcurrent) / denom;
		ScaleAdd(x, vec<T, C>(alpha), p);
		ScaleSubtract(*rnext, *rcurrent, vec<T, C>(alpha), Ap);
		vec<double, C> err = lengthVecSqr(*rnext);
		if (lengthL1(err) < N * tolerance)
			break;
		denom = lengthVecSqr(*rcurrent);
		for (int c = 0; c < C; c++) {
			if (std::abs(denom[c]) < ZERO_TOLERANCE) {
				denom[c] = (denom[c] < 0) ? -ZERO_TOLERANCE : ZERO_TOLERANCE;
			}
		}
		vec<double, C> beta = err / denom;
		ScaleAdd(p, *rnext, vec<T, C>(beta), p);
		std::swap(rcurrent, rnext);
	}
}
}
#endif
