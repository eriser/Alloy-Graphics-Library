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

#ifndef ALLOYLINEARALGEBRA_H_
#define ALLOYLINEARALGEBRA_H_

#include "AlloyMath.h"
#include <vector>
#include <functional>
#include <iomanip>
#include <limits>
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
namespace aly {
bool SANITY_CHECK_LINALG();
template<class T, int C> struct Vector {
private:
	std::vector<vec<T, C>> storage;
public:
	std::vector<vec<T, C>>& data;
	const int channels = C;
	typedef vec<T, C> ValueType;
	typedef typename std::vector<ValueType>::iterator iterator;
	typedef typename std::vector<ValueType>::const_iterator const_iterator;
	iterator begin() const {
		return data.begin();
	}
	iterator end() {
		return data.end();
	}
	const_iterator cbegin() const {
		return data.cbegin();
	}
	const_iterator cend() const {
		return data.cend();
	}
	template<class Archive>
	void save(Archive & archive) const {
		archive(
				cereal::make_nvp(MakeString() << "vector" << C,
						EncodeBase64(data)));
	}

	template<class Archive>
	void load(Archive & archive) {
		std::string str;
		archive(cereal::make_nvp(MakeString() << "vector" << C, str));
		DecodeBase64(str, data);
	}

	void set(const T& val) {
		data.assign(data.size(), vec<T, C>(val));
	}
	void set(const vec<T, C>& val) {
		data.assign(data.size(), val);
	}
	void set(T* val) {
		if (val == nullptr)
			return;
		size_t offset = 0;
		for (vec<T, C>& x : data) {
			for (int c = 0; c < C; c++) {
				x[c] = val[offset++];
			}
		}
	}
	void set(vec<T, C>* val) {
		if (val == nullptr)
			return;
		size_t offset = 0;
		for (vec<T, C>& x : data) {
			x = val[offset++];
		}
	}
	template<class F> void apply(F f) {
		size_t sz = size();
#pragma omp parallel for
		for (size_t offset = 0; offset < sz; offset++) {
			f(offset, data[offset]);
		}
	}
	Vector(size_t sz) :
			data(storage) {
		data.resize(sz);
		data.shrink_to_fit();
	}
	Vector(const Vector<T, C>& img) :Vector(img.size()) {
		set(img.data.data());
	}
	Vector<T, C>& operator=(const Vector<T, C>& rhs)
	{
		if (this == &rhs)
			return *this;
		return *this;
	}
	Vector() :
			data(storage) {
	}
	Vector(T* ptr, size_t sz) :
			Vector(sz) {
		set(ptr);
	}
	Vector(vec<T, C>* ptr, size_t sz) :
			Vector(sz) {
		set(ptr);
	}
	Vector(std::vector<vec<T, C>>& ref) :
			data(ref) {
	}
	size_t size() const {
		return data.size();
	}
	size_t typeSize() const {
		return sizeof(vec<T, C> );
	}
	void resize(size_t sz) {
		data.resize(sz);
		data.shrink_to_fit();
	}
	void resize(size_t sz, const vec<T, C>& val) {
		data.resize(sz, val);
		data.shrink_to_fit();
	}
	void append(const vec<T, C>& val) {
		data.push_back(val);
	}
	void push_back(const vec<T, C>& val) {
		data.push_back(val);
	}
	T* ptr() {
		if (data.size() == 0)
			return nullptr;
		return &(data.front()[0]);
	}
	const T* ptr() const {
		if (data.size() == 0)
			return nullptr;
		return &(data.front()[0]);
	}
	void setZero() {
		data.assign(data.size(), vec<T, C>((T) 0));
	}
	const vec<T, C>& operator[](const size_t i) const {
		return data[i];
	}
	vec<T, C>& operator[](const size_t i) {
		return data[i];
	}
	inline void clear() {
		data.clear();
		data.shrink_to_fit();
	}
};

template<class T, int C> void Transform(Vector<T, C>& im1, Vector<T, C>& im2,
		const std::function<void(vec<T, C>&, vec<T, C>&)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (size_t offset = 0; offset < sz; offset++) {
		func(im1.data[offset], im2.data[offset]);
	}
}
template<class T, int C> void Transform(Vector<T, C>& im1,
		const std::function<void(vec<T, C>&)>& func) {
	size_t sz = im1.size();
#pragma omp parallel for
	for (size_t offset = 0; offset < sz; offset++) {
		func(im1.data[offset]);
	}
}
template<class T, int C> void Transform(Vector<T, C>& im1,
		const Vector<T, C>& im2,
		const std::function<void(vec<T, C>&, const vec<T, C>&)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (size_t offset = 0; offset < sz; offset++) {
		func(im1.data[offset], im2.data[offset]);
	}
}
template<class T, int C> void Transform(Vector<T, C>& im1,
		const Vector<T, C>& im2, const Vector<T, C>& im3,
		const std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (size_t offset = 0; offset < sz; offset++) {
		func(im1.data[offset], im2.data[offset], im3.data[offset]);
	}
}
template<class T, int C> void Transform(Vector<T, C>& im1, Vector<T, C>& im2,
		const std::function<
				void(size_t offset, vec<T, C>& val1, vec<T, C>& val2)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (size_t offset = 0; offset < sz; offset++) {
		func(offset, im1.data[offset], im2.data[offset]);
	}
}
template<class T, class L, class R, int C> std::basic_ostream<L, R> & operator <<(
		std::basic_ostream<L, R> & ss, const Vector<T, C> & A) {
	size_t index = 0;
	for (vec<T, C>& val : A.data) {
		ss << std::setw(5) << index++ << ": " << val << std::endl;
	}
	return ss;
}
template<class T, int C> Vector<T, C> operator+(const vec<T, C>& scalar,
		const Vector<T, C>& img) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = scalar + val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> void ScaleAdd(Vector<T, C>& out, const vec<T, C>& scalar,const Vector<T, C>& in) {
	out.resize(in.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
		[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 += scalar * val2;};
	Transform(out, in, f);
}
template<class T, int C> void ScaleAdd(Vector<T, C>& out, const Vector<T, C>& in1, const vec<T, C>& scalar, const Vector<T, C>& in2) {
	out.resize(in1.size());
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
		[=](vec<T, C>& val1, const vec<T, C>& val2, const vec<T, C>& val3) {val1 = val2+scalar * val3;};
	Transform(out, in1,in2, f);
}
template<class T, int C> void Subtract(Vector<T, C>& out, const Vector<T, C>& v1, const Vector<T, C>& v2) {
	out.resize(v1.size());
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
		[=](vec<T, C>& val1, const vec<T, C>& val2, const vec<T, C>& val3) {val1 = val2-val3;};
	Transform(out, v1,v2, f);
}
template<class T, int C> void Add(Vector<T, C>& out, const Vector<T, C>& v1, const Vector<T, C>& v2) {
	out.resize(v1.size());
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
		[=](vec<T, C>& val1, const vec<T, C>& val2, const vec<T, C>& val3) {val1 = val2 + val3;};
	Transform(out, v1, v2, f);
}
template<class T, int C> Vector<T, C> operator-(const vec<T, C>& scalar,
		const Vector<T, C>& img) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = scalar - val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator*(const vec<T, C>& scalar,
		const Vector<T, C>& img) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = scalar*val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator*(const T& scalar,
	const Vector<T, C>& img) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
		[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = scalar*val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator/(const vec<T, C>& scalar,
		const Vector<T, C>& img) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = scalar / val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator+(const Vector<T, C>& img,
		const vec<T, C>& scalar) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = val2 + scalar;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator-(const Vector<T, C>& img,
		const vec<T, C>& scalar) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = val2 - scalar;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator*(const Vector<T, C>& img,
		const vec<T, C>& scalar) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = val2*scalar;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator/(const Vector<T, C>& img,
		const vec<T, C>& scalar) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = val2 / scalar;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator-(const Vector<T, C>& img) {
	Vector<T, C> out(img.size());
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 = -val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator+=(Vector<T, C>& out,
		const Vector<T, C>& img) {
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 += val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator-=(Vector<T, C>& out,
		const Vector<T, C>& img) {
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 -= val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator*=(Vector<T, C>& out,
		const Vector<T, C>& img) {
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 *= val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C> Vector<T, C> operator/=(Vector<T, C>& out,
		const Vector<T, C>& img) {
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2) {val1 /= val2;};
	Transform(out, img, f);
	return out;
}

template<class T, int C> Vector<T, C> operator+=(Vector<T, C>& out,
		const vec<T, C>& scalar) {
	std::function<void(vec<T, C>&)> f = [=](vec<T, C>& val1) {val1 += scalar;};
	Transform(out, f);
	return out;
}
template<class T, int C> Vector<T, C> operator-=(Vector<T, C>& out,
		const vec<T, C>& scalar) {
	std::function<void(vec<T, C>&)> f = [=](vec<T, C>& val1) {val1 -= scalar;};
	Transform(out, f);
	return out;
}
template<class T, int C> Vector<T, C> operator*=(Vector<T, C>& out,
		const vec<T, C>& scalar) {
	std::function<void(vec<T, C>&)> f = [=](vec<T, C>& val1) {val1 *= scalar;};
	Transform(out, f);
	return out;
}
template<class T, int C> Vector<T, C> operator/=(Vector<T, C>& out,
		const vec<T, C>& scalar) {
	std::function<void(vec<T, C>&)> f = [=](vec<T, C>& val1) {val1 /= scalar;};
	Transform(out, f);
	return out;
}

template<class T, int C> Vector<T, C> operator+(const Vector<T, C>& img1,
		const Vector<T, C>& img2) {
	Vector<T, C> out(img1.size());
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2, const vec<T, C>& val3) {val1 = val2 + val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T, int C> Vector<T, C> operator-(const Vector<T, C>& img1,
		const Vector<T, C>& img2) {
	Vector<T, C> out(img1.size());
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2, const vec<T, C>& val3) {val1 = val2 - val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T, int C> Vector<T, C> operator*(const Vector<T, C>& img1,
		const Vector<T, C>& img2) {
	Vector<T, C> out(img1.size());
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2, const vec<T, C>& val3) {val1 = val2*val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T, int C> Vector<T, C> operator/(const Vector<T, C>& img1,
		const Vector<T, C>& img2) {
	Vector<T, C> out(img1.size());
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T, C>& val1, const vec<T, C>& val2, const vec<T, C>& val3) {val1 = val2 / val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T, int C> vec<T, C> dotVec(const Vector<T, C>& a,const Vector<T, C>& b) {
	vec<T, C> ans((T) 0);
	if (a.size() != b.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << a.size()
						<< "!=" << b.size());
	size_t sz = a.size();
#pragma omp parallel for
	for (int c = 0; c < C; c++) {
		T cans = 0;
#pragma omp parallel for reduction(+:cans)
		for (size_t i = 0; i < sz; i++) {
			cans += a[i][c] * b[i][c];
		}
		ans[c] = cans;
	}
	return ans;
}
template<class T, int C> T dot(const Vector<T, C>& a,const Vector<T, C>& b) {
	T ans(0);
	if (a.size() != b.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << a.size()
						<< "!=" << b.size());
	size_t sz = a.size();
#pragma omp parallel for reduction(+:ans)
	for (size_t i = 0; i < sz; i++) {
		ans += dot(a[i], b[i]);
	}
	return ans;
}

template<class T, int C> T lengthSqr(const Vector<T, C>& a) {
	T ans(0);
	size_t sz = a.size();
#pragma omp parallel for reduction(+:ans)
	for (size_t i = 0; i < sz; i++) {
		ans += dot(a[i], a[i]);
	}
	return ans;
}
template<class T, int C> T lengthL1(const Vector<T, C>& a) {
	T ans(0);
	size_t sz = a.size();
#pragma omp parallel for reduction(+:ans)
	for (size_t i = 0; i < sz; i++) {
		for (int c = 0; c < C; c++) {
			ans += std::abs(a[i][c]);
		}
	}
	return ans;
}
template<class T, int C> vec<T, C> lengthVecL1(const Vector<T, C>& a) {
	vec<T, C> ans((T) 0);
	size_t sz = a.size();
#pragma omp parallel for
	for (int c = 0; c < C; c++) {
		T cans = 0;
#pragma omp parallel for reduction(+:cans)
		for (size_t i = 0; i < sz; i++) {
			cans += std::abs(a[i][c]);
		}
		ans[c] = cans;
	}
	return ans;
}
template<class T, int C> vec<T, C> maxVec(const Vector<T, C>& a) {
	vec<T, C> ans((T) 0);
	size_t sz = a.size();
#pragma omp parallel for
	for (int c = 0; c < C; c++) {
		T tmp(std::numeric_limits<T>::min());
#pragma omp parallel for reduction(max:tmp)
		for (size_t i = 0; i < sz; i++) {
			if (a[i][c] > tmp) {
				tmp = a[i][c];
			}
		}
		ans[c] = tmp;
	}
	return ans;
}
template<class T, int C> vec<T, C> minVec(const Vector<T, C>& a) {
	vec<T, C> ans((T) 0);
	size_t sz = a.size();
#pragma omp parallel for
	for (int c = 0; c < C; c++) {
		T tmp(std::numeric_limits<T>::max());
#pragma omp parallel for reduction(min:tmp)
		for (size_t i = 0; i < sz; i++) {
			if (a[i][c] < tmp) {
				tmp = a[i][c];
			}
		}
		ans[c] = tmp;
	}
	return ans;
}
template<class T, int C> T max(const Vector<T, C>& a) {
	size_t sz = a.size();
	T tmp(std::numeric_limits<T>::min());
#pragma omp parallel for reduction(max:tmp)
	for (size_t i = 0; i < sz; i++) {
		for (int c = 0; c < C; c++) {
			if (a[i][c] > tmp) {
				tmp = a[i][c];
			}
		}
	}
	return tmp;
}
template<class T, int C> T min(const Vector<T, C>& a) {
	size_t sz = a.size();
	T tmp(std::numeric_limits<T>::max());
#pragma omp parallel for reduction(min:tmp)
	for (size_t i = 0; i < sz; i++) {
		for (int c = 0; c < C; c++) {
			if (a[i][c] < tmp) {
				tmp = a[i][c];
			}
		}
	}
	return tmp;
}
template<class T, int C> T length(const Vector<T, C>& a) {
	return std::sqrt(lengthSqr(a));
}
template<class T, int C> vec<T, C> lengthVecSqr(const Vector<T, C>& a) {
	vec<T, C> ans((T) 0);
	size_t sz = a.size();
#pragma omp parallel for
	for (int c = 0; c < C; c++) {
		T cans = 0;
#pragma omp parallel for reduction(+:cans)
		for (size_t i = 0; i < sz; i++) {
			cans += a[i][c] * a[i][c];
		}
		ans[c] = cans;
	}
	return ans;
}
template<class T, int C> vec<T, C> lengthVec(const Vector<T, C>& a) {
	return sqrt(lengthVecSqr(a));
}
typedef Vector<uint8_t, 4> VectorRGBA;
typedef Vector<int, 4> VectorRGBAi;
typedef Vector<float, 4> VectorRGBAf;

typedef Vector<uint8_t, 3> VectorRGB;
typedef Vector<int, 3> VectorRGBi;
typedef Vector<float, 3> VectorRGBf;

typedef Vector<uint8_t, 1> VectorA;
typedef Vector<int, 1> VectorAi;
typedef Vector<float, 1> VectorAf;

typedef Vector<uint8_t, 4> Vector4b;
typedef Vector<uint16_t, 4> Vector4us;
typedef Vector<int16_t, 4> Vector4s;
typedef Vector<int, 4> Vector4i;
typedef Vector<uint32_t, 4> Vector4ui;
typedef Vector<float, 4> Vector4f;

typedef Vector<uint8_t, 3> Vector3b;
typedef Vector<uint16_t, 3> Vector3us;
typedef Vector<int16_t, 3> Vector3s;
typedef Vector<int, 3> Vector3i;
typedef Vector<uint32_t, 3> Vector3ui;
typedef Vector<float, 3> Vector3f;

typedef Vector<uint8_t, 2> Vector2b;
typedef Vector<uint16_t, 2> Vector2us;
typedef Vector<int16_t, 2> Vector2s;
typedef Vector<int, 2> Vector2i;
typedef Vector<uint32_t, 2> Vector2ui;
typedef Vector<float, 2> Vector2f;

typedef Vector<uint8_t, 1> Vector1b;
typedef Vector<uint16_t, 1> Vector1us;
typedef Vector<int16_t, 1> Vector1s;
typedef Vector<int, 1> Vector1i;
typedef Vector<uint32_t, 1> Vector1ui;
typedef Vector<float, 1> Vector1f;

}
;

#endif /* ALLOYLINEARALGEBRA_H_ */
