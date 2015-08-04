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
using namespace std;
namespace aly {
CheckBox::CheckBox(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool checked) :
		Widget(label, position, dimensions), checked(checked) {
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
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->checked=!this->checked;
					this->valueLabel->textColor=(this->checked)?MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT):MakeColor(AlloyApplicationContext()->theme.DARK);
					return true;
				}
				return false;
			};
	checkLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->checked=!this->checked;
					this->valueLabel->textColor=(this->checked)?MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT):MakeColor(AlloyApplicationContext()->theme.DARK);
					return true;
				}
				return false;
			};
}
void CheckBox::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
				context->theme.HIGHLIGHT.toSemiTransparent(0));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
		checkLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
	} else {
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

	if (context->isMouseOver(valueLabel.get())
			|| context->isMouseOver(checkLabel.get())) {
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

ToggleBox::ToggleBox(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool checked) :
		Widget(label, position, dimensions), toggledOn(checked) {
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
	clickRegion->setAspectRule(AspectRule::FixedHeight);
	clickRegion->add(onLabel);
	clickRegion->add(offLabel);
	valueContainer->add(toggleLabel);
	valueContainer->add(clickRegion);
	add(valueContainer);
	onLabel->setVisible(this->toggledOn);
	offLabel->setVisible(!this->toggledOn);

	onLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->toggledOn=!this->toggledOn;
					onLabel->setVisible(this->toggledOn);
					offLabel->setVisible(!this->toggledOn);
					return true;
				}
				return false;
			};
	offLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->toggledOn=!this->toggledOn;
					onLabel->setVisible(this->toggledOn);
					offLabel->setVisible(!this->toggledOn);
					return true;
				}
				return false;
			};
	clickRegion->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->toggledOn=!this->toggledOn;
					onLabel->setVisible(this->toggledOn);
					offLabel->setVisible(!this->toggledOn);
					return true;
				}
				return false;
			};
	toggleLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->toggledOn=!this->toggledOn;
					onLabel->setVisible(this->toggledOn);
					offLabel->setVisible(!this->toggledOn);
					return true;
				}
				return false;
			};
}
void ToggleBox::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
				context->theme.HIGHLIGHT.toSemiTransparent(0));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
		toggleLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
	} else {
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
	} else {
		pos = clickbox.position.x + radius;
	}
	if (context->isMouseOver(clickRegion.get())
			|| context->isMouseOver(toggleLabel.get())) {
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
		Widget(name, pt, dims), value(0), label(name) {

}
TextButton::TextButton(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions) :
		Widget(label) {
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
	float lineWidth = 2.0f;

	int xoff = 0;
	int yoff = 0;
	if (down) {
		xoff = 2;
		yoff = 2;
	}
	if (hover || down) {
		if (!down) {
			nvgBeginPath(nvg);
			NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
					bounds.position.y, bounds.dimensions.x - 2,
					bounds.dimensions.y, context->theme.CORNER_RADIUS, 8,
					context->theme.SHADOW,
					context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
			nvgFillPaint(nvg, shadowPaint);
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
					bounds.dimensions.x, bounds.dimensions.y,
					context->theme.CORNER_RADIUS);
			nvgFill(nvg);
		}

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);

	} else {
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
	this->aspectRatio = tw / th;
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	pixel2 offset(0, 0);
	nvgText(nvg, bounds.position.x + bounds.dimensions.x / 2 + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff, name.c_str(),
			nullptr);

}

TextIconButton::TextIconButton(const std::string& label, int iconCode,
		const AUnit2D& position, const AUnit2D& dimensions) :
		iconCodeString(CodePointToUTF8(iconCode)), Widget(label) {
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
	float lineWidth = 2.0f;
	int xoff = 0;
	int yoff = 0;
	if (down) {
		xoff = 2;
		yoff = 2;
	}
	if (hover || down) {
		if (!down) {
			nvgBeginPath(nvg);
			NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
					bounds.position.y, bounds.dimensions.x - 2,
					bounds.dimensions.y, context->theme.CORNER_RADIUS, 8,
					context->theme.SHADOW,
					context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
			nvgFillPaint(nvg, shadowPaint);
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
					bounds.dimensions.x, bounds.dimensions.y,
					context->theme.CORNER_RADIUS);
			nvgFill(nvg);
		}

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);

	} else {
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
	nvgFillColor(nvg, *textColor);
	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);

	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	float tw = nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr);

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	float iw = nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr,
			nullptr);

	float ww = tw + iw + AlloyApplicationContext()->theme.SPACING.x;
	pixel2 offset(0, 0);

	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgText(nvg, bounds.position.x + (bounds.dimensions.x - ww) / 2 + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff, name.c_str(),
			nullptr);

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	nvgText(nvg,
			AlloyApplicationContext()->theme.SPACING.x + bounds.position.x
					+ (bounds.dimensions.x - ww) / 2 + tw + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff,
			iconCodeString.c_str(), nullptr);
}
IconButton::IconButton(const std::shared_ptr<Glyph>& glyph,
		const AUnit2D& position, const AUnit2D& dimensions) :
		Widget("Icon", position, dimensions) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	iconGlyph = glyph;
	this->aspectRule = AspectRule::FixedHeight;
}
void IconButton::draw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();

	pixel2 center = bounds.position + HALF_PIX(bounds.dimensions);
	pixel2 radii = HALF_PIX(bounds.dimensions);

	float lineWidth = 2.0f;

	nvgBeginPath(nvg);
	NVGpaint shadowPaint = nvgRadialGradient(nvg, center.x + 1, center.y + 1,
			radii.x - 2, radii.x + 2, context->theme.SHADOW,
			context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
	nvgFillPaint(nvg, shadowPaint);
	nvgEllipse(nvg, center.x + 1, center.y + 1, radii.x + 2, radii.y + 2);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgEllipse(nvg, center.x, center.y, radii.x, radii.y);
	nvgFillColor(nvg, *backgroundColor);
	nvgFill(nvg);

	if (hover) {
		iconGlyph->draw(bounds, context->theme.HIGHLIGHT, *backgroundColor,
				context);
	} else {
		iconGlyph->draw(bounds, context->theme.LIGHT_TEXT, *backgroundColor,
				context);

	}
	if (borderColor->a > 0) {
		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y,
				context->dpmm.y, context->pixelRatio);
		nvgBeginPath(nvg);
		nvgEllipse(nvg, center.x, center.y, radii.x, radii.y);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
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
	} else if (orientation == Orientation::Vertical) {
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

Selection::Selection(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, const std::vector<std::string>& options) :
		Widget(label) {
	this->position = position;
	this->dimensions = dimensions;
	CompositePtr valueContainer = MakeComposite(label,
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	selectionLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	arrowLabel = MakeTextLabel(CodePointToUTF8(0xf13a),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), FontType::Icon,
			UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);
	selectionBox = SelectionBoxPtr(new SelectionBox(label, options));
	selectionBox->setDetacted(true);
	selectionBox->setVisible(false);
	selectionBox->setPosition(CoordPercent(0.0f, 0.0f));
	selectionBox->setDimensions(CoordPercent(1.0f, 1.0f));
	selectionBox->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	selectionBox->borderColor = MakeColor(
			AlloyApplicationContext()->theme.HIGHLIGHT);
	selectionBox->borderWidth = UnitPX(1.0f);
	selectionBox->textColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT_TEXT);
	selectionBox->textAltColor = MakeColor(
			AlloyApplicationContext()->theme.DARK_TEXT);
	arrowLabel->setAspectRatio(1.0f);
	arrowLabel->setOrigin(Origin::TopRight);
	arrowLabel->setAspectRule(AspectRule::FixedHeight);
	valueContainer->add(selectionLabel);
	valueContainer->add(arrowLabel);
	add(selectionBox);
	add(valueContainer);

	selectionLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					context->setOnTopRegion(selectionBox.get());
					selectionBox->setVisible(true);
					selectionBox->setSelectionOffset(0);
					selectionBox->setSelectedIndex(0);
					return true;
				}
				return false;
			};
	arrowLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					context->setOnTopRegion(selectionBox.get());
					selectionBox->setVisible(true);
					selectionBox->setSelectionOffset(0);
					selectionBox->setSelectedIndex(0);
					return true;
				} else if(event.button==GLFW_MOUSE_BUTTON_RIGHT) {
					context->removeOnTopRegion(selectionBox.get());
					selectionBox->setVisible(false);
				}
				return false;
			};
	selectionBox->onMouseUp =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					context->removeOnTopRegion(selectionBox.get());
					selectionBox->setVisible(false);
					int newSelection=selectionBox->getSelectedIndex();
					if(newSelection<0) {
						selectionBox->setSelectedIndex(selectedIndex);
					} else {
						selectedIndex=selectionBox->getSelectedIndex();
						selectionBox->setSelectedIndex(selectedIndex);
					}
					selectionLabel->label=this->getSelection();
					return true;
				} else if(event.button==GLFW_MOUSE_BUTTON_RIGHT) {
					context->removeOnTopRegion(selectionBox.get());
					selectionBox->setVisible(false);
				}
				return false;
			};
	selectionBox->onSelect = [this](SelectionBox* box) {
		AlloyApplicationContext()->removeOnTopRegion(selectionBox.get());
		selectionBox->setVisible(false);
		int newSelection=selectionBox->getSelectedIndex();
		if(newSelection<0) {
			selectionBox->setSelectedIndex(selectedIndex);
		} else {
			selectedIndex=selectionBox->getSelectedIndex();
			selectionBox->setSelectedIndex(selectedIndex);
		}
		selectionLabel->label=this->getSelection();
		return true;
	};
}

void Selection::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	bool hover = context->isMouseContainedIn(this);
	box2px bounds = getBounds();
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);

		arrowLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
		selectionLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
	} else {

		arrowLabel->textColor = MakeColor(context->theme.LIGHT_TEXT);
		selectionLabel->textColor = MakeColor(context->theme.LIGHT_TEXT);
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
			context->theme.SHADOW.toSemiTransparent(0.0f),
			context->theme.HIGHLIGHT);
	nvgFillPaint(nvg, hightlightPaint);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);
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
	textColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
	borderColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
	sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

	sliderHandle->setPosition(CoordPercent(0.0, 0.0));
	sliderHandle->setDimensions(CoordPX(handleSize, handleSize));
	sliderHandle->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT);
	sliderHandle->setEnableDrag(true);

	sliderTrack = std::shared_ptr<SliderTrack>(
			new SliderTrack("Scroll Track", Orientation::Horizontal,
					AlloyApplicationContext()->theme.HIGHLIGHT,
					AlloyApplicationContext()->theme.HIGHLIGHT));

	sliderTrack->setPosition(CoordPerPX(0.0f, 1.0f, 0.0f, -handleSize));
	sliderTrack->setDimensions(CoordPerPX(1.0f, 0.0f, 0.0f, handleSize));

	sliderTrack->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	sliderTrack->add(sliderHandle);
	sliderTrack->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {return this->onMouseDown(context,sliderTrack.get(),e);};
	sliderHandle->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {return this->onMouseDown(context,sliderHandle.get(),e);};
	sliderHandle->onMouseUp =
			[this](AlloyContext* context,const InputEvent& e) {return this->onMouseUp(context,sliderHandle.get(),e);};
	sliderHandle->onMouseDrag =
			[this](AlloyContext* context,const InputEvent& e) {
				return this->onMouseDrag(context,sliderHandle.get(),e);};

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
		this->setValue(this->value.toDouble());
	};
	this->onEvent =
			[this](AlloyContext* context, const InputEvent& event) {
				if(event.type==InputType::Scroll&&isVisible()&&context->isMouseContainedIn(this)) {
					double oldV=getBlendValue();
					double newV=clamp(event.scroll.y*0.1f+oldV,0.0,1.0);
					if(newV!=oldV) {
						this->setBlendValue(newV);
						if(onChangeEvent)onChangeEvent(this->value);
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
	float xoff = (float) (sliderTrack->getBoundsPositionX()
			+ interp
					* (sliderTrack->getBoundsDimensionsX()
							- sliderHandle->getBoundsDimensionsX()));
	sliderHandle->setDragOffset(
			pixel2(xoff, sliderHandle->getBoundsDimensionsY()),
			pixel2(0.0f, 0.0f));
	this->value.setValue(value);
}
void HorizontalSlider::update() {
	double interp = (sliderHandle->getBoundsPositionX()
			- sliderTrack->getBoundsPositionX())
			/ (double) (sliderTrack->getBoundsDimensionsX()
					- sliderHandle->getBoundsDimensionsX());
	float val = (float) ((1.0 - interp) * minValue.toDouble()
			+ interp * maxValue.toDouble());
	value.setValue(val);
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
		} else if (region == sliderHandle.get()) {
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
	return (value.toDouble() - minValue.toDouble())
			/ (maxValue.toDouble() - minValue.toDouble());
}
bool HorizontalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if (region == sliderHandle.get()) {
		region->setDragOffset(event.cursor, context->getCursorDownPosition());
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
	bool hover = context->isMouseContainedIn(this);
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.DARK,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
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
	float trackPadding = 10.0f;
	this->aspectRatio = 4.0f;
	textColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
	borderColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
	sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

	sliderHandle->setPosition(CoordPercent(0.0, 0.0));
	sliderHandle->setDimensions(CoordPX(handleSize, handleSize));
	sliderHandle->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT);
	sliderHandle->setEnableDrag(true);

	sliderTrack = std::shared_ptr<SliderTrack>(
			new SliderTrack("Scroll Track", Orientation::Vertical,
					AlloyApplicationContext()->theme.HIGHLIGHT,
					AlloyApplicationContext()->theme.HIGHLIGHT));

	sliderTrack->setPosition(CoordPerPX(0.5f, 0.1f, -handleSize * 0.5f, 2.0f));
	sliderTrack->setDimensions(CoordPerPX(0.0f, 0.8f, handleSize, -4.0f));

	sliderTrack->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	sliderTrack->add(sliderHandle);
	sliderTrack->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {return this->onMouseDown(context,sliderTrack.get(),e);};
	sliderHandle->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {
				return this->onMouseDown(context,sliderHandle.get(),e);};
	sliderHandle->onMouseUp =
			[this](AlloyContext* context,const InputEvent& e) {return this->onMouseUp(context,sliderHandle.get(),e);};
	sliderHandle->onMouseDrag =
			[this](AlloyContext* context,const InputEvent& e) {
				return this->onMouseDrag(context,sliderHandle.get(),e);};

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
		this->setValue(this->value.toDouble());
	};
	this->onEvent =
			[this](AlloyContext* context, const InputEvent& event) {
				if(event.type==InputType::Scroll&&isVisible()&&context->isMouseContainedIn(this)) {
					double oldV=getBlendValue();
					double newV=clamp(event.scroll.y*0.1f+getBlendValue(),0.0,1.0);
					if(newV!=oldV) {
						this->setBlendValue(newV);
						if(onChangeEvent)onChangeEvent(this->value);
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
	float yoff = (float) (sliderTrack->getBoundsPositionY()
			+ interp
					* (sliderTrack->getBoundsDimensionsY()
							- sliderHandle->getBoundsDimensionsY()));
	sliderHandle->setDragOffset(
			pixel2(sliderHandle->getBoundsDimensionsX(), yoff),
			pixel2(0.0f, 0.0f));
	this->value.setValue(value);
}
void VerticalSlider::update() {
	double interp = (sliderHandle->getBoundsPositionY()
			- sliderTrack->getBoundsPositionY())
			/ (double) (sliderTrack->getBoundsDimensionsY()
					- sliderHandle->getBoundsDimensionsY());
	float val = (float) (interp * minValue.toDouble()
			+ (1.0 - interp) * maxValue.toDouble());
	value.setValue(val);
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
		} else if (region == sliderHandle.get()) {
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
		region->setDragOffset(event.cursor, context->getCursorDownPosition());
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
	bool hover = context->isMouseContainedIn(this);
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.DARK,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
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
		Widget(name, pos, dims) {
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
	colorLabel->setPosition(CoordPerPX(1.0f, 0.0f, -4.0f, 4.0f));
	colorLabel->setDimensions(CoordPerPX(0.0f, 1.0f, 0.0f, -8.0f));
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
			[this](AlloyContext* context,const InputEvent& e) {
				if(e.button == GLFW_MOUSE_BUTTON_LEFT) {
					if(!context->isOnTop(colorSelectionPanel.get())) {
						colorWheel->reset();
						context->setOnTopRegion(colorSelectionPanel.get());
						context->setDragObject(colorSelectionPanel.get());

					} else {
						context->removeOnTopRegion(colorSelectionPanel.get());
						colorSelectionPanel->setVisible(false);
					}
					return true;
				}
				return false;
			};
	textLabel->onMouseDown = [this](AlloyContext* context,const InputEvent& e) {
		if(e.button == GLFW_MOUSE_BUTTON_LEFT) {
			if(!context->isOnTop(colorSelectionPanel.get())) {
				colorWheel->reset();
				context->setOnTopRegion(colorSelectionPanel.get());
				context->setDragObject(colorSelectionPanel.get());

			} else {
				context->removeOnTopRegion(colorSelectionPanel.get());
				colorSelectionPanel->setVisible(false);
			}
			return true;
		}
		return false;
	};

	colorSelectionPanel = MakeComposite("Color Selection Panel",
			CoordPerPX(0.5f, 1.0, 0.0f, 2.0f),
			CoordPX(
					300.0f
							+ (60 + AlloyApplicationContext()->theme.SPACING.x)
									* 5, 300.0f),
			AlloyApplicationContext()->theme.LIGHT);
	colorSelectionPanel->setOrigin(Origin::TopCenter);
	colorSelectionPanel->setVisible(false);
	colorSelectionPanel->setDetacted(true);
	colorSelectionPanel->setRoundCorners(true);
	redSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("R", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	redSlider->setLabelFormatter(
			[](const Number& value) {
				std::string str=MakeString()<<(int)std::floor(100.0f*value.toFloat())<<"%";
				return str;
			});
	redSlider->setOnChangeEvent([this](const Number& value) {
		Color c=colorWheel->getSelectedColor();
		c.r=value.toFloat();
		colorWheel->setColor(c);
		HSV hsv=c.toHSV();
		lumSlider->setValue(hsv.z);
		updateColorSliders(c);
	});

	greenSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("G", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	greenSlider->setOnChangeEvent([this](const Number& value) {
		Color c=colorWheel->getSelectedColor();
		c.g=value.toFloat();
		colorWheel->setColor(c);
		HSV hsv=c.toHSV();
		lumSlider->setValue(hsv.z);
		updateColorSliders(c);
	});
	greenSlider->setLabelFormatter([](const Number& value) {
		string str=MakeString()<<(int)std::floor(100.0f*value.toFloat())<<"%";
		return str;
	});
	blueSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("B", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	blueSlider->setOnChangeEvent([this](const Number& value) {
		Color c=colorWheel->getSelectedColor();
		c.b=value.toFloat();
		colorWheel->setColor(c);
		HSV hsv=c.toHSV();
		lumSlider->setValue(hsv.z);
		updateColorSliders(c);
	});
	blueSlider->setLabelFormatter([](const Number& value) {
		string str=MakeString()<<(int)std::floor(100.0f*value.toFloat())<<"%";
		return str;
	});
	lumSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("L", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	lumSlider->setLabelFormatter([](const Number& value) {
		string str=MakeString()<<(int)std::floor(100.0f*value.toFloat())<<"%";
		return str;
	});
	lumSlider->setOnChangeEvent([this](const Number& value) {
		Color c=colorWheel->getSelectedColor();
		HSVA hsv=c.toHSVA();
		hsv.z=value.toFloat();
		c=HSVAtoRGBAf(hsv);
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
		HSV hsv=c.toHSV();
		lumSlider->setValue(hsv.z);
		updateColorSliders(c);
	});
	colorSelectionPanel->setOrientation(Orientation::Horizontal);

	colorSelectionPanel->add(colorWheel);
	colorSelectionPanel->add(redSlider);
	colorSelectionPanel->add(greenSlider);
	colorSelectionPanel->add(blueSlider);
	colorSelectionPanel->add(lumSlider);
	colorSelectionPanel->add(alphaSlider);
	colorSelectionPanel->onEvent =
			[this](AlloyContext* context,const InputEvent& e) {
				if(context->isOnTop(colorSelectionPanel.get())) {
					if(e.isDown()&&!context->isMouseContainedIn(getBounds())) {
						context->removeOnTopRegion(colorSelectionPanel.get());
						colorSelectionPanel->setVisible(false);
						return true;
					}
				}
				return false;
			};
	colorSelectionPanel->onRemoveFromOnTop = [this]() {
		colorSelectionPanel->setVisible(false);
	};
	Application::addListener(colorSelectionPanel.get());
	add(textLabel);
	add(colorLabel);
	add(colorSelectionPanel);

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
			[this](AlloyContext* context,const InputEvent& e) {
				if(context->isOnTop(this)&&e.button==GLFW_MOUSE_BUTTON_LEFT&&e.isDown()) {
					float r2=distanceSqr(e.cursor,center);
					if(r2<rInner*rInner) {
						triangleSelected=true;
						circleSelected=false;
					} else if(r2<rOuter*rOuter) {
						triangleSelected=false;
						circleSelected=true;
					} else {
						circleSelected=false;
						triangleSelected=false;
					}
					this->setColor(e.cursor);
					return true;
				}
				return false;
			};
	this->onEvent =
			[this](AlloyContext* context,const InputEvent& e) {
				if(context->isOnTop(this)) {
					if(e.type==InputType::Cursor&&context->isLeftMouseButtonDown()) {
						this->setColor(e.cursor);
						return true;
					} else if(e.type==InputType::Scroll&&context->isMouseContainedIn(getBounds())) {

						hsvColor.x+=e.scroll.y*0.01f;
						if(hsvColor.x<0.0f)hsvColor.x+=1.0f;
						if(hsvColor.x>1.0f)hsvColor.x-=1.0f;
						setColor(HSVAtoColor(hsvColor));
						updateWheel();
						if(onChangeEvent)onChangeEvent(selectedColor);
						return true;
					}
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
		float2 pt = cursor - tBounds[0];
		RGBAf pivot = HSVAtoRGBAf(HSVA(hsvColor.x, 1.0f, 0.5f, 1.0f));

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
	} else if (circleSelected) {
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

	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;
	float t = 0.3f;
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
		float a0 = (float) i / 6.0f * NVG_PI * 2.0f - aeps;
		float a1 = (float) (i + 1.0f) / 6.0f * NVG_PI * 2.0f + aeps;
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
		} else if (cross(tPoints[0] - tPoints[2], pt - tPoints[2]) < 0) {
			pt = dot(pt - tPoints[2], tPoints[0] - tPoints[2])
					* (tPoints[0] - tPoints[2])
					/ lengthSqr(tPoints[0] - tPoints[2]) + tPoints[2];
		}
	} else {
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
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
		textLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
		colorLabel->borderWidth = UnitPX(2.0f);
		colorLabel->borderColor = MakeColor(context->theme.HIGHLIGHT);
	} else {
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
	this->expanded = expanded;
	contentRegion->setVisible(expanded);
	arrowIcon->label =
			(expanded) ? CodePointToUTF8(0xf056) : CodePointToUTF8(0xf055);
}
ExpandRegion::ExpandRegion(const std::string& name,
		const std::shared_ptr<Region>& region, const AUnit2D& pos,
		const AUnit2D& dims) :
		Composite(name + "_eregion", pos, dims) {
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
	//add(region);
	selectionLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					setExpanded(!expanded);
					return true;
				}
				return false;
			};
	this->onMouseDown = [this](AlloyContext* context, const InputEvent& event) {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
			setExpanded(!expanded);
			return true;
		}
		return false;
	};
	arrowIcon->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					setExpanded(!expanded);
					return true;
				}
				return false;
			};
	setExpanded(false);
}
void FileSelector::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);

	box2px fileBounds = fileLabel->getBounds(false);
	box2px openBounds = openIcon->getBounds(false);

	pixel2 tdim = fileLabel->getTextDimensions(context);

	box2px locationBounds = fileLocationLabel->getBounds(false);
	float spacing = AlloyApplicationContext()->theme.SPACING.x;
	fileLocationLabel->setBounds(
			pixel2(fileBounds.position.x + tdim.x + spacing,
					locationBounds.position.y),
			pixel2(
					openBounds.position.x
							- (fileBounds.position.x + tdim.x + spacing * 2),
					locationBounds.dimensions.y));

	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
				context->theme.HIGHLIGHT.toSemiTransparent(0));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
		fileLabel->textColor = MakeColor(context->theme.HIGHLIGHT);
		openIcon->textColor = MakeColor(context->theme.HIGHLIGHT);

	} else {
		fileLabel->textColor = MakeColor(context->theme.LIGHT_TEXT);
		openIcon->textColor = MakeColor(context->theme.LIGHT_TEXT);

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
	box2px clickbox = fileLocationLabel->getBounds();
	nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
			clickbox.dimensions.x, clickbox.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);

	if (context->isMouseOver(fileLabel.get())
			|| context->isMouseOver(openIcon.get())) {
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
FileSelector::FileSelector(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Widget(name, pos, dims) {
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	setRoundCorners(true);
	CompositePtr valueContainer = MakeComposite(
			MakeString() << name << "_container",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));

	std::shared_ptr<Composite>& glassPanel =
			AlloyApplicationContext()->getGlassPanel();

	fileDialog = std::shared_ptr<FileDialog>(
			new FileDialog("File Dialog",
					CoordPerPX(0.5, 0.5, -200 + 7.5f, -150 - 7.5f),
					CoordPX(400, 300)));
	glassPanel->add(fileDialog);

	fileLabel = MakeTextLabel(name, CoordPX(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);

	fileLocationLabel = MakeTextLabel("None", CoordPX(0.0f, 0.0f),
			CoordPerPX(1.0f, 1.0f, 0.0f, 0.0f), FontType::Normal,
			UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);

	fileLocationLabel->setRoundCorners(true);
	fileLocationLabel->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.SHADOW.toSemiTransparent(0.5f));
	openIcon = MakeTextLabel(CodePointToUTF8(0xf115), CoordPercent(1.0f, 0.0f),
			CoordPercent(0.0f, 1.0f), FontType::Icon, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);

	openIcon->setAspectRatio(1.0f);
	openIcon->setOrigin(Origin::TopRight);
	openIcon->setAspectRule(AspectRule::FixedHeight);
	valueContainer->add(fileLabel);
	valueContainer->add(fileLocationLabel);
	valueContainer->add(openIcon);
	add(valueContainer);
	fileLocationLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					openFileDialog(context);
					return true;
				}
				return false;
			};

	Composite::onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					openFileDialog(context);
					return true;
				}
				return false;
			};

	openIcon->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					openFileDialog(context);
					return true;
				}
				return false;
			};
	setFileLocation("");
}
void FileSelector::setFileLocation(const std::string& file) {
	fileLocation = file;
	if (file.length() > 0) {
		fileLocationLabel->label = fileLocation;
	} else {
		fileLocationLabel->label = "None";
	}
}
void FileSelector::openFileDialog(AlloyContext* context,
		const std::string& workingDirectory) {
	fileDialog->setValue(workingDirectory);
	if (!fileDialog->isVisible()) {
		context->getGlassPanel()->setVisible(true);
	} else {
		context->getGlassPanel()->setVisible(false);
	}
}
FileEntry::FileEntry(FileDialog* dialog, const std::string& name,
		const AUnit2D& pos, const AUnit2D& dims) :
		Region(name, pos, dims), fileDescription(), fontSize(UnitPercent(0.8f)), dialog(
				dialog) {
	this->backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	this->selected = false;
}
void FileEntry::setValue(const FileDescription& description) {
	this->fileDescription = description;
	iconCodeString =
			(fileDescription.fileType == FileType::Directory) ?
					CodePointToUTF8(0xf07b) : CodePointToUTF8(0xf15b);
	fileName = GetFileName(fileDescription.fileLocation);
	fileSize = FormatSize(fileDescription.fileSize);
	creationTime = FormatDateAndTime(fileDescription.creationTime);
	lastAccessTime = FormatDateAndTime(fileDescription.lastModifiedTime);
	lastModifiedTime = FormatDateAndTime(fileDescription.lastModifiedTime);
	this->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		return dialog->onMouseDown(this, context, e);
	};
	this->onMouseUp = [this](AlloyContext* context, const InputEvent& e) {
		return dialog->onMouseUp(this, context, e);
	};
	this->onMouseDrag = [this](AlloyContext* context, const InputEvent& e) {
		return dialog->onMouseDrag(this,context,e);
	};
	this->onMouseOver = [this](AlloyContext* context, const InputEvent& e) {
		return dialog->onMouseDown(this, context, e);
	};

}
void FileEntry::setSelected(bool selected) {
	this->selected = selected;
}
bool FileEntry::isSelected() {
	return selected;
}
void FileEntry::draw(AlloyContext* context) {
	box2px bounds = getBounds();
	NVGcontext* nvg = context->nvgContext;
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);

	float lineWidth = 2.0f;
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
		} else {
			nvgFillColor(nvg, context->theme.NEUTRAL);
		}
		nvgFill(nvg);
	} else {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
		if (selected) {
			nvgFillColor(nvg, context->theme.LINK);
		} else {
			nvgFillColor(nvg, context->theme.LIGHT);
		}
		nvgFill(nvg);
	}

	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	float iw = nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr,
			nullptr);

	pixel2 offset(0, 0);

	if (selected) {
		if (hover) {

			nvgFillColor(nvg, context->theme.HIGHLIGHT);
		} else {
			nvgFillColor(nvg, context->theme.LIGHT_TEXT);
		}
	} else {
		if (hover) {

			nvgFillColor(nvg, context->theme.HIGHLIGHT);
		} else {
			nvgFillColor(nvg, context->theme.DARK_TEXT);
		}
	}

	box2px labelBounds = getCursorBounds();
	pushScissor(nvg, labelBounds);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	nvgText(nvg,
			AlloyApplicationContext()->theme.SPACING.x + bounds.position.x
					+ xoff, bounds.position.y + bounds.dimensions.y / 2 + yoff,
			iconCodeString.c_str(), nullptr);

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgText(nvg,
			2 * AlloyApplicationContext()->theme.SPACING.x + bounds.position.x
					+ iw + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff,
			fileName.c_str(), nullptr);
	popScissor(nvg);
}
bool FileDialog::onMouseDown(FileEntry* entry, AlloyContext* context,
		const InputEvent& e) {
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
			} else {
				entry->setSelected(true);
				lastSelected.push_back(entry);
			}
		} else {
			if (entry->fileDescription.fileType == FileType::File) {
				if (!entry->isSelected()) {
					for (FileEntry* child : lastSelected) {
						child->setSelected(false);
					}
					entry->setSelected(true);
					fileLocation->setValue(entry->fileDescription.fileLocation);
					lastSelected.clear();
					lastSelected.push_back(entry);
				}
			} else {
				setSelectedFile(entry->fileDescription.fileLocation);
				fileLocation->setValue(entry->fileDescription.fileLocation);
			}
		}
		return true;
	} else if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (!enableMultiSelection) {
			if (lastSelected.size() > 0) {
				fileLocation->setValue(
						GetParentDirectory(
								lastSelected.front()->fileDescription.fileLocation));
			}
		}
		for (FileEntry* child : lastSelected) {
			child->setSelected(false);
		}
		return true;
	}
	return false;
}
bool FileDialog::onMouseOver(FileEntry* entry, AlloyContext* context,
		const InputEvent& e) {
	return false;
}
bool FileDialog::onMouseDrag(FileEntry* entry, AlloyContext* context,
		const InputEvent& e) {
	return false;
}
bool FileDialog::onMouseUp(FileEntry* entry, AlloyContext* context,
		const InputEvent& e) {
	return false;
}
void FileDialog::setEnableMultiSelection(bool enable) {
	enableMultiSelection = enable;
}
bool FileDialog::isMultiSelectionEnabled() {
	return enableMultiSelection;
}
void FileDialog::setSelectedFile(const std::string& file) {
	std::string dir;
	bool select = false;
	if (IsDirectory(file)) {
		dir = file;
	} else {
		dir = GetParentDirectory(file);
		select = true;
	}
	lastSelected.clear();
	fileEntries.clear();
	std::vector<FileDescription> descriptions = GetDirectoryDescriptionListing(
			dir);
	int i = 0;
	for (FileDescription& fd : descriptions) {
		FileEntry* entry = new FileEntry(this, MakeString() << "Entry " << i,
				CoordPX(0, 0),
				CoordPerPX(1.0f, 0.0f, -Composite::scrollBarSize, 30.0f));
		fileEntries.push_back(std::shared_ptr<FileEntry>(entry));
		entry->setValue(fd);
		if (select && entry->fileDescription.fileLocation == file) {
			entry->setSelected(true);
		}
		i++;
	}
	directoryList->clear();
	AlloyApplicationContext()->addDeferredTask([this]() {
		for (std::shared_ptr<FileEntry>& entry : fileEntries) {
			directoryList->add(entry);
		}
	});
}
FileDialog::FileDialog(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Widget(name, pos, dims) {
	containerRegion = std::shared_ptr<BorderComposite>(
			new BorderComposite("Container", CoordPX(0, 15),
					CoordPerPX(1.0, 1.0, -15, -15)));
	openButton = std::shared_ptr<TextIconButton>(
			new TextIconButton("Open", 0xf115,
					CoordPerPX(1.0f, 1.0f, -7.0f, -7.0f), CoordPX(100, 30)));
	openButton->setOrigin(Origin::BottomRight);
	fileLocation = std::shared_ptr<FileField>(
			new FileField("File Location", CoordPX(7, 7),
					CoordPerPX(1.0f, 0.0f, -14.0f, 30.0f)));
	fileLocation->onSelect = [this](FileField* field) {
		std::cout << "Selected " << field->getValue() << std::endl;
		this->setSelectedFile(field->getValue());
	};
	cancelButton = std::shared_ptr<IconButton>(
			new IconButton(
					AlloyApplicationContext()->createAwesomeGlyph(0xf00d,
							FontStyle::Normal, 21),
					CoordPerPX(1.0, 0.0, -30, 30), CoordPX(30, 30)));
	cancelButton->setOrigin(Origin::BottomLeft);
	containerRegion->setNorth(fileLocation, 0.15f);
	containerRegion->setSouth(openButton, 0.15f);
	directoryTree = std::shared_ptr<Composite>(
			new Composite("Container", CoordPX(7, 0),
					CoordPerPX(1.0, 1.0, -7, 0)));
	directoryList = std::shared_ptr<Composite>(
			new Composite("Container", CoordPX(7, 0),
					CoordPerPX(1.0f, 1.0, -14.0f, 0.0f)));
	directoryList->setOrientation(Orientation::Vertical);
	directoryList->setScrollEnabled(true);
	containerRegion->setWest(directoryTree, 0.3f);
	containerRegion->setCenter(directoryList);
	add(containerRegion);
	add(cancelButton);
	this->onEvent = [this](AlloyContext* context, const InputEvent& e) {

		return false;
	};
}
void FileDialog::setValue(const std::string& file) {
	fileLocation->setValue(file);
	setSelectedFile(file);
}
std::string FileDialog::getValue() const {
	return fileLocation->getValue();
}
void FileDialog::setFileSelectionType(FileType type) {
	fileType = type;
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
		Widget(name, pos, dims) {
	setOrientation(Orientation::Vertical);
	setScrollEnabled(true);
}
void ExpandBar::add(const std::shared_ptr<Region>& region, bool expanded) {
	CompositePtr container = MakeComposite("Content Container",
			CoordPX(Composite::scrollBarSize, 0.0f),
			CoordPerPX(1.0f, 0.0f, -2.0f * Composite::scrollBarSize, 0.0f));
	container->setOrientation(Orientation::Vertical);
	region->backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	region->borderColor = MakeColor(AlloyApplicationContext()->theme.NEUTRAL);
	region->borderWidth = UnitPX(2.0f);
	region->setRoundCorners(true);
	container->add(region);
	std::shared_ptr<ExpandRegion> eregion = std::shared_ptr<ExpandRegion>(
			new ExpandRegion(region->name, container,
					CoordPX(Composite::scrollBarSize, 0.0f),
					CoordPerPX(1.0f, 0.0f, -2.0f * Composite::scrollBarSize,
							30.0f)));

	eregion->setExpanded(expanded);
	regions.push_back(eregion);
	Widget::add(eregion);

	Widget::add(container);
	//std::cout << "DRAW OFFSET " << region->parent->drawOffset() << " " << eregion->parent->drawOffset() << std::endl;
}
void ExpandBar::add(Region* region, bool expanded) {
	add(std::shared_ptr<Region>(region), expanded);
}

}

