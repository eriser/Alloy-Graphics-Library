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
#pragma once
#include "AlloyMath.h"
#include "AlloyVector.h"
#include "AlloySparseMatrix.h"
namespace aly {
	bool SANITY_CHECK_ALGO();
	template<class T, int C> Vector<T, C> SolveCG(const Vector<T, C>& b, const SparseMatrix<T, C>& A, Vector<T, C>& x, int iters=100,T tolerance=1E-6f) {
		vec<T,C> err((T)0);
		size_t N = b.size();
		Vector<T, C> p(N);
		Vector<T,C> Ap = A*x;
		Vector<T, C> tmp1(N), tmp2(N);
		Vector<T, C>* rcurrent = &tmp1;
		Vector<T, C>* rnext = &tmp2;
		*rcurrent = b - Ap;
		p = *rcurrent;
		err = lengthVecSqr(*rcurrent);
		for (int iter = 0; iter < iters; iter++)
		{
			Multiply(Ap,A,p);
			vec<T, C> denom = dotVec(p, Ap);
			for (int c = 0;c < C;c++) {
				if (std::abs(denom[c]) < 1E-12f) {
					denom[c] = (denom[c] < 0) ? -1E-12f : 1E-12f;
				}
			}
			vec<T, C> alpha = lengthVecSqr(*rcurrent) / denom;
			ScaleAdd(x, alpha, p);
			ScaleAdd(*rnext, *rcurrent, ((T)-1.0)*alpha, Ap);
			vec<T, C> err = lengthVecSqr(*rnext);
			if (lengthL1(err) < N * tolerance)
				break;

			denom =lengthVecSqr(*rcurrent);
			for (int c = 0;c < C;c++) {
				if (std::abs(denom[c]) < 1E-12f) {
					denom[c] = (denom[c] < 0) ? -1E-12f : 1E-12f;
				}
			}
			vec<T,C> beta = err / denom;
			ScaleAdd(p, *rnext, beta, p);
			std::swap(*rcurrent, *rnext);
		}
		return x;
	}
}