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

#include "AlloyApplication.h"
#include "AlloyWidget.h"
#include "AlloyDrawUtil.h"
#include "AlloyContext.h"
#include <future>
#include <cstdlib>
#include <cctype>

using namespace std;
namespace aly {
	bool CheckBox::handleMouseDown(AlloyContext* context, const InputEvent& event) {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
			this->checked = !this->checked;
			this->valueLabel->textColor =
				(this->checked) ?
				MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT) :
				MakeColor(AlloyApplicationContext()->theme.DARK);
			if (onChange)
				onChange(this->checked);
			return true;
		}
		return false;
	}
	CheckBox::CheckBox(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool checked) :
		Composite(label, position, dimensions), checked(checked) {
		this->aspectRatio = 4.0f;
		CompositePtr valueContainer = MakeComposite("Check Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
		checkLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
		valueLabel = MakeTextLabel(CodePointToUTF8(0xf00c),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), FontType::Icon,
			UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);
		valueLabel->setAspectRatio(1.0f);
		valueLabel->setOrigin(Origin::TopRight);
		valueLabel->setAspectRule(AspectRule::FixedHeight);
		valueContainer->add(checkLabel);
		valueContainer->add(valueLabel);
		add(valueContainer);
		this->valueLabel->textColor =
			(this->checked) ?
			MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT) :
			MakeColor(AlloyApplicationContext()->theme.DARK);
		valueLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
		Region::onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
		checkLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
	}
	void CheckBox::setValue(bool value) {
		this->checked = value;
		this->valueLabel->textColor =
			(this->checked) ?
			MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT) :
			MakeColor(AlloyApplicationContext()->theme.DARK);
	}
	void CheckBox::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		bool hover = context->isMouseContainedIn(this);
		if (hover) {

			checkLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
		}
		else {
			checkLabel->textColor = MakeColor(context->theme.LIGHT_TEXT);
		}
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, context->theme.DARK);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0), context->theme.HIGHLIGHT);
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgFillColor(nvg, context->theme.NEUTRAL);
		box2px clickbox = valueLabel->getBounds();
		nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
			clickbox.dimensions.x, clickbox.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);

		if (hover) {
			nvgBeginPath(nvg);
			nvgStrokeColor(nvg, context->theme.HIGHLIGHT);
			nvgStrokeWidth(nvg, 2.0f);
			nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
				clickbox.dimensions.x, clickbox.dimensions.y,
				context->theme.CORNER_RADIUS);
			nvgStroke(nvg);
		}

		Composite::draw(context);
	}
	bool ToggleBox::handleMouseDown(AlloyContext* context,
		const InputEvent& event) {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
			this->toggledOn = !this->toggledOn;
			onLabel->setVisible(this->toggledOn);
			offLabel->setVisible(!this->toggledOn);
			if (onChange)
				onChange(this->toggledOn);
			return true;
		}
		return false;
	}
	ToggleBox::ToggleBox(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool checked) :
		Composite(label, position, dimensions), toggledOn(checked) {
		this->aspectRatio = 4.0f;
		CompositePtr valueContainer = MakeComposite("Check Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
		toggleLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
		onLabel = MakeTextLabel("ON", CoordPercent(0.2f, 0.0f),
			CoordPercent(0.3f, 1.0f), FontType::Bold, UnitPerPX(1.0f, -4.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT,
			HorizontalAlignment::Center, VerticalAlignment::Middle);
		offLabel = MakeTextLabel("OFF", CoordPercent(0.5f, 0.0f),
			CoordPercent(0.3f, 1.0f), FontType::Bold, UnitPerPX(1.0f, -4.0f),
			AlloyApplicationContext()->theme.DARK_TEXT,
			HorizontalAlignment::Center, VerticalAlignment::Middle);

		clickRegion = MakeComposite("tog select", CoordPercent(1.0f, 0.0f),
			CoordPercent(0.42f, 1.0f));
		clickRegion->setOrigin(Origin::TopRight);
		clickRegion->setAspectRatio(2.5f);
		clickRegion->setAspectRule(AspectRule::FixedHeight);
		clickRegion->add(onLabel);
		clickRegion->add(offLabel);
		valueContainer->add(toggleLabel);
		valueContainer->add(clickRegion);
		add(valueContainer);
		onLabel->setVisible(this->toggledOn);
		offLabel->setVisible(!this->toggledOn);

		onLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
		offLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
		clickRegion->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
		toggleLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
		Region::onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};

	}
	void ToggleBox::setValue(bool value) {
		this->toggledOn = value;
		onLabel->setVisible(this->toggledOn);
		offLabel->setVisible(!this->toggledOn);
	}
	void ToggleBox::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		bool hover = context->isMouseContainedIn(this);
		if (hover) {
			toggleLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
		}
		else {
			toggleLabel->textColor = MakeColor(context->theme.LIGHT_TEXT);
		}
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, context->theme.DARK);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0), context->theme.HIGHLIGHT);
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgFillColor(nvg, context->theme.NEUTRAL);
		box2px clickbox = clickRegion->getBounds();
		float radius = clickbox.dimensions.y / 2;
		nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
			clickbox.dimensions.x, clickbox.dimensions.y, radius);
		nvgFill(nvg);
		float pos;
		if (toggledOn) {
			pos = clickbox.position.x + clickbox.dimensions.x - radius;
		}
		else {
			pos = clickbox.position.x + radius;
		}
		if (hover) {
			nvgBeginPath(nvg);
			nvgStrokeColor(nvg, context->theme.HIGHLIGHT);
			nvgStrokeWidth(nvg, 2.0f);
			nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
				clickbox.dimensions.x, clickbox.dimensions.y, radius);
			nvgStroke(nvg);
		}
		nvgBeginPath(nvg);
		nvgFillColor(nvg, context->theme.LIGHT);
		nvgCircle(nvg, pos, clickbox.position.y + radius, radius - 4);
		nvgFill(nvg);

		Composite::draw(context);
	}
	void ProgressBar::draw(AlloyContext* context) {
		box2px bounds = getBounds();
		NVGcontext* nvg = context->nvgContext;
		float x = bounds.position.x;
		float y = bounds.position.y;
		float w = bounds.dimensions.x;
		float h = bounds.dimensions.y;
		const float FADE = 8;
		NVGpaint shadowPaint = nvgBoxGradient(nvg, x, y, //+1
			w, h, (h) / 2, FADE, context->theme.LIGHT, context->theme.SHADOW);
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, x, y, w, h, h / 2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);

		NVGpaint gradPaint = nvgLinearGradient(nvg, x, y, x, y + h,
			context->theme.NEUTRAL, context->theme.DARK);
		pushScissor(nvg, x, y, w * value, h);
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, x, y, w, h, h / 2);
		nvgFillPaint(nvg, gradPaint);
		nvgFill(nvg);
		shadowPaint = nvgBoxGradient(nvg, x,
			y, //+1
			w, h, (h) / 2, FADE, context->theme.LIGHT.toSemiTransparent(0.0f),
			context->theme.SHADOW.toSemiTransparent(1.0f));
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
		nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		nvgFontSize(nvg, std::max(8.0f, h - FADE));
		drawText(nvg, pixel2(x + 0.5f * w, y + 0.5f * h), label, FontStyle::Normal,
			context->theme.LIGHT_TEXT, context->theme.DARK_TEXT);
		popScissor(nvg);

		pushScissor(nvg, x + w * value, y, w * (1.0f - value), h);
		drawText(nvg, pixel2(x + 0.5f * w, y + 0.5f * h), label, FontStyle::Normal,
			context->theme.DARK_TEXT, context->theme.LIGHT_TEXT);
		popScissor(nvg);
	}
	ProgressBar::ProgressBar(const std::string& name, const AUnit2D& pt,
		const AUnit2D& dims) :
		Composite(name, pt, dims), value(0), label(name) {

	}
	TextButton::TextButton(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions) :
		Region(label) {
		this->position = position;
		this->dimensions = dimensions;
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
		textColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
		borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
		fontSize = UnitPerPX(1.0f, -10);
		this->aspectRule = AspectRule::FixedHeight;
	}
	void TextButton::draw(AlloyContext* context) {
		bool hover = context->isMouseOver(this);
		bool down = context->isMouseDown(this);
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();

		int xoff = 0;
		int yoff = 0;
		if (down) {
			xoff = 2;
			yoff = 2;
		}
		if (hover) {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);

		}
		else {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}

		if (hover) {

			nvgBeginPath(nvg);
			NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x + xoff,
				bounds.position.y + yoff, bounds.dimensions.x,
				bounds.dimensions.y, context->theme.CORNER_RADIUS, 4,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f),
				context->theme.DARK);
			nvgFillPaint(nvg, hightlightPaint);
			nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
			nvgFill(nvg);
		}

		float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		nvgFontSize(nvg, th);
		nvgFillColor(nvg, *textColor);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		float tw = nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr);
		this->aspectRatio = (tw + 10.0f) / (th + 10.0f);
		nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
		pixel2 offset(0, 0);
		nvgText(nvg, bounds.position.x + bounds.dimensions.x / 2 + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff, name.c_str(),
			nullptr);

	}
	TextIconButton::TextIconButton(const std::string& label, int iconCode,
		const AUnit2D& position, const AUnit2D& dimensions,
		const HorizontalAlignment& alignment,
		const IconAlignment& iconAlignment) :
		Composite(label), iconCodeString(CodePointToUTF8(iconCode)), iconAlignment(
			iconAlignment), alignment(alignment) {
		this->position = position;
		this->dimensions = dimensions;
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT);
		textColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
		borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
		fontSize = UnitPerPX(1.0f, -10);
	}
	void TextIconButton::draw(AlloyContext* context) {
		bool hover = context->isMouseOver(this);
		bool down = context->isMouseDown(this);
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		int xoff = 0;
		int yoff = 0;
		if (down) {
			xoff = 2;
			yoff = 2;
		}
		float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		if (hover) {

			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}
		else {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}
		nvgFillColor(nvg, *textColor);
		nvgFontSize(nvg, th);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		float tw = nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr);

		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		float iw = nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr,
			nullptr);

		float ww = tw + iw + AlloyApplicationContext()->theme.SPACING.x;
		pixel2 offset(0, 0);
		pixel xoffset = (pixel)xoff;
		if (alignment == HorizontalAlignment::Center) {
			xoffset += (bounds.dimensions.x - ww) / 2;
		}
		else if (alignment == HorizontalAlignment::Right) {
			xoffset += bounds.dimensions.x - ww
				- AlloyApplicationContext()->theme.SPACING.x;
		}
		else {
			xoffset += AlloyApplicationContext()->theme.SPACING.x;
		}
		nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
		if (iconAlignment == IconAlignment::Right) {
			nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
			nvgText(nvg, bounds.position.x + xoffset,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				name.c_str(), nullptr);

			nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
			nvgText(nvg,
				bounds.position.x + xoffset
				+ AlloyApplicationContext()->theme.SPACING.x + tw,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				iconCodeString.c_str(), nullptr);
		}
		else if (iconAlignment == IconAlignment::Left) {
			nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
			nvgText(nvg,
				bounds.position.x + xoffset
				+ AlloyApplicationContext()->theme.SPACING.x + iw,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				name.c_str(), nullptr);

			nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
			nvgText(nvg, bounds.position.x + xoffset,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				iconCodeString.c_str(), nullptr);
		}
	}
	IconButton::IconButton(int iconCode, const AUnit2D& position,
		const AUnit2D& dimensions, IconType iconType) :
		Composite("Icon", position, dimensions), iconCodeString(
			CodePointToUTF8(iconCode)), iconType(iconType) {
		this->position = position;
		this->dimensions = dimensions;
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		foregroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
		iconColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);

		this->aspectRatio = 1.0f;
		this->aspectRule = AspectRule::FixedHeight;
	}
	void IconButton::draw(AlloyContext* context) {
		bool hover = context->isMouseOver(this);
		bool down = context->isMouseDown(this);
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		pixel2 center = bounds.position + HALF_PIX(bounds.dimensions);
		pixel2 radii = HALF_PIX(bounds.dimensions);
		pixel2 offset;
		if (down) {
			offset = pixel2(1, 1);
		}
		else {
			offset = pixel2(0, 0);
		}
		float hoverOffset = (hover) ? 1.0f : 0.0f;
		if (iconType == IconType::CIRCLE) {
			nvgBeginPath(nvg);
			nvgEllipse(nvg, center.x + offset.x, center.y + offset.y,
				radii.x - 1 + hoverOffset, radii.y - 1 + hoverOffset);
			nvgFillColor(nvg, *foregroundColor);
			nvgFill(nvg);
		}
		else {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x + 1 + offset.x - hoverOffset,
				bounds.position.y + 1 + offset.y - hoverOffset,
				bounds.dimensions.x - 2 + hoverOffset * 2,
				bounds.dimensions.y - 2 + hoverOffset * 2,
				context->theme.CORNER_RADIUS);
			nvgFillColor(nvg, *foregroundColor);
			nvgFill(nvg);

		}
		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		box2px ibounds = bounds;
		ibounds.position += offset;
		float th = ibounds.dimensions.y - 2 * lineWidth - ((hover) ? 2 : 4);
		nvgFontSize(nvg, th);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
		drawText(nvg, ibounds.position + HALF_PIX(ibounds.dimensions),
			iconCodeString, FontStyle::Normal,
			(hover && borderColor->a > 0) ?
			context->theme.HIGHLIGHT : *iconColor, *backgroundColor,
			nullptr);

		if (borderColor->a > 0) {
			if (iconType == IconType::CIRCLE) {

				nvgBeginPath(nvg);
				nvgEllipse(nvg, center.x + offset.x, center.y + offset.y,
					radii.x - HALF_PIX(lineWidth) + hoverOffset,
					radii.y - HALF_PIX(lineWidth) + hoverOffset);
				nvgStrokeColor(nvg,
					(hover) ? context->theme.HIGHLIGHT : *borderColor);
				nvgStrokeWidth(nvg, lineWidth);
				nvgStroke(nvg);
			}
			else {
				nvgBeginPath(nvg);
				nvgRoundedRect(nvg,
					bounds.position.x + offset.x + lineWidth - hoverOffset,
					bounds.position.y + offset.y + lineWidth - hoverOffset,
					bounds.dimensions.x - 2 * lineWidth + hoverOffset * 2.0f,
					bounds.dimensions.y - 2 * lineWidth + hoverOffset * 2.0f,
					context->theme.CORNER_RADIUS);
				nvgStrokeColor(nvg,
					(hover) ? context->theme.HIGHLIGHT : *borderColor);
				nvgStrokeWidth(nvg, lineWidth);
				nvgStroke(nvg);
			}
		}
	}
	SliderTrack::SliderTrack(const std::string& name, Orientation orient,
		const Color& st, const Color& ed) :
		Composite(name), orientation(orient), startColor(st), endColor(ed) {
	}

	void SliderTrack::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		float ax, ay, bx, by;
		if (orientation == Orientation::Horizontal) {
			nvgBeginPath(nvg);
			nvgMoveTo(nvg, ax =
				(bounds.position.x + bounds.dimensions.y * 0.5f - 1),
				ay = (bounds.position.y + bounds.dimensions.y * 0.5f));
			nvgLineTo(nvg,
				bx = (bounds.position.x - bounds.dimensions.y * 0.5f + 1
					+ bounds.dimensions.x),
				by = (bounds.position.y + bounds.dimensions.y * 0.5f));
			NVGpaint paint = nvgLinearGradient(nvg, ax, ay, bx, by, endColor,
				startColor);
			nvgStrokePaint(nvg, paint);
			nvgStrokeWidth(nvg, 10.0f);
			nvgLineCap(nvg, NVG_ROUND);
			nvgStroke(nvg);
		}
		else if (orientation == Orientation::Vertical) {
			nvgBeginPath(nvg);
			nvgMoveTo(nvg, ax = (bounds.position.x + bounds.dimensions.x * 0.5f),
				ay = (bounds.position.y + bounds.dimensions.x * 0.5f - 1));
			nvgLineTo(nvg, bx = (bounds.position.x + bounds.dimensions.x * 0.5f),
				by = (bounds.position.y - bounds.dimensions.x * 0.5f + 1
					+ bounds.dimensions.y));
			NVGpaint paint = nvgLinearGradient(nvg, ax, ay, bx, by, endColor,
				startColor);
			nvgStrokePaint(nvg, paint);
			nvgStrokeWidth(nvg, 10.0f);
			nvgLineCap(nvg, NVG_ROUND);
			nvgStroke(nvg);

		}
		for (std::shared_ptr<Region> ptr : children) {
			ptr->draw(context);
		}
	}
	void SliderHandle::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		if (context->isMouseOver(this) || context->isMouseDown(this)) {
			nvgBeginPath(nvg);
			nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.y * 0.5f,
				bounds.dimensions.y * 0.5f);
			nvgFillColor(nvg, context->theme.LIGHT.toSemiTransparent(0.5f));
			nvgFill(nvg);
		}
		nvgBeginPath(nvg);
		nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.y * 0.25f);
		nvgFillColor(nvg, context->theme.HIGHLIGHT);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgStrokeWidth(nvg, 2.0f);
		nvgStrokeColor(nvg, context->theme.NEUTRAL.toSemiTransparent(0.5f));
		nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.y * 0.25f);
		nvgStroke(nvg);

	}
	bool Selection::handleMouseClick(AlloyContext* context,
		const InputEvent& event) {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
			box2px bounds = getBounds(false);
			selectionBox->pack(bounds.position, bounds.dimensions, context->dpmm,
				context->pixelRatio);
			selectionBox->setSelectionOffset(0);
			selectionBox->setSelectedIndex(0);
			show(context);
			return true;
		}
		else if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
			hide(context);
		}
		return false;
	}
	void Selection::hide(AlloyContext* context) {
		context->removeOnTopRegion(selectionBox.get());
		selectionBox->setVisible(false);
	}
	void Selection::show(AlloyContext* context) {
		context->setOnTopRegion(selectionBox.get());
		selectionBox->setVisible(true);
	}
	Selection::Selection(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, const std::vector<std::string>& options) :
		Composite(label), selectedIndex(-1) {
		this->position = position;
		this->dimensions = dimensions;
		borderColor = MakeColor(COLOR_NONE);
		CompositePtr valueContainer = MakeComposite(label,
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
		selectionLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.DARK_TEXT.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
		arrowLabel = MakeTextLabel(CodePointToUTF8(0xf13a),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), FontType::Icon,
			UnitPercent(1.0f),
			AlloyApplicationContext()->theme.DARK_TEXT.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);
		selectionBox = SelectionBoxPtr(new SelectionBox(label + "_box", options));
		selectionBox->setDetached(true);
		selectionBox->setVisible(false);
		selectionBox->position = CoordPercent(0.0f, 0.0f);
		selectionBox->dimensions = CoordPercent(1.0f, 0.8f);
		selectionBox->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
		selectionBox->borderColor = MakeColor(
			AlloyApplicationContext()->theme.HIGHLIGHT);
		selectionBox->borderWidth = UnitPX(1.0f);
		selectionBox->textColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT_TEXT);
		selectionBox->textAltColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT_TEXT);
		arrowLabel->setAspectRatio(1.0f);
		arrowLabel->setOrigin(Origin::TopRight);
		arrowLabel->setAspectRule(AspectRule::FixedHeight);
		valueContainer->add(selectionLabel);
		valueContainer->add(arrowLabel);
		add(valueContainer);
		add(selectionBox);

		Region::onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseClick(context, event);
		};
		selectionLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseClick(context, event);
		};
		arrowLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseClick(context, event);
		};
		selectionBox->onMouseUp =
			[this](AlloyContext* context, const InputEvent& event) {
			if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
				hide(context);
				int newSelection = selectionBox->getSelectedIndex();
				if (newSelection < 0) {
					selectionBox->setSelectedIndex(selectedIndex);
				}
				else {
					selectedIndex = selectionBox->getSelectedIndex();
					selectionBox->setSelectedIndex(selectedIndex);

				}
				if (selectionBox->onSelect) {
					selectionBox->onSelect(selectionBox.get());
				}
				selectionLabel->label = this->getValue();
				return true;
			}
			else if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
				hide(context);
			}
			return false;
		};
		selectionBox->onSelect = [this](SelectionBox* box) {
			AlloyApplicationContext()->removeOnTopRegion(selectionBox.get());
			selectionBox->setVisible(false);
			int newSelection = selectionBox->getSelectedIndex();
			if (newSelection < 0) {
				selectionBox->setSelectedIndex(selectedIndex);
			}
			else {
				selectedIndex = selectionBox->getSelectedIndex();
				selectionBox->setSelectedIndex(selectedIndex);
			}
			selectionLabel->label = this->getValue();
			if (this->onSelect) {
				this->onSelect(selectedIndex);
			}
			return true;
		};

	}

	void Selection::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		bool hover = context->isMouseContainedIn(this);
		box2px bounds = getBounds();
		if (!hover && selectionBox->isVisible()
			&& !context->isLeftMouseButtonDown()) {
			hide(context);
		}
		if (hover) {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
			nvgFillColor(nvg, context->theme.LIGHT_TEXT);
			nvgFill(nvg);
		}
		else {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
			nvgFillColor(nvg, context->theme.LIGHT_TEXT);
			nvgFill(nvg);
		}
		Composite::draw(context);
	}
	HorizontalSlider::HorizontalSlider(const std::string& label,
		const AUnit2D& position, const AUnit2D& dimensions, const Number& min,
		const Number& max, const Number& value) :
		Slider(label, min, max, value) {
		this->position = position;
		this->dimensions = dimensions;
		float handleSize = 30.0f;
		float trackPadding = 10.0f;
		this->aspectRatio = 4.0f;
		sliderPosition = value.toDouble();
		textColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
		borderColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
		sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

		sliderHandle->position = CoordPercent(0.0, 0.0);
		sliderHandle->dimensions = CoordPX(handleSize, handleSize);
		sliderHandle->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT);
		sliderHandle->setDragEnabled(true);

		sliderTrack = std::shared_ptr<SliderTrack>(
			new SliderTrack("Scroll Track", Orientation::Horizontal,
				AlloyApplicationContext()->theme.HIGHLIGHT,
				AlloyApplicationContext()->theme.HIGHLIGHT));

		sliderTrack->position = CoordPerPX(0.0f, 1.0f, 0.0f, -handleSize);
		sliderTrack->dimensions = CoordPerPX(1.0f, 0.0f, 0.0f, handleSize);

		sliderTrack->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
		sliderTrack->add(sliderHandle);
		sliderTrack->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {return this->onMouseDown(context, sliderTrack.get(), e);};
		sliderHandle->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {return this->onMouseDown(context, sliderHandle.get(), e);};
		sliderHandle->onMouseDrag =
			[this](AlloyContext* context, const InputEvent& e) {
			return this->onMouseDrag(context, sliderHandle.get(), e);};

		add(
			sliderLabel = MakeTextLabel(label,
				CoordPerPX(0.0f, 0.0f, trackPadding, 2.0f),
				CoordPerPX(0.5f, 1.0f, 0.0f,
					-(handleSize - trackPadding * 0.75f)),
				FontType::Bold, UnitPerPX(1.0f, 0.0f),
				AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
				HorizontalAlignment::Left, VerticalAlignment::Bottom));
		add(
			valueLabel = MakeTextLabel("Value",
				CoordPerPX(0.0f, 0.0f, 0.0f, 2.0f),
				CoordPerPX(1.0f, 1.0f, -trackPadding,
					-(handleSize - trackPadding * 0.75f)),
				FontType::Normal, UnitPerPX(1.0f, -2),
				AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
				HorizontalAlignment::Right, VerticalAlignment::Bottom));
		add(sliderTrack);
		this->onPack = [this]() {
			this->setValue(sliderPosition);
		};
		this->onEvent =
			[this](AlloyContext* context, const InputEvent& event) {
			if (event.type == InputType::Scroll&&isVisible() && context->isMouseContainedIn(this)) {
				double oldV = getBlendValue();
				double newV = clamp(event.scroll.y*0.1f + oldV, 0.0, 1.0);
				if (newV != oldV) {
					this->setBlendValue(newV);
					if (onChangeEvent)onChangeEvent(this->value);
					return true;
				}
			}
			return false;
		};
		Application::addListener(this);
	}
	void HorizontalSlider::setValue(double value) {
		double interp = clamp(
			(value - minValue.toDouble())
			/ (maxValue.toDouble() - minValue.toDouble()), 0.0, 1.0);
		float xoff = (float)(sliderTrack->getBoundsPositionX()
			+ interp
			* (sliderTrack->getBoundsDimensionsX()
				- sliderHandle->getBoundsDimensionsX()));
		sliderHandle->setDragOffset(
			pixel2(xoff, sliderHandle->getBoundsDimensionsY()),
			pixel2(0.0f, 0.0f));
		sliderPosition = value;
		this->value.setValue(clamp(value, minValue.toDouble(), maxValue.toDouble()));
	}
	void HorizontalSlider::update() {
		double interp = (sliderHandle->getBoundsPositionX()
			- sliderTrack->getBoundsPositionX())
			/ (double)(sliderTrack->getBoundsDimensionsX()
				- sliderHandle->getBoundsDimensionsX());
		double val = (double)((1.0 - interp) * minValue.toDouble()
			+ interp * maxValue.toDouble());
		sliderPosition = val;
		value.setValue(clamp(val, minValue.toDouble(), maxValue.toDouble()));
	}
	bool HorizontalSlider::onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event) {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
			if (region == sliderTrack.get()) {
				sliderHandle->setDragOffset(event.cursor,
					sliderHandle->getBoundsDimensions() * 0.5f);
				context->setDragObject(sliderHandle.get());
				update();
				if (onChangeEvent)
					onChangeEvent(value);
				return true;
			}
			else if (region == sliderHandle.get()) {
				update();
				if (onChangeEvent)
					onChangeEvent(value);
				return true;
			}
		}
		return false;
	}
	void Slider::setBlendValue(double value) {
		value = clamp(value, 0.0, 1.0);
		setValue(
			value * (maxValue.toDouble() - minValue.toDouble())
			+ minValue.toDouble());
	}

	double Slider::getBlendValue() const {
		return (sliderPosition - minValue.toDouble())
			/ (maxValue.toDouble() - minValue.toDouble());
	}
	bool HorizontalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event) {
		if (region == sliderHandle.get()) {
			region->setDragOffset(event.cursor,
				context->getRelativeCursorDownPosition());
			update();
			if (onChangeEvent)
				onChangeEvent(value);
			return true;
		}
		return false;
	}
	void HorizontalSlider::draw(AlloyContext* context) {

		valueLabel->label = labelFormatter(value);
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, context->theme.DARK);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0.0f),
			context->theme.HIGHLIGHT);
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);
		for (std::shared_ptr<Region>& region : children) {
			if (region->isVisible()) {
				region->draw(context);
			}
		}
	}
	////
	VerticalSlider::VerticalSlider(const std::string& label,
		const AUnit2D& position, const AUnit2D& dimensions, const Number& min,
		const Number& max, const Number& value) :
		Slider(label, min, max, value) {
		this->position = position;
		this->dimensions = dimensions;
		float handleSize = 30.0f;
		this->aspectRatio = 4.0f;
		sliderPosition = value.toDouble();
		textColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
		borderColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
		sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

		sliderHandle->position = CoordPercent(0.0, 0.0);
		sliderHandle->dimensions = CoordPX(handleSize, handleSize);
		sliderHandle->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT);
		sliderHandle->setDragEnabled(true);

		sliderTrack = std::shared_ptr<SliderTrack>(
			new SliderTrack("Scroll Track", Orientation::Vertical,
				AlloyApplicationContext()->theme.HIGHLIGHT,
				AlloyApplicationContext()->theme.HIGHLIGHT));

		sliderTrack->position = CoordPerPX(0.5f, 0.1f, -handleSize * 0.5f, 2.0f);
		sliderTrack->dimensions = CoordPerPX(0.0f, 0.8f, handleSize, -4.0f);

		sliderTrack->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
		sliderTrack->add(sliderHandle);
		sliderTrack->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {return this->onMouseDown(context, sliderTrack.get(), e);};
		sliderHandle->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
			return this->onMouseDown(context, sliderHandle.get(), e);};
		sliderHandle->onMouseDrag =
			[this](AlloyContext* context, const InputEvent& e) {
			return this->onMouseDrag(context, sliderHandle.get(), e);};

		add(
			sliderLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
				CoordPercent(1.0f, 0.1f), FontType::Bold,
				UnitPerPX(1.0f, 0),
				AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
				HorizontalAlignment::Center, VerticalAlignment::Top));
		add(
			valueLabel = MakeTextLabel("Value", CoordPercent(0.0f, 1.0f),
				CoordPercent(1.0f, 0.1f), FontType::Normal,
				UnitPerPX(1.0f, -2),
				AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
				HorizontalAlignment::Center, VerticalAlignment::Bottom));
		valueLabel->setOrigin(Origin::BottomLeft);
		add(sliderTrack);
		this->onPack = [this]() {
			this->setValue(sliderPosition);
		};
		this->onEvent =
			[this](AlloyContext* context, const InputEvent& event) {
			if (event.type == InputType::Scroll&&isVisible() && context->isMouseContainedIn(this)) {
				double oldV = getBlendValue();
				double newV = clamp(event.scroll.y*0.1f + oldV, 0.0, 1.0);
				if (newV != oldV) {
					this->setBlendValue(newV);
					if (onChangeEvent)onChangeEvent(this->value);
					return true;
				}
			}
			return false;
		};
		Application::addListener(this);
	}

	void VerticalSlider::setValue(double value) {
		double interp = 1.0f
			- clamp(
				(value - minValue.toDouble())
				/ (maxValue.toDouble() - minValue.toDouble()), 0.0,
				1.0);
		float yoff = (float)(sliderTrack->getBoundsPositionY()
			+ interp
			* (sliderTrack->getBoundsDimensionsY()
				- sliderHandle->getBoundsDimensionsY()));
		sliderHandle->setDragOffset(
			pixel2(sliderHandle->getBoundsDimensionsX(), yoff),
			pixel2(0.0f, 0.0f));
		sliderPosition = value;
		this->value.setValue(clamp(value, minValue.toDouble(), maxValue.toDouble()));
	}
	void VerticalSlider::update() {
		double interp = (sliderHandle->getBoundsPositionY()
			- sliderTrack->getBoundsPositionY())
			/ (double)(sliderTrack->getBoundsDimensionsY()
				- sliderHandle->getBoundsDimensionsY());
		double val = (double)(interp * minValue.toDouble()
			+ (1.0 - interp) * maxValue.toDouble());
		sliderPosition = val;
		value.setValue(clamp(val, minValue.toDouble(), maxValue.toDouble()));
	}
	bool VerticalSlider::onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event) {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
			if (region == sliderTrack.get()) {
				sliderHandle->setDragOffset(event.cursor,
					sliderHandle->getBoundsDimensions() * 0.5f);
				context->setDragObject(sliderHandle.get());
				update();
				if (onChangeEvent)
					onChangeEvent(value);
				return true;
			}
			else if (region == sliderHandle.get()) {
				update();
				if (onChangeEvent)
					onChangeEvent(value);
				return true;
			}
		}
		return false;
	}
	bool VerticalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event) {
		if (region == sliderHandle.get()) {
			region->setDragOffset(event.cursor,
				context->getRelativeCursorDownPosition());
			update();
			if (onChangeEvent)
				onChangeEvent(value);
			return true;
		}
		return false;
	}
	void VerticalSlider::draw(AlloyContext* context) {
		valueLabel->label = labelFormatter(value);
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, context->theme.DARK);
		nvgFill(nvg);

		nvgBeginPath(nvg);

		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0.0f),
			context->theme.HIGHLIGHT);
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);

		for (std::shared_ptr<Region>& region : children) {
			if (region->isVisible()) {
				region->draw(context);
			}
		}
	}
	ColorSelector::ColorSelector(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Composite(name, pos, dims) {
		CompositePtr valueContainer = MakeComposite("Check Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
		textLabel = MakeTextLabel(name, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT,
			HorizontalAlignment::Left, VerticalAlignment::Middle);

		colorLabel = std::shared_ptr<GlyphRegion>(new GlyphRegion("Color"));
		static std::shared_ptr<CheckerboardGlyph> checkerboard;
		if (checkerboard.get() == nullptr) {
			checkerboard = std::shared_ptr<CheckerboardGlyph>(
				new CheckerboardGlyph(64, 64, 8, 8,
					AlloyApplicationContext().get()));
		}
		colorLabel->glyph = checkerboard;
		colorLabel->position = CoordPerPX(1.0f, 0.0f, -4.0f, 4.0f);
		colorLabel->dimensions = CoordPerPX(0.0f, 1.0f, 0.0f, -8.0f);
		colorLabel->backgroundColor = MakeColor(COLOR_BLACK);
		colorLabel->foregroundColor = MakeColor(255, 128, 32, 255);
		colorLabel->borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
		colorLabel->borderWidth = UnitPX(1.0f);
		colorLabel->setAspectRatio(1.0f);
		colorLabel->setAspectRule(AspectRule::FixedHeight);
		colorLabel->setOrigin(Origin::TopRight);
		colorWheel = ColorWheelPtr(
			new ColorWheel("Color Wheel", CoordPX(0.0f, 0.0f),
				CoordPerPX(1.0f, 0.0f, 0.0f, 300.0f)));
		colorWheel->setAspectRatio(1.0f);
		colorWheel->setAspectRule(AspectRule::FixedHeight);
		colorLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (!colorSelectionPanel->isVisible()) {
					colorWheel->reset();
				}
				colorSelectionPanel->setVisible(true);
				context->getGlassPanel()->setVisible(true);
				return true;
			}
			return false;
		};
		textLabel->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (!colorSelectionPanel->isVisible()) {
					colorWheel->reset();
				}
				colorSelectionPanel->setVisible(true);
				context->getGlassPanel()->setVisible(true);
				return true;
			}
			return false;
		};

		colorSelectionPanel = MakeComposite("Color Selection Panel",
			CoordPerPX(0.5f, 0.5, 0.0f, 0.0f),
			CoordPX(18 + 300.0f + (60 + AlloyApplicationContext()->theme.SPACING.x)
				* 5, 18 + 300.0f), COLOR_NONE);
		colorSelectionPanel->setVisible(false);
		colorSelectionPanel->setOrigin(Origin::MiddleCenter);


		redSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("R", CoordPX(0.0f, 0.0f),
				CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
				Float(0.5)));
		redSlider->setLabelFormatter(
			[](const Number& value) {
			std::string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
			return str;
		});
		redSlider->setOnChangeEvent([this](const Number& value) {
			Color c = colorWheel->getSelectedColor();
			c.r = value.toFloat();
			colorWheel->setColor(c);
			HSV hsv = c.toHSV();
			lumSlider->setValue(hsv.z);
			updateColorSliders(c);
		});

		greenSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("G", CoordPX(0.0f, 0.0f),
				CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
				Float(0.5)));
		greenSlider->setOnChangeEvent([this](const Number& value) {
			Color c = colorWheel->getSelectedColor();
			c.g = value.toFloat();
			colorWheel->setColor(c);
			HSV hsv = c.toHSV();
			lumSlider->setValue(hsv.z);
			updateColorSliders(c);
		});
		greenSlider->setLabelFormatter([](const Number& value) {
			string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
			return str;
		});
		blueSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("B", CoordPX(0.0f, 0.0f),
				CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
				Float(0.5)));
		blueSlider->setOnChangeEvent([this](const Number& value) {
			Color c = colorWheel->getSelectedColor();
			c.b = value.toFloat();
			colorWheel->setColor(c);
			HSV hsv = c.toHSV();
			lumSlider->setValue(hsv.z);
			updateColorSliders(c);
		});
		blueSlider->setLabelFormatter([](const Number& value) {
			string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
			return str;
		});
		lumSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("L", CoordPX(0.0f, 0.0f),
				CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
				Float(0.5)));
		lumSlider->setLabelFormatter([](const Number& value) {
			string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
			return str;
		});
		lumSlider->setOnChangeEvent([this](const Number& value) {
			Color c = colorWheel->getSelectedColor();
			HSVA hsv = c.toHSVA();
			hsv.z = value.toFloat();
			c = HSVAtoRGBAf(hsv);
			colorWheel->setColor(c);
			redSlider->setValue(c.r);
			greenSlider->setValue(c.g);
			blueSlider->setValue(c.b);
			updateColorSliders(c);
		});

		alphaSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("A", CoordPX(0.0f, 0.0f),
				CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
				Float(0.5)));
		alphaSlider->setLabelFormatter(
			[](const Number& value) {
			string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
			return str;
		});
		alphaSlider->setOnChangeEvent([this](const Number& value) {
			Color c = colorWheel->getSelectedColor();
			c.a = value.toFloat();
			colorWheel->setColor(c);
			updateColorSliders(c);
		});
		colorWheel->setOnChangeEvent([this](const Color& c) {

			redSlider->setValue(c.r);
			greenSlider->setValue(c.g);
			blueSlider->setValue(c.b);
			HSV hsv = c.toHSV();
			lumSlider->setValue(hsv.z);
			updateColorSliders(c);
		});
		IconButtonPtr cancelButton = std::shared_ptr<IconButton>(
			new IconButton(0xf00d, CoordPerPX(1.0, 0.0, -30, 30),
				CoordPX(30, 30), IconType::CIRCLE));
		cancelButton->setOrigin(Origin::BottomLeft);

		cancelButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			colorSelectionPanel->setVisible(false);
			context->getGlassPanel()->setVisible(false);
			return true;
		};
		CompositePtr hContainer = MakeComposite("Horizontal Layout", CoordPX(0.0f, 18.0f), CoordPerPX(1.0f, 1.0f, -30.0f, -22.0f), AlloyApplicationContext()->theme.LIGHT);

		hContainer->setRoundCorners(true);
		hContainer->setOrientation(Orientation::Horizontal);
		hContainer->add(colorWheel);
		hContainer->add(redSlider);
		hContainer->add(greenSlider);
		hContainer->add(blueSlider);
		hContainer->add(lumSlider);
		hContainer->add(alphaSlider);
		colorSelectionPanel->add(hContainer);
		colorSelectionPanel->onEvent =
			[=](AlloyContext* context, const InputEvent& e) {
			if (colorSelectionPanel->isVisible()) {
				if (e.type == InputType::MouseButton&&e.isDown() && !context->isMouseContainedIn(hContainer->getBounds())) {
					colorSelectionPanel->setVisible(false);
					context->getGlassPanel()->setVisible(false);
					return true;
				}
			}
			return false;
		};
		colorSelectionPanel->add(cancelButton);
		Application::addListener(colorSelectionPanel.get());
		add(textLabel);
		add(colorLabel);
		AlloyApplicationContext()->getGlassPanel()->add(colorSelectionPanel);
		setColor(*colorLabel->foregroundColor);
	}
	void ColorSelector::updateColorSliders(const Color& c) {
		redSlider->setSliderColor(Color(0.0f, c.g, c.b), Color(1.0f, c.g, c.b));
		greenSlider->setSliderColor(Color(c.r, 0.0f, c.b), Color(c.r, 1.0f, c.b));
		blueSlider->setSliderColor(Color(c.r, c.g, 0.0f), Color(c.r, c.g, 1.0f));
		HSV hsv = c.toHSV();
		lumSlider->setSliderColor(HSVtoColor(HSV(hsv.x, hsv.y, 0.0f)),
			HSVtoColor(HSV(hsv.x, hsv.y, 1.0f)));
		alphaSlider->setSliderColor(Color(c.r, c.g, c.b, 0.0f),
			Color(c.r, c.g, c.b, 1.0f));

	}
	void ColorSelector::setColor(const Color& c) {
		*colorLabel->foregroundColor = c;
		HSVA hsv = c.toHSVA();
		colorWheel->setColor(c);
		redSlider->setValue(c.r);
		greenSlider->setValue(c.g);
		blueSlider->setValue(c.b);
		lumSlider->setValue(hsv.z);
		alphaSlider->setValue(c.a);
		updateColorSliders(c);
	}
	Color ColorSelector::getColor() {
		return colorWheel->getSelectedColor();
	}
	void ColorWheel::reset() {
		circleSelected = false;
		triangleSelected = false;
		updateWheel();
	}
	ColorWheel::ColorWheel(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Composite(name, pos, dims) {

		setColor(Color(32, 64, 255));
		this->onPack = [this]() {
			this->updateWheel();
		};
		this->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT&&e.isDown()) {
				float r2 = distanceSqr(e.cursor, center);
				if (r2 < rInner*rInner) {
					triangleSelected = true;
					circleSelected = false;
				}
				else if (r2 < rOuter*rOuter) {
					triangleSelected = false;
					circleSelected = true;
				}
				else {
					circleSelected = false;
					triangleSelected = false;
				}
				this->setColor(e.cursor);
				return true;
			}
			return false;
		};
		this->onScroll = [this](AlloyContext* context, const InputEvent& e) {
			hsvColor.x += e.scroll.y*0.01f;
			if (hsvColor.x < 0.0f)hsvColor.x += 1.0f;
			if (hsvColor.x > 1.0f)hsvColor.x -= 1.0f;
			setColor(HSVAtoColor(hsvColor));
			updateWheel();
			if (onChangeEvent)onChangeEvent(selectedColor);
			return true;
		};
		this->onMouseOver = [this](AlloyContext* context, const InputEvent& e) {
			if (context->isLeftMouseButtonDown()) {
				this->setColor(e.cursor);
				return true;
			}
			return false;
		};
		Application::addListener(this);
	}

	void ColorWheel::updateWheel() {
		box2px bounds = getBounds();
		float x = bounds.position.x;
		float y = bounds.position.y;
		float w = bounds.dimensions.x;
		float h = bounds.dimensions.y;

		float cx = x + w * 0.5f;
		float cy = y + h * 0.5f;
		rOuter = (w < h ? w : h) * 0.5f - 5.0f;
		rInner = rOuter - 20.0f;
		center = float2(cx, cy);
		float r = rInner - 6;
		tPoints[0] = float2(r, 0);
		tPoints[1] = float2(cos(120.0f / 180.0f * NVG_PI) * r,
			sin(120.0f / 180.0f * NVG_PI) * r);
		tPoints[2] = float2(cosf(-120.0f / 180.0f * NVG_PI) * r,
			sin(-120.0f / 180.0f * NVG_PI) * r);
		float angle = -hsvColor.x * NVG_PI * 2;
		for (int i = 0; i < 3; i++) {
			tBounds[i] = Rotate(tPoints[i], angle) + center;
		}
	}
	void ColorWheel::setColor(const Color& c) {
		selectedColor = c;
		hsvColor = c.toHSVA();
		updateWheel();
	}
	void ColorWheel::setColor(const pixel2& cursor) {
		if (triangleSelected) {

			float2 mid = 0.5f * (tBounds[0] + tBounds[1]);
			float u = clamp(
				dot(cursor - tBounds[0], tBounds[1] - tBounds[0])
				/ lengthSqr(tBounds[1] - tBounds[0]), 0.0f, 1.0f);
			float v = clamp(
				dot(cursor - tBounds[2], mid - tBounds[2])
				/ lengthSqr(mid - tBounds[2]), 0.0f, 1.0f);
			RGBAf hc = HSVAtoRGBAf(HSVA(hsvColor.x, 1.0f, 1.0f, 1.0f));
			RGBAf c = v * (hc + u * (1.0f - hc));
			HSVA hsv = Color(c).toHSVA();
			hsvColor.y = hsv.y;
			hsvColor.z = hsv.z;
			selectedColor = HSVAtoColor(hsvColor);
			updateWheel();
			if (onChangeEvent)
				onChangeEvent(selectedColor);
		}
		else if (circleSelected) {
			float2 vec = cursor - center;
			hsvColor.x = (atan2(vec.y, vec.x)) / (2.0f * NVG_PI);
			if (hsvColor.x < 0.0f) {
				hsvColor.x += 1.0f;
			}
			selectedColor = HSVAtoColor(hsvColor);
			updateWheel();
			if (onChangeEvent)
				onChangeEvent(selectedColor);
		}
	}
	void ColorWheel::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, context->theme.DARK);
		nvgFill(nvg);

		nvgBeginPath(nvg);

		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0.0f),
			context->theme.HIGHLIGHT);
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);

		int i;
		float ax, ay, bx, by, aeps;

		float hue = hsvColor.x;

		NVGpaint paint;
		nvgSave(nvg);
		aeps = 0.5f / rOuter; // half a pixel arc length in radians (2pi cancels out).
		for (i = 0; i < 6; i++) {
			float a0 = (float)i / 6.0f * NVG_PI * 2.0f - aeps;
			float a1 = (float)(i + 1.0f) / 6.0f * NVG_PI * 2.0f + aeps;
			nvgBeginPath(nvg);
			nvgArc(nvg, center.x, center.y, rInner, a0, a1, NVG_CW);
			nvgArc(nvg, center.x, center.y, rOuter, a1, a0, NVG_CCW);
			nvgClosePath(nvg);
			ax = center.x + cosf(a0) * (rInner + rOuter) * 0.5f;
			ay = center.y + sinf(a0) * (rInner + rOuter) * 0.5f;
			bx = center.x + cosf(a1) * (rInner + rOuter) * 0.5f;
			by = center.y + sinf(a1) * (rInner + rOuter) * 0.5f;
			paint = nvgLinearGradient(nvg, ax, ay, bx, by,
				nvgHSLA(a0 / (NVG_PI * 2), 1.0f, 0.55f, 255),
				nvgHSLA(a1 / (NVG_PI * 2), 1.0f, 0.55f, 255));
			nvgFillPaint(nvg, paint);
			nvgFill(nvg);
		}

		nvgBeginPath(nvg);
		nvgCircle(nvg, center.x, center.y, rInner - 0.5f);
		nvgCircle(nvg, center.x, center.y, rOuter + 0.5f);
		nvgStrokeColor(nvg, context->theme.NEUTRAL.toSemiTransparent(0.5f));
		nvgStrokeWidth(nvg, 1.0f);
		nvgStroke(nvg);

		// Selector
		nvgSave(nvg);
		nvgTranslate(nvg, center.x, center.y);
		nvgRotate(nvg, hue * NVG_PI * 2);
		// Marker on
		nvgStrokeWidth(nvg, 2.0f);
		nvgBeginPath(nvg);
		nvgRect(nvg, rInner - 1, -3, rOuter - rInner + 2, 6);
		nvgStrokeColor(nvg, context->theme.HIGHLIGHT.toSemiTransparent(0.9f));
		nvgStroke(nvg);

		paint = nvgBoxGradient(nvg, rInner - 3, -5, rOuter - rInner + 6, 10, 2, 4,
			nvgRGBA(0, 0, 0, 128), nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(nvg);
		nvgRect(nvg, rInner - 2 - 10, -4 - 10, rOuter - rInner + 4 + 20, 8 + 20);
		nvgRect(nvg, rInner - 2, -4, rOuter - rInner + 4, 8);
		nvgPathWinding(nvg, NVG_HOLE);
		nvgFillPaint(nvg, paint);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgMoveTo(nvg, tPoints[0].x, tPoints[0].y);
		nvgLineTo(nvg, tPoints[1].x, tPoints[1].y);
		nvgLineTo(nvg, tPoints[2].x, tPoints[2].y);
		nvgClosePath(nvg);
		NVGcolor chue = nvgHSLA(hue, 1.0f, 0.5f, 255);
		paint = nvgLinearGradient(nvg, tPoints[0].x, tPoints[0].y, tPoints[1].x,
			tPoints[1].y, chue, nvgRGBA(255, 255, 255, 255));
		nvgFillPaint(nvg, paint);
		nvgFill(nvg);
		paint = nvgLinearGradient(nvg, (tPoints[0].x + tPoints[1].x) * 0.5f,
			(tPoints[0].y + tPoints[1].y) * 0.5f, tPoints[2].x, tPoints[2].y,
			nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 255));
		nvgFillPaint(nvg, paint);
		nvgFill(nvg);
		nvgStrokeColor(nvg, context->theme.NEUTRAL.toSemiTransparent(0.5f));
		nvgStroke(nvg);

		RGBf c3(0.0f, 0.0f, 0.0f);
		RGBf c2(1.0f, 1.0f, 1.0f);
		RGBf hc(chue.r, chue.g, chue.b);
		RGBf c = selectedColor.toRGBf();

		float2 bvec, pt;
		float u, v;

		v = std::max(std::max(c.x, c.y), c.z);
		if (v > 0.0f) {
			u = dot(c / v - hc, 1.0f - hc) / lengthSqr(1.0f - hc);

			float2 mid = 0.5f * (tPoints[0] + tPoints[1]);
			bvec.x = dot(tPoints[0], tPoints[1] - tPoints[0])
				+ lengthSqr(tPoints[1] - tPoints[0]) * u;
			bvec.y = dot(tPoints[2], mid - tPoints[2])
				+ lengthSqr(mid - tPoints[2]) * v;
			float2x2 M;
			float2 row1 = tPoints[1] - tPoints[0];
			float2 row2 = mid - tPoints[2];
			M(0, 0) = row1.x;
			M(0, 1) = row1.y;
			M(1, 0) = row2.x;
			M(1, 1) = row2.y;
			pt = inverse(M) * bvec;

			if (cross(pt - tPoints[2], tPoints[1] - tPoints[2]) < 0) {
				pt = dot(pt - tPoints[2], tPoints[1] - tPoints[2])
					* (tPoints[1] - tPoints[2])
					/ lengthSqr(tPoints[1] - tPoints[2]) + tPoints[2];
			}
			else if (cross(tPoints[0] - tPoints[2], pt - tPoints[2]) < 0) {
				pt = dot(pt - tPoints[2], tPoints[0] - tPoints[2])
					* (tPoints[0] - tPoints[2])
					/ lengthSqr(tPoints[0] - tPoints[2]) + tPoints[2];
			}
		}
		else {
			pt = tPoints[2];
		}
		ax = pt.x;
		ay = pt.y;

		nvgStrokeWidth(nvg, 2.0f);
		nvgBeginPath(nvg);
		nvgCircle(nvg, ax, ay, 5);
		nvgStrokeColor(nvg, context->theme.HIGHLIGHT.toSemiTransparent(0.9f));
		nvgStroke(nvg);
		paint = nvgRadialGradient(nvg, ax, ay, 7, 9, nvgRGBA(0, 0, 0, 64),
			nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(nvg);
		nvgRect(nvg, ax - 20, ay - 20, 40, 40);
		nvgCircle(nvg, ax, ay, 7);
		nvgPathWinding(nvg, NVG_HOLE);
		nvgFillPaint(nvg, paint);
		nvgFill(nvg);

		nvgRestore(nvg);
		nvgRestore(nvg);
	}
	void ColorSelector::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		bool hover = context->isMouseContainedIn(this);
		if (colorWheel->isVisible()) {
			*colorLabel->foregroundColor = colorWheel->getSelectedColor();
		}
		if (hover) {
			textLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
			colorLabel->borderWidth = UnitPX(2.0f);
			colorLabel->borderColor = MakeColor(context->theme.HIGHLIGHT);
		}
		else {
			textLabel->textColor = MakeColor(context->theme.LIGHT_TEXT);
			colorLabel->borderWidth = UnitPX(1.0f);
			colorLabel->borderColor = MakeColor(context->theme.NEUTRAL);
		}
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, context->theme.DARK);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0), context->theme.HIGHLIGHT);
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgFillColor(nvg, context->theme.NEUTRAL);
		box2px clickbox = colorLabel->getBounds();
		nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
			clickbox.dimensions.x, clickbox.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);
		Composite::draw(context);
	}

	void ExpandRegion::setExpanded(bool expanded) {
		if (this->expanded != expanded) {
			if (expanded) {
				for (std::shared_ptr<Region>& child : contentRegion->getChildren()) {
					child->setVisible(false);
				}
				AlloyApplicationContext()->addTween(contentRegion->dimensions,
					CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize, 0.0f),
					CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize,
						(float)expandHeight), 0.3, SineOut())->onComplete =
					[this](Tweenable*) {
					for (std::shared_ptr<Region>& child : contentRegion->getChildren()) {
						child->setVisible(true);
					}
					contentRegion->pack();
				};
			}
			else {
				for (std::shared_ptr<Region>& child : contentRegion->getChildren()) {
					child->setVisible(false);
				}
				AlloyApplicationContext()->addTween(contentRegion->dimensions,
					CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize,
						(float)expandHeight),
					CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize, 0.0f),
					0.3, SineOut())->onComplete =
					[this](Tweenable*) {
					for (std::shared_ptr<Region>& child : contentRegion->getChildren()) {
						child->setVisible(true);
					}

					contentRegion->pack();
				};
			}
		}
		this->expanded = expanded;
		arrowIcon->label =
			(expanded) ? CodePointToUTF8(0xf056) : CodePointToUTF8(0xf055);

	}
	ExpandRegion::ExpandRegion(const std::string& name,
		const std::shared_ptr<Composite>& region, const AUnit2D& pos,
		const AUnit2D& dims, pixel expandHeight, bool expanded) :
		Composite(name + "_eregion", pos, dims), expanded(expanded), expandHeight(
			expandHeight) {
		this->contentRegion = region;
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		setRoundCorners(true);
		CompositePtr valueContainer = MakeComposite(
			MakeString() << name << "_container",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
		selectionLabel = MakeTextLabel(name, CoordPX(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);

		arrowIcon = MakeTextLabel(CodePointToUTF8(0xf056), CoordPercent(1.0f, 0.0f),
			CoordPercent(0.0f, 1.0f), FontType::Icon, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);

		arrowIcon->setAspectRatio(1.0f);
		arrowIcon->setOrigin(Origin::TopRight);
		arrowIcon->setAspectRule(AspectRule::FixedHeight);
		valueContainer->add(selectionLabel);
		valueContainer->add(arrowIcon);
		add(valueContainer);
		selectionLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
				setExpanded(!this->expanded);
				return true;
			}
			return false;
		};
		this->onMouseDown = [this](AlloyContext* context, const InputEvent& event) {
			if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
				setExpanded(!this->expanded);
				return true;
			}
			return false;
		};
		arrowIcon->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
				setExpanded(!this->expanded);
				return true;
			}
			return false;
		};
		arrowIcon->label =
			(expanded) ? CodePointToUTF8(0xf056) : CodePointToUTF8(0xf055);
		if (expanded) {
			contentRegion->dimensions =
				CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize,
					(float)expandHeight);
		}
		else {
			contentRegion->dimensions =
				CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize, 0.0f);
		}
	}
	FileSelector::FileSelector(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		BorderComposite(name, pos, dims) {
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT);

		setRoundCorners(true);
		std::shared_ptr<Composite> &glassPanel =
			AlloyApplicationContext()->getGlassPanel();

		fileDialog = std::shared_ptr<FileDialog>(
			new FileDialog("Open File",
				CoordPerPX(0.5, 0.5, -350 + 7.5f, -250 - 7.5f),
				CoordPX(700, 500), FileDialogType::OpenFile));
		fileDialog->setVisible(false);
		glassPanel->add(fileDialog);
		fileLocation = std::shared_ptr<FileField>(
			new FileField("None", CoordPX(0, 0), CoordPercent(1.0f, 1.0f)));
		fileDialog->onSelect = [this](const std::vector<std::string>& file) {
			fileLocation->setValue(file.front());

			if (onChange)onChange(file.front());
		};
		openIcon = std::shared_ptr<IconButton>(
			new IconButton(0xf115, CoordPerPX(0.0f, 0.0f, 2.0f, 4.0f),
				CoordPerPX(1.0f, 1.0f, 0.0f, -4.0f)));
		openIcon->foregroundColor = MakeColor(COLOR_NONE);
		openIcon->borderColor = MakeColor(COLOR_NONE);
		openIcon->backgroundColor = MakeColor(COLOR_NONE);
		openIcon->iconColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		setCenter(fileLocation);
		setEast(openIcon, UnitPX(32.0f));
		openIcon->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
				fileLocation->hideDropDown(context);
				std::string file = getValue();
				AlloyApplicationContext()->setMouseFocusObject(nullptr);
				if (FileExists(file)) {
					openFileDialog(context, GetParentDirectory(file));
				}
				else {
					openFileDialog(context, GetCurrentWorkingDirectory());
				}
				return true;
			}
			return false;
		};
		fileLocation->setValue(GetCurrentWorkingDirectory());
		fileLocation->onTextEntered = [this](TextField* field) {
			fileDialog->setValue(field->getValue());
		};
	}
	void FileSelector::setValue(const std::string& file) {
		fileLocation->setValue(file);
		fileDialog->setValue(file);
	}
	void FileSelector::openFileDialog(AlloyContext* context,
		const std::string& workingDirectory) {
		if (!fileDialog->isVisible()) {
			fileDialog->setVisible(true);
			context->getGlassPanel()->setVisible(true);
		}
		else {
			fileDialog->setVisible(false);
			context->getGlassPanel()->setVisible(false);
		}
		fileDialog->setValue(workingDirectory);
	}

	FileButton::FileButton(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims, const FileDialogType& type) :
		IconButton((type == FileDialogType::SaveFile) ? 0xF0C7 : 0xf115, pos,
			dims) {
		setRoundCorners(true);
		std::shared_ptr<Composite> &glassPanel =
			AlloyApplicationContext()->getGlassPanel();
		fileDialog = std::shared_ptr<FileDialog>(
			new FileDialog("File Dialog",
				CoordPerPX(0.5, 0.5, -350 + 7.5f, -250 - 7.5f),
				CoordPX(700, 500), type));

		fileDialog->setVisible(false);
		glassPanel->add(fileDialog);
		if (type == FileDialogType::SaveFile) {
			fileDialog->onSelect = [this](const std::vector<std::string>& file) {
				if (onSave)onSave(file.front());
			};
		}
		else {
			fileDialog->onSelect = [this](const std::vector<std::string>& file) {
				if (onOpen)onOpen(file);
			};
		}

		foregroundColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
		iconColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
		//borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
		borderColor = MakeColor(COLOR_NONE);

		borderWidth = UnitPX(2.0f);
		backgroundColor = MakeColor(COLOR_NONE);
		setRoundCorners(true);
		onMouseDown = [this](AlloyContext* context, const InputEvent& event) {
			if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
				std::string file = getValue();
				AlloyApplicationContext()->setMouseFocusObject(nullptr);
				if (FileExists(file)) {
					openFileDialog(context, GetParentDirectory(file));
				}
				else {
					openFileDialog(context, GetCurrentWorkingDirectory());
				}
				return true;
			}
			return false;
		};
	}
	void FileButton::setValue(const std::string& file) {

		fileDialog->setValue(file);
	}
	void FileButton::openFileDialog(AlloyContext* context,
		const std::string& workingDirectory) {
		if (!fileDialog->isVisible()) {
			fileDialog->setVisible(true);
			context->getGlassPanel()->setVisible(true);
		}
		else {
			fileDialog->setVisible(false);
			context->getGlassPanel()->setVisible(false);
		}
		fileDialog->setValue(workingDirectory);
	}
	ListEntry::ListEntry(ListBox* listBox, const std::string& name, float entryHeight) :
		Region(name), dialog(listBox), entryHeight(entryHeight) {
		this->backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
		this->borderColor = MakeColor(COLOR_NONE);
		this->selected = false;
		iconCodeString = "";
		setLabel(name);
		this->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
			return dialog->onMouseDown(this, context, e);
		};
	}
	void ListEntry::setLabel(const std::string& label) {
		this->label = label;
		float th = entryHeight - 2 * TextField::PADDING;
		fontSize = UnitPX(th);
		AlloyContext* context = AlloyApplicationContext().get();
		NVGcontext* nvg = context->nvgContext;
		nvgFontSize(nvg, th);
		nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
		if (iconCodeString.size() > 0) {
			nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
			tw += nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr,
				nullptr) + 3 * context->theme.SPACING.x;
		}
		else {
			tw += 2 * context->theme.SPACING.x;
		}
		position = CoordPX(0.0f, 0.0f);
		dimensions = CoordPX(tw, entryHeight);
	}
	void ListEntry::setIcon(int icon) {
		this->iconCodeString = CodePointToUTF8(icon);
		float th = entryHeight - 2 * TextField::PADDING;
		fontSize = UnitPX(th);
		AlloyContext* context = AlloyApplicationContext().get();
		NVGcontext* nvg = context->nvgContext;
		nvgFontSize(nvg, th);
		nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
		if (iconCodeString.size() > 0) {
			nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
			tw += nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr,
				nullptr) + 3 * context->theme.SPACING.x;
		}
		else {
			tw += 2 * context->theme.SPACING.x;
		}
		position = CoordPX(0.0f, 0.0f);
		dimensions = CoordPX(tw, entryHeight);
	}
	bool ListBox::onMouseDown(ListEntry* entry, AlloyContext* context,
		const InputEvent& e) {
		if (e.isDown()) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (enableMultiSelection) {
					if (entry->isSelected()) {
						entry->setSelected(false);
						for (auto iter = lastSelected.begin();
						iter != lastSelected.end(); iter++) {
							if (*iter == entry) {
								lastSelected.erase(iter);
								break;
							}
						}
					}
					else {
						entry->setSelected(true);
						lastSelected.push_back(entry);
					}

				}
				else {
					if (!entry->isSelected()) {
						for (ListEntry* child : lastSelected) {
							child->setSelected(false);
						}
						entry->setSelected(true);
						lastSelected.clear();
						lastSelected.push_back(entry);
					}
				}
				if (onSelect)
					onSelect(entry);
				return true;
			}
			else if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
				for (ListEntry* child : lastSelected) {
					child->setSelected(false);
				}
				if (onSelect)
					onSelect(nullptr);
				return true;
			}
		}
		return false;
	}
	FileEntry::FileEntry(FileDialog* dialog, const std::string& name, float fontHeight) :
		ListEntry(dialog->directoryList.get(), name, fontHeight), fileDescription() {
	}
	void FileEntry::setValue(const FileDescription& description) {
		this->fileDescription = description;
		iconCodeString =
			(fileDescription.fileType == FileType::Directory) ?
			CodePointToUTF8(0xf07b) : CodePointToUTF8(0xf15b);
		fileSize = FormatSize(fileDescription.fileSize);
		creationTime = FormatDateAndTime(fileDescription.creationTime);
		lastAccessTime = FormatDateAndTime(fileDescription.lastModifiedTime);
		lastModifiedTime = FormatDateAndTime(fileDescription.lastModifiedTime);
		setLabel(GetFileName(fileDescription.fileLocation));
	}
	void ListEntry::setSelected(bool selected) {
		this->selected = selected;
	}
	bool ListEntry::isSelected() {
		return selected;
	}
	void ListEntry::draw(AlloyContext* context) {
		box2px bounds = getBounds();
		NVGcontext* nvg = context->nvgContext;
		bool hover = context->isMouseOver(this);
		bool down = context->isMouseDown(this);
		bool selected = this->selected || dialog->isDraggingOver(this);
		int xoff = 0;
		int yoff = 0;
		if (down) {
			xoff = 2;
			yoff = 2;
		}
		if (hover || down) {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
			if (selected) {
				nvgFillColor(nvg, context->theme.LINK);
			}
			else {
				nvgFillColor(nvg, context->theme.NEUTRAL);
			}
			nvgFill(nvg);
		}
		else {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
			if (selected) {
				nvgFillColor(nvg, context->theme.LINK);
			}
			else {
				nvgFillColor(nvg, context->theme.LIGHT);
			}
			nvgFill(nvg);
		}
		float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		nvgFontSize(nvg, th);
		nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		float iw = (iconCodeString.size() > 0) ? nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr, nullptr) + AlloyApplicationContext()->theme.SPACING.x : 0;
		pixel2 offset(0, 0);

		if (selected) {
			if (hover) {

				nvgFillColor(nvg, context->theme.HIGHLIGHT);
			}
			else {
				nvgFillColor(nvg, context->theme.LIGHT_TEXT);
			}
		}
		else {
			if (hover) {

				nvgFillColor(nvg, context->theme.HIGHLIGHT);
			}
			else {
				nvgFillColor(nvg, context->theme.DARK_TEXT);
			}
		}

		box2px labelBounds = getCursorBounds();
		pushScissor(nvg, labelBounds);
		if (iconCodeString.size() > 0) {
			nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
			nvgText(nvg, AlloyApplicationContext()->theme.SPACING.x + bounds.position.x
				+ xoff, bounds.position.y + bounds.dimensions.y / 2 + yoff,
				iconCodeString.c_str(), nullptr);
		}
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		nvgText(nvg, AlloyApplicationContext()->theme.SPACING.x + bounds.position.x + iw + xoff, bounds.position.y + bounds.dimensions.y / 2 + yoff, label.c_str(), nullptr);
		popScissor(nvg);
	}

	void FileDialog::updateDirectoryList() {
		setSelectedFile(fileLocation->getValue());
	}
	bool FileDialog::updateValidity() {
		FileFilterRule* rule =
			(fileTypeSelect->getSelectedIndex() >= 0) ?
			filterRules[fileTypeSelect->getSelectedIndex()].get() :
			nullptr;
		if (type == FileDialogType::SaveFile) {
			std::string file = fileLocation->getValue();
			std::string fileName = GetFileName(file);
			if (fileName.size() > 0 && !IsDirectory(file)
				&& (rule == nullptr || rule->accept(file))) {
				valid = true;
				actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHT_TEXT);
			}
			else {
				actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHT_TEXT.toDarker(0.5f));
				valid = false;
			}
		}
		else if (type == FileDialogType::OpenFile) {
			std::string file = fileLocation->getValue();
			if (FileExists(file) && IsFile(file)
				&& (rule == nullptr || rule->accept(file))) {
				valid = true;
				actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHT_TEXT);
			}
			else {
				actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHT_TEXT.toDarker(0.5f));
				valid = false;
			}
		}
		else if (type == FileDialogType::OpenMultiFile) {
			valid = true;
			int count = 0;
			for (std::shared_ptr<ListEntry> entry : directoryList->getEntries()) {
				if (entry->isSelected()) {
					count++;
					std::string file =
						dynamic_cast<FileEntry*>(entry.get())->fileDescription.fileLocation;
					if (FileExists(file) && IsFile(file)
						&& (rule == nullptr || rule->accept(file))) {
					}
					else {
						valid = false;
						break;
					}
				}
			}
			valid &= (count > 0);
			if (valid) {
				actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHT_TEXT);
			}
			else {
				actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHT_TEXT.toDarker(0.5f));
			}
		}
		return valid;
	}
	void FileDialog::setSelectedFile(const std::string& file) {
		std::string dir;
		bool select = false;
		if (IsDirectory(file)) {
			dir = file;
		}
		else {
			dir = RemoveTrailingSlash(GetParentDirectory(file));
			select = true;
		}
		std::vector<FileDescription> descriptions = GetDirectoryDescriptionListing(
			dir);
		int i = 0;
		FileFilterRule* rule =
			(fileTypeSelect->getSelectedIndex() >= 0) ?
			filterRules[fileTypeSelect->getSelectedIndex()].get() :
			nullptr;

		if (!AlloyApplicationContext()->hasDeferredTasks()) {
			directoryList->clearEntries();
			//Fixes bug in padding out entry width.
			AlloyApplicationContext()->getGlassPanel()->pack();
			for (FileDescription& fd : descriptions) {
				if (rule != nullptr && fd.fileType == FileType::File
					&& !rule->accept(fd.fileLocation)) {
					continue;
				}
				FileEntry* entry = new FileEntry(this, MakeString() << "Entry " << i, fileEntryHeight);
				directoryList->addEntry(std::shared_ptr<FileEntry>(entry));
				entry->setValue(fd);
				if (select && entry->fileDescription.fileLocation == file) {
					entry->setSelected(true);
				}
				i++;
			}
			updateValidity();
			directoryList->update();
		}
	}
	void ListBox::update() {
		clear();
		lastSelected.clear();
		AlloyApplicationContext()->addDeferredTask([this]() {
			lastSelected.clear();
			AlloyContext* context = AlloyApplicationContext().get();
			pixel2 maxDim = pixel2(this->getBoundsDimensionsX(), 0.0f);
			for (std::shared_ptr<ListEntry> entry : listEntries) {
				maxDim = aly::max(entry->dimensions.toPixels(context->getScreenSize(), context->dpmm, context->pixelRatio), maxDim);
			}
			for (std::shared_ptr<ListEntry> entry : listEntries) {
				if (entry->parent == nullptr) {
					entry->dimensions = CoordPX(maxDim);
					add(entry);
				}
				if (entry->isSelected()) {
					lastSelected.push_back(entry.get());
				}
			}
			AlloyApplicationContext()->requestPack();
		});

	}
	ListBox::ListBox(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Composite(name, pos, dims) {
		enableMultiSelection = false;
		scrollingDown = false;
		scrollingUp = false;
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
		borderColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		borderWidth = UnitPX(1.0f);
		setOrientation(Orientation::Vertical, pixel2(0, 2), pixel2(0, 2));
		setScrollEnabled(true);
		dragBox = box2px(float2(0, 0), float2(0, 0));
		onEvent =
			[this](AlloyContext* context, const InputEvent& e) {
			if (!context->isMouseOver(this, true))return false;
			if (e.type == InputType::Cursor || e.type == InputType::MouseButton) {
				if (context->isMouseDrag()) {
					if (enableMultiSelection) {
						float2 cursorDown = context->getCursorDownPosition();
						float2 stPt = aly::min(cursorDown, e.cursor);
						float2 endPt = aly::max(cursorDown, e.cursor);
						dragBox.position = stPt;
						dragBox.dimensions = endPt - stPt;
						dragBox.intersect(getBounds());
					}
				}
				else if (!context->isMouseDown() && e.type == InputType::MouseButton) {
					if (enableMultiSelection) {
						for (std::shared_ptr<ListEntry> entry : listEntries) {
							if (!entry->isSelected()) {
								if (dragBox.intersects(entry->getBounds())) {
									lastSelected.push_back(entry.get());
									entry->setSelected(true);
								}
							}
						}
					}
					dragBox = box2px(float2(0, 0), float2(0, 0));
				}
				else {
					dragBox = box2px(float2(0, 0), float2(0, 0));
				}
			}
			if (e.type == InputType::Cursor) {
				box2px bounds = this->getBounds();
				box2px lastBounds = bounds, firstBounds = bounds;
				float entryHeight = 30;
				lastBounds.position.y = bounds.position.y + bounds.dimensions.y - entryHeight;
				lastBounds.dimensions.y = entryHeight;
				firstBounds.dimensions.y = entryHeight;
				if ((!isHorizontalScrollVisible() && lastBounds.contains(e.cursor)) || (dragBox.dimensions.x*dragBox.dimensions.y > 0 && e.cursor.y > bounds.dimensions.y + bounds.position.y)) {
					if (downTimer.get() == nullptr) {
						downTimer = std::shared_ptr<Timer>(new Timer([this] {
							double deltaT = 200;
							scrollingDown = true;
							while (scrollingDown) {
								if (!addVerticalScrollPosition(10.0f))break;
								std::this_thread::sleep_for(std::chrono::milliseconds((long)deltaT));
								deltaT = std::max(30.0, 0.75*deltaT);
							}
						}, nullptr, 500, 30));
						downTimer->execute();
					}
				}
				else {
					if (downTimer.get() != nullptr) {
						scrollingDown = false;
						downTimer.reset();
					}
				}
				if (firstBounds.contains(e.cursor) || (dragBox.dimensions.x*dragBox.dimensions.y > 0 && e.cursor.y < bounds.position.y)) {
					if (upTimer.get() == nullptr) {
						upTimer = std::shared_ptr<Timer>(new Timer([this] {
							double deltaT = 200;
							scrollingUp = true;
							while (scrollingUp) {
								if (!addVerticalScrollPosition(-10.0f))break;
								std::this_thread::sleep_for(std::chrono::milliseconds((long)deltaT));
								deltaT = std::max(30.0, 0.75*deltaT);
							}
						}, nullptr, 500, 30));
						upTimer->execute();
					}
				}
				else {
					if (upTimer.get() != nullptr) {
						scrollingUp = false;
						upTimer.reset();
					}
				}
			}

			return false;
		};
	}
	void ListBox::draw(AlloyContext* context) {
		Composite::draw(context);
		NVGcontext* nvg = context->nvgContext;
		if (dragBox.dimensions.x > 0 && dragBox.dimensions.y > 0) {
			nvgBeginPath(nvg);
			nvgRect(nvg, dragBox.position.x, dragBox.position.y,
				dragBox.dimensions.x, dragBox.dimensions.y);
			nvgFillColor(nvg, context->theme.DARK.toSemiTransparent(0.5f));
			nvgFill(nvg);

			nvgBeginPath(nvg);
			nvgRect(nvg, dragBox.position.x, dragBox.position.y,
				dragBox.dimensions.x, dragBox.dimensions.y);
			nvgStrokeWidth(nvg, 2.0f);
			nvgStrokeColor(nvg, context->theme.DARK);
			nvgStroke(nvg);
		}
	}
	bool ListBox::isDraggingOver(ListEntry* entry) {
		if (entry->isSelected() || dragBox.intersects(entry->getBounds())) {
			return true;
		}
		else {
			return false;
		}
	}
	FileDialog::FileDialog(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims, const FileDialogType& type, pixel fileEntryHeight) :
		Composite(name, pos, dims), type(type), fileEntryHeight(fileEntryHeight) {
		containerRegion = std::shared_ptr<BorderComposite>(
			new BorderComposite("Container", CoordPX(0, 15),
				CoordPerPX(1.0, 1.0, -15, -15)));
		actionButton = std::shared_ptr<TextIconButton>(
			new TextIconButton(
				(type == FileDialogType::SaveFile) ? "Save" : "Open",
				0xf115, CoordPerPX(1.0f, 0.0f, -10.0f, 5.0f),
				CoordPX(100, 30)));
		actionButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (valid) {
					if (this->onSelect)
					{
						std::vector<std::string> files;
						if (this->type != FileDialogType::OpenMultiFile) {
							files.push_back(this->getValue());
						}
						else {
							for (std::shared_ptr<ListEntry> entry : directoryList->getEntries()) {
								if (entry->isSelected()) {
									files.push_back(dynamic_cast<FileEntry*>(entry.get())->fileDescription.fileLocation);
								}
							}
						}
						if (files.size() > 0)this->onSelect(files);
					}
					this->setVisible(false);
					context->getGlassPanel()->setVisible(false);
					return true;
				}
				else {
					return false;
				}
			}
			return false;
		};

		fileTypeSelect = std::shared_ptr<Selection>(
			new Selection("File Type", CoordPerPX(0.0f, 0.0f, 10.0f, 5.0f),
				CoordPerPX(1.0f, 0.0f, -125.0f, 30.0f)));

		std::shared_ptr<FileFilterRule> filterRule =
			std::shared_ptr<FileFilterRule>(new FileFilterRule("All Files"));
		filterRules.push_back(filterRule);
		fileTypeSelect->addSelection(filterRule->toString());

		fileTypeSelect->setSelectionIndex(0);
		fileTypeSelect->onSelect = [this](int index) {
			this->updateDirectoryList();
		};
		actionButton->setOrigin(Origin::TopRight);
		fileLocation = std::shared_ptr<FileField>(
			new FileField("File Location", CoordPX(10, 7),
				CoordPerPX(1.0f, 0.0f, -55.0f, 30.0f)));
		fileLocation->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT);
		fileLocation->onTextEntered = [this](TextField* field) {
			this->updateDirectoryList();
		};
		fileLocation->onKeyInput = [this](TextField* field) {
			if (this->type == FileDialogType::SaveFile) {
				this->updateValidity();
			}
		};
		upDirButton = std::shared_ptr<IconButton>(
			new IconButton(0xf062, CoordPerPX(1.0, 0.0, -40, 7),
				CoordPX(30, 30)));
		upDirButton->foregroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT_TEXT);
		upDirButton->borderColor = MakeColor(COLOR_NONE);
		upDirButton->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.SHADOW);
		upDirButton->iconColor = MakeColor(
			AlloyApplicationContext()->theme.DARK_TEXT);
		upDirButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			this->setValue(GetParentDirectory(RemoveTrailingSlash(this->getValue())));
			return true;
		};
		cancelButton = std::shared_ptr<IconButton>(
			new IconButton(0xf00d, CoordPerPX(1.0, 0.0, -30, 30),
				CoordPX(30, 30), IconType::CIRCLE));
		cancelButton->setOrigin(Origin::BottomLeft);

		cancelButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			this->setVisible(false);
			context->getGlassPanel()->setVisible(false);
			return true;
		};
		CompositePtr southRegion = MakeComposite("File Options", CoordPX(0, 0),
			CoordPercent(1.0f, 1.0f));
		CompositePtr northRegion = MakeComposite("Selection Bar", CoordPX(0, 0),
			CoordPercent(1.0f, 1.0f));
		southRegion->add(actionButton);
		southRegion->add(fileTypeSelect);
		northRegion->add(fileLocation);
		northRegion->add(upDirButton);

		directoryTree = std::shared_ptr<Composite>(
			new Composite("Container", CoordPX(10, 0),
				CoordPerPX(1.0, 1.0, -10, 0)));

		TextIconButtonPtr homeDir = TextIconButtonPtr(
			new TextIconButton("Home", 0xf015, CoordPX(0.0f, 0.0f),
				CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
				HorizontalAlignment::Left));
		TextIconButtonPtr docsDir = TextIconButtonPtr(
			new TextIconButton("Documents", 0xf115, CoordPX(0.0f, 0.0f),
				CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
				HorizontalAlignment::Left));
		TextIconButtonPtr downloadDir = TextIconButtonPtr(
			new TextIconButton("Downloads", 0xf019, CoordPX(0.0f, 0.0f),
				CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
				HorizontalAlignment::Left));
		TextIconButtonPtr desktopDir = TextIconButtonPtr(
			new TextIconButton("Desktop", 0xf108, CoordPX(0.0f, 0.0f),
				CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
				HorizontalAlignment::Left));

		homeDir->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				this->setValue(GetHomeDirectory());
				return true;
			}
			return false;
		};
		directoryTree->add(homeDir);
		docsDir->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				this->setValue(GetDocumentsDirectory());
				return true;
			}
			return false;
		};
		directoryTree->add(docsDir);
		downloadDir->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				this->setValue(GetDownloadsDirectory());
				return true;
			}
			return false;
		};
		directoryTree->add(downloadDir);
		desktopDir->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				this->setValue(GetDesktopDirectory());
				return true;
			}
			return false;
		};
		directoryTree->add(desktopDir);

		std::vector<std::string> drives = GetDrives();
		for (std::string file : drives) {
			TextIconButtonPtr diskDir = TextIconButtonPtr(
				new TextIconButton(
					GetFileName(
						RemoveTrailingSlash(file)) + ALY_PATH_SEPARATOR,
					0xf0a0, CoordPX(0.0f, 0.0f),
					CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
					HorizontalAlignment::Left));
			diskDir->onMouseDown =
				[this, file](AlloyContext* context, const InputEvent& e) {
				if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
					this->setValue(file);
					return true;
				}
				return false;
			};
			directoryTree->add(diskDir);
		}
		directoryList = std::shared_ptr<ListBox>(
			new ListBox("Container", CoordPX(0, 0),
				CoordPerPX(1.0f, 1.0, -10.0f, 0.0f)));
		directoryList->setEnableMultiSelection(
			type == FileDialogType::OpenMultiFile);
		directoryList->onSelect =
			[this](ListEntry* lentry) {
			if (lentry != nullptr) {
				FileEntry* entry = dynamic_cast<FileEntry*>(lentry);
				if (this->type == FileDialogType::OpenMultiFile) {
					if (entry->fileDescription.fileType == FileType::Directory) {
						setSelectedFile(entry->fileDescription.fileLocation);
						fileLocation->setValue(entry->fileDescription.fileLocation);
					}
					updateValidity();

				}
				else if (this->type == FileDialogType::OpenFile) {
					setSelectedFile(entry->fileDescription.fileLocation);
					fileLocation->setValue(entry->fileDescription.fileLocation);
					updateValidity();
				}
				else if (this->type == FileDialogType::SaveFile) {
					if (entry->fileDescription.fileType == FileType::Directory) {
						setSelectedFile(entry->fileDescription.fileLocation);
						fileLocation->setValue(entry->fileDescription.fileLocation);
					}
					else {
						fileLocation->setValue(entry->fileDescription.fileLocation);
					}
					updateValidity();
				}
			}
			else {
				if (this->type != FileDialogType::OpenMultiFile) {
					ListEntry* entry = directoryList->getLastSelected();
					if (entry != nullptr) {
						fileLocation->setValue(
							GetParentDirectory(dynamic_cast<FileEntry*>(entry)->fileDescription.fileLocation));
					}
				}
				updateValidity();
			}
		};
		directoryTree->borderColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
		directoryTree->borderWidth = UnitPX(1.0f);
		directoryTree->setOrientation(Orientation::Vertical);
		directoryTree->setScrollEnabled(true);

		containerRegion->setNorth(northRegion, UnitPX(40));
		containerRegion->setSouth(southRegion, UnitPX(40));
		containerRegion->setWest(directoryTree, UnitPX(140.0f));
		containerRegion->setCenter(directoryList);
		add(containerRegion);
		add(cancelButton);
	}
	std::string FileFilterRule::toString() {
		std::stringstream ss;
		if (extensions.size() == 0) {
			ss << name << " (*.*)";
			return ss.str();
		}
		ss << name << " (";
		int index = 0;
		for (std::string ext : extensions) {
			ss << "*." << ext;
			if (index < (int)extensions.size() - 1) {
				ss << ", ";
			}
			index++;
		}
		ss << ")";
		return ss.str();
	}
	bool FileFilterRule::accept(const std::string& file) {
		if (extensions.size() == 0)
			return true;
		std::string ext = GetFileExtension(file);
		for (char& c : ext) {
			c = std::tolower(c);
		}
		for (std::string extension : extensions) {
			if (ext == extension)
				return true;
		}
		return false;
	}
	void FileDialog::addFileExtensionRule(const std::string& name,
		const std::string& extension) {
		using extensions = std::initializer_list<std::string>;
		filterRules.push_back(
			std::shared_ptr<FileFilterRule>(
				new FileFilterRule(name, extensions{ extension })));
		fileTypeSelect->addSelection(filterRules.back()->toString());
	}
	void FileDialog::addFileExtensionRule(const std::string& name,
		const std::initializer_list<std::string> & extension) {
		filterRules.push_back(
			std::shared_ptr<FileFilterRule>(
				new FileFilterRule(name, extension)));
		fileTypeSelect->addSelection(filterRules.back()->toString());
	}

	void FileDialog::setValue(const std::string& file) {
		fileLocation->setValue(file);
		setSelectedFile(file);
	}
	std::string FileDialog::getValue() const {
		return fileLocation->getValue();
	}

	void FileDialog::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = containerRegion->getBounds();

		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
			context->theme.HIGHLIGHT.toSemiTransparent(0.0f));

		nvgBeginPath(nvg);
		nvgFillPaint(nvg, shadowPaint);

		nvgRoundedRect(nvg, bounds.position.x + 2, bounds.position.y + 2,
			bounds.dimensions.x + 2, bounds.dimensions.y + 2,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, context->theme.DARK);
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStrokeColor(nvg, context->theme.LIGHT);
		nvgStroke(nvg);

		Composite::draw(context);

	}
	ExpandBar::ExpandBar(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Composite(name, pos, dims) {
		setOrientation(Orientation::Vertical);
		setScrollEnabled(true);
		setAlwaysShowVerticalScrollBar(true);
		cellPadding.y = 2;

	}

	CompositePtr ExpandBar::add(const std::shared_ptr<Region>& region,
		pixel expandHeight,
		bool expanded) {
		CompositePtr container = MakeComposite(MakeString() << region->name << " content",
			CoordPX(0.0f, 0.0f),
			CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize, expandHeight));
		container->setScrollEnabled(true);
		region->backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		region->borderColor = MakeColor(AlloyApplicationContext()->theme.NEUTRAL);
		region->borderWidth = UnitPX(2.0f);
		container->add(region);
		std::shared_ptr<ExpandRegion> eregion = std::shared_ptr<ExpandRegion>(
			new ExpandRegion(region->name, container, CoordPX(0.0f, 0.0f),
				CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize, 30.0f),
				expandHeight, expanded));
		expandRegions.push_back(eregion);
		contentRegions.push_back(container);
		Composite::add(eregion);
		Composite::add(container);
		return container;
	}
	CompositePtr ExpandBar::add(Region* region, pixel expandHeight, bool expanded) {
		return add(std::shared_ptr<Region>(region), expandHeight, expanded);
	}
	Graph::Graph(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :Region(name, pos, dims)
	{
		graphBounds = box2f(float2(0.0f), float2(-1.0f));
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		setRoundCorners(true);
		xAxisLabel = "x";
		yAxisLabel = "y=f(x)";
		cursorPosition.x = -1;
		cursorPosition.y = -1;
		onEvent = [this](const AlloyContext* context, const InputEvent& e) {
			if (context->isMouseContainedIn(this)) {
				cursorPosition=e.cursor;
				glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			}
			else {
				cursorPosition=float2(-1,-1);
				glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			return false;
		};
		Application::addListener(this);
	}
	void Graph::draw(AlloyContext* context) {
		Region::draw(context);
		box2px rbounds = getBounds();
		NVGcontext* nvg = context->nvgContext;
		box2px gbounds = rbounds;
		const float LARGE_TEXT = 18.0f;
		const float MEDIUM_TEXT = 16.0f;
		const float SMALL_TEXT = 12.0f;
		float2 gpos(-1, -1);
		gbounds.position = pixel2(rbounds.position.x + GRAPH_PADDING, rbounds.position.y + GRAPH_PADDING);
		gbounds.dimensions = pixel2(rbounds.dimensions.x - GRAPH_PADDING * 2, rbounds.dimensions.y - GRAPH_PADDING * 2);
		if (graphBounds.dimensions.x < 0 || graphBounds.dimensions.y < 0) {
			updateGraphBounds();
		}
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg,gbounds.position.x - 2,gbounds.position.y - 2,gbounds.dimensions.x + 4,gbounds.dimensions.y + 4,context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, context->theme.HIGHLIGHT);
		nvgFill(nvg);
		//Draw vertical line for x=0
		if (graphBounds.position.x < 0 && graphBounds.position.x + graphBounds.dimensions.x > 0)
		{
			float xpos = -graphBounds.position.x / graphBounds.dimensions.x;
			nvgBeginPath(nvg);
			nvgMoveTo(nvg, xpos * gbounds.dimensions.x + gbounds.position.x,gbounds.position.y);
			nvgLineTo(nvg, xpos * gbounds.dimensions.x + gbounds.position.x,gbounds.position.y + gbounds.dimensions.y);
			nvgStrokeWidth(nvg, 2.0f);
			nvgStrokeColor(nvg, context->theme.DARK.toSemiTransparent(0.75f));
			nvgStroke(nvg);
		}
		//Draw horizontal line for y=0
		if (graphBounds.position.y < 0 && graphBounds.position.y + graphBounds.dimensions.y > 0)
		{
			float ypos = -graphBounds.position.y / graphBounds.dimensions.y;
			nvgBeginPath(nvg);
			nvgMoveTo(nvg, gbounds.position.x, ypos * gbounds.dimensions.y + gbounds.position.y);
			nvgLineTo(nvg, gbounds.position.x + gbounds.dimensions.x, ypos * gbounds.dimensions.y + gbounds.position.y);
			nvgStrokeWidth(nvg, 2.0f);
			nvgStrokeColor(nvg, context->theme.DARK.toSemiTransparent(0.75f));
			nvgStroke(nvg);
		}
		if (gbounds.contains(cursorPosition))
		{
			gpos = (cursorPosition - gbounds.position) / gbounds.dimensions;
			gpos.y = 1 - gpos.y;
			gpos = gpos*graphBounds.dimensions + graphBounds.position;
			
			nvgBeginPath(nvg);
			nvgMoveTo(nvg, cursorPosition.x,gbounds.position.y);
			nvgLineTo(nvg, cursorPosition.x,gbounds.position.y + gbounds.dimensions.y);
			nvgStrokeWidth(nvg, 1.0f);
			nvgStrokeColor(nvg, context->theme.DARK.toSemiTransparent(0.25f));
			nvgStroke(nvg);
			
			nvgBeginPath(nvg);
			nvgMoveTo(nvg, gbounds.position.x,cursorPosition.y);
			nvgLineTo(nvg, gbounds.position.x + gbounds.dimensions.x,cursorPosition.y);
			nvgStrokeWidth(nvg, 1.0f);
			nvgStrokeColor(nvg, context->theme.DARK.toSemiTransparent(0.25f));
			nvgStroke(nvg);

		}
		for (GraphDataPtr& curve : curves)
		{
			std::vector<float2> points = curve->points;
			if (points.size() > 1 && graphBounds.dimensions.x > 0.0f&& graphBounds.dimensions.y > 0.0f)
			{
				NVGcontext* nvg = context->nvgContext;
				float2 last = points[0];
				last = (last - graphBounds.position) / graphBounds.dimensions;
				last.y = 1.0f - last.y;
				last = last * gbounds.dimensions + gbounds.position;
				nvgBeginPath(nvg);
				nvgMoveTo(nvg, last.x, last.y);
				for (int i = 1; i < points.size(); i++)
				{
					float2 pt = points[i];
					pt = (pt - graphBounds.position) / graphBounds.dimensions;
					pt.y = 1.0f - pt.y;
					pt = pt * gbounds.dimensions + gbounds.position;
					nvgLineTo(nvg, pt.x, pt.y);
					last = pt;
				}
				nvgStrokeWidth(nvg, 2.0f);
				nvgStrokeColor(nvg, curve->color);
				nvgStroke(nvg);
			}
		}

		

		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		nvgFontSize(nvg, LARGE_TEXT);
		nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
		drawText(nvg,rbounds.position + float2(rbounds.dimensions.x / 2, 2.0f),name,FontStyle::Outline,context->theme.HIGHLIGHT,context->theme.DARK);
		nvgFontSize(nvg, MEDIUM_TEXT);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
		drawText(nvg,rbounds.position+ float2(rbounds.dimensions.x / 2,rbounds.dimensions.y - 4.0f),xAxisLabel,FontStyle::Outline,context->theme.HIGHLIGHT, context->theme.DARK);
		nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
		nvgSave(nvg);
		pixel2 center =rbounds.position + float2(2.0f, rbounds.dimensions.y * 0.5f);
		nvgTranslate(nvg, center.x, center.y);
		nvgRotate(nvg, -ALY_PI * 0.5f);
		drawText(nvg,pixel2(0, 2),yAxisLabel,FontStyle::Outline, context->theme.HIGHLIGHT, context->theme.DARK);
		nvgRestore(nvg);
		nvgFontSize(nvg,SMALL_TEXT);
		nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
		drawText(nvg,rbounds.position + float2(GRAPH_PADDING, GRAPH_PADDING),MakeString() << std::setprecision(2)<< (graphBounds.position.y + graphBounds.dimensions.y),FontStyle::Outline,context->theme.LIGHT_TEXT, context->theme.DARK);
		nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);
		drawText(nvg,rbounds.position+ float2(GRAPH_PADDING,rbounds.dimensions.y - GRAPH_PADDING),MakeString() << std::setprecision(2) << graphBounds.position.y,FontStyle::Outline,context->theme.LIGHT_TEXT, context->theme.DARK);
		nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
		drawText(nvg,rbounds.position+ float2(rbounds.dimensions.x - GRAPH_PADDING,rbounds.dimensions.y - GRAPH_PADDING + 2),MakeString() << std::setprecision(2)<< (graphBounds.position.x + graphBounds.dimensions.x),FontStyle::Outline, context->theme.LIGHT_TEXT, context->theme.DARK);
		nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		drawText(nvg,rbounds.position+ float2(GRAPH_PADDING,rbounds.dimensions.y - GRAPH_PADDING + 2),MakeString() << std::setprecision(2) << graphBounds.position.x,	FontStyle::Outline, context->theme.LIGHT_TEXT, context->theme.DARK);
		
		if (cursorPosition.x >= 0) {
			float minDist = 1E30f;
			float bestY;
			GraphDataPtr closestCurve;
			for (GraphDataPtr& curve : curves) {
				float y = curve->interpolate(gpos.x);
				if (y != GraphData::NO_INTERSECT) {
					if (std::abs(y - gpos.y) < minDist) {
						minDist = std::abs(y - gpos.y);
						bestY = y;
						closestCurve = curve;
					}
				}
			}
			if (closestCurve.get() != nullptr) {
				nvgBeginPath(nvg);
				nvgStrokeWidth(nvg, 2.0f);
				nvgFillColor(nvg, closestCurve->color);
				nvgStrokeColor(nvg, context->theme.LIGHT_TEXT);
				float2 pt(gpos.x, bestY);
				pt = (pt - graphBounds.position) / graphBounds.dimensions;
				pt.y = 1.0f - pt.y;
				pt = pt * gbounds.dimensions + gbounds.position;
				nvgCircle(nvg, pt.x, pt.y, 4);
				nvgFill(nvg);
				nvgStroke(nvg);


				nvgBeginPath(nvg);
				nvgFillColor(nvg, context->theme.DARK);
				nvgCircle(nvg, cursorPosition.x, cursorPosition.y, 2);
				nvgFill(nvg);

				nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
				nvgFontSize(nvg, MEDIUM_TEXT);
				drawText(nvg, float2(pt.x - 8, pt.y), closestCurve->name, FontStyle::Outline, context->theme.HIGHLIGHT, context->theme.DARK);
				nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				drawText(nvg, float2(pt.x+8,pt.y), 
					MakeString()<<"("<<std::setprecision(2)<<gpos.x<<", "<< std::setprecision(2) << bestY<<")", FontStyle::Outline, context->theme.HIGHLIGHT, context->theme.DARK);

			}
			else {

				nvgBeginPath(nvg);
				nvgFillColor(nvg, context->theme.DARK);
				nvgCircle(nvg, cursorPosition.x, cursorPosition.y, 2);
				nvgFill(nvg);
			}
		}
	}
	void Graph::add(const GraphDataPtr& curve) {
		curves.push_back(curve);
	}
	std::shared_ptr<GraphData> Graph::add(const GraphData& curve) {
		std::shared_ptr<GraphData> curvePtr = std::shared_ptr<GraphData>(new GraphData(curve));
		curves.push_back(curvePtr);
		return curvePtr;
	}
	box2f Graph::updateGraphBounds() {
		float2 minPt(std::numeric_limits<float>::max());
		float2 maxPt(std::numeric_limits<float>::min());
		for (GraphDataPtr& curve : curves) {
			for(float2& pt:curve->points){
				minPt = aly::min(pt, minPt);
				maxPt = aly::max(pt, maxPt);
			}
		}
		graphBounds = box2f(minPt, maxPt - minPt);
		return graphBounds;
	}
	const float GraphData::NO_INTERSECT = std::numeric_limits<float>::max();
	float GraphData::interpolate(float x) const {
		if (points.size() < 2) {
			return NO_INTERSECT;
		}
		if (x<points.front().x || x>points.back().x) {
			return NO_INTERSECT;
		}
		float y = 0;
		int startX = 0;
		int endX = (int)points.size() - 1;
		for (int i = 1;i < points.size();i++) {
			if (x < points[i].x) {
				startX = i-1;
				break;
			}
		}
		for (int i = (int)points.size() - 2;i >= 0;i--) {
			if (x > points[i].x) {
				endX = i+1;
				break;
			}
		}
		if (startX == endX) {
			return points[startX].y;
		}
		else if (endX - startX > 0) {
			float diff = (points[endX].x - points[startX].x);
			if (diff < 1E-10f) {
				return 0.5f*(points[endX].y + points[startX].y);
			}
			else {
				y=points[startX].y + (x - points[startX].x) * (points[endX].y - points[startX].y) / diff;
				return y;
			}
		}
		return NO_INTERSECT;
	}
}

