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
#ifndef ALLOYIMAGE2D_H_INCLUDE_GUARD
#define ALLOYIMAGE2D_H_INCLUDE_GUARD
#include "AlloyCommon.h"
#include "AlloyMath.h"
#include "sha2.h"
#include "AlloyFileUtil.h"
#include "cereal/types/vector.hpp"
#include <vector>
#include <functional>
#include <fstream>
#include <random>
namespace aly {
bool SANITY_CHECK_IMAGE();
bool SANITY_CHECK_PYRAMID();
enum class ImageType {
	BYTE = 0,
	UBYTE = 1,
	SHORT = 2,
	USHORT = 3,
	INT = 4,
	UINT = 5,
	FLOAT = 6,
	DOUBLE = 7
};
template<class L, class R> std::basic_ostream<L, R>& operator <<(
		std::basic_ostream<L, R> & ss, const ImageType& type) {
	switch (type) {
	case ImageType::BYTE:
		return ss << "byte";
	case ImageType::UBYTE:
		return ss << "ubyte";
	case ImageType::SHORT:
		return ss << "short";
	case ImageType::USHORT:
		return ss << "ushort";
	case ImageType::INT:
		return ss << "int";
	case ImageType::UINT:
		return ss << "uint";
	case ImageType::FLOAT:
		return ss << "float";
	case ImageType::DOUBLE:
		return ss << "double";
	}
	return ss;
}
template<class T, int C, ImageType I> struct Image;
template<class T, int C, ImageType I> void WriteImageToRawFile(
		const std::string& fileName, const Image<T, C, I>& img);
template<class T, int C, ImageType I> struct Image {
private:
	std::vector<vec<T, C>> storage;
	std::string hashCode;
public:

	std::vector<vec<T, C>>& data;
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

	int width;
	int height;
	int x, y;
	uint64_t id;
	const int channels;
	const ImageType type;
	std::string updateHashCode(size_t MAX_SAMPLES = 0, HashMethod method =
			HashMethod::SHA256);
	std::string getHashCode() {
		return hashCode;
	}
	iterator begin() {
		return data.begin();
	}

	template<class Archive> void serialize(Archive & archive) {
		archive(cereal::make_nvp(MakeString() << type << channels, id),
				CEREAL_NVP(width), CEREAL_NVP(height), CEREAL_NVP(x),
				CEREAL_NVP(y), CEREAL_NVP(hashCode));
	}

	void writeToXML(const std::string& fileName) const {
		WriteImageToRawFile(fileName, *this);
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
	void set(const Image<T, C, I>& other) {
		resize(other.width, other.height);
		id = other.id;
		x = other.x;
		y = other.y;
		set(&other.data[0]);
	}
	std::string getTypeName() const {
		return MakeString() << type << channels;
	}

	Image(int w, int h, int x = 0, int y = 0, uint64_t id = 0) :
			data(storage), width(w), height(h), x(x), y(y), id(id), channels(C), type(
					I) {
		data.resize(w * h);
		data.shrink_to_fit();
	}
	Image(T* ptr, int w, int h, int x = 0, int y = 0, uint64_t id = 0) :
			Image(w, h, x, y, id) {
		set(ptr);
	}
	Image(vec<T, C>* ptr, int w, int h, int x = 0, int y = 0, uint64_t id = 0) :
			Image(w, h, x, y, id) {
		set(ptr);
	}
	Image(int w, int h, vec<T, C>* ptr) :
			Image(w, h, 0, 0, 0) {
		set(ptr);
	}
	Image(std::vector<vec<T, C>>& ref, int w, int h, int x = 0, int y = 0,
			uint64_t id = 0) :
			data(ref), width(w), height(h), x(x), y(y), id(id), channels(C), type(
					I) {
	}
	Image() :
			data(storage), width(0), height(0), x(0), y(0), id(0), channels(C), type(
					I) {
	}
	Image(const Image<T, C, I>& img) :
			Image(img.width, img.height, img.x, img.y, img.id) {
		set(img.data.data());
	}

	Image<T, C, I>& operator=(const Image<T, C, I>& rhs) {
		if (this == &rhs)
			return *this;
		this->resize(rhs.width, rhs.height);
		this->x = x;
		this->y = y;
		this->id = id;
		this->set(rhs.data.data());
		return *this;
	}
	int2 dimensions() const {
		return int2(width, height);
	}
	int2 position() const {
		return int2(x, y);
	}
	size_t size() const {
		return data.size();
	}
	size_t typeSize() const {
		return sizeof(vec<T, C> );
	}
	void resize(int w, int h) {
		data.resize(w * h);
		data.shrink_to_fit();
		width = w;
		height = h;
	}
	inline void clear() {
		data.clear();
		data.shrink_to_fit();
		width = 0;
		height = 0;
	}
	vec<T, C>* vecPtr() {
		if (data.size() == 0)
			return nullptr;
		return data.data();
	}
	const vec<T, C>* vecPtr() const {
		if (data.size() == 0)
			return nullptr;
		return data.data();
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
	vec<T, C>& operator()(const int i, const int j) {
		return data[clamp(i, 0, width - 1) + clamp(j, 0, height - 1) * width];
	}
	vec<T, C>& operator()(const int2 ij) {
		return data[clamp(ij.x, 0, width - 1)
				+ clamp(ij.y, 0, height - 1) * width];
	}
	const vec<T, C>& operator()(const int i, const int j) const {
		return data[clamp(i, 0, width - 1) + clamp(j, 0, height - 1) * width];
	}
	const vec<T, C>& operator()(const int2 ij) const {
		return data[clamp(ij.x, 0, width - 1)
				+ clamp(ij.y, 0, height - 1) * width];
	}
	template<class K> vec<K, C> operator()(const K x, const K y) {
		int i = static_cast<int>(std::floor(x));
		int j = static_cast<int>(std::floor(y));
		vec<K, C> rgb00 = ConvertType<K, T, C>(operator()(i, j));
		vec<K, C> rgb10 = ConvertType<K, T, C>(operator()(i + 1, j));
		vec<K, C> rgb11 = ConvertType<K, T, C>(operator()(i + 1, j + 1));
		vec<K, C> rgb01 = ConvertType<K, T, C>(operator()(i, j + 1));
		K dx = x - i;
		K dy = y - j;
		return ((rgb00 * (K(1) - dx) + rgb10 * dx) * (K(1) - dy)
				+ (rgb01 * (K(1) - dx) + rgb11 * dx) * dy);
	}
	template<class K> inline vec<K, C> operator()(const vec<K, 2>& pt) {
		return operator()(pt.x, pt.y);
	}
	template<class F> void apply(F f) {
		size_t sz = size();
#pragma omp parallel for
		for (int offset = 0; offset < (int) sz; offset++) {
			f(offset, data[offset]);
		}
	}
	void downsample(Image<T, C, I>& out) {
		static const double Kernel[5][5] = { { 1, 4, 6, 4, 1 }, { 4, 16, 24, 16,
				4 }, { 6, 24, 36, 24, 6 }, { 4, 16, 24, 16, 4 },
				{ 1, 4, 6, 4, 1 } };
		out.resize(width / 2, height / 2);
#pragma omp parallel for
		for (int i = 0; i < out.width; i++) {
			for (int j = 0; j < out.height; j++) {
				vec<double, C> vsum(0.0);
				for (int ii = 0; ii < 5; ii++) {
					for (int jj = 0; jj < 5; jj++) {
						vsum += Kernel[ii][jj]
								* vec<double, C>(
										operator()(2 * i + ii - 2,
												2 * j + jj - 2));
					}
				}
				out(i, j) = vec<T, C>(vsum / 256.0);
			}
		}
	}
	void upsample(Image<T, C, I>& out) {
		static const double Kernel[5][5] = { { 1, 4, 6, 4, 1 }, { 4, 16, 24, 16,
				4 }, { 6, 24, 36, 24, 6 }, { 4, 16, 24, 16, 4 },
				{ 1, 4, 6, 4, 1 } };
		if (out.size() == 0)
			out.resize(width * 2, height * 2);
#pragma omp parallel for
		for (int i = 0; i < out.width; i++) {
			for (int j = 0; j < out.height; j++) {
				vec<double, C> vsum(0.0);
				for (int ii = 0; ii < 5; ii++) {
					for (int jj = 0; jj < 5; jj++) {
						int iii = i + ii - 2;
						int jjj = j + jj - 2;
						if (iii % 2 == 0 && jjj % 2 == 0) {
							vsum += Kernel[ii][jj]
									* vec<double, C>(
											operator()(iii / 2, jjj / 2));
						}
					}
				}
				out(i, j) = vec<T, C>(vsum / 64.0);
			}
		}
	}
	Image<T, C, I> downsample() {
		Image<T, C, I> out;
		downsample(out);
		return out;
	}
	Image<T, C, I> upsample() {
		Image<T, C, I> out;
		upsample(out);
		return out;
	}
	vec<T, C> min() {
		vec<T, C> minVal(std::numeric_limits<T>::max());
		for (vec<T, C>& val : data) {
			minVal = aly::minVec(val, minVal);
		}
		return minVal;
	}
	vec<T, C> max() {
		vec<T, C> maxVal(std::numeric_limits<T>::min());
		for (vec<T, C>& val : data) {
			maxVal = aly::maxVec(val, maxVal);
		}
		return maxVal;
	}
	std::pair<vec<T, C>, vec<T, C>> range() {
		vec<T, C> maxVal(std::numeric_limits<T>::min());
		vec<T, C> minVal(std::numeric_limits<T>::max());
		for (vec<T, C>& val : data) {
			maxVal = aly::maxVec(val, maxVal);
			minVal = aly::minVec(val, minVal);
		}
		return std::pair<vec<T, C>, vec<T, C>>(minVal, maxVal);
	}
	vec<T, C> mean() {
		vec<double, C> mean(0.0);
		for (vec<T, C>& val : data) {
			mean += vec<double, C>(val);
		}
		mean = mean / (double) data.size();
		return vec<T, C>(mean);
	}
	vec<T, C> median() {
		std::vector<T> bands[C];
		for (int c = 0; c < C; c++) {
			bands[c].resize(data.size());
		}
		size_t index = 0;
		for (vec<T, C>& val : data) {
			for (int c = 0; c < C; c++) {
				bands[c][index] = val[c];
			}
			index++;
		}
#pragma omp parallel for
		for (int c = 0; c < C; c++) {
			std::sort(bands[c].begin(), bands[c].end());
		}
		vec<T, C> med;
		if (data.size() % 2 == 0) {
			for (int c = 0; c < C; c++) {
				med[c] = T(
						((double) bands[c][data.size() / 2]
								+ (double) bands[c][data.size() / 2 - 1])
								* 0.5f);
			}
		} else {
			for (int c = 0; c < C; c++) {
				med[c] = bands[c][data.size() / 2];
			}
		}
		return med;
	}
	vec<T, C> mad() {
		if (data.size() <= 2)
			return vec<T, C>(T(0));
		vec<T, C> med = median();
		std::vector<T> bands[C];
		for (int c = 0; c < C; c++) {
			bands[c].resize(data.size());
		}
		size_t index = 0;
		for (vec<T, C>& val : data) {
			vec<T, C> e = aly::abs(val - med);
			for (int c = 0; c < C; c++) {
				bands[c][index] = e[c];
			}
			index++;
		}
#pragma omp parallel for
		for (int c = 0; c < C; c++) {
			std::sort(bands[c].begin(), bands[c].end());
		}
		vec<T, C> mad;
		if (data.size() % 2 == 0) {
			for (int c = 0; c < C; c++) {
				mad[c] = T(
						((double) bands[c][data.size() / 2]
								+ (double) bands[c][data.size() / 2 - 1])
								* 0.5f);
			}
		} else {
			for (int c = 0; c < C; c++) {
				mad[c] = bands[c][data.size() / 2];
			}
		}
		return mad;
	}
	vec<T, C> madStdDev() {
		return vec<T, C>(1.4826 * vec<double, C>(mad()));
	}
	vec<T, C> stdDev() {
		if (data.size() < 2) {
			return vec<T, C>(T(0));
		}
		vec<T, C> avg = mean();
		vec<double, C> var(0.0);
		for (vec<T, C>& val : data) {
			vec<double, C> e = vec<double, C>(val - avg);
			var += e * e;
		}
		var = var / (double) (data.size() - 1);
		return vec<T, C>(aly::sqrt(var));
	}
};
template<class T, int C, ImageType I> std::string Image<T, C, I>::updateHashCode(
		size_t MAX_SAMPLES, HashMethod method) {
	if (MAX_SAMPLES == 0) {
		hashCode = HashCode(data, method);
	} else {
		const size_t seed = 83128921L;
		std::mt19937 mt(seed);
		std::uniform_int_distribution<int> wSampler(0, width - 1);
		std::uniform_int_distribution<int> hSampler(0, height - 1);
		std::vector<vec<T, C>> sample(MAX_SAMPLES);
		for (int i = 0; i < (int) MAX_SAMPLES; i++) {
			sample[i] = this->operator()(wSampler(mt), hSampler(mt));
		}
		hashCode = HashCode(sample, method);
	}
	return hashCode;
}
template<class T, int C, ImageType I> void Transform(Image<T, C, I>& im1,
		Image<T, C, I>& im2,
		const std::function<void(vec<T, C>&, vec<T, C>&)>& func) {
	if (im1.dimensions() != im2.dimensions())
		throw std::runtime_error(
				MakeString() << "Image dimensions do not match. "
						<< im1.dimensions() << "!=" << im2.dimensions());
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1.data[offset], im2.data[offset]);
	}
}
template<class T, int C, ImageType I> void Transform(Image<T, C, I>& im1,
		const std::function<void(vec<T, C>&)>& func) {
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1.data[offset]);
	}
}
template<class T, int C, ImageType I> void Transform(Image<T, C, I>& im1,
		const Image<T, C, I>& im2,
		const std::function<void(vec<T, C>&, const vec<T, C>&)>& func) {
	if (im1.dimensions() != im2.dimensions())
		throw std::runtime_error(
				MakeString() << "Image dimensions do not match. "
						<< im1.dimensions() << "!=" << im2.dimensions());
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1.data[offset], im2.data[offset]);
	}
}
template<class T, int C, ImageType I> void Transform(Image<T, C, I>& im1,
		const Image<T, C, I>& im2, const Image<T, C, I>& im3,
		const std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)>& func) {
	if (im1.dimensions() != im2.dimensions())
		throw std::runtime_error(
				MakeString() << "Image dimensions do not match. "
						<< im1.dimensions() << "!=" << im2.dimensions());
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1.data[offset], im2.data[offset], im3.data[offset]);
	}
}
template<class T, int C, ImageType I> void Transform(Image<T, C, I>& im1,
		const Image<T, C, I>& im2, const Image<T, C, I>& im3,
		const Image<T, C, I>& im4,
		const std::function<
				void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&,
						const vec<T, C>&)>& func) {
	if (im1.dimensions() != im2.dimensions())
		throw std::runtime_error(
				MakeString() << "Image dimensions do not match. "
						<< im1.dimensions() << "!=" << im2.dimensions());
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1.data[offset], im2.data[offset], im3.data[offset],
				im4.data[offset]);
	}
}
template<class T, int C, ImageType I> void Transform(Image<T, C, I>& im1,
		Image<T, C, I>& im2,
		const std::function<void(int i, int j, vec<T, C>& val1, vec<T, C>& val2)>& func) {
	if (im1.dimensions() != im2.dimensions())
		throw std::runtime_error(
				MakeString() << "Image dimensions do not match. "
						<< im1.dimensions() << "!=" << im2.dimensions());
#pragma omp parallel for
	for (int j = 0; j < im1.height; j++) {
		for (int i = 0; i < im1.width; i++) {
			size_t offset = i + j * im1.width;
			func(i, j, im1.data[offset], im2.data[offset]);
		}
	}
}
template<class T, int C, ImageType I> void Transform(Image<T, C, I>& im1,
		Image<T, C, I>& im2,
		const std::function<
				void(size_t offset, vec<T, C>& val1, vec<T, C>& val2)>& func) {
	if (im1.dimensions() != im2.dimensions())
		throw std::runtime_error(
				MakeString() << "Image dimensions do not match. "
						<< im1.dimensions() << "!=" << im2.dimensions());
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(offset, im1.data[offset], im2.data[offset]);
	}
}
template<class T, class L, class R, int C, ImageType I> std::basic_ostream<L, R> & operator <<(
		std::basic_ostream<L, R> & ss, const Image<T, C, I> & A) {
	ss << "Image (" << A.getTypeName() << "): " << A.id << " Position: (" << A.x
			<< "," << A.y << ") Dimensions: [" << A.width << "," << A.height
			<< "]\n";
	return ss;
}
template<class T, int C, ImageType I> Image<T, C, I> operator+(
		const vec<T, C>& scalar, const Image<T, C, I>& img) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=scalar+val2;};
	Transform(out, img, f);
	return out;
}

template<class T, int C, ImageType I> Image<T, C, I> operator-(
		const vec<T, C>& scalar, const Image<T, C, I>& img) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=scalar-val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator*(
		const vec<T, C>& scalar, const Image<T, C, I>& img) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=scalar*val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator/(
		const vec<T, C>& scalar, const Image<T, C, I>& img) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=scalar/val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator+(
		const Image<T, C, I>& img, const vec<T, C>& scalar) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=val2+scalar;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator-(
		const Image<T, C, I>& img, const vec<T, C>& scalar) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=val2-scalar;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator*(
		const Image<T, C, I>& img, const vec<T, C>& scalar) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=val2*scalar;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator/(
		const Image<T, C, I>& img, const vec<T, C>& scalar) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=val2/scalar;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator-(
		const Image<T, C, I>& img) {
	Image<T, C, I> out(img.width, img.height, img.x, img.y);
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1=-val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator+=(
		Image<T, C, I>& out, const Image<T, C, I>& img) {
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1+=val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator-=(
		Image<T, C, I>& out, const Image<T, C, I>& img) {
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1-=val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator*=(
		Image<T, C, I>& out, const Image<T, C, I>& img) {
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1*=val2;};
	Transform(out, img, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator/=(
		Image<T, C, I>& out, const Image<T, C, I>& img) {
	std::function<void(vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2) {val1/=val2;};
	Transform(out, img, f);
	return out;
}

template<class T, int C, ImageType I> Image<T, C, I> operator+=(
		Image<T, C, I>& out, const vec<T, C>& scalar) {
	std::function<void(vec<T, C>&)> f = [=](vec<T,C>& val1) {val1+=scalar;};
	Transform(out, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator-=(
		Image<T, C, I>& out, const vec<T, C>& scalar) {
	std::function<void(vec<T, C>&)> f = [=](vec<T,C>& val1) {val1-=scalar;};
	Transform(out, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator*=(
		Image<T, C, I>& out, const vec<T, C>& scalar) {
	std::function<void(vec<T, C>&)> f = [=](vec<T,C>& val1) {val1*=scalar;};
	Transform(out, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator/=(
		Image<T, C, I>& out, const vec<T, C>& scalar) {
	std::function<void(vec<T, C>&)> f = [=](vec<T,C>& val1) {val1/=scalar;};
	Transform(out, f);
	return out;
}

template<class T, int C, ImageType I> Image<T, C, I> operator+(
		const Image<T, C, I>& img1, const Image<T, C, I>& img2) {
	Image<T, C, I> out(img1.width, img1.height);
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3) {val1=val2+val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator-(
		const Image<T, C, I>& img1, const Image<T, C, I>& img2) {
	Image<T, C, I> out(img1.width, img1.height);
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3) {val1=val2-val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator*(
		const Image<T, C, I>& img1, const Image<T, C, I>& img2) {
	Image<T, C, I> out(img1.width, img1.height);
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3) {val1=val2*val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T, int C, ImageType I> Image<T, C, I> operator/(
		const Image<T, C, I>& img1, const Image<T, C, I>& img2) {
	Image<T, C, I> out(img1.width, img1.height);
	std::function<void(vec<T, C>&, const vec<T, C>&, const vec<T, C>&)> f =
			[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3) {val1=val2/val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T, int C, ImageType I> void WriteImageToRawFile(
		const std::string& file, const Image<T, C, I>& img) {
	std::ostringstream vstr;
	std::string fileName = GetFileWithoutExtension(file);
	vstr << fileName << ".raw";
	FILE* f = fopen(vstr.str().c_str(), "wb");
	if (f == NULL) {
		throw std::runtime_error(
				MakeString() << "Could not open " << vstr.str().c_str()
						<< " for writing.");
	}
	for (int c = 0; c < img.channels; c++) {
		for (int j = 0; j < img.height; j++) {
			for (int i = 0; i < img.width; i++) {
				T val = img(i, j)[c];
				fwrite(&val, sizeof(float), 1, f);
			}
		}
	}
	fclose(f);
	std::string typeName = "";
	switch (img.type) {
	case ImageType::BYTE:
		typeName = "Byte";
		break;
	case ImageType::UBYTE:
		typeName = "Unsigned Byte";
		break;
	case ImageType::SHORT:
		typeName = "Short";
		break;
	case ImageType::USHORT:
		typeName = "Unsigned Short";
		break;
	case ImageType::INT:
		typeName = "Integer";
		break;
	case ImageType::UINT:
		typeName = "Unsigned Integer";
		break;
	case ImageType::FLOAT:
		typeName = "Float";
		break;
	case ImageType::DOUBLE:
		typeName = "Double";
		break;
	}
//std::cout << vstr.str() << std::endl;
	std::stringstream sstr;
	sstr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	sstr << "<!-- MIPAV header file -->\n";
	sstr
			<< "<image xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" nDimensions=\"3\">\n";
	sstr << "	<Dataset-attributes>\n";
	sstr << "		<Image-offset>0</Image-offset>\n";
	sstr << "		<Data-type>" << typeName << "</Data-type>\n";
	sstr << "		<Endianess>Little</Endianess>\n";
	sstr << "		<Extents>" << img.width << "</Extents>\n";
	sstr << "		<Extents>" << img.height << "</Extents>\n";
	sstr << "		<Extents>" << img.channels << "</Extents>\n";
	sstr << "		<Resolutions>\n";
	sstr << "			<Resolution>1.0</Resolution>\n";
	sstr << "			<Resolution>1.0</Resolution>\n";
	sstr << "			<Resolution>1.0</Resolution>\n";
	sstr << "		</Resolutions>\n";
	sstr << "		<Slice-spacing>1.0</Slice-spacing>\n";
	sstr << "		<Slice-thickness>0.0</Slice-thickness>\n";
	sstr << "		<Units>Millimeters</Units>\n";
	sstr << "		<Units>Millimeters</Units>\n";
	sstr << "		<Units>Millimeters</Units>\n";
	sstr << "		<Compression>none</Compression>\n";
	sstr << "		<Orientation>Unknown</Orientation>\n";
	sstr << "		<Subject-axis-orientation>Unknown</Subject-axis-orientation>\n";
	sstr << "		<Subject-axis-orientation>Unknown</Subject-axis-orientation>\n";
	sstr << "		<Subject-axis-orientation>Unknown</Subject-axis-orientation>\n";
	sstr << "		<Origin>0.0</Origin>\n";
	sstr << "		<Origin>0.0</Origin>\n";
	sstr << "		<Origin>0.0</Origin>\n";
	sstr << "		<Modality>Unknown Modality</Modality>\n";
	sstr << "	</Dataset-attributes>\n";
	sstr << "</image>\n";
	std::ofstream myfile;
	std::stringstream xmlFile;
	xmlFile << fileName << ".xml";
	myfile.open(xmlFile.str().c_str(), std::ios_base::out);
	if (!myfile.is_open()) {
		throw std::runtime_error(
				MakeString() << "Could not open " << xmlFile.str()
						<< " for writing.");
	}
	myfile << sstr.str();
	myfile.close();
}
typedef Image<uint8_t, 4, ImageType::UBYTE> ImageRGBA;
typedef Image<int, 4, ImageType::INT> ImageRGBAi;
typedef Image<float, 4, ImageType::FLOAT> ImageRGBAf;

typedef Image<uint8_t, 3, ImageType::UBYTE> ImageRGB;
typedef Image<int, 3, ImageType::INT> ImageRGBi;
typedef Image<float, 3, ImageType::FLOAT> ImageRGBf;

typedef Image<uint8_t, 1, ImageType::UBYTE> ImageA;
typedef Image<int, 1, ImageType::INT> ImageAi;
typedef Image<float, 1, ImageType::FLOAT> ImageAf;

typedef Image<uint8_t, 4, ImageType::UBYTE> Image4b;
typedef Image<uint16_t, 4, ImageType::USHORT> Image4us;
typedef Image<int16_t, 4, ImageType::SHORT> Image4s;
typedef Image<int, 4, ImageType::INT> Image4i;
typedef Image<uint32_t, 4, ImageType::UINT> Image4ui;
typedef Image<float, 4, ImageType::FLOAT> Image4f;

typedef Image<uint8_t, 3, ImageType::UBYTE> Image3b;
typedef Image<uint16_t, 3, ImageType::USHORT> Image3us;
typedef Image<int16_t, 3, ImageType::SHORT> Image3s;
typedef Image<int, 3, ImageType::INT> Image3i;
typedef Image<uint32_t, 3, ImageType::UINT> Image3ui;
typedef Image<float, 3, ImageType::FLOAT> Image3f;

typedef Image<uint8_t, 2, ImageType::UBYTE> Image2b;
typedef Image<uint16_t, 2, ImageType::USHORT> Image2us;
typedef Image<int16_t, 2, ImageType::SHORT> Image2s;
typedef Image<int, 2, ImageType::INT> Image2i;
typedef Image<uint32_t, 2, ImageType::UINT> Image2ui;
typedef Image<float, 2, ImageType::FLOAT> Image2f;

typedef Image<uint8_t, 1, ImageType::UBYTE> Image1b;
typedef Image<uint16_t, 1, ImageType::USHORT> Image1us;
typedef Image<int16_t, 1, ImageType::SHORT> Image1s;

typedef Image<int, 1, ImageType::INT> Image1i;
typedef Image<uint32_t, 1, ImageType::UINT> Image1ui;
typedef Image<float, 1, ImageType::FLOAT> Image1f;

void WriteImageToFile(const std::string& file, const ImageRGBA& img);
void WriteImageToFile(const std::string& file, const ImageRGB& img);
void WriteImageToFile(const std::string& file, const ImageRGBAf& img);
void WriteImageToFile(const std::string& file, const ImageRGBf& img);
void ReadImageFromFile(const std::string& file, ImageRGBA& img);
void ReadImageFromFile(const std::string& file, ImageRGB& img);
void ReadImageFromFile(const std::string& file, ImageRGBAf& img);
void ReadImageFromFile(const std::string& file, ImageRGBf& img);

void ConvertImage(const ImageRGBAf& in, ImageRGBA& out);
void ConvertImage(const ImageRGBf& in, ImageRGB& out);
void ConvertImage(const ImageRGBA& in, ImageRGBAf& out);
void ConvertImage(const ImageRGB& in, ImageRGBf& out);
void ConvertImage(const ImageRGBA& in, ImageRGB& out);
void ConvertImage(const ImageRGBAf& in, ImageRGBf& out);
void ConvertImage(const ImageRGB& in, ImageRGBA& out);
void ConvertImage(const ImageRGBf& in, ImageRGBAf& out);
}
;

#endif
