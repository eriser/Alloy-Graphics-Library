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
using namespace std;
namespace aly {
CheckBox::CheckBox(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool checked) :
		Widget(label), checked(checked) {
	this->position = position;
	this->dimensions = dimensions;
	this->aspectRatio = 4.0f;
	CompositePtr valueContainer = MakeComposite("Check Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	checkLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			Application::getContext()->theme.LIGHT_TEXT.rgba(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	valueLabel = MakeGlyphRegion(
			Application::getContext()->createAwesomeGlyph(0xf00c),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), COLOR_NONE,
			Application::getContext()->theme.LIGHT_TEXT.rgba());
	valueLabel->setOrigin(Origin::TopRight);
	valueLabel->setAspectRule(AspectRule::FixedHeight);
	valueContainer->add(checkLabel);
	valueContainer->add(valueLabel);
	add(valueContainer);
	this->valueLabel->foregroundColor =
			(this->checked) ?
					MakeColor(Application::getContext()->theme.LIGHT_TEXT) :
					MakeColor(Application::getContext()->theme.DARK);
	valueLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->checked=!this->checked;
					this->valueLabel->foregroundColor=(this->checked)?MakeColor(Application::getContext()->theme.LIGHT_TEXT):MakeColor(Application::getContext()->theme.DARK);

				}
			};
	checkLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->checked=!this->checked;
					this->valueLabel->foregroundColor=(this->checked)?MakeColor(Application::getContext()->theme.LIGHT_TEXT):MakeColor(Application::getContext()->theme.DARK);
				}
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
		nvgStrokeColor(nvg, context->theme.LIGHT_TEXT);
		nvgStrokeWidth(nvg, 2.0f);
		nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
				clickbox.dimensions.x, clickbox.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgStroke(nvg);
	}

	Composite::draw(context);
}
void Widget::add(const std::shared_ptr<Region>& region) {
	Composite::add(region);
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
Button::Button(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions) :
		Widget(label) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(Application::getContext()->theme.HIGHLIGHT);
	textColor = MakeColor(Application::getContext()->theme.DARK_TEXT);
	borderColor = MakeColor(Application::getContext()->theme.LIGHT);
	fontSize = UnitPerPX(1.0f, -10);
	this->aspectRule = AspectRule::FixedHeight;
}
void SliderTrack::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	nvgBeginPath(nvg);
	nvgMoveTo(nvg, bounds.position.x + 14,
			bounds.position.y + bounds.dimensions.y * 0.5f);
	nvgLineTo(nvg, bounds.position.x - 14 + bounds.dimensions.x,
			bounds.position.y + bounds.dimensions.y * 0.5f);
	nvgStrokeColor(nvg, context->theme.HIGHLIGHT);
	nvgStrokeWidth(nvg, 10.0f);
	nvgLineCap(nvg, NVG_ROUND);
	nvgStroke(nvg);
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
void SelectionBox::draw(AlloyContext* context) {

}
void SelectionBox::drawOnTop(AlloyContext* context) {
	context->setDragObject(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	float entryHeight = std::min(
			context->viewport.dimensions.y / (float) options.size(),
			bounds.dimensions.y);
	float boxHeight = (options.size()) * entryHeight;

	float yOffset = std::min(bounds.position.y + boxHeight + entryHeight,
			(float) context->viewport.dimensions.y) - boxHeight;
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x, yOffset, bounds.dimensions.x,
				boxHeight);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}

	float th = entryHeight - 2;
	nvgFontSize(nvg, th);
	nvgFillColor(nvg, *textColor);

	float tw = 0.0f;
	for (std::string& label : options) {
		tw = std::max(tw,
				nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr));
	}
	nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	pixel2 offset(0, 0);

	nvgFillColor(nvg, context->theme.DARK);
	int index = 0;
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));

	selectedIndex = -1;
	for (std::string& label : options) {
		if (context->isMouseContainedIn(
				pixel2(bounds.position.x, yOffset) + offset,
				pixel2(bounds.dimensions.x, entryHeight))) {
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x + offset.x, yOffset + offset.y,
					bounds.dimensions.x, entryHeight);
			nvgFillColor(nvg, context->theme.NEUTRAL);
			nvgFill(nvg);
			selectedIndex = index;
		}
		index++;
		nvgFillColor(nvg, *textColor);
		nvgText(nvg, bounds.position.x + offset.x + lineWidth,
				yOffset + entryHeight / 2 + offset.y, label.c_str(), nullptr);
		offset.y += entryHeight;
	}
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				yOffset + lineWidth * 0.5f, bounds.dimensions.x - lineWidth,
				boxHeight - lineWidth);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
	}
}
SelectionBox::SelectionBox(const std::string& name,
		const std::vector<std::string>& labels) :
		Region(name), options(labels), label(name) {
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
			Application::getContext()->theme.LIGHT_TEXT.rgba(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	arrowLabel = MakeGlyphRegion(
			Application::getContext()->createAwesomeGlyph(0xf13a),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f),
			Application::getContext()->theme.DARK.rgba(),
			Application::getContext()->theme.LIGHT_TEXT.rgba());
	selectionBox = SelectionBoxPtr(new SelectionBox("Selection", options));
	selectionBox->setPosition(CoordPercent(0.0f, 0.0f));
	selectionBox->setDimensions(CoordPercent(1.0f, 1.0f));
	selectionBox->fontSize = selectionLabel->fontSize;
	selectionBox->backgroundColor = MakeColor(
			Application::getContext()->theme.DARK);
	selectionBox->borderColor = MakeColor(
			Application::getContext()->theme.HIGHLIGHT);
	selectionBox->borderWidth = UnitPX(1.0f);
	selectionBox->setVisible(false);
	selectionBox->textColor = MakeColor(
			Application::getContext()->theme.LIGHT_TEXT);
	selectionBox->textAltColor = MakeColor(
			Application::getContext()->theme.DARK_TEXT);
	arrowLabel->setOrigin(Origin::TopRight);
	arrowLabel->setAspectRule(AspectRule::FixedHeight);
	valueContainer->add(selectionLabel);
	valueContainer->add(arrowLabel);
	add(valueContainer);
	add(selectionBox);
	selectionLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					selectionBox->setVisible(true);
				}
			};
	arrowLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					selectionBox->setVisible(true);
				}
			};
	selectionBox->onMouseUp =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					selectionBox->setVisible(false);
					int newSelection=selectionBox->getSelectedIndex();
					if(newSelection<0) {
						selectionBox->setSelectedIndex(selectedIndex);
					} else {
						selectedIndex=selectionBox->getSelectedIndex();
						selectionBox->setSelectedIndex(selectedIndex);
					}
					selectionLabel->label=this->getSelection();

				}
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
		Widget(label), minValue(min), maxValue(max), value(value) {
	this->position = position;
	this->dimensions = dimensions;
	float handleSize = 30.0f;
	float trackPadding = 10.0f;
	this->aspectRatio = 4.0f;
	textColor = MakeColor(Application::getContext()->theme.DARK_TEXT);
	borderColor = MakeColor(Application::getContext()->theme.HIGHLIGHT);
	sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

	sliderHandle->setPosition(CoordPercent(0.0, 0.0));
	sliderHandle->setDimensions(CoordPX(handleSize, handleSize));
	sliderHandle->backgroundColor = MakeColor(
			Application::getContext()->theme.LIGHT);
	sliderHandle->setEnableDrag(true);

	sliderTrack = std::shared_ptr<SliderTrack>(new SliderTrack("Scroll Track"));

	sliderTrack->setPosition(CoordPerPX(0.0f, 1.0f, 0.0f, -handleSize));
	sliderTrack->setDimensions(CoordPerPX(1.0f, 0.0f, 0.0f, handleSize));

	sliderTrack->backgroundColor = MakeColor(
			Application::getContext()->theme.DARK);
	sliderTrack->add(sliderHandle);
	sliderTrack->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseDown(context,sliderTrack.get(),e);};
	sliderHandle->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseDown(context,sliderHandle.get(),e);};
	sliderHandle->onMouseUp =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseUp(context,sliderHandle.get(),e);};
	sliderHandle->onMouseDrag =
			[this](AlloyContext* context,const InputEvent& e,const pixel2& lastDragPosition) {this->onMouseDrag(context,sliderHandle.get(),e,lastDragPosition);};

	add(
			sliderLabel = MakeTextLabel(label,
					CoordPerPX(0.0f, 0.0f, trackPadding, 2.0f),
					CoordPerPX(0.5f, 1.0f, 0,
							-(handleSize - trackPadding * 0.75f)),
					FontType::Bold, UnitPerPX(1.0f, 0),
					Application::getContext()->theme.LIGHT_TEXT.rgba(),
					HorizontalAlignment::Left, VerticalAlignment::Bottom));
	add(
			valueLabel = MakeTextLabel("Value",
					CoordPerPX(0.0f, 0.0f, 0.0f, 2.0f),
					CoordPerPX(1.0f, 1.0f, -trackPadding,
							-(handleSize - trackPadding * 0.75f)),
					FontType::Normal, UnitPerPX(1.0f, -2),
					Application::getContext()->theme.LIGHT_TEXT.rgba(),
					HorizontalAlignment::Right, VerticalAlignment::Bottom));
	add(sliderTrack);
	this->onPack = [this]() {
		this->setValue(this->value.toDouble());
	};
}
/*
 void HorizontalSlider::pack(const pixel2& pos, const pixel2& dims,const double2& dpmm, double pixelRatio){
 Region::pack(pos, dims,dpmm, pixelRatio);
 box2px bounds=getBounds();
 bounds.position-=drawOffset();
 for (std::shared_ptr<Region>& region : children) {
 region->pack(bounds.position,bounds.dimensions,dpmm, pixelRatio);
 }
 for (std::shared_ptr<Region>& region : children) {
 if (region->onPack)region->onPack();
 }
 if (onPack)
 onPack();
 }
 */
void HorizontalSlider::setValue(double value) {
	double interp = clamp(
			(value - minValue.toDouble())
					/ (maxValue.toDouble() - minValue.toDouble()), 0.0, 1.0);
	double xoff = sliderTrack->getBoundsPositionX()
			+ interp
					* (sliderTrack->getBoundsDimensionsX()
							- sliderHandle->getBoundsDimensionsX());
	sliderHandle->setDragOffset(
			pixel2(xoff, sliderHandle->getBoundsDimensionsY()),
			pixel2(0.0f, 0.0f));
}
void HorizontalSlider::update() {
	double interp = (sliderHandle->getBoundsPositionX()
			- sliderTrack->getBoundsPositionX())
			/ (double) (sliderTrack->getBoundsDimensionsX()
					- sliderHandle->getBoundsDimensionsX());
	double val = (1.0 - interp) * minValue.toDouble()
			+ interp * maxValue.toDouble();
	value.setValue(val);
}
void HorizontalSlider::onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (region == sliderTrack.get()) {
			sliderHandle->setDragOffset(event.cursor,
					sliderHandle->getBoundsDimensions() * 0.5f);
			context->setDragObject(sliderHandle.get());
			update();
		} else if (region == sliderHandle.get()) {
			update();
		}
	}
}
void HorizontalSlider::onMouseUp(AlloyContext* context, Region* region,
		const InputEvent& event) {
}
void HorizontalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event, const pixel2& lastCursorLocation) {
	if (region == sliderHandle.get()) {
		region->setDragOffset(event.cursor, lastCursorLocation);
		update();
	}
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
void Button::drawOnTop(AlloyContext* context) {
	if (isDragEnabled() && context->isMouseDrag(this)) {
		internalDraw(context);
	}
}
void Button::draw(AlloyContext* context) {
	if (!isDragEnabled() || !context->isMouseDrag(this)) {
		internalDraw(context);
	}
}
void Button::internalDraw(AlloyContext* context) {
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
}

