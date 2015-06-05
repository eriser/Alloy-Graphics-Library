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
#include "AlloyDrawUtil.h"
namespace aly {
uint64_t Region::REGION_COUNTER = 0;
const RGBA DEBUG_STROKE_COLOR = RGBA(32, 32, 200, 255);
const RGBA DEBUG_HOVER_COLOR = RGBA(32, 200, 32, 255);
const RGBA DEBUG_DOWN_COLOR = RGBA(200, 64, 32, 255);
const float Composite::scrollBarSize=15.0f;
bool Region::isVisible() {
	if (!visible)
		return false;
	if (parent != nullptr) {
		return parent->isVisible();
	}
	return true;
}
void Region::setVisible(bool vis) {
	visible = vis;
	Application::getContext()->requestUpdateCursor();
}
void Region::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
	}
}
void Region::drawBoundsLabel(AlloyContext* context, const std::string& name,
		int font) {

	if(bounds.dimensions.x<=20&&bounds.dimensions.y<=20){
		return;
	}
	NVGcontext* nvg = context->nvgContext;
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	Color c;
	if (down) {
		c = DEBUG_DOWN_COLOR;
	} else if (hover) {
		c = DEBUG_HOVER_COLOR;
	} else {
		c = DEBUG_STROKE_COLOR;
	}
	const int FONT_PADDING = 2;
	const int FONT_SIZE_PX = 16;

	nvgBeginPath(nvg);
	nvgRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
			bounds.dimensions.x - 2, bounds.dimensions.y - 2);
	nvgStrokeColor(nvg, c);
	nvgStrokeWidth(nvg, 1.0f);
	nvgStroke(nvg);
	nvgFontSize(nvg, FONT_SIZE_PX);
	nvgFontFaceId(nvg, font);
	nvgTextAlign(nvg, NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
	float twidth = std::ceil(
			nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr));
	float xoffset = (bounds.dimensions.x - twidth - 2 * FONT_PADDING) * 0.5f;
	if(twidth<=bounds.dimensions.x&&FONT_SIZE_PX<=bounds.dimensions.y){
	nvgBeginPath(nvg);
	nvgFillColor(nvg, c);
	nvgRoundedRect(nvg, bounds.position.x + xoffset, bounds.position.y -4,
			twidth + 2 * FONT_PADDING, FONT_SIZE_PX + FONT_PADDING+5,5);
	nvgFill(nvg);

	nvgFillColor(nvg, Color(COLOR_WHITE));
	nvgText(nvg, bounds.position.x + FONT_PADDING + xoffset,
			bounds.position.y + 1 + FONT_PADDING, name.c_str(), nullptr);
	}
}
void Region::setDragOffset(const pixel2& cursor, const pixel2& delta) {
	pixel2 d = (bounds.position - dragOffset);
	dragOffset = bounds.clamp(cursor - delta, parent->bounds) - d;
	bounds.position = dragOffset + d;
}
Region::Region(const std::string& name) :
		position(CoordPX(0, 0)), dimensions(CoordPercent(1, 1)), name(name) {

}
void Region::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
}
void Composite::drawOnTop(AlloyContext* context) {
	for (std::shared_ptr<Region>& region : children) {
		if(region->isVisible()){
			region->drawOnTop(context);
		}
	}
}
void Composite::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	float x=bounds.position.x;
	float y=bounds.position.y;
	float w=bounds.dimensions.x;
	float h=bounds.dimensions.y;
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, x,y,w,h);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	if(isScrollEnabled()){
		nvgScissor(nvg,bounds.position.x,bounds.position.y,bounds.dimensions.x,bounds.dimensions.y);
	}
	float stackw;
	float u=scrollPosition.y;
	for (std::shared_ptr<Region>& region : children) {
		if(region->isVisible()){
			region->draw(context);
		}
	}

	if(isScrollEnabled()&&verticalScrollTrack.get()!=nullptr){
		float nudge=(verticalScrollExtent>h&&horizontalScrollExtent>w)?-scrollBarSize:0;

		if(verticalScrollExtent>h){
			float scrollh = std::max(verticalScrollTrack->getBoundsDimensionsX(),(h / verticalScrollExtent) * (verticalScrollTrack->getBoundsDimensionsY()));
			verticalScrollTrack->setDimensions(CoordPerPX(0.0f,1.0f,scrollBarSize,nudge));
			verticalScrollHandle->setDimensions(CoordPerPX(1.0f,0.0f,0.0f,scrollh));
			verticalScrollTrack->setVisible(true);
			verticalScrollHandle->setVisible(true);
			verticalScrollTrack->draw(context);
			verticalScrollHandle->draw(context);
		} else {
			verticalScrollTrack->setVisible(false);
			verticalScrollHandle->setVisible(false);
		}
		if(horizontalScrollExtent>w){
			float scrollw = std::max(horizontalScrollTrack->getBoundsDimensionsY(),(w / horizontalScrollExtent) * (horizontalScrollTrack->getBoundsDimensionsX()));
			horizontalScrollTrack->setDimensions(CoordPerPX(1.0f,0.0f,nudge,scrollBarSize));
			horizontalScrollHandle->setDimensions(CoordPerPX(0.0f,1.0f,scrollw,0.0f));
			horizontalScrollTrack->setVisible(true);
			horizontalScrollHandle->setVisible(true);
			horizontalScrollTrack->draw(context);
			horizontalScrollHandle->draw(context);
		} else {
			horizontalScrollTrack->setVisible(false);
			horizontalScrollHandle->setVisible(false);
		}
	}

	if(isScrollEnabled()){
		nvgResetScissor(nvg);
	}
}
void Composite::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;

	drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
	if(isScrollEnabled()){
		nvgScissor(nvg,bounds.position.x,bounds.position.y,bounds.dimensions.x,bounds.dimensions.y);
	}
		for (std::shared_ptr<Region>& region : children) {
			region->drawDebug(context);
		}
		if(verticalScrollTrack.get()){
			verticalScrollTrack->drawDebug(context);
		}
		if(verticalScrollHandle.get()){
			verticalScrollHandle->drawDebug(context);
		}
		if(horizontalScrollTrack.get()){
			horizontalScrollTrack->drawDebug(context);
		}
		if(horizontalScrollHandle.get()){
			horizontalScrollHandle->drawDebug(context);
		}
	if(isScrollEnabled()){
		nvgReset(nvg);
	}
}

void Composite::pack() {
	pack(Application::getContext().get());
}
void Composite::draw() {
	draw(Application::getContext().get());
}
void Composite::pack(const pixel2& pos, const pixel2& dims,const double2& dpmm,
		double pixelRatio) {
	if(verticalScrollTrack.get()==nullptr&&isScrollEnabled()){
		verticalScrollTrack=std::shared_ptr<ScrollTrack>(new ScrollTrack("Vert Track",Orientation::Vertical));
		verticalScrollTrack->setPosition(CoordPercent(1.0f,0.0f));
		verticalScrollTrack->setDimensions(CoordPerPX(0.0,1.0f,scrollBarSize,0.0f));
		verticalScrollTrack->setOrigin(Origin::TopRight);

		verticalScrollHandle=std::shared_ptr<ScrollHandle>(new ScrollHandle("Vert Handle",Orientation::Vertical));
		verticalScrollHandle->setPosition(CoordPX(0.0f,0.0f));
		verticalScrollHandle->setDimensions(CoordPerPX(1.0f,0.0f,0.0f,scrollBarSize));
		verticalScrollHandle->parent=verticalScrollTrack.get();
		verticalScrollHandle->setEnableDrag(true);

		verticalScrollTrack->onMouseDown=[this](AlloyContext* context,const InputEvent& event){
			if(event.button==GLFW_MOUSE_BUTTON_LEFT){
				this->verticalScrollHandle->setDragOffset(event.cursor,this->verticalScrollHandle->getBoundsDimensions() * 0.5f);
				context->setDragObject(verticalScrollHandle.get());
			}
		};

		horizontalScrollTrack=std::shared_ptr<ScrollTrack>(new ScrollTrack("Horiz Track",Orientation::Horizontal));
		horizontalScrollTrack->setPosition(CoordPercent(0.0f,1.0f));
		horizontalScrollTrack->setDimensions(CoordPerPX(1.0,0.0f,0.0f,scrollBarSize));
		horizontalScrollTrack->setOrigin(Origin::BottomLeft);

		horizontalScrollHandle=std::shared_ptr<ScrollHandle>(new ScrollHandle("Horiz Handle",Orientation::Horizontal));
		horizontalScrollHandle->setPosition(CoordPX(0.0f,0.0f));
		horizontalScrollHandle->setDimensions(CoordPerPX(0.0f,1.0f,scrollBarSize,0.0f));
		horizontalScrollHandle->parent=horizontalScrollTrack.get();
		horizontalScrollHandle->setEnableDrag(true);

		horizontalScrollTrack->onMouseDown=[this](AlloyContext* context,const InputEvent& event){
			if(event.button==GLFW_MOUSE_BUTTON_LEFT){
				this->horizontalScrollHandle->setDragOffset(event.cursor,this->horizontalScrollHandle->getBoundsDimensions() * 0.5f);
				context->setDragObject(horizontalScrollHandle.get());
			}
		};
	}
	Region::pack(pos, dims,dpmm, pixelRatio);
	pixel2 offset(0,0);
	verticalScrollExtent=0;
	horizontalScrollExtent=0;

	for (std::shared_ptr<Region>& region : children) {
		if(orientation!=Orientation::Unspecified){
			region->position=CoordPX(offset);
		}

		region->pack(bounds.position,bounds.dimensions,dpmm, pixelRatio);
		if(orientation==Orientation::Vertical){
			offset.y+=CELL_SPACING.y+region->bounds.dimensions.y;
		}
		if(orientation==Orientation::Horizontal){
			offset.x+=CELL_SPACING.x+region->bounds.dimensions.x;
		}

		verticalScrollExtent=std::max(region->getBoundsDimensionsY()+region->getBoundsPositionY()-bounds.position.y,verticalScrollExtent);
		horizontalScrollExtent=std::max(region->getBoundsDimensionsX()+region->getBoundsPositionX()-bounds.position.x,horizontalScrollExtent);

	}
	if(verticalScrollTrack.get()!=nullptr){
		verticalScrollTrack->pack(bounds.position,bounds.dimensions,dpmm, pixelRatio);
		verticalScrollHandle->pack(verticalScrollTrack->getBoundsPosition(),verticalScrollTrack->getBoundsDimensions(),dpmm, pixelRatio);

		horizontalScrollTrack->pack(bounds.position,bounds.dimensions,dpmm, pixelRatio);
		horizontalScrollHandle->pack(horizontalScrollTrack->getBoundsPosition(),horizontalScrollTrack->getBoundsDimensions(),dpmm, pixelRatio);

	}
	for (std::shared_ptr<Region>& region : children) {
		if (region->onPack)region->onPack();
	}

	if (onPack)
		onPack();
}
void ScrollHandle::draw(AlloyContext* context){
	float x=bounds.position.x;
	float y=bounds.position.y;
	float w=bounds.dimensions.x;
	float h=bounds.dimensions.y;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	NVGcontext* nvg=context->nvgContext;
	if(orientation==Orientation::Vertical){
		NVGpaint shadowPaint = nvgBoxGradient(nvg,
				x+ lineWidth- 1,
				y+ lineWidth- 1,
				w-2*lineWidth,
				h-2*lineWidth,
				(w-2-2*lineWidth)/2, 4,
				context->theme.HIGHLIGHT,context->theme.NEUTRAL);
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg,
				x+1 + lineWidth,
				y+1 + lineWidth,
				w-2-2*lineWidth,
				h-2-2*lineWidth, (w-2-2*lineWidth)/2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
	} else if(orientation==Orientation::Horizontal){
		NVGpaint shadowPaint = nvgBoxGradient(nvg,
				x+ lineWidth- 1,
				y+ lineWidth- 1,
				w-2*lineWidth,
				h-2*lineWidth,
				(h-2-2*lineWidth)/2, 4,
				context->theme.HIGHLIGHT,context->theme.NEUTRAL);
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg,
				x+1 + lineWidth,
				y+1 + lineWidth,
				w-2-2*lineWidth,
				h-2-2*lineWidth, (h-2-2*lineWidth)/2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
	}
}

void ScrollTrack::draw(AlloyContext* context){
	float x=bounds.position.x;
	float y=bounds.position.y;
	float w=bounds.dimensions.x;
	float h=bounds.dimensions.y;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	NVGcontext* nvg=context->nvgContext;

	if(orientation==Orientation::Vertical){
		NVGpaint shadowPaint = nvgBoxGradient(nvg,
				x + lineWidth+ 1, //-1
				y+ lineWidth+ 1, //+1
				w-2*lineWidth,
				h-2*lineWidth, (w-2*lineWidth)/2, 4,
				context->theme.SHADOW.toSemiTransparent(32),context->theme.SHADOW.toSemiTransparent(92));
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg,
				x + lineWidth,
				y + lineWidth,
				w-2*lineWidth,
				h-2*lineWidth, (w-2*lineWidth)/2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
	} else if(orientation==Orientation::Horizontal){
		NVGpaint shadowPaint = nvgBoxGradient(nvg,
				x+ lineWidth+ 1, //-1
				y+ lineWidth+ 1, //+1
				w-2*lineWidth,
				h-2*lineWidth, (h-2*lineWidth)/2, 4,
				context->theme.SHADOW.toSemiTransparent(32),context->theme.SHADOW.toSemiTransparent(92));
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg,
				x + lineWidth,
				y + lineWidth,
				w-2*lineWidth,
				h-2*lineWidth, (h-2*lineWidth)/2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
	}
}
Composite::Composite(const std::string& name):Region(name) {

}
void Composite::update(CursorLocator* cursorLocator) {
	cursorLocator->add(this);
	for (std::shared_ptr<Region>& region : children) {
		region->update(cursorLocator);
	}
	if(verticalScrollTrack.get()!=nullptr){
		verticalScrollTrack->update(cursorLocator);
	}
	if(verticalScrollHandle.get()!=nullptr){
		verticalScrollHandle->update(cursorLocator);
	}
	if(horizontalScrollTrack.get()!=nullptr){
		horizontalScrollTrack->update(cursorLocator);
	}
	if(horizontalScrollHandle.get()!=nullptr){
		horizontalScrollHandle->update(cursorLocator);
	}
}

void Region::update(CursorLocator* cursorLocator) {
	cursorLocator->add(this);
}
void Region::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio) {
	pixel2 computedPos = position.toPixels(dims, dpmm, pixelRatio);
	pixel2 xy = pos + dragOffset + computedPos;
	pixel2 d = dimensions.toPixels(dims, dpmm, pixelRatio);
	if (aspectRatio < 0) {
		aspectRatio = d.x / std::max((float) d.y, 0.0f);
	}
	switch (aspectRule) {
		case AspectRule::FixedWidth:
			bounds.dimensions = pixel2(d.x, d.x / aspectRatio);
			break;
		case AspectRule::FixedHeight:
			bounds.dimensions = pixel2(d.y * aspectRatio, d.y);
			break;
		case AspectRule::Unspecified:
		default:
			bounds.dimensions = d;
	}
	if (parent != nullptr&&!parent->isScrollEnabled()) {
		bounds.dimensions=aly::clamp(bounds.dimensions,pixel2(0,0),parent->bounds.dimensions);
	}
	switch (origin) {
	case Origin::TopLeft:
		bounds.position = xy;
		break;
	case Origin::BottomRight:
		bounds.position = xy - bounds.dimensions;
		break;
	case Origin::Center:
		bounds.position = xy - bounds.dimensions / (pixel) 2;
		break;
	case Origin::TopRight:
		bounds.position = xy - pixel2(bounds.dimensions.x, 0);
		break;
	case Origin::BottomLeft:
		bounds.position = xy - pixel2(0, bounds.dimensions.y);
		break;
	case Origin::CenterLeft:
		bounds.position = xy - pixel2(0, bounds.dimensions.y / (pixel) 2);
		break;
	case Origin::CenterRight:
		bounds.position = xy - pixel2(bounds.dimensions.x, bounds.dimensions.y / (pixel) 2);
		break;
	case Origin::CenterTop:
		bounds.position = xy - pixel2(bounds.dimensions.x / (pixel) 2, 0);
		break;
	case Origin::CenterBottom:
		bounds.position = xy - pixel2(bounds.dimensions.x / (pixel) 2, bounds.dimensions.y);
		break;
	}
	if (parent != nullptr&&!parent->isScrollEnabled()) {
		bounds.position = aly::clamp(bounds.position, parent->bounds.position,
				 parent->bounds.position +  parent->bounds.dimensions - bounds.dimensions);
	}
	dragOffset = xy - pos - computedPos;
}

void Composite::pack(AlloyContext* context) {

	pack(pixel2(context->viewport.position),
			pixel2(context->viewport.dimensions),context->dpmm, context->pixelRatio);
}

void Composite::add(const std::shared_ptr<Region>& region) {
	children.push_back(region);
	if (region->parent != nullptr)
		throw std::runtime_error(
				"Cannot add child node because it already has a parent.");
	region->parent = this;
}
void Composite::add(Region* region) {
	add(std::shared_ptr<Region>(region));
}

void TextLabel::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;

	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(fontType));
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	nvgTextAlign(nvg,
			static_cast<int>(horizontalAlignment)
					| static_cast<int>(verticalAlignment));
	pixel2 offset(0, 0);
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	switch (horizontalAlignment) {
	case HorizontalAlignment::Left:
		offset.x = lineWidth;
		break;
	case HorizontalAlignment::Center:
		offset.x = bounds.dimensions.x / 2;
		break;
	case HorizontalAlignment::Right:
		offset.x = bounds.dimensions.x - lineWidth;
		break;
	}
	switch (verticalAlignment) {
	case VerticalAlignment::Top:
		offset.y = lineWidth;
		break;
	case VerticalAlignment::Middle:
		offset.y = bounds.dimensions.y / 2;
		break;
	case VerticalAlignment::Bottom:
	case VerticalAlignment::Baseline:
		offset.y = bounds.dimensions.y - lineWidth;
		break;
	}
	drawText(nvg,bounds.position+offset,label,fontStyle,*textColor,*textAltColor);
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
	}
}
void GlyphRegion::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	drawBoundsLabel(context, name,
			(glyph->type == GlyphType::Awesome) ?
					context->getFontHandle(FontType::Icon) :
					context->getFontHandle(FontType::Bold));
}

void GlyphRegion::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	if (glyph.get() != nullptr) {
		glyph->draw(bounds, *foregroundColor, context);
	}

	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
	}
}
std::shared_ptr<Composite> MakeComposite(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions, const RGBA& bgColor,
		const Orientation& orientation) {
	std::shared_ptr<Composite> composite = std::shared_ptr<Composite>(
			new Composite(name));
	composite->setPosition(position);
	composite->setDimensions(dimensions);
	composite->backgroundColor = MakeColor(bgColor);
	composite->setOrientation(orientation);
	return composite;
}

std::shared_ptr<GlyphRegion> MakeGlyphRegion(
		const std::shared_ptr<AwesomeGlyph>& glyph, const AUnit2D& position,
		const AUnit2D& dimensions, const RGBA& bgColor, const RGBA& fgColor,
		const RGBA& borderColor, const AUnit1D& borderWidth) {
	std::shared_ptr<GlyphRegion> region = std::shared_ptr<GlyphRegion>(
			new GlyphRegion(glyph->name));
	region->glyph = glyph;
	region->setPosition(position);
	region->setDimensions(dimensions);
	region->backgroundColor = MakeColor(bgColor);
	region->foregroundColor = MakeColor(fgColor);
	region->borderColor = MakeColor(borderColor);
	region->borderWidth = borderWidth;
	region->setAspectRule(AspectRule::FixedHeight);
	region->setAspectRatio(glyph->width / (float) glyph->height);
	return region;
}
std::shared_ptr<GlyphRegion> MakeGlyphRegion(
		const std::shared_ptr<ImageGlyph>& glyph, const AUnit2D& position,
		const AUnit2D& dimensions, const AspectRule& aspectRatio,
		const RGBA& bgColor, const RGBA& fgColor, const RGBA& borderColor,
		const AUnit1D& borderWidth) {
	std::shared_ptr<GlyphRegion> region = std::shared_ptr<GlyphRegion>(
			(glyph->name.length() > 0) ?
					new GlyphRegion(glyph->name) : new GlyphRegion());
	region->glyph = glyph;
	region->setPosition(position);
	region->setDimensions(dimensions);
	region->backgroundColor = MakeColor(bgColor);
	region->foregroundColor = MakeColor(fgColor);
	region->borderColor = MakeColor(borderColor);
	region->borderWidth = borderWidth;
	region->setAspectRule(aspectRatio);
	region->setAspectRatio(glyph->width / (float) glyph->height);
	return region;
}
std::shared_ptr<TextLabel> MakeTextLabel(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const FontType& fontType, const AUnit1D& fontSize,
		const RGBA& fontColor, const HorizontalAlignment& halign,
		const VerticalAlignment& valign) {
	std::shared_ptr<TextLabel> region = std::shared_ptr<TextLabel>(
			new TextLabel(name));
	region->setPosition(position);
	region->setDimensions(dimensions);
	region->textColor = MakeColor(fontColor);
	region->fontType = fontType;
	region->fontSize = fontSize;
	region->horizontalAlignment = halign;
	region->verticalAlignment = valign;
	return region;
}
std::shared_ptr<Region> MakeRegion(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions, const RGBA& bgColor,
		const RGBA& borderColor, const AUnit1D& borderWidth) {
	std::shared_ptr<Region> region = std::shared_ptr<Region>(new Region(name));
	region->setPosition(position);
	region->setDimensions(dimensions);
	region->backgroundColor = MakeColor(bgColor);
	region->borderColor = MakeColor(borderColor);
	region->borderWidth = borderWidth;
	return region;
}
}
;

