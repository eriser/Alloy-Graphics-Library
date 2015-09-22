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
#ifndef ALLOYDENSEMATRIX_H_
#define ALLOYDENSEMATRIX_H_
#include "AlloyVector.h"
#include "cereal/types/vector.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/tuple.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/memory.hpp"
#include <vector>
#include <list>
#include <map>
namespace aly {

template<class T, int C> struct DenseMatrix {
private:
	std::vector<std::vector<vec<T, C>>>storage;
public:
	int rows, cols;
	template<class Archive> void serialize(Archive & archive) {
		archive(CEREAL_NVP(rows), CEREAL_NVP(cols),
				cereal::make_nvp(
						MakeString() << "matrix"<<C,
						storage));
	}
	std::vector<size_t, vec<T, C>>& operator[](size_t i) {
		if (i >= rows || i < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i
				<< ",*) exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		return storage[i];
	}
	const std::vector<size_t, vec<T, C>>& operator[](size_t i) const {
		if (i >= rows || i < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i
				<< ",*) exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		return storage[i];
	}
	DenseMatrix(): rows(0),cols(0) {
	}
	DenseMatrix(size_t rows, size_t cols) :
	rows(rows), cols(cols),storage(rows,std::vector<vec<T,C>>(cols)) {
	}
	void set(size_t i, size_t j, const vec<T, C>& value) {
		if (i >= rows || j >= cols || i < 0 || j < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i << "," << j
				<< ") exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		storage[i][j] = value;
	}
	void set(size_t i, size_t j, const T& value) {
		if (i >= rows || j >= cols || i < 0 || j < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i << "," << j
				<< ") exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		storage[i][j] = vec<T, C>(value);
	}
	vec<T, C>& operator()(size_t i, size_t j) {
		if (i >= rows || j >= cols || i < 0 || j < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i << "," << j
				<< ") exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		return storage[i][j];
	}

	vec<T, C> get(size_t i, size_t j) const {
		if (i >= rows || j >= cols || i < 0 || j < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i << "," << j
				<< ") exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		return storage[i][j];
	}
	const vec<T, C>& operator()(size_t i, size_t j) const {
		return storage[i][j];
	}
	DenseMatrix<T, C> transpose() const {
		DenseMatrix<T, C> M(cols, rows);
		for (int i = 0; i < rows; i++) {
			for (int j=0;j < cols;j++) {
				M[j][i]=storage[i][j];
			}
		}
		return M;
	}
	static DenseMatrix<T, C> identity(size_t M, size_t N) {
		DenseMatrix<T, C> A(M, N);
		int K = (int) aly::min(M, N);
		for (int k = 0; k < K; k++) {
			A[k][k] = vec<T, C>(T(1));
		}
		return A;
	}
	static DenseMatrix<T, C> diagonal(const Vector<T, C>& v) {
		DenseMatrix<T, C> A(v.size(), v.size());
		for (int k = 0; k < (int) v.size(); k++) {
			A[k][k] = v[k];
		}
		return A;
	}
};
template<class A, class B, class T, int C> std::basic_ostream<A, B> & operator <<(
		std::basic_ostream<A, B> & ss, const DenseMatrix<T, C>& M) {
	ss << "\n";
	for (int n = 0; n < M.rows; n++) {
		ss << "[";
		for (int m = 0; m < M.cols; m++) {
			ss << std::setprecision(10) << std::setw(16) << A(n, m)
					<< ((m < M.cols - 1) ? "," : "]\n");
		}
	}
	return ss;
}

template<class T, int C> Vector<T, C> operator*(const DenseMatrix<T, 1>& A,
		const Vector<T, C>& v) {
	Vector<T, C> out(A.rows);
	for (int i = 0; i < A.rows; i++) {
		vec<T, C> sum(0.0);
		for (int j = 0; j < A.cols; j++) {
			sum += A[i][j].x * v[j];
		}
		out[i] = sum;
	}
	return out;
}
template<class T, int C> Vector<T, C> operator*(const DenseMatrix<T, C>& A,
		const Vector<T, C>& v) {
	Vector<T, C> out(A.rows);
	for (int i = 0; i < A.rows; i++) {
		vec<T, C> sum(0.0);
		for (int j = 0; j < A.cols; j++) {
			sum += A[i][j] * v[j];
		}
		out[i] = sum;
	}
	return out;
}
template<class T, int C> DenseMatrix<T, C>& operator*=(
		const DenseMatrix<T, C>& A, const vec<T, C>& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] * v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator/=(
		const DenseMatrix<T, C>& A, const vec<T, C>& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] / v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator+=(
		const DenseMatrix<T, C>& A, const vec<T, C>& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] + v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator-=(
		const DenseMatrix<T, C>& A, const vec<T, C>& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] - v;
		}
	}
	return A;
}
typedef DenseMatrix<float, 4> DenseMatrix4f;
typedef DenseMatrix<float, 3> DenseMatrix3f;
typedef DenseMatrix<float, 2> DenseMatrix2f;
typedef DenseMatrix<float, 1> DenseMatrix1f;

typedef DenseMatrix<double, 4> DenseMatrix4d;
typedef DenseMatrix<double, 3> DenseMatrix3d;
typedef DenseMatrix<double, 2> DenseMatrix2d;
typedef DenseMatrix<double, 1> DenseMatrix1d;
}

#endif
