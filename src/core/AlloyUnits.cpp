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
#include "AlloyUnits.h"
namespace aly {
const Theme Theme::Default = Theme();
HSVA Color::toHSVA() const {
	HSVA hsva;
	float h, s, v;
	float min, max, delta;
	min = std::min(std::min(r, g), b);
	max = std::max(std::max(r, g), b);
	v = max;
	delta = max - min;
	if (max != 0) {
		s = delta / max;
	} else {
		return float4(0, 0, 0, a);
	}
	if (delta < 1E-6f)
		h = 0;
	else if (r == max)
		h = (g - b) / delta;
	else if (g == max)
		h = 2 + (b - r) / delta;
	else
		h = 4 + (r - g) / delta;
	h *= 60;
	if (h < 0)
		h += 360;
	return float4(h / 360.0f, s, v, a);
}
RGBA Color::toRGBA() const {
	return RGBA((int) clamp(255.0f * r, 0.0f, 255.0f),
			(int) clamp(255.0f * g, 0.0f, 255.0f),
			(int) clamp(255.0f * b, 0.0f, 255.0f),
			(int) clamp(255.0f * a, 0.0f, 255.0f));
}
HSV Color::toHSV() const {
	HSV hsva;
	float h, s, v;
	float min, max, delta;
	min = std::min(std::min(r, g), b);
	max = std::max(std::max(r, g), b);
	v = max;
	delta = max - min;

	if (max != 0) {
		s = delta / max;
	} else {
		return float3(0, 0, 0);
	}
	if (delta < 1E-6f)
		h = 0;
	else if (r == max)
		h = (g - b) / delta;
	else if (g == max)
		h = 2 + (b - r) / delta;
	else
		h = 4 + (r - g) / delta;
	h *= 60;
	if (h < 0)
		h += 360;
	return float3(h / 360.0f, s, v);
}
RGBf HSVtoRGBf(const HSV& hsv) {
	float h = hsv.x * 360.0f;
	float s = hsv.y;
	float v = hsv.z;
	int i;
	float f, p, q, t;
	if (s == 0) {
		return RGBf(v, v, v);
	}
	h /= 60.0f; // sector 0 to 5
	i = (int) std::floor(h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	switch (i) {
	case 0:
		return RGBf(v, t, p);
	case 1:
		return RGBf(q, v, p);
	case 2:
		return RGBf(p, v, t);
	case 3:
		return RGBf(p, q, v);
	case 4:
		return RGBf(t, p, v);
	default:
		return RGBf(v, p, q);
	}
}
RGBAf HSVAtoRGBAf(const HSVA& hsv) {
	float h = hsv.x * 360.0f;
	float s = hsv.y;
	float v = hsv.z;
	int i;
	float f, p, q, t;
	if (s == 0) {
		return RGBAf(v, v, v, hsv.w);
	}
	h /= 60.0f; // sector 0 to 5
	i = (int) std::floor(h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	switch (i) {
	case 0:
		return RGBAf(v, t, p, hsv.w);
	case 1:
		return RGBAf(q, v, p, hsv.w);
	case 2:
		return RGBAf(p, v, t, hsv.w);
	case 3:
		return RGBAf(p, q, v, hsv.w);
	case 4:
		return RGBAf(t, p, v, hsv.w);
	default:
		return RGBAf(v, p, q, hsv.w);
	}
}
Color HSVtoColor(const HSV& hsv) {
	float h = hsv.x * 360.0f;
	float s = hsv.y;
	float v = hsv.z;
	int i;
	float f, p, q, t;
	if (s == 0) {
		return Color(v, v, v);
	}
	h /= 60.0f; // sector 0 to 5
	i = (int) std::floor(h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	switch (i) {
	case 0:
		return Color(v, t, p);
	case 1:
		return Color(q, v, p);
	case 2:
		return Color(p, v, t);
	case 3:
		return Color(p, q, v);
	case 4:
		return Color(t, p, v);
	default:
		return Color(v, p, q);
	}
}
Color HSVAtoColor(const HSVA& hsva) {
	Color c = HSVtoColor(hsva.xyz());
	c.a = hsva.w;
	return c;
}
}

