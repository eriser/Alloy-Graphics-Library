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

#ifndef ALLOYUNITS_H_
#define ALLOYUNITS_H_
#include "AlloyMath.h"
#include "nanovg.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <memory>
#include <list>
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/tuple.hpp"

namespace aly {

typedef float pixel;

const float MM_TO_PIX = 1.0f;
const float DP_TO_PIX = 1.0f / 160.0f;
const float IN_TO_PIX = 25.4f;
const float PT_TO_PIX = 1.333333f;

typedef vec<pixel, 2> pixel2;
typedef box<pixel, 2> box2px;

#define HALF_PIX(x) ((x)/(pixel)2)
#define THIRD_PIX(x) ((x)/(pixel)3)
#define QUATER_PIX(x) ((x)/(pixel)4)

struct Tweenable {
protected:
	double t = 0;
public:
	Tweenable(double t) :
			t(t) {

	}
	virtual void setTweenValue(double val) {
		t = clamp(val, 0.0, 1.0);
	}
	inline double getTweenValue() const {
		return t;
	}
	inline void reset() {
		t = 0;
	}
	void operator+=(const double& dt) {
		t = std::min(1.0, t + dt);
	}
	void operator-=(const double& dt) {
		t = std::max(0.0, t - dt);
	}
	void operator=(const double& val) {
		t = clamp(val, 0.0, 1.0);
	}
	void operator+=(const float& dt) {
		t = std::min(1.0, t + (double) dt);
	}
	void operator-=(const float& dt) {
		t = std::max(0.0, t - (double) dt);
	}
	void operator=(const float& val) {
		t = clamp(val, 0.0f, 1.0f);
	}

	virtual inline ~Tweenable() {
	}
};
class Number {
public:
	struct Interface {
		virtual int toInteger() const = 0;
		virtual float toFloat() const = 0;
		virtual double toDouble() const = 0;
		virtual bool toBoolean() const = 0;
		virtual void setValue(int value) = 0;
		virtual void setValue(float value) = 0;
		virtual void setValue(double value) = 0;
		virtual void setValue(bool value) = 0;
		virtual std::string toString() const = 0;
		std::string virtual type() const = 0;
	};
private:
	template<class T> struct Impl: public Interface {
		T value;
		Impl(const T& value) :
				value(value) {
		}
		std::string virtual type() const {
			return value.type();
		}
		virtual int toInteger() const {
			return value.toInteger();
		}
		virtual float toFloat() const {
			return value.toFloat();
		}
		virtual double toDouble() const {
			return value.toDouble();
		}
		virtual bool toBoolean() const {
			return value.toBoolean();
		}
		virtual std::string toString() const {
			return value.toString();
		}
		virtual void setValue(int val) {
			value.setValue(val);
		}
		virtual void setValue(float val) {
			value.setValue(val);
		}
		virtual void setValue(double val) {
			value.setValue(val);
		}
		virtual void setValue(bool val) {
			value.setValue(val);
		}
	};
	std::shared_ptr<Interface> impl;
public:
	template<class Archive> void save(Archive& archive) const {
		if (impl.get()) {
			archive(cereal::make_nvp(impl->type(), impl->toString()));
		} else {
			archive("");
		}
	}
	template<class Archive> void load(Archive& archive) {

	}
	Number() {
	}
	Number(const Number & r) :
			impl(r.impl) {
	}

	template<class T> Number(const T & value) :
			impl(new Impl<T> { value }) {
	}
	template<class T> Number(T* value) :
			impl(value) {
	}
	Number & operator =(const Number & r) {
		impl = r.impl;
		return *this;
	}
	template<class T> Number & operator =(const T & value) {
		return *this = Number(value);
	}
	virtual int toInteger() const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		return impl->toInteger();
	}
	virtual float toFloat() const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		return impl->toFloat();
	}
	virtual double toDouble() const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		return impl->toDouble();
	}
	virtual bool toBoolean() const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		return impl->toBoolean();
	}
	std::string toString() const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		return impl->toString();
	}
	virtual void setValue(int val) {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		impl->setValue(val);
	}
	virtual void setValue(float val) {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		impl->setValue(val);
	}
	virtual void setValue(double val) {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		impl->setValue(val);
	}
	virtual void setValue(bool val) {
		if (impl.get() == nullptr)
			throw std::runtime_error("Number storage type has not been defined.");
		impl->setValue(val);
	}
	virtual std::string type() const {
                if (impl.get() == nullptr)
                        throw std::runtime_error("Number storage type has not been defined.");
                return impl->type();
        }
	virtual inline ~Number() {

	}
};
struct Integer {
private:
	int value = 0;
public:
	std::string virtual type() const {
		return "int";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}

	int toInteger() const {
		return value;
	}
	float toFloat() const {
		return (float) value;
	}
	double toDouble() const {
		return (double) value;
	}
	bool toBoolean() const {
		return (value != 0);
	}
	std::string toString() const {
		return MakeString() << value;
	}
	void setValue(float other) {
		value = (int) other;
	}
	void setValue(double other) {
		value = (int) other;
	}
	void setValue(int other) {
		value = (int) other;
	}
	void setValue(bool other) {
		value = other ? 1 : 0;
	}
	Integer(int value) :
			value(value) {
	}
	virtual ~Integer() {
	}
};

struct Float {
private:
	float value = 0;
public:
	std::string virtual type() const {
		return "float";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	int toInteger() const {
		return (int) value;
	}
	float toFloat() const {
		return (float) value;
	}
	double toDouble() const {
		return (double) value;
	}
	bool toBoolean() const {
		return (value != 0);
	}
	std::string toString() const {
		return MakeString() << value;
	}
	void setValue(float other) {
		value = (float) other;
	}
	void setValue(double other) {
		value = (float) other;
	}
	void setValue(int other) {
		value = (float) other;
	}
	void setValue(bool other) {
		value = other ? 1.0f : 0.0f;
	}
	Float(float value) :
			value(value) {
	}
	virtual ~Float() {
	}
};
struct Double {
private:
	double value = 0;
public:
	std::string virtual type() const {
		return "double";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	int toInteger() const {
		return (int) value;
	}
	float toFloat() const {
		return (float) value;
	}
	double toDouble() const {
		return (double) value;
	}
	bool toBoolean() const {
		return (value != 0);
	}
	std::string toString() const {
		return MakeString() << value;
	}
	void setValue(float other) {
		value = (double) other;
	}
	void setValue(double other) {
		value = (double) other;
	}
	void setValue(int other) {
		value = (double) other;
	}
	void setValue(bool other) {
		value = other ? 1.0 : 0.0;
	}
	Double(double value) :
			value(value) {
	}
	virtual ~Double() {
	}
};
struct Boolean {
private:
	bool value = 0;
public:
	std::string virtual type() const {
		return "boolean";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	int toInteger() const {
		return value ? 1 : 0;
	}
	float toFloat() const {
		return value ? 1.0f : 0.0f;
	}
	double toDouble() const {
		return value ? 1.0 : 0.0;
	}
	bool toBoolean() const {
		return value;
	}
	std::string toString() const {
		return (value?"true":"false");
	}
	void setValue(float other) {
		value = (other != 0);
	}
	void setValue(double other) {
		value = (other != 0);
	}
	void setValue(int other) {
		value = (other != 0);
	}
	void setValue(bool other) {
		value = other;
	}
	Boolean(bool value) :
		value(value) {
	}
	virtual ~Boolean() {
	}
};
class AUnit1D {
public:
	struct Interface {
		virtual pixel toPixels(pixel screenSize, double dpmm,
				double pixelRatio) const = 0;
		virtual std::string toString() const = 0;
		std::string virtual type() const = 0;
	};
private:
	template<class T> struct Impl: public Interface {
		T value;
		Impl(const T& value) :
				value(value) {
		}
		std::string virtual type() const {
			return value.type();
		}
		virtual pixel toPixels(pixel screenSize, double dpmm,
				double pixelRatio) const {
			return value.toPixels(screenSize, dpmm, pixelRatio);
		}
		virtual std::string toString() const {
			return value.toString();
		}
	};
	std::shared_ptr<Interface> impl;
public:
	template<class Archive> void save(Archive& archive) const {
		if (impl.get()) {
			archive(cereal::make_nvp(impl->type(), impl->toString()));
		} else {
			archive("");
		}
	}
	// Can construct or assign any type that implements the implicit interface (According to Sterling)
	AUnit1D() {
	}
	AUnit1D(const AUnit1D & r) :
			impl(r.impl) {
	}
	virtual inline ~AUnit1D() {
	}
	template<class T> AUnit1D(const T & value) :
			impl(new Impl<T> { value }) {
	}
	template<class T> AUnit1D(T* value) :
			impl(value) {
	}
	AUnit1D & operator =(const AUnit1D & r) {
		impl = r.impl;
		return *this;
	}
	template<class T> AUnit1D & operator =(const T & value) {
		return *this = AUnit1D(value);
	}
	std::string virtual type() const {
		return impl->type();
	}
	// Implicit interface
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Unit storage type has not been defined.");
		return impl->toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Unit storage type has not been defined.");
		return impl->toString();
	}

	template<class Archive> void load(Archive& archive) {
	}
};
struct Color: public NVGcolor {
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp("red", r), cereal::make_nvp("green", g),
				cereal::make_nvp("blue", b), cereal::make_nvp("alpha", a));
	}
	Color() :
			NVGcolor() {
	}
	Color(const NVGcolor& color) :
			NVGcolor(color) {
	}
	Color(RGB color) :
			Color(nvgRGB(color.x, color.y, color.z)) {
	}
	Color(const RGBA& color) :
			Color(nvgRGBA(color.x, color.y, color.z, color.w)) {
	}
	Color(const RGBi& color) :
			Color(nvgRGB(color.x, color.y, color.z)) {
	}
	Color(const RGBAi& color) :
			Color(nvgRGBA(color.x, color.y, color.z, color.w)) {
	}
	Color(const RGBAf& color) :
			Color(nvgRGBAf(color.x, color.y, color.z, color.w)) {
	}
	Color(const RGBf& color) :
			Color(nvgRGBf(color.x, color.y, color.z)) {
	}
	Color(float r, float g, float b, float a) :
			Color(nvgRGBAf(r, g, b, a)) {
	}
	Color(float r, float g, float b) :
			Color(nvgRGBf(r, g, b)) {
	}
	Color(const Color& color, int alpha) :
			Color(color.r, color.g, color.b, alpha / 255.0f) {
	}
	Color(const Color& color, float alpha) :
			Color(color.r, color.g, color.b, alpha) {
	}
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
			Color(nvgRGBA(r, g, b, a)) {
	}
	Color(uint8_t r, uint8_t g, uint8_t b) :
			Color(nvgRGB(r, g, b)) {
	}
	Color(int r, int g, int b, int a) :
			Color(nvgRGBA(r, g, b, a)) {
	}
	Color(int r, int g, int b) :
			Color(nvgRGB(r, g, b)) {
	}
	Color(int lum) :
			Color(nvgRGB(lum, lum, lum)) {
	}
	inline Color toSemiTransparent(float alpha) const {
		return Color(r, g, b, alpha);
	}
	inline Color toDarker(float alpha) const {
		alpha = clamp(alpha, 0.0f, 1.0f);
		return Color(alpha * r, alpha * g, alpha * b, a);
	}
	inline Color toLighter(float alpha) const {
		alpha += 1.0f;
		return Color(clamp(alpha * r, 0.0f, 1.0f), clamp(alpha * g, 0.0f, 1.0f),
				clamp(alpha * b, 0.0f, 1.0f), a);
	}
	inline Color toOpposite() const {
		return Color(1.0f - r, 1.0f - g, 1.0f - b, a);
	}
	inline Color toSemiTransparent(int alpha) const {
		return Color(r, g, b, alpha / 255.0f);
	}
	HSVA toHSVA() const;
	HSV toHSV() const;
	RGBA toRGBA() const;
	RGBf toRGBf() const {
		return RGBf(r, g, b);
	}
	RGBAf toRGBAf() const {
		return RGBAf(r, g, b, a);
	}
	float3 toXYZ();
	float3 toLAB();
	float4 toXYZA();
	float4 toLABA();
};
Color HSVtoColor(const HSV& hsv);
Color HSVAtoColor(const HSVA& hsv);
RGBf HSVtoRGBf(const HSV& hsv);
RGBAf HSVAtoRGBAf(const HSVA& hsv);
float3 RGBtoXYZ(const RGBf& rgb);
float3 RGBtoLAB(const float3& rgb);
float4 RGBAtoXYZA(const RGBAf& rgb);
float4 RGBAtoLABA(const float4& rgb);

typedef std::shared_ptr<Color> AColor;
class AUnit2D {
public:
	struct Interface {
		virtual pixel2 toPixels(pixel2 screenSize, double2 dpmm,
				double pixelRatio) const = 0;
		virtual std::string toString() const = 0;
		std::string virtual type() const = 0;
	};
private:
	template<class T> struct Impl: public Interface {
		T value;
		Impl(const T& value) :
				value(value) {
		}
		;
		std::string virtual type() const {
			return value.type();
		}
		virtual pixel2 toPixels(pixel2 screenSize, double2 dpmm,
				double pixelRatio) const {
			return value.toPixels(screenSize, dpmm, pixelRatio);
		}
		virtual std::string toString() const {
			return value.toString();
		}
	};
	std::shared_ptr<Interface> impl;
public:
	// Can construct or assign any type that implements the implicit interface (According to Sterling)
	AUnit2D() {
	}
	explicit AUnit2D(const AUnit2D & r) :
			impl(r.impl) {
	}
	virtual inline ~AUnit2D() {
	}
	template<class T> AUnit2D(const T& value) :
			impl(new Impl<T> { value }) {
	}
	template<class T> AUnit2D(T* value) :
			impl(value) {
	}
	AUnit2D & operator =(const AUnit2D & r) {
		impl = r.impl;
		return *this;
	}
	template<class T> AUnit2D & operator =(const T & value) {
		return *this = AUnit2D(value);
	}
	std::string virtual type() const {
		return impl->type();
	}
	// Implicit interface
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Coord storage type has not been defined.");
		return impl->toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		if (impl.get() == nullptr)
			throw std::runtime_error("Coord storage type has not been defined.");
		return impl->toString();
	}
	template<class Archive> void save(Archive& archive) const {
		if (impl.get()) {
			archive(cereal::make_nvp(impl->type(), impl->toString()));
		} else {
			archive("");
		}
	}
	template<class Archive> void load(Archive& archive) {
	}
};
struct UnitDP {
	int value;
	std::string type() const {
		return "UnitDP";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitDP(int x) :
			value(x) {
	}
	UnitDP(float x) :
			value((int) x) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return (float) (DP_TO_PIX * dpmm * value);
	}
	std::string toString() const {
		return MakeString() << value;
	}
};

struct UnitPX {
	pixel value;
	std::string type() const {
		return "UnitPX";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitPX(float x) :
			value((pixel) x) {
	}
	UnitPX(double x) :
			value((pixel) x) {
	}
	UnitPX(int x) :
			value((pixel) x) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return value;
	}
	std::string toString() const {
		return MakeString() << value;
	}
};

struct UnitPT {
	float value;
	std::string type() const {
		return "UnitPT";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitPT(float x) :
			value(x) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return (float) (PT_TO_PIX * value * pixelRatio);
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct UnitMM {
	float value;
	std::string type() const {
		return "UnitMM";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitMM(float x) :
			value(x) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return (float) (MM_TO_PIX * dpmm * value);
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct UnitIN {
	float value;
	std::string type() const {
		return "UnitIN";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitIN(float x) :
			value(x) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return (float) (IN_TO_PIX * dpmm * value);
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct UnitPercent {
	float value;
	std::string type() const {
		return "UnitPercent";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitPercent(float x) :
			value(x) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return (float) (screenSize * value);
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct UnitPerPX {
	std::pair<UnitPercent, UnitPX> value;
	std::string type() const {
		return "UnitPerPX";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitPerPX(float px, float x) :
			value(UnitPercent(px), UnitPX(x)) {
	}
	UnitPerPX(float px, double x) :
			value(UnitPercent(px), UnitPX(x)) {
	}
	UnitPerPX(float px, int x) :
			value(UnitPercent(px), UnitPX(x)) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct UnitPerPT {
	std::pair<UnitPercent, UnitPT> value;
	std::string type() const {
		return "UnitPerPT";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitPerPT(float px, float x) :
			value(UnitPercent(px), UnitPT(x)) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct UnitPerMM {
	std::pair<UnitPercent, UnitMM> value;
	std::string type() const {
		return "UnitPerMM";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitPerMM(float px, float x) :
			value(UnitPercent(px), UnitMM(x)) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct UnitPerIN {
	std::pair<UnitPercent, UnitIN> value;
	std::string type() const {
		return "UnitPerIN";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	UnitPerIN(float px, float x) :
			value(UnitPercent(px), UnitIN(x)) {
	}
	pixel toPixels(pixel screenSize, double dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct UnitTween: public AUnit1D::Interface, Tweenable {
	std::pair<AUnit1D, AUnit1D> value;
	std::string type() const {
		return "UnitTween";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value), cereal::make_nvp("time", t));
	}
	UnitTween(const AUnit1D& start, const AUnit1D end, double t = 0) :
			Tweenable(t), value(start, end) {
	}
	virtual std::string toString() const override {
		return MakeString() << "{" << value.first << ", " << value.second
				<< ", tween = " << t << "}";
	}
	virtual pixel toPixels(pixel screenSize, double dpmm,
			double pixelRatio) const override {
		return mix(value.first.toPixels(screenSize, dpmm, pixelRatio),
				value.second.toPixels(screenSize, dpmm, pixelRatio), t);
	}
};
struct CoordDP {
	int2 value;
	std::string type() const {
		return "CoordDP";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordDP(int x, int y) :
			value(x, y) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return pixel2((pixel) (DP_TO_PIX * dpmm.x * value.x),
				(pixel) (DP_TO_PIX * dpmm.y * value.y));
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct CoordPX {
	pixel2 value;
	std::string type() const {
		return "CoordPX";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordPX(float x, float y) :
			value((pixel) x, (pixel) y) {
	}
	CoordPX(double x, double y) :
			value((pixel) x, (pixel) y) {
	}
	CoordPX(const pixel2& pix) :
			value(pix) {
	}
	CoordPX(int x, int y) :
			value((pixel) x, (pixel) y) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return value;
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct CoordMM {
	float2 value;
	std::string type() const {
		return "CoordMM";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordMM(float x, float y) :
			value(x, y) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return pixel2((pixel) (MM_TO_PIX * dpmm.x * value.x),
				(pixel) (MM_TO_PIX * dpmm.y * value.y));
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct CoordPT {
	float2 value;
	std::string type() const {
		return "CoordPT";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordPT(float x, float y) :
			value(x, y) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return pixel2((pixel) (PT_TO_PIX * pixelRatio * value.x),
				(pixel) (PT_TO_PIX * pixelRatio * value.y));
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct CoordIN {
	float2 value;
	std::string type() const {
		return "CoordIN";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordIN(float x, float y) :
			value(x, y) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return pixel2((pixel) (IN_TO_PIX * dpmm.x * value.x),
				(pixel) (IN_TO_PIX * dpmm.y * value.y));
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct CoordPercent {
	float2 value;
	std::string type() const {
		return "CoordPercent";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordPercent(float x, float y) :
			value(x, y) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return pixel2((pixel) (screenSize.x * value.x),
				(pixel) (screenSize.y * value.y));
	}
	std::string toString() const {
		return MakeString() << value;
	}
};
struct CoordPerDP {
	std::pair<CoordPercent, CoordDP> value;
	std::string type() const {
		return "CoordPerDP";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordPerDP(float px, float py, int x, int y) :
			value(CoordPercent(px, py), CoordDP(x, y)) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct CoordPerPX {
	std::pair<CoordPercent, CoordPX> value;
	std::string type() const {
		return "CoordPerPX";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordPerPX(float px, float py, int x, int y) :
			value(CoordPercent(px, py), CoordPX(x, y)) {
	}
	CoordPerPX(float px, float py, float x, float y) :
			value(CoordPercent(px, py), CoordPX(x, y)) {
	}
	CoordPerPX(float px, float py, double x, double y) :
			value(CoordPercent(px, py), CoordPX(x, y)) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct CoordPerPT {
	std::pair<CoordPercent, CoordPT> value;
	std::string type() const {
		return "CoordPerPT";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordPerPT(float px, float py, float x, float y) :
			value(CoordPercent(px, py), CoordPT(x, y)) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct CoordPerMM {
	std::pair<CoordPercent, CoordMM> value;
	std::string type() const {
		return "CoordPerMM";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordPerMM(float px, float py, float x, float y) :
			value(CoordPercent(px, py), CoordMM(x, y)) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct CoordPerIN {
	std::pair<CoordPercent, CoordIN> value;
	std::string type() const {
		return "CoordPerIN";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value));
	}
	CoordPerIN(float px, float py, float x, float y) :
			value(CoordPercent(px, py), CoordIN(x, y)) {
	}
	pixel2 toPixels(pixel2 screenSize, double2 dpmm, double pixelRatio) const {
		return value.first.toPixels(screenSize, dpmm, pixelRatio)
				+ value.second.toPixels(screenSize, dpmm, pixelRatio);
	}
	std::string toString() const {
		return MakeString() << "{" << value.first.toString() << ","
				<< value.second.toString() << "}";
	}
};
struct CoordTween: public Tweenable, AUnit2D::Interface {
	std::pair<AUnit2D, AUnit2D> value;
	std::string type() const {
		return "CoordTween";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value), cereal::make_nvp("time", t));
	}
	CoordTween(const AUnit2D& start, const AUnit2D& end, double t = 0) :
			Tweenable(t), value(start, end) {
	}
	virtual std::string toString() const override {
		return MakeString() << "{" << value.first << ", " << value.second
				<< ", tween = " << t << "}";
	}
	virtual pixel2 toPixels(pixel2 screenSize, double2 dpmm,
			double pixelRatio) const override {
		return mix(value.first.toPixels(screenSize, dpmm, pixelRatio),
				value.second.toPixels(screenSize, dpmm, pixelRatio), t);
	}
};

struct ColorTween: public Tweenable, Color {
	std::pair<Color, Color> value;
	std::string type() const {
		return "ColorTween";
	}
	template<class Archive> void serialize(Archive& archive) {
		archive(cereal::make_nvp(type(), value), cereal::make_nvp("time", t));
	}
	ColorTween(const Color& start, const Color& end, double t = 0) :
			Tweenable(t), value(start, end) {
		setTweenValue(t);
	}
	virtual void setTweenValue(double val) override {
		t = val;
		r = mix(value.first.r, value.second.r, t);
		g = mix(value.first.g, value.second.g, t);
		b = mix(value.first.b, value.second.b, t);
		a = mix(value.first.a, value.second.a, t);
	}
	virtual std::string toString() const {
		return MakeString() << "{" << value.first << ", " << value.second
				<< ", tween = " << t << "}";
	}
};
inline AColor MakeColor(int r, int g, int b, int a) {
	return AColor(new Color(r, g, b, a));
}

inline AColor MakeColor(unsigned char r, unsigned char g, unsigned char b,
		unsigned char a) {
	return AColor(new Color(r, g, b, a));
}
inline AColor MakeColor(float r, float g, float b, float a) {
	return AColor(new Color(r, g, b, a));
}
inline AColor MakeColor(int r, int g, int b) {
	return AColor(new Color(r, g, b));
}
inline AColor MakeColor(unsigned char r, unsigned char g, unsigned char b) {
	return AColor(new Color(r, g, b));
}
inline AColor MakeColor(float r, float g, float b) {
	return AColor(new Color(r, g, b));
}
inline AColor MakeColor(const Color& c) {
	return AColor(new Color(c));
}
inline AColor MakeColor(const RGBA& c) {
	return AColor(new Color(c));
}
inline AColor MakeColor(const RGB& c) {
	return AColor(new Color(c));
}
inline AColor MakeColor(const RGBAf& c) {
	return AColor(new Color(c));
}
inline AColor MakeColor(const RGBf& c) {
	return AColor(new Color(c));
}
inline AColor MakeColor(const RGBAi& c) {
	return AColor(new Color(c));
}
inline AColor MakeColor(const RGBi& c) {
	return AColor(new Color(c));
}
inline AColor MakeColor(const Color& start, const Color& end) {
	return AColor(new ColorTween(start, end));
}

struct Theme {
public:
	Color HIGHLIGHT = Color(255, 255, 255);
	Color SHADOW = Color(0, 0, 0);
	Color LIGHT = Color(180, 180, 180);
	Color DARK = Color(64, 64, 64);
	Color NEUTRAL = Color(120, 120, 120);
	Color LIGHT_TEXT = Color(230, 230, 230);
	Color DARK_TEXT = Color(64, 64, 64);
	Color NEUTRAL_TEXT = Color(120, 120, 120);

	Color LINK = Color(128, 0, 0);
	float CORNER_RADIUS = 5.0f;
	float2 SPACING = float2(4.0f, 4.0f);
	Theme() {
	}
	static const Theme Default;
};

template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitDP & v) {
	return ss << v.value << " dp";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitPX & v) {
	return ss << v.value << " px";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitMM & v) {
	return ss << v.value << " mm";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitIN & v) {
	return ss << v.value << " in";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitPT & v) {
	return ss << v.value << " pt";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitPercent & v) {
	return ss << v.value * 100 << " %";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const AUnit1D& v) {
	return ss << v.toString();
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitPerPX & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitPerPT & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitPerMM & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitPerIN & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const UnitTween & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << ", tween = "
			<< v.getTweenValue() << "}";
}

template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordDP & v) {
	return ss << "(" << v.value.x << " dp, " << v.value.y << " dp)";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordPX & v) {
	return ss << "(" << v.value.x << " px, " << v.value.y << " px)";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordPT & v) {
	return ss << "(" << v.value.x << " pt, " << v.value.y << " pt)";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordMM & v) {
	return ss << "(" << v.value.x << " mm, " << v.value.y << " mm)";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordIN & v) {
	return ss << "(" << v.value.x << " in, " << v.value.y << " in)";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordPercent & v) {
	return ss << "(" << v.value.x * 100 << " %, " << v.value.y * 100 << " %)";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const AUnit2D& v) {
	return ss << v.toString();
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Number& v) {
	return ss << v.toString();
}

template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordPerDP & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordPerPX & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordPerPT & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordPerMM & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordPerIN & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const CoordTween & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << ", tween = "
			<< v.getTweenValue() << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Color & v) {
	return ss << "(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a << ")";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const ColorTween & v) {
	return ss << "{" << v.value.first << ", " << v.value.second << ", tween = "
			<< v.getTweenValue() << "}";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const AColor & v) {
	return ss << *v;
}
}
#endif /* ALLOYUNITS_H_ */
