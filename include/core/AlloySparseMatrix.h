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
#include "AlloyVector.h"
#include <vector>
#include <list>
namespace aly{
	template<class T, int C> struct KeyValue: public std::pair<size_t, vec<T, C>> {
		KeyValue() :std::pair<size_t, vec<T, C>>() {

		}
		KeyValue(size_t index, const vec<T, C>& v) :std::pair<size_t, vec<T, C>>(index, v) {

		}
	};
	template<class T, int C> struct SparseMatrix {
	private:
		std::vector<std::list<KeyValue<T, C>>> storage;
	public:
		std::list<KeyValue<T, C>>& operator[](size_t index) {
			return storage[index];
		}
		const std::list<KeyValue<T, C>>& operator[](size_t index) const {
			return storage[index];
		}
		size_t rows, cols;
		SparseMatrix(size_t rows, size_t cols) :storage(rows),rows(rows),cols(cols) {
		}
		void insert(size_t i, size_t j, const vec<T, C>& value) {
			storage[i].push_back(KeyValue<T, C>(i, value));
		}
		
	};
	template<class T, int C> Vector<T, C> operator*(const SparseMatrix<T,C>& A,const Vector<T, C>& v) {
		Vector<T, C> out(v.size());
		size_t sz = v.size();
#pragma omp parallel for
		for (size_t i = 0; i < sz; i++)
		{
			vec<T, C> sum((T)0);
			for (const KeyValue<T,C>& pr : A[i]) {
				sum += v[pr.first] * pr.second;
			}
			out[i] = sum;
		}
		return out;
	}
	template<class T, int C> void Multiply(Vector<T, C>& out,const SparseMatrix<T, C>& A, const Vector<T, C>& v) {
		out.resize(v.size());
		size_t sz = v.size();
#pragma omp parallel for
		for (size_t i = 0; i < sz; i++)
		{
			vec<T, C> sum((T)0);
			for (const KeyValue<T, C>& pr : A[i]) {
				sum += v[pr.first] * pr.second;
			}
			out[i] = sum;
		}
	}
}

