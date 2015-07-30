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
#include "AlloyFileUtil.h"
namespace aly {
uint64_t Region::REGION_COUNTER = 0;
const RGBA DEBUG_STROKE_COLOR = RGBA(32, 32, 200, 255);
const RGBA DEBUG_HIDDEN_COLOR = RGBA(128, 128, 128, 32);
const RGBA DEBUG_HOVER_COLOR = RGBA(32, 200, 32, 255);
const RGBA DEBUG_DOWN_COLOR = RGBA(200, 64, 32, 255);
const RGBA DEBUG_ON_TOP_COLOR = RGBA(120, 120, 0, 255);
const RGBA DEBUG_ON_TOP_DOWN_COLOR = RGBA(220, 220, 0, 255);
const RGBA DEBUG_ON_TOP_HOVER_COLOR = RGBA(180, 180, 0, 255);
const float Composite::scrollBarSize = 15.0f;
const float TextField::PADDING = 2;
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
	AlloyApplicationContext()->requestUpdateCursor();
}
bool Region::onEventHandler(AlloyContext* context, const InputEvent& event) {
	if (isVisible() && onEvent)
		return onEvent(context, event);
	else
		return false;
}
Region* Region::locate(const pixel2& cursor) {
	if (isVisible() && getCursorBounds().contains(cursor)) {
		return this;
	} else {
		return nullptr;
	}
}

void Region::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);

	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		if(roundCorners){
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
		} else {
			nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth);
		}
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}

	if (borderColor->a > 0) {

		nvgLineJoin(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		if(roundCorners){
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
		} else {
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		}
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
		nvgLineJoin(nvg, NVG_MITER);
	}
}
void Region::drawBoundsLabel(AlloyContext* context, const std::string& name,
		int font) {
	bool ontop = context->isOnTop(this);
	box2px bounds = getCursorBounds();
	if ((bounds.dimensions.x <= 20 && bounds.dimensions.y <= 20)
			|| bounds.dimensions.x * bounds.dimensions.y == 0) {
		return;
	}

	NVGcontext* nvg = context->nvgContext;
	pushScissor(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
			bounds.dimensions.y);
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);

	Color c;
	if (isVisible()) {
		if (down) {
			if (ontop) {
				c = DEBUG_ON_TOP_DOWN_COLOR;
			} else {
				c = DEBUG_DOWN_COLOR;
			}
		} else if (hover) {
			if (ontop) {
				c = DEBUG_ON_TOP_HOVER_COLOR;
			} else {
				c = DEBUG_HOVER_COLOR;
			}
		} else if (ontop) {
			c = DEBUG_ON_TOP_COLOR;
		} else {
			c = DEBUG_STROKE_COLOR;
		}
	} else {
		c = DEBUG_HIDDEN_COLOR;
	}
	const int FONT_PADDING = 2;
	const int FONT_SIZE_PX = 16;

	nvgBeginPath(nvg);

	nvgLineJoin(nvg, NVG_ROUND);
	nvgRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
			bounds.dimensions.x - 2, bounds.dimensions.y - 2);
	nvgStrokeColor(nvg, c);
	nvgStrokeWidth(nvg, 1.0f);
	nvgStroke(nvg);

	nvgLineJoin(nvg, NVG_MITER);
	nvgFontSize(nvg, (float)FONT_SIZE_PX);
	nvgFontFaceId(nvg, font);
	nvgTextAlign(nvg, NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
	float twidth = std::ceil(
			nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr));
	float xoffset = (bounds.dimensions.x - twidth - 2 * FONT_PADDING) * 0.5f;
	if (twidth <= bounds.dimensions.x && FONT_SIZE_PX <= bounds.dimensions.y) {
		nvgBeginPath(nvg);
		nvgFillColor(nvg, c);
		nvgRoundedRect(nvg, bounds.position.x + xoffset, bounds.position.y - 4,
				twidth + 2 * FONT_PADDING, FONT_SIZE_PX + FONT_PADDING + 5, 5);
		nvgFill(nvg);

		nvgFillColor(nvg, Color(COLOR_WHITE));
		nvgText(nvg, bounds.position.x + FONT_PADDING + xoffset,
				bounds.position.y + 1 + FONT_PADDING, name.c_str(), nullptr);
	}
	popScissor(nvg);
}
void Region::setDragOffset(const pixel2& cursor, const pixel2& delta) {
	pixel2 d = (bounds.position - dragOffset);
	dragOffset = bounds.clamp(cursor - delta, parent->bounds) - d;
	bounds.position = dragOffset + d;
}
void Region::addDragOffset(const pixel2& delta) {
	pixel2 d = (bounds.position - dragOffset);
	dragOffset = bounds.clamp(bounds.position + delta, parent->bounds) - d;
	bounds.position = dragOffset + d;
}
Region::Region(const std::string& name) :
		position(CoordPX(0, 0)), dimensions(CoordPercent(1, 1)), name(name) {
}
Region::Region(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
		position(pos), dimensions(dims), name(name) {

}
void Region::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
}
box2px Region::getBounds(bool includeOffset) const {
	box2px box = bounds;
	if (parent != nullptr&&includeOffset) {
		box.position += parent->drawOffset();
	}
	return box;
}
box2px Region::getCursorBounds(bool includeOffset) const {
	box2px box = (isDetached()?getBounds():bounds);
	if (parent != nullptr&&includeOffset) {
		box.position += parent->drawOffset();
		if (AlloyApplicationContext()->getOnTopRegion() != this) {
			box.intersect(parent->getCursorBounds());
		}
	}
	return box;
}
Region* Composite::locate(const pixel2& cursor) {
	if (isVisible()) {
		for (auto iter = children.rbegin(); iter != children.rend(); iter++) {
			Region* r = (*iter)->locate(cursor);
			if (r != nullptr)
				return r;
		}
		if (getCursorBounds().contains(cursor)) {
			return this;
		}
	}
	return nullptr;
}

void Composite::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (backgroundColor->a > 0) {
			nvgBeginPath(nvg);
			if(roundCorners){
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
						bounds.position.y + lineWidth * 0.5f,
						bounds.dimensions.x - lineWidth,
						bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
			} else {
				nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
						bounds.position.y + lineWidth * 0.5f,
						bounds.dimensions.x - lineWidth,
						bounds.dimensions.y - lineWidth);
			}
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}
	if (isScrollEnabled()) {
		pushScissor(nvg, bounds.position.x, bounds.position.y,
				bounds.dimensions.x, bounds.dimensions.y);
	}
	for (std::shared_ptr<Region>& region : children) {
		if (region->isVisible()) {
			region->draw(context);
		}
	}
	if (borderColor->a > 0) {

		nvgLineJoin(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		if(roundCorners){
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
		} else {
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		}
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
		nvgLineJoin(nvg, NVG_MITER);
	}
	if (isScrollEnabled() && verticalScrollTrack.get() != nullptr) {

		if (scrollExtent.y > h) {

			verticalScrollTrack->setVisible(true);
			verticalScrollHandle->setVisible(true);
			verticalScrollTrack->draw(context);
			verticalScrollHandle->draw(context);
		} else {
			verticalScrollTrack->setVisible(false);
			verticalScrollHandle->setVisible(false);
		}
		if (scrollExtent.x > w) {
			horizontalScrollTrack->setVisible(true);
			horizontalScrollHandle->setVisible(true);
			horizontalScrollTrack->draw(context);
			horizontalScrollHandle->draw(context);
		} else {
			horizontalScrollTrack->setVisible(false);
			horizontalScrollHandle->setVisible(false);
		}
	}

	if (isScrollEnabled()) {
		popScissor(nvg);
	}
}
void Composite::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
	for (std::shared_ptr<Region>& region : children) {
		region->drawDebug(context);
	}
	if (verticalScrollTrack.get()) {
		verticalScrollTrack->drawDebug(context);
	}
	if (verticalScrollHandle.get()) {
		verticalScrollHandle->drawDebug(context);
	}
	if (horizontalScrollTrack.get()) {
		horizontalScrollTrack->drawDebug(context);
	}
	if (horizontalScrollHandle.get()) {
		horizontalScrollHandle->drawDebug(context);
	}
}

void Composite::pack() {
	pack(AlloyApplicationContext().get());
}
void Composite::draw() {
	draw(AlloyApplicationContext().get());
}

void Composite::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
	Region::pack(pos, dims, dpmm, pixelRatio);
	box2px bounds = getBounds(false);
	
	if (verticalScrollTrack.get() == nullptr && isScrollEnabled()) {
		verticalScrollTrack = std::shared_ptr<ScrollTrack>(
				new ScrollTrack("Vert Track", Orientation::Vertical));
		verticalScrollTrack->setPosition(CoordPercent(1.0f, 0.0f));
		verticalScrollTrack->setDimensions(
				CoordPerPX(0.0, 1.0f, scrollBarSize, 0.0f));
		verticalScrollTrack->setOrigin(Origin::TopRight);

		verticalScrollHandle = std::shared_ptr<ScrollHandle>(
				new ScrollHandle("Vert Handle", Orientation::Vertical));
		verticalScrollHandle->setPosition(CoordPX(0.0f, 0.0f));
		verticalScrollHandle->setDimensions(
				CoordPerPX(1.0f, 0.0f, 0.0f, scrollBarSize));
		verticalScrollHandle->parent = verticalScrollTrack.get();
		verticalScrollHandle->setEnableDrag(true);

		verticalScrollTrack->onMouseDown =
				[this](AlloyContext* context,const InputEvent& event) {
					if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
						this->verticalScrollHandle->setDragOffset(event.cursor,this->verticalScrollHandle->getBoundsDimensions() * 0.5f);
						context->setDragObject(verticalScrollHandle.get());
						this->scrollPosition.y=(this->verticalScrollHandle->getBounds().position.y-this->verticalScrollTrack->getBounds().position.y)/
						std::max(1.0f,(float)this->verticalScrollTrack->getBounds().dimensions.y-(float)this->verticalScrollHandle->getBounds().dimensions.y);
						context->requestPack();
						return true;
					}
					return false;
				};
		verticalScrollHandle->onMouseDrag =
				[this](AlloyContext* context,const InputEvent& event) {
					if(context->isLeftMouseButtonDown()) {
						this->verticalScrollHandle->setDragOffset(event.cursor,context->getCursorDownPosition());
						this->scrollPosition.y=(this->verticalScrollHandle->getBounds().position.y-this->verticalScrollTrack->getBounds().position.y)/
						std::max(1.0f,(float)this->verticalScrollTrack->getBounds().dimensions.y-(float)this->verticalScrollHandle->getBounds().dimensions.y);
						return true;
					}
					return false;
				};
		horizontalScrollTrack = std::shared_ptr<ScrollTrack>(
				new ScrollTrack("Horiz Track", Orientation::Horizontal));
		horizontalScrollTrack->setPosition(CoordPercent(0.0f, 1.0f));
		horizontalScrollTrack->setDimensions(
				CoordPerPX(1.0, 0.0f, 0.0f, scrollBarSize));
		horizontalScrollTrack->setOrigin(Origin::BottomLeft);

		horizontalScrollHandle = std::shared_ptr<ScrollHandle>(
				new ScrollHandle("Horiz Handle", Orientation::Horizontal));
		horizontalScrollHandle->setPosition(CoordPX(0.0f, 0.0f));
		horizontalScrollHandle->setDimensions(
				CoordPerPX(0.0f, 1.0f, scrollBarSize, 0.0f));
		horizontalScrollHandle->parent = horizontalScrollTrack.get();
		horizontalScrollHandle->setEnableDrag(true);

		horizontalScrollTrack->onMouseDown =
				[this](AlloyContext* context,const InputEvent& event) {
					if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
						this->horizontalScrollHandle->setDragOffset(event.cursor,this->horizontalScrollHandle->getBoundsDimensions() * 0.5f);
						context->setDragObject(horizontalScrollHandle.get());
						this->scrollPosition.x=(this->horizontalScrollHandle->getBounds().position.x-this->horizontalScrollTrack->getBounds().position.x)/
						std::max(1.0f,(float)this->horizontalScrollTrack->getBounds().dimensions.x-(float)this->horizontalScrollHandle->getBounds().dimensions.x);
						context->requestPack();
						return true;
					}
					return false;
				};
		horizontalScrollHandle->onMouseDrag =
				[this](AlloyContext* context,const InputEvent& event) {
					if(context->isLeftMouseButtonDown()) {
						this->horizontalScrollHandle->setDragOffset(event.cursor,context->getCursorDownPosition());
						this->scrollPosition.x=(this->horizontalScrollHandle->getBounds().position.x-this->horizontalScrollTrack->getBounds().position.x)/
						std::max(1.0f,(float)this->horizontalScrollTrack->getBounds().dimensions.x-(float)this->horizontalScrollHandle->getBounds().dimensions.x);
						return true;
					}
					return false;
				};
		Application::addListener(this);
	}
	pixel2 offset(0, 0);
	scrollExtent = pixel2(0, 0);
	pixel2 CELL_SPACING(AlloyApplicationContext()->theme.SPACING);
	for (std::shared_ptr<Region>& region : children) {
		if (!region->isVisible()) {
			continue;
		}
		if (orientation == Orientation::Vertical) {
			pixel2 pix = region->position.toPixels(bounds.dimensions, dpmm, pixelRatio);
			region->setPosition(CoordPX(pix.x,offset.y));
		}
		if (orientation == Orientation::Horizontal) {
			pixel2 pix = region->position.toPixels(bounds.dimensions, dpmm, pixelRatio);
			region->setPosition(CoordPX(offset.x,pix.y));
		}
		region->pack(bounds.position, bounds.dimensions, dpmm, pixelRatio);
			if (orientation == Orientation::Horizontal) {
				offset.x += CELL_SPACING.x + region->getBoundsDimensionsX();

			}
			if (orientation == Orientation::Vertical) {
				offset.y += CELL_SPACING.y + region->getBoundsDimensionsY();
			}
		scrollExtent = max(
			region->getBoundsDimensions() + region->getBoundsPosition()
			- bounds.position - region->drawOffset(), scrollExtent);
	}
	if(!isScrollEnabled()){
		if (orientation == Orientation::Horizontal)this->bounds.dimensions.x=bounds.dimensions.x=std::max(bounds.dimensions.x,offset.x-CELL_SPACING.x);
		if (orientation == Orientation::Vertical)this->bounds.dimensions.y=bounds.dimensions.y=std::max(bounds.dimensions.y,offset.y-CELL_SPACING.y);
	}
	if (verticalScrollTrack.get() != nullptr) {
		float nudge =
				(scrollExtent.y > bounds.dimensions.y
						&& scrollExtent.x > bounds.dimensions.x) ?
						-scrollBarSize : 0;

		verticalScrollTrack->setDimensions(
				CoordPerPX(0.0f, 1.0f, scrollBarSize, nudge));
		verticalScrollHandle->setDimensions(
				CoordPerPX(1.0f, 0.0f, 0.0f,
						std::max(scrollBarSize,
								(verticalScrollTrack->getBoundsDimensionsY()
										* bounds.dimensions.y)
										/ scrollExtent.y)));

		verticalScrollTrack->pack(bounds.position, bounds.dimensions, dpmm,
				pixelRatio);
		verticalScrollHandle->pack(verticalScrollTrack->getBoundsPosition(),
				verticalScrollTrack->getBoundsDimensions(), dpmm, pixelRatio,
				true);

		horizontalScrollTrack->setDimensions(
				CoordPerPX(1.0f, 0.0f, nudge, scrollBarSize));
		horizontalScrollHandle->setDimensions(
				CoordPerPX(0.0f, 1.0f,
						std::max(scrollBarSize,
								(horizontalScrollTrack->getBoundsDimensionsX()
										* bounds.dimensions.x)
										/ scrollExtent.x), 0.0f));

		horizontalScrollTrack->pack(bounds.position, bounds.dimensions, dpmm,
				pixelRatio);
		horizontalScrollHandle->pack(horizontalScrollTrack->getBoundsPosition(),
				horizontalScrollTrack->getBoundsDimensions(), dpmm, pixelRatio,
				true);

	}
	for (std::shared_ptr<Region>& region : children) {
		if (region->onPack)
			region->onPack();
	}

	if (onPack)
		onPack();
}


Region* BorderComposite::locate(const pixel2& cursor) {
	if (isVisible()) {
		for (auto iter = children.rbegin(); iter != children.rend(); iter++) {
			if(iter->get()==nullptr)continue;
			Region* r = (*iter)->locate(cursor);
			if (r != nullptr)
				return r;
		}
		if (getCursorBounds().contains(cursor)) {
			return this;
		}
	}
	return nullptr;
}
void BorderComposite::pack(AlloyContext* context) {
	if (parent == nullptr) {
		pack(pixel2(0,0),
				pixel2(context->dimensions()), context->dpmm,
				context->pixelRatio);
	} else {
		pack(parent->getBoundsPosition(), parent->getBoundsDimensions(),
				context->dpmm, context->pixelRatio);
	}
}
void BorderComposite::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (backgroundColor->a > 0) {
			nvgBeginPath(nvg);
			if(roundCorners){
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
						bounds.position.y + lineWidth * 0.5f,
						bounds.dimensions.x - lineWidth,
						bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
			} else {
				nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
						bounds.position.y + lineWidth * 0.5f,
						bounds.dimensions.x - lineWidth,
						bounds.dimensions.y - lineWidth);
			}
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}
	if (isScrollEnabled()) {
		pushScissor(nvg, bounds.position.x, bounds.position.y,
				bounds.dimensions.x, bounds.dimensions.y);
	}
	for (std::shared_ptr<Region>& region : children) {
		if(region.get()==nullptr)continue;
		if (region->isVisible()) {
			region->draw(context);
		}
	}
	if (borderColor->a > 0) {

		nvgLineJoin(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		if(roundCorners){
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
		} else {
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		}
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
		nvgLineJoin(nvg, NVG_MITER);
	}

	if (isScrollEnabled()) {
		popScissor(nvg);
	}
}
BorderComposite::BorderComposite(const std::string& name) :
		Region(name),
		northRegion(children[0]),
		southRegion(children[1]),
		eastRegion(children[2]),
		westRegion(children[3]),
		centerRegion(children[4]){

}
BorderComposite::BorderComposite(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Region(name, pos, dims),
		northRegion(children[0]),
		southRegion(children[1]),
		eastRegion(children[2]),
		westRegion(children[3]),
		centerRegion(children[4]) {
}
void BorderComposite::update(CursorLocator* cursorLocator) {
	if(!ignoreCursorEvents)cursorLocator->add(this);
	for (std::shared_ptr<Region>& region : children) {
		if(region.get()==nullptr)continue;
		region->update(cursorLocator);
	}
}

void BorderComposite::drawDebug(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
	for (std::shared_ptr<Region>& region : children) {
		if(region.get()==nullptr)continue;
		region->drawDebug(context);
	}
}

void BorderComposite::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
	Region::pack(pos, dims, dpmm, pixelRatio);
	box2px bounds = getBounds(false);

	if(northRegion.get()!=nullptr)northRegion->pack(bounds.position, bounds.dimensions*float2(1.0f,northFraction), dpmm, pixelRatio);
	if(southRegion.get()!=nullptr)southRegion->pack(bounds.position+bounds.dimensions*float2(0.0f,1.0f-southFraction), bounds.dimensions*float2(1.0f,southFraction), dpmm, pixelRatio);
	if(westRegion.get()!=nullptr)westRegion->pack(bounds.position+bounds.dimensions*float2(0.0f,northFraction), bounds.dimensions*float2(westFraction,1.0f-northFraction-southFraction), dpmm, pixelRatio);
	if(eastRegion.get()!=nullptr)eastRegion->pack(bounds.position+bounds.dimensions*float2(1.0f-eastFraction,northFraction), bounds.dimensions*float2(eastFraction,1.0f-northFraction-southFraction), dpmm, pixelRatio);
	if(centerRegion.get()!=nullptr)centerRegion->pack(bounds.position+bounds.dimensions*float2(westFraction,northFraction), bounds.dimensions*float2(1.0f-eastFraction-westFraction,1.0f-northFraction-southFraction), dpmm, pixelRatio);

	for (std::shared_ptr<Region>& region : children) {
		if(region.get()==nullptr)continue;
		if (region->onPack)region->onPack();
	}
	if (onPack)
		onPack();
}
void BorderComposite::pack() {
	pack(AlloyApplicationContext().get());
}
void BorderComposite::draw() {
	draw(AlloyApplicationContext().get());
}

void ScrollHandle::draw(AlloyContext* context) {
	box2px bounds = getBounds();
	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	NVGcontext* nvg = context->nvgContext;
	if (orientation == Orientation::Vertical) {
		NVGpaint shadowPaint = nvgBoxGradient(nvg, x + lineWidth - 1,
				y + lineWidth - 1, w - 2 * lineWidth, h - 2 * lineWidth,
				(w - 2 - 2 * lineWidth) / 2, 4, context->theme.HIGHLIGHT,
				context->theme.NEUTRAL);
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, x + 1 + lineWidth, y + 1 + lineWidth,
				w - 2 - 2 * lineWidth, h - 2 - 2 * lineWidth,
				(w - 2 - 2 * lineWidth) / 2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
	} else if (orientation == Orientation::Horizontal) {
		NVGpaint shadowPaint = nvgBoxGradient(nvg, x + lineWidth - 1,
				y + lineWidth - 1, w - 2 * lineWidth, h - 2 * lineWidth,
				(h - 2 - 2 * lineWidth) / 2, 4, context->theme.HIGHLIGHT,
				context->theme.NEUTRAL);
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, x + 1 + lineWidth, y + 1 + lineWidth,
				w - 2 - 2 * lineWidth, h - 2 - 2 * lineWidth,
				(h - 2 - 2 * lineWidth) / 2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
	}
}

void ScrollTrack::draw(AlloyContext* context) {
	box2px bounds = getBounds();
	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	NVGcontext* nvg = context->nvgContext;

	if (orientation == Orientation::Vertical) {
		NVGpaint shadowPaint = nvgBoxGradient(nvg, x + lineWidth + 1, //-1
		y + lineWidth + 1, //+1
		w - 2 * lineWidth, h - 2 * lineWidth, (w - 2 * lineWidth) / 2, 4,
				context->theme.SHADOW.toSemiTransparent(32),
				context->theme.SHADOW.toSemiTransparent(92));
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, x + lineWidth, y + lineWidth, w - 2 * lineWidth,
				h - 2 * lineWidth, (w - 2 * lineWidth) / 2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
	} else if (orientation == Orientation::Horizontal) {
		NVGpaint shadowPaint = nvgBoxGradient(nvg, x + lineWidth + 1, //-1
		y + lineWidth + 1, //+1
		w - 2 * lineWidth, h - 2 * lineWidth, (h - 2 * lineWidth) / 2, 4,
				context->theme.SHADOW.toSemiTransparent(32),
				context->theme.SHADOW.toSemiTransparent(92));
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, x + lineWidth, y + lineWidth, w - 2 * lineWidth,
				h - 2 * lineWidth, (h - 2 * lineWidth) / 2);
		nvgFillPaint(nvg, shadowPaint);
		nvgFill(nvg);
	}
}
Composite::Composite(const std::string& name) :
		Region(name) {

}
Composite::Composite(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Region(name, pos, dims) {
}
bool Composite::onEventHandler(AlloyContext* context, const InputEvent& event) {
	if (isVisible() && event.type == InputType::Scroll && isScrollEnabled()) {
		box2px bounds = getBounds();
		if (bounds.contains(event.cursor)) {
			if (event.scroll.y != 0&&verticalScrollHandle.get()!=nullptr&&verticalScrollHandle->isVisible()) {
				verticalScrollHandle->addDragOffset(
						pixel2(-10.0f * event.scroll));
				this->scrollPosition.y =
						(this->verticalScrollHandle->getBounds().position.y
								- this->verticalScrollTrack->getBounds().position.y)
								/ std::max(1.0f,
										(float) this->verticalScrollTrack->getBounds().dimensions.y
												- (float) this->verticalScrollHandle->getBounds().dimensions.y);
				return true;
			}
			if (event.scroll.x != 0&&horizontalScrollHandle.get()!=nullptr&&horizontalScrollHandle->isVisible()) {
				horizontalScrollHandle->addDragOffset(
						pixel2(-10.0f * event.scroll));
				this->scrollPosition.x =
						(this->horizontalScrollHandle->getBounds().position.x
								- this->horizontalScrollTrack->getBounds().position.x)
								/ std::max(1.0f,
										(float) this->horizontalScrollTrack->getBounds().dimensions.x
												- (float) this->horizontalScrollHandle->getBounds().dimensions.x);

				return true;
			}
		}
	}
	return Region::onEventHandler(context, event);
}
void Composite::update(CursorLocator* cursorLocator) {
	if(!ignoreCursorEvents)cursorLocator->add(this);
	for (std::shared_ptr<Region>& region : children) {
		region->update(cursorLocator);
	}
	if (verticalScrollTrack.get() != nullptr) {
		verticalScrollTrack->update(cursorLocator);
	}
	if (verticalScrollHandle.get() != nullptr) {
		verticalScrollHandle->update(cursorLocator);
	}
	if (horizontalScrollTrack.get() != nullptr) {
		horizontalScrollTrack->update(cursorLocator);
	}
	if (horizontalScrollHandle.get() != nullptr) {
		horizontalScrollHandle->update(cursorLocator);
	}
}

void Region::update(CursorLocator* cursorLocator) {
	if(!ignoreCursorEvents)cursorLocator->add(this);
}
void Region::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {

	pixel2 computedPos = position.toPixels(dims, dpmm, pixelRatio);
	pixel2 xy = pos + dragOffset + computedPos;
	
	pixel2 d = dimensions.toPixels(dims, dpmm, pixelRatio);
	if (aspectRatio < 0) {
		aspectRatio = d.x / std::max((float) d.y, 0.0f);
	}
	switch (aspectRule) {
	case AspectRule::FixedWidth:
		bounds.dimensions = pixel2(d.x, d.x / (float)aspectRatio);
		break;
	case AspectRule::FixedHeight:
		bounds.dimensions = pixel2(d.y * (float)aspectRatio, d.y);
		break;
	case AspectRule::Unspecified:
	default:
		bounds.dimensions = d;
	}
	bounds.position = xy;
	
	if (clamp && parent != nullptr) {
		bounds.dimensions = aly::clamp(bounds.dimensions, pixel2(0, 0),parent->bounds.dimensions);
	}
	
	switch (origin) {
	case Origin::TopLeft:
		bounds.position = xy;
		break;
	case Origin::BottomRight:
		bounds.position = xy - bounds.dimensions;
		break;
	case Origin::MiddleCenter:
		bounds.position = xy - bounds.dimensions / (pixel) 2;
		break;
	case Origin::TopRight:
		bounds.position = xy - pixel2(bounds.dimensions.x, 0);
		break;
	case Origin::BottomLeft:
		bounds.position = xy - pixel2(0, bounds.dimensions.y);
		break;
	case Origin::MiddleLeft:
		bounds.position = xy - pixel2(0, bounds.dimensions.y / (pixel) 2);
		break;
	case Origin::MiddleRight:
		bounds.position = xy
				- pixel2(bounds.dimensions.x, bounds.dimensions.y / (pixel) 2);
		break;
	case Origin::TopCenter:
		bounds.position = xy - pixel2(bounds.dimensions.x / (pixel) 2, 0);
		break;
	case Origin::BottomCenter:
		bounds.position = xy
				- pixel2(bounds.dimensions.x / (pixel) 2, bounds.dimensions.y);
		break;
	}
	
	if (clamp && parent != nullptr && !parent->isScrollEnabled()) {
		pixel2 ppos = parent->getBoundsPosition();
		pixel2 dims = parent->bounds.dimensions;
		bounds.position = aly::clamp(bounds.position, ppos,
				ppos + dims - bounds.dimensions);
	}
	dragOffset = xy - pos - computedPos;
	if (detached) {
		box2px vp = AlloyApplicationContext()->getViewport();
		bounds.position=aly::clamp(bounds.position,vp.position,vp.position+vp.dimensions-bounds.dimensions);
	}
}

void Composite::pack(AlloyContext* context) {
	if (parent == nullptr) {
		pack(pixel2(0,0),
				pixel2(context->dimensions()), context->dpmm,
				context->pixelRatio);
	} else {
		pack(parent->getBoundsPosition(), parent->getBoundsDimensions(),
				context->dpmm, context->pixelRatio);
	}
}

void Composite::add(const std::shared_ptr<Region>& region) {
	children.push_back(region);
	if (region->parent != nullptr)
		throw std::runtime_error(
				MakeString()<<"Cannot add child node ["<<region->name<<"] to ["<<name<<"] because it already has a parent ["<<region->parent->name<<"].");
	region->parent = this;
}
void Composite::add(Region* region) {
	add(std::shared_ptr<Region>(region));
}
pixel2 TextLabel::getTextDimensions(AlloyContext* context){
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();

	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(fontType));
	float tw= nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	return pixel2(tw,th);
}
void TextLabel::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	box2px pbounds = parent->getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		if(roundCorners){
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
		} else {
			nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth);
		}
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}

	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(fontType));
	float tw= nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);

	nvgTextAlign(nvg,
			static_cast<int>(horizontalAlignment)
					| static_cast<int>(verticalAlignment));
	pixel2 offset(0, 0);
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
	drawText(nvg, bounds.position + offset, label, fontStyle, *textColor,
			*textAltColor);
	if (borderColor->a > 0) {

		nvgLineJoin(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		if(roundCorners){
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
		} else {
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		}
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
		nvgLineJoin(nvg, NVG_MITER);
	}

}
void TextField::setValue(const std::string& text) {
	this->value = text;
	moveCursorTo((int)text.size());
}

void TextField::erase() {
	int lo = std::min(cursorEnd, cursorStart);
	int hi = std::max(cursorEnd, cursorStart);
	if (hi != lo) {
		value.erase(value.begin() + lo, value.begin() + hi);
	}
	cursorEnd = cursorStart = lo;
}

void TextField::dragCursorTo(int index) {
	if (index < 0 || index > value.size())
		throw std::runtime_error(
				MakeString() << name << ": Cursor position out of range.");
	cursorStart = index;
}

void TextField::moveCursorTo(int index, bool isShiftHeld) {
	dragCursorTo(index);
	if (!isShiftHeld)
		cursorEnd = cursorStart;
}

void TextField::clear() {
	setValue("");
}

TextField::TextField(const std::string& name) :
		Region(name), label(name), value("") {
	Application::addListener(this);
	lastTime = std::chrono::high_resolution_clock::now();
}
TextField::TextField(const std::string& name,const AUnit2D& position,const AUnit2D& dimensions):Region(name,position,dimensions),label(name),value(""){
	Application::addListener(this);
	lastTime = std::chrono::high_resolution_clock::now();
}
void TextField::handleCharacterInput(AlloyContext* context,
		const InputEvent& e) {
	if (e.codepoint < 128 && isprint(e.codepoint) && !e.isControlDown()) {
		erase();
		value.insert(value.begin() + cursorStart, e.codepoint);
		showCursor = true;
		cursorEnd = ++cursorStart;
		showDefaultLabel = false;
		if (onKeyInput)
			onKeyInput(this);
	}
}
void TextField::handleKeyInput(AlloyContext* context, const InputEvent& e) {
	showCursor = true;
	if (e.isDown()) {
		switch (e.key) {
		case GLFW_KEY_RIGHT:
			if (cursorStart < (int)value.size())
				moveCursorTo(cursorStart + 1, e.isShiftDown());
			break;
		case GLFW_KEY_LEFT:
			if (cursorStart > 0)
				moveCursorTo(cursorStart - 1, e.isShiftDown());
			break;
		case GLFW_KEY_END:
			moveCursorTo((int)value.size(), e.isShiftDown());
			break;
		case GLFW_KEY_HOME:
			moveCursorTo(0, e.isShiftDown());
			break;
		case GLFW_KEY_ENTER:
			if (onTextEntered)
				onTextEntered(this);
			break;
		case GLFW_KEY_BACKSPACE:
			if (cursorEnd != cursorStart)
				erase();
			else if (cursorStart > 0) {
				moveCursorTo(cursorStart - 1);
				value.erase(value.begin() + cursorStart);
				showDefaultLabel = false;
				if (onKeyInput)
					onKeyInput(this);
			}
			break;
		case GLFW_KEY_A:
			if (e.isControlDown()) {
				cursorEnd = 0;
				cursorStart = (int)(value.size());
			}
			break;
		case GLFW_KEY_C:
			if (e.isControlDown()) {
				glfwSetClipboardString(context->window,
						value.substr(std::min(cursorEnd, cursorStart),
								std::abs(cursorEnd - cursorStart)).c_str());
			}
			break;
		case GLFW_KEY_X:
			if (e.isControlDown()) {
				glfwSetClipboardString(context->window,
						value.substr(std::min(cursorEnd, cursorStart),
								std::abs(cursorEnd - cursorStart)).c_str());
				erase();
			}
			break;
		case GLFW_KEY_V:
			if (e.isControlDown()) {
				erase();
				auto pasteText = glfwGetClipboardString(context->window);
				value.insert(cursorStart, pasteText);
				moveCursorTo(cursorStart + (int)std::string(pasteText).size(),
						e.isShiftDown());
			}
			break;
		case GLFW_KEY_DELETE:
			if (cursorEnd != cursorStart)
				erase();
			else if (cursorStart < value.size())
				value.erase(value.begin() + cursorStart);
			showDefaultLabel = false;
			if (onKeyInput)
				onKeyInput(this);
			break;
		}
	}
}

void TextField::handleMouseInput(AlloyContext* context, const InputEvent& e) {
	FontPtr fontFace = context->getFont(FontType::Bold);
	box2px bounds = getBounds();
	if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (e.isDown()) {
			showCursor = true;
			showDefaultLabel = false;
			int shift = (int)(e.cursor.x - textOffsetX);
			int cursorPos = fontFace->getCursorPosition(value, fontSize,
					shift);
			moveCursorTo(cursorPos);
			dragging = true;
		} else {
			dragging = false;
		}
	}
}
void TextField::handleCursorInput(AlloyContext* context, const InputEvent& e) {
	FontPtr fontFace = context->getFont(FontType::Bold);
	box2px bounds = getBounds();
	if (dragging) {
		int shift = (int)(e.cursor.x - textOffsetX);
		dragCursorTo(fontFace->getCursorPosition(value, fontSize, shift));
	}
}
bool TextField::onEventHandler(AlloyContext* context, const InputEvent& e) {
	if (isVisible()) {
		if (!context->isFocused(this) || fontSize <= 0)
			return false;
		switch (e.type) {
		case InputType::MouseButton:
			handleMouseInput(context, e);
			break;
		case InputType::Character:
			handleCharacterInput(context, e);
			break;
		case InputType::Key:
			handleKeyInput(context, e);
			break;
		case InputType::Cursor:
			handleCursorInput(context, e);
			break;
		}
	}
	return Region::onEventHandler(context, e);
}
void TextField::draw(AlloyContext* context) {

	float ascender, descender, lineh;
	std::vector<NVGglyphPosition> positions(value.size());
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;

	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, x, y, w, h, context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);

	auto currentTime = std::chrono::high_resolution_clock::now();
	double elapsed =
			std::chrono::duration<double>(currentTime - lastTime).count();
	if (elapsed >= 0.5f) {
		showCursor = !showCursor;
		lastTime = currentTime;
	}
	textOffsetX = x + 2.0f * lineWidth + PADDING;
	float textY = y;

	NVGpaint bg = nvgBoxGradient(nvg, x + 1, y + 3, w - 2 * PADDING,
			h - 2 * PADDING, context->theme.CORNER_RADIUS, 4,
			context->theme.HIGHLIGHT.toSemiTransparent(0.5f),
			context->theme.SHADOW.toSemiTransparent(0.5f));
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, x + PADDING, y + PADDING, w - 2 * PADDING,
			h - 2 * PADDING, context->theme.CORNER_RADIUS);
	nvgFillPaint(nvg, bg);
	nvgFill(nvg);

	fontSize = std::max(8.0f, h - 4 * PADDING);
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgTextMetrics(nvg, &ascender, &descender, &lineh);
	float twidth = nvgTextBounds(nvg, 0, textY, value.c_str(), nullptr,
			nullptr);
	if (!showDefaultLabel) {
		textOffsetX = textOffsetX
				+ std::min((w - 3.0f * PADDING - 2.0f * lineWidth) - twidth,
						0.0f);
	}
	pushScissor(nvg, x + PADDING, y, w - 2 * PADDING, h);

	nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	positions.resize(
			nvgTextGlyphPositions(nvg, textOffsetX, textY, value.data(),
					value.data() + value.size(), positions.data(),
					(int)positions.size()));
	bool isFocused = context->isFocused(this);

	if (cursorEnd != cursorStart) {
		int lo = std::min(cursorEnd, cursorStart), hi = std::max(cursorEnd,
				cursorStart);
		float x0 = lo ? positions[lo - 1].maxx - 1 : textOffsetX;
		float x1 = hi ? positions[hi - 1].maxx - 1 : textOffsetX;

		nvgBeginPath(nvg);
		nvgRect(nvg, x0, textY + (h - lineh) / 2 + PADDING, x1 - x0,
				lineh - 2 * PADDING);
		nvgFillColor(nvg,
				isFocused ?
						context->theme.DARK.toSemiTransparent(0.5f) :
						context->theme.DARK.toSemiTransparent(0.25f));
		nvgFill(nvg);
	}

	if (showDefaultLabel) {
		nvgFillColor(nvg, backgroundColor->toDarker(0.75f));
		nvgText(nvg, textOffsetX, textY + h / 2, label.c_str(), NULL);
	} else {
		nvgFillColor(nvg, *textColor);
		nvgText(nvg, textOffsetX, textY + h / 2, value.c_str(), NULL);
	}
	if (isFocused && showCursor) {
		nvgBeginPath(nvg);
		float xOffset =
				cursorStart ?
						positions[cursorStart - 1].maxx - 1 : (textOffsetX);
		nvgMoveTo(nvg, xOffset, textY + h / 2 - lineh / 2 + PADDING);
		nvgLineTo(nvg, xOffset, textY + h / 2 + lineh / 2 - PADDING);
		nvgStrokeWidth(nvg, lineWidth);
		nvgLineCap(nvg, NVG_ROUND);
		nvgStrokeColor(nvg, context->theme.SHADOW);
		nvgStroke(nvg);
	}
	popScissor(nvg);
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		nvgStrokeWidth(nvg, lineWidth);
		nvgRoundedRect(nvg, x + lineWidth, y + lineWidth, w - 2 * lineWidth,
				h - 2 * lineWidth, context->theme.CORNER_RADIUS);
		nvgStrokeColor(nvg, *borderColor);
		nvgStroke(nvg);
	}
	if (!isFocused && value.size() == 0) {
		showDefaultLabel = true;
	}
}
FileField::FileField(const std::string& name) :
		TextField(name){
}
FileField::FileField(const std::string& name,const AUnit2D& position,const AUnit2D& dimensions):
		TextField(name,position,dimensions){
}
void FileField::setValue(const std::string& text) {
	this->value = text;
	segmentedPath=splitPath(value);
	moveCursorTo((int)text.size());
}
bool FileField::onEventHandler(AlloyContext* context, const InputEvent& e) {
	if (isVisible()) {
		if (!context->isFocused(this) || fontSize <= 0)
			return false;
		switch (e.type) {
		case InputType::MouseButton:
			handleMouseInput(context, e);
			break;
		case InputType::Character:
			handleCharacterInput(context, e);
			break;
		case InputType::Key:

			if (e.isDown()) {
				if(e.key==GLFW_KEY_TAB){
					showCursor=true;
					std::string root=GetFileDirectoryPath(value);
					std::vector<std::string> listing=GetDirectoryFileListing(value);
					std::cout<<"File: "<<value<<std::endl;
					std::cout<<"Root: "<<root<<" "<<listing.size()<<std::endl;
					std::vector<std::string> results=AutoComplete(GetFileName(value),listing);
					for(std::string file:results){
						std::cout<<"FILE "<<GetFileName(file)<<std::endl;
					}
					break;
				}
			}
			handleKeyInput(context, e);
			break;
		case InputType::Cursor:
			handleCursorInput(context, e);
			break;
		}
		segmentedPath=splitPath(value);
	}
	return Region::onEventHandler(context, e);
}
void FileField::draw(AlloyContext* context) {

	float ascender, descender, lineh;
	std::vector<NVGglyphPosition> positions(value.size());
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;

	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, x, y, w, h, context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);

	auto currentTime = std::chrono::high_resolution_clock::now();
	double elapsed =
			std::chrono::duration<double>(currentTime - lastTime).count();
	if (elapsed >= 0.5f) {
		showCursor = !showCursor;
		lastTime = currentTime;
	}
	textOffsetX = x + 2.0f * lineWidth + PADDING;
	float textY = y;

	NVGpaint bg = nvgBoxGradient(nvg, x + 1, y + 3, w - 2 * PADDING,
			h - 2 * PADDING, context->theme.CORNER_RADIUS, 4,
			context->theme.HIGHLIGHT.toSemiTransparent(0.5f),
			context->theme.SHADOW.toSemiTransparent(0.5f));
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, x + PADDING, y + PADDING, w - 2 * PADDING,
			h - 2 * PADDING, context->theme.CORNER_RADIUS);
	nvgFillPaint(nvg, bg);
	nvgFill(nvg);

	fontSize = std::max(8.0f, h - 4 * PADDING);
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgTextMetrics(nvg, &ascender, &descender, &lineh);
	float twidth = nvgTextBounds(nvg, 0, textY, value.c_str(), nullptr,
			nullptr);
	if (!showDefaultLabel) {
		textOffsetX = textOffsetX
				+ std::min((w - 3.0f * PADDING - 2.0f * lineWidth) - twidth,
						0.0f);
	}
	pushScissor(nvg, x + PADDING, y, w - 2 * PADDING, h);

	nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	positions.resize(
			nvgTextGlyphPositions(nvg, textOffsetX, textY, value.data(),
					value.data() + value.size(), positions.data(),
					(int)positions.size()));
	bool isFocused = context->isFocused(this);

	if (cursorEnd != cursorStart) {
		int lo = std::min(cursorEnd, cursorStart), hi = std::max(cursorEnd,
				cursorStart);
		float x0 = lo ? positions[lo - 1].maxx - 1 : textOffsetX;
		float x1 = hi ? positions[hi - 1].maxx - 1 : textOffsetX;

		nvgBeginPath(nvg);
		nvgRect(nvg, x0, textY + (h - lineh) / 2 + PADDING, x1 - x0,
				lineh - 2 * PADDING);
		nvgFillColor(nvg,
				isFocused ?
						context->theme.DARK.toSemiTransparent(0.5f) :
						context->theme.DARK.toSemiTransparent(0.25f));
		nvgFill(nvg);
	}

	if (showDefaultLabel) {
		nvgFillColor(nvg, backgroundColor->toDarker(0.75f));
		nvgText(nvg, textOffsetX, textY + h / 2, label.c_str(), NULL);
	} else {

		float xOffset=textOffsetX;
		std::stringstream path;
		bool underline;
		for(std::string comp:segmentedPath){
			path<<comp;
			underline=false;
			if(comp==ALY_PATH_SEPARATOR){
				nvgFillColor(nvg, *textColor);
			} else {
				if(FileExists(path.str())){
					underline=true;
					nvgFillColor(nvg,  context->theme.LINK);
				} else {
					nvgFillColor(nvg, context->theme.DARK);
				}
			}
			nvgText(nvg, xOffset, textY + h / 2, comp.c_str(), NULL);
			float lastOffset=xOffset;
			xOffset+= nvgTextBounds(nvg, 0, textY, comp.c_str(), nullptr,nullptr);
			if(underline){
				nvgBeginPath(nvg);
				nvgStrokeWidth(nvg,2.0f);
				nvgStrokeColor(nvg,  textColor->toSemiTransparent(0.75f));
				nvgMoveTo(nvg,lastOffset,textY+fontSize+1);
				nvgLineTo(nvg,xOffset,textY+fontSize+1);
				nvgStroke(nvg);
			}
		}
		/*
		nvgFillColor(nvg, *textColor);
		nvgText(nvg, textOffsetX, textY + h / 2, value.c_str(), NULL);
	*/
	}
	if (isFocused && showCursor) {
		nvgBeginPath(nvg);
		float xOffset =
				cursorStart ?
						positions[cursorStart - 1].maxx - 1 : (textOffsetX);
		nvgMoveTo(nvg, xOffset, textY + h / 2 - lineh / 2 + PADDING);
		nvgLineTo(nvg, xOffset, textY + h / 2 + lineh / 2 - PADDING);
		nvgStrokeWidth(nvg, lineWidth);
		nvgLineCap(nvg, NVG_ROUND);
		nvgStrokeColor(nvg, context->theme.SHADOW);
		nvgStroke(nvg);
	}
	popScissor(nvg);
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		nvgStrokeWidth(nvg, lineWidth);
		nvgRoundedRect(nvg, x + lineWidth, y + lineWidth, w - 2 * lineWidth,
				h - 2 * lineWidth, context->theme.CORNER_RADIUS);
		nvgStrokeColor(nvg, *borderColor);
		nvgStroke(nvg);
	}
	if (!isFocused && value.size() == 0) {
		showDefaultLabel = true;
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
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		if(roundCorners){
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
		} else {
			nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth);
		}
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	if (glyph.get() != nullptr) {
		box2px b=bounds;
		b.position.x=bounds.position.x + lineWidth * 0.5f;
		b.position.y=bounds.position.y + lineWidth * 0.5f;
		b.dimensions.x=bounds.dimensions.x - lineWidth;
		b.dimensions.y=bounds.dimensions.y - lineWidth;
		glyph->draw(b, *foregroundColor, *backgroundColor, context);
	}

	if (borderColor->a > 0) {

		nvgLineJoin(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		if(roundCorners){
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,context->theme.CORNER_RADIUS);
		} else {
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth);
		}
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
		nvgLineJoin(nvg, NVG_MITER);
	}
}
std::shared_ptr<Composite> MakeComposite(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const Color& bgColor, const Orientation& orientation) {
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
		const AUnit2D& dimensions, const Color& bgColor, const Color& fgColor,
		const Color& borderColor, const AUnit1D& borderWidth) {
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
		const Color& bgColor, const Color& fgColor, const Color& borderColor,
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
		const Color& fontColor, const HorizontalAlignment& halign,
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
std::shared_ptr<TextField> MakeTextField(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const Color& bgColor, const Color& textColor,
		const std::string& value) {
	std::shared_ptr<TextField> region = std::shared_ptr<TextField>(
			new TextField(name));
	region->setPosition(position);
	region->setDimensions(dimensions);
	region->backgroundColor = MakeColor(bgColor);
	region->textColor = MakeColor(textColor);
	region->borderColor = MakeColor(bgColor.toDarker(0.5f));
	region->setValue(value);
	return region;
}
std::shared_ptr<Region> MakeRegion(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const Color& bgColor, const Color& borderColor,
		const AUnit1D& borderWidth) {
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

