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

#ifndef ALLOYENUM_H_
#define ALLOYENUM_H_

#include "nanovg.h"
#include <iostream>
#include <iomanip>
namespace aly {
enum class Origin {
	TopLeft, Center, TopRight, BottomLeft, BottomRight,CenterLeft,CenterRight,CenterTop,CenterBottom
};
enum class GlyphType {
	Image, Awesome
};
enum class InputType {
	Unspecified, Cursor, MouseButton, Key, Character, Scroll
};
enum class HorizontalAlignment {
	Left = NVG_ALIGN_LEFT, Center = NVG_ALIGN_CENTER, Right = NVG_ALIGN_RIGHT
};
enum class VerticalAlignment {
	Top = NVG_ALIGN_TOP,
	Middle = NVG_ALIGN_MIDDLE,
	Bottom = NVG_ALIGN_BOTTOM,
	Baseline = NVG_ALIGN_BASELINE
};
enum class AspectRule {
	Unspecified, FixedWidth, FixedHeight,
};
enum class Shape {
	Rectangle, Ellipse
};
enum class Orientation {
	Unspecified = 0, Horizontal = 1, Vertical = 2
};
enum class FontType {
	Normal = 0, Bold = 1, Italic = 2, Icon = 3
};
enum class FontStyle {
	Normal = 0, Shadow = 1, Glow = 2, Outline = 3
};
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const GlyphType& type) {
	switch (type) {
	case GlyphType::Image:
		return ss << "Image";
	case GlyphType::Awesome:
		return ss << "Awesome";
	}
	return ss;
}

template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Origin& type) {
	switch (type) {
	case Origin::TopLeft:
		return ss << "Top Left";
	case Origin::TopRight:
		return ss << "Top Right";
	case Origin::BottomLeft:
		return ss << "Bottom Left";
	case Origin::BottomRight:
		return ss << "Bottom Right";
	case Origin::Center:
		return ss << "Center";
	}
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const FontType& type) {
	switch (type) {
	case FontType::Normal:
		return ss << "Normal";
	case FontType::Bold:
		return ss << "Bold";
	case FontType::Italic:
		return ss << "Italic";
	case FontType::Icon:
		return ss << "Icon";
	}
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const FontStyle& type) {
	switch (type) {
	case FontStyle::Normal:
		return ss << "Normal";
	case FontStyle::Shadow:
		return ss << "Shadow";
	case FontStyle::Glow:
		return ss << "Glow";
	case FontStyle::Outline:
		return ss << "Outline";
	}
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const InputType& type) {
	switch (type) {
	case InputType::Unspecified:
		return ss << "Unspecified";
	case InputType::Cursor:
		return ss << "Cursor";
	case InputType::MouseButton:
		return ss << "Mouse";
	case InputType::Key:
		return ss << "Key";
	case InputType::Character:
		return ss << "Character";
	case InputType::Scroll:
		return ss << "Scroll";
	}
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const HorizontalAlignment& type) {
	switch (type) {
	case HorizontalAlignment::Left:
		return ss << "Left";
	case HorizontalAlignment::Center:
		return ss << "Center";
	case HorizontalAlignment::Right:
		return ss << "Right";
	}
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const VerticalAlignment& type) {
	switch (type) {
	case VerticalAlignment::Top:
		return ss << "Top";
	case VerticalAlignment::Middle:
		return ss << "Middle";
	case VerticalAlignment::Bottom:
		return ss << "Bottom";
	case VerticalAlignment::Baseline:
		return ss << "Baseline";
	}
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Orientation& type) {
	switch (type) {
	case Orientation::Unspecified:
		return ss << "Unspecified";
	case Orientation::Horizontal:
		return ss << "Horizontal";
	case Orientation::Vertical:
		return ss << "Vertical";
	}
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const AspectRule& type) {
	switch (type) {
	case AspectRule::Unspecified:
		return ss << "Unspecified";
	case AspectRule::FixedWidth:
		return ss << "Fixed Width";
	case AspectRule::FixedHeight:
		return ss << "Fixed Height";
	}
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Shape& type) {
	switch (type) {
	case Shape::Rectangle:
		return ss << "Rectangle";
	case Shape::Ellipse:
		return ss << "Ellipse";
	}
	return ss;
}
}
#endif /* ALLOYENUM_H_ */
