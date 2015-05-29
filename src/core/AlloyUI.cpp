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
#include "AlloyUI.h"
#include "nanovg.h"
#include "nanovg_gl.h"
#include "AlloyApplication.h"
namespace aly {
uint64_t Region::REGION_COUNTER = 0;
const RGBA DEBUG_STROKE_COLOR = RGBA(32, 32, 200, 255);
const RGBA DEBUG_HOVER_COLOR = RGBA(32, 200, 32, 255);
const RGBA DEBUG_DOWN_COLOR = RGBA(200, 64,32, 255);
bool Region::isVisible(){
	if(!visible)return false;
	if(parent!=nullptr){
		return parent->isVisible();
	}
	return true;
}
void Region::setPosition(const AUnit2D& pt){
	position=pt;
	Application::getContext()->requestPack();
}
void Region::setDimensions(const AUnit2D& dims){
	dimensions=dims;
	Application::getContext()->requestPack();
}
void Region::setVisible(bool vis){
	visible=vis;
	Application::getContext()->requestUpdateCursor();

}
void Region::draw(AlloyContext* context){
	NVGcontext* nvg = context->nvgContext;
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		pixel lineWidth=borderWidth.toPixels(context->height(), context->dpmm.y,context->pixelRatio);
		nvgRect(nvg, bounds.position.x+lineWidth*0.5f, bounds.position.y+lineWidth*0.5f, bounds.dimensions.x-lineWidth,
				bounds.dimensions.y-lineWidth);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg,lineWidth);
		nvgStroke(nvg);
	}
}
void Region::drawBoundsLabel(AlloyContext* context, const std::string& name,
		int font) {
	if(!context->hasFocus||context->cursorPosition.x<0||context->cursorPosition.y<0)return;
	NVGcontext* nvg = context->nvgContext;
	bool hover = (this == context->mouseOverRegion);
	bool down=(this==context->mouseDownRegion);
	Color c;
	if(down){
		c=DEBUG_DOWN_COLOR;
	} else if(hover){
		c=DEBUG_HOVER_COLOR;
	} else {
		c=DEBUG_STROKE_COLOR;
	}
	const int FONT_PADDING = 2;
	const int FONT_SIZE_PX = 16;

	nvgBeginPath(nvg);
	nvgRect(nvg, bounds.position.x+2, bounds.position.y+2, bounds.dimensions.x-4,
			bounds.dimensions.y-4);
	nvgStrokeColor(nvg, c);
	nvgStrokeWidth(nvg, 4.0f);
	nvgStroke(nvg);

	nvgFontSize(nvg, FONT_SIZE_PX);
	nvgFontFaceId(nvg, font);
	nvgTextAlign(nvg, NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
	float twidth = std::ceil(
			nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr));
	float xoffset = (bounds.dimensions.x - twidth - 2 * FONT_PADDING) * 0.5f;

	nvgBeginPath(nvg);
	nvgFillColor(nvg, c);
	nvgRect(nvg, bounds.position.x + xoffset, bounds.position.y + 1,
			twidth + 2 * FONT_PADDING, FONT_SIZE_PX + FONT_PADDING);
	nvgFill(nvg);

	nvgFillColor(nvg, Color(COLOR_WHITE));
	nvgText(nvg, bounds.position.x + FONT_PADDING + xoffset,
			bounds.position.y + 1 + FONT_PADDING, name.c_str(), nullptr);

}
Region::Region(const std::string& name) :
		position(CoordPX(0, 0)), dimensions(CoordPercent(1, 1)), name(name) {

}
void Region::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	//if (parent != nullptr)nvgScissor(nvg, parent->bounds.position.x, parent->bounds.position.y,parent->bounds.dimensions.x, parent->bounds.dimensions.y);
	drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
}

void Composite::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	if (parent != nullptr){
		box2px pbounds=parent->getBounds();
		nvgScissor(nvg, pbounds.position.x, pbounds.position.y,
				pbounds.dimensions.x, pbounds.dimensions.y);
	}
	if (bgColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
		nvgFillColor(nvg, *bgColor);
		nvgFill(nvg);
	}
	for (std::shared_ptr<Region>& region : children) {
		region->draw(context);
	}
}
void Composite::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	//if (parent != nullptr)nvgScissor(nvg, parent->bounds.position.x, parent->bounds.position.y,parent->bounds.dimensions.x, parent->bounds.dimensions.y);

	drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
	for (std::shared_ptr<Region>& region : children) {
		region->drawDebug(context);
	}
}

void Composite::pack() {
	pack(Application::getContext().get());
}
void Composite::draw() {
	draw(Application::getContext().get());
}
void Composite::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio) {
	Region::pack(pos, dims, dpmm, pixelRatio);
	for (std::shared_ptr<Region>& region : children) {
		region->pack(bounds.position, bounds.dimensions, dpmm, pixelRatio);
	}
}
void Composite::update(CursorLocator* cursorLocator) {
	cursorLocator->add(this);
	for (std::shared_ptr<Region>& region : children) {
		region->update(cursorLocator);
	}
}

void Region::update(CursorLocator* cursorLocator) {
	cursorLocator->add(this);
}

void Region::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio) {
	pixel2 computedPos=position.toPixels(dims, dpmm, pixelRatio);
	pixel2 xy = pos + dragOffset+computedPos;
	pixel2 d = dimensions.toPixels(dims, dpmm, pixelRatio);
	bounds.dimensions = d;
	switch (origin) {
	case Origin::TopLeft:
		bounds.position = xy;
		break;
	case Origin::BottomRight:
		bounds.position = xy - d;
		break;
	case Origin::Center:
		bounds.position = xy - d / (pixel) 2;
		break;
	case Origin::TopRight:
		bounds.position = xy - pixel2(d.x, 0);
		break;
	case Origin::BottomLeft:
		bounds.position = xy - pixel2(0, d.y);
		break;
	}
	d = bounds.dimensions;
	if (aspect < 0) {
		aspect = dims.x / std::max((float) dims.y, 0.0f);
	}
	switch (aspectRatio) {
	case AspectRatio::FixedWidth:
		bounds.dimensions = pixel2(d.x, d.x / aspect);
		break;
	case AspectRatio::FixedHeight:
		bounds.dimensions = pixel2(d.y * aspect, d.y);
		break;
	case AspectRatio::Unspecified:
	default:
		bounds.dimensions = d;
	}
	if (parent != nullptr){
		bounds.clamp(parent->bounds);
		dragOffset=xy-pos-computedPos;
	}
}

void Composite::pack(AlloyContext* context) {
	pack(pixel2(context->viewport.position),
			pixel2(context->viewport.dimensions), context->dpmm,
			context->pixelRatio);
}

Composite& Composite::add(const std::shared_ptr<Region>& region) {
	children.push_back(region);
	if (region->parent != nullptr)
		throw std::runtime_error(
				"Cannot add child node because it already has a parent.");
	region->parent = this;
	return *this;
}
Composite& Composite::add(Region* region) {
	children.push_back(std::shared_ptr<Region>(region));
	return *this;
}

void TextLabel::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	if (parent != nullptr){
		box2px pbounds=parent->getBounds();
		nvgScissor(nvg, pbounds.position.x, pbounds.position.y,
				pbounds.dimensions.x, pbounds.dimensions.y);
	}
	nvgFontSize(nvg,
			fontSize.toPixels(context->height(), context->dpmm.y,
					context->pixelRatio));
	nvgFillColor(nvg, *fontColor);
	nvgFontFaceId(nvg, context->getFontHandle(fontType));
	nvgTextAlign(nvg,
			static_cast<int>(horizontalAlignment)
					| static_cast<int>(verticalAlignment));
	nvgText(nvg, bounds.position.x, bounds.position.y, name.c_str(), nullptr);
}
void GlyphRegion::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	//if (parent != nullptr)nvgScissor(nvg, parent->bounds.position.x, parent->bounds.position.y,parent->bounds.dimensions.x, parent->bounds.dimensions.y);
	drawBoundsLabel(context, name,
			(glyph->type == GlyphType::Awesome) ?
					context->getFontHandle(FontType::Icon) :
					context->getFontHandle(FontType::Bold));
}

void GlyphRegion::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	if (parent != nullptr){
		box2px pbounds=parent->getBounds();
		nvgScissor(nvg, pbounds.position.x, pbounds.position.y,
				pbounds.dimensions.x, pbounds.dimensions.y);
	}
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	if (glyph.get() != nullptr) {
		glyph->draw(bounds, *fontColor, context);
	}
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		pixel lineWidth=borderWidth.toPixels(context->height(), context->dpmm.y,context->pixelRatio);
		nvgRect(nvg, bounds.position.x+lineWidth*0.5f, bounds.position.y+lineWidth*0.5f, bounds.dimensions.x-lineWidth,
				bounds.dimensions.y-lineWidth);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg,lineWidth);
		nvgStroke(nvg);
	}
}

}
;

