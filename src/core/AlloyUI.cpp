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
	bool Region::isVisible() const {
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
		}
		else {
			return nullptr;
		}
	}

	void Region::pack(AlloyContext* context) {
		if (parent == nullptr) {
			pack(pixel2(0, 0), pixel2(context->dimensions()), context->dpmm,
				context->pixelRatio);
		}
		else {
			pack(parent->getBoundsPosition(), parent->getBoundsDimensions(),
				context->dpmm, context->pixelRatio);
		}
	}
	void Region::pack() {
		pack(AlloyApplicationContext().get());
	}
	void Region::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);

		if (backgroundColor->a > 0) {
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
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
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
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
				}
				else {
					c = DEBUG_DOWN_COLOR;
				}
			}
			else if (hover) {
				if (ontop) {
					c = DEBUG_ON_TOP_HOVER_COLOR;
				}
				else {
					c = DEBUG_HOVER_COLOR;
				}
			}
			else if (ontop) {
				c = DEBUG_ON_TOP_COLOR;
			}
			else {
				c = DEBUG_STROKE_COLOR;
			}
		}
		else {
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
		box2px bounds = getBounds();
		pixel2 d = (bounds.position - dragOffset);
		if (!clampToParentBounds) {
			dragOffset = cursor - delta - d;
		}
		else {
			box2px pbounds = parent->getBounds();
			dragOffset = bounds.clamp(cursor - delta, pbounds) - d;
		}
	}
	bool Region::addDragOffset(const pixel2& delta) {
		pixel2 oldOffset = dragOffset;
		box2px bounds = getBounds();
		pixel2 d = (bounds.position - dragOffset);
		box2px pbounds = parent->getBounds();
		if (!clampToParentBounds) {
			dragOffset = bounds.position + delta - d;
		}
		else {
			dragOffset = bounds.clamp(bounds.position + delta, pbounds) - d;
		}
		return (oldOffset != dragOffset);
	}
	Region::Region(const std::string& name) :
		position(CoordPX(0, 0)), dimensions(CoordPercent(1, 1)), name(name) {
	}
	Region::Region(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
		position(pos), dimensions(dims), name(name) {

	}
	Region::~Region() {
		Application::clearEvents(this);
	}
	void Region::drawDebug(AlloyContext* context) {
		drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
	}
	box2px Region::getBounds(bool includeOffset) const {
		box2px box = bounds;
		if (parent != nullptr && includeOffset) {
			box.position += parent->drawOffset();
		}
		box.position += dragOffset;
		return box;
	}
	box2px Region::getCursorBounds(bool includeOffset) const {
		box2px box = (isDetached() ? getBounds(includeOffset) : bounds);
		box.position += dragOffset;
		if (parent != nullptr && (!isDetached() && includeOffset)) {
			box.position += parent->drawOffset();
			if (AlloyApplicationContext()->getOnTopRegion() != this) {
				box.intersect(parent->getCursorBounds());
			}
		}

		return box;
	}
	void Draw::draw(AlloyContext* context) {
		if (onDraw) {
			onDraw(context, getBounds());
		}
	}
	void Composite::putLast(const std::shared_ptr<Region>& region) {
		size_t idx = 0;
		size_t pivot = children.size() - 1;
		std::vector<std::shared_ptr<Region>> newList;
		for (RegionPtr& child : children) {
			if (child.get() == region.get()) {
				pivot = idx;
			}
			else {
				newList.push_back(child);
			}
			idx++;
		}
		newList.push_back(children[pivot]);
		children = newList;
		AlloyApplicationContext()->requestUpdateCursorLocator();
	}
	void Composite::putFirst(const std::shared_ptr<Region>& region) {
		size_t idx = 0;
		size_t pivot = 0;
		std::vector<std::shared_ptr<Region>> newList;
		for (RegionPtr& child : children) {
			if (child.get() == region.get()) {
				pivot = idx;
			}
			else {
				newList.push_back(child);
			}
			idx++;
		}
		newList.insert(newList.begin(), children[pivot]);
		children = newList;
		AlloyApplicationContext()->requestUpdateCursorLocator();
	}
	void Composite::putLast(Region* region) {
		size_t idx = 0;
		size_t pivot = children.size() - 1;
		std::vector<std::shared_ptr<Region>> newList;
		for (RegionPtr& child : children) {
			if (child.get() == region) {
				pivot = idx;
			}
			else {
				newList.push_back(child);
			}
			idx++;
		}
		newList.push_back(children[pivot]);
		children = newList;
		AlloyApplicationContext()->requestUpdateCursorLocator();
	}
	void Composite::putFirst(Region* region) {
		size_t idx = 0;
		size_t pivot = 0;
		std::vector<std::shared_ptr<Region>> newList;
		for (RegionPtr& child : children) {
			if (child.get() == region) {
				pivot = idx;
			}
			else {
				newList.push_back(child);
			}
			idx++;
		}
		newList.insert(newList.begin(), children[pivot]);
		children = newList;
		AlloyApplicationContext()->requestUpdateCursorLocator();
	}
	void Composite::clear() {
		setDragOffset(pixel2(0, 0));
		AlloyApplicationContext()->addDeferredTask(
			[this] {this->children.clear();});
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
		float w = bounds.dimensions.x;
		float h = bounds.dimensions.y;
		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		if (isScrollEnabled()) {
			pushScissor(nvg, getCursorBounds());
		}
		if (backgroundColor->a > 0) {
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
					bounds.dimensions.x, bounds.dimensions.y,
					context->theme.CORNER_RADIUS);
			}
			else {
				nvgRect(nvg, bounds.position.x, bounds.position.y,
					bounds.dimensions.x, bounds.dimensions.y);
			}
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}

		for (std::shared_ptr<Region>& region : children) {
			if (region->isVisible()) {
				region->draw(context);
			}
		}

		if (isScrollEnabled() && verticalScrollTrack.get() != nullptr) {
			if (scrollExtent.y > h) {
				verticalScrollTrack->setVisible(true);
				verticalScrollHandle->setVisible(true);
				verticalScrollTrack->draw(context);
				verticalScrollHandle->draw(context);
			}
			else {
				verticalScrollTrack->setVisible(alwaysShowVerticalScrollBar);
				if (alwaysShowVerticalScrollBar)verticalScrollTrack->draw(context);
				verticalScrollHandle->setVisible(false);
			}
			if (scrollExtent.x > w) {
				horizontalScrollTrack->setVisible(true);
				horizontalScrollHandle->setVisible(true);
				horizontalScrollTrack->draw(context);
				horizontalScrollHandle->draw(context);
			}
			else {
				horizontalScrollTrack->setVisible(alwaysShowHorizontalScrollBar);
				if (alwaysShowHorizontalScrollBar)horizontalScrollTrack->draw(context);
				horizontalScrollHandle->setVisible(false);
			}
		}

		if (isScrollEnabled()) {
			popScissor(nvg);
		}
		if (borderColor->a > 0) {

			nvgLineJoin(nvg, NVG_ROUND);
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
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
	void Composite::drawDebug(AlloyContext* context) {
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

	void Composite::draw() {
		draw(AlloyApplicationContext().get());
	}
	bool Composite::addVerticalScrollPosition(float t) {
		if (verticalScrollHandle->addDragOffset(pixel2(0.0f, t))) {
			this->scrollPosition.y =
				(this->verticalScrollHandle->getBoundsPositionY()
					- this->verticalScrollTrack->getBoundsPositionY())
				/ std::max(1.0f,
					(float) this->verticalScrollTrack->getBoundsDimensionsY()
					- (float) this->verticalScrollHandle->getBoundsDimensionsY());
			AlloyApplicationContext()->requestPack();
			return true;
		}
		return false;
	}

	bool Composite::addHorizontalScrollPosition(float t) {
		if (horizontalScrollHandle->addDragOffset(pixel2(t, 0.0f))) {
			this->scrollPosition.x =
				(this->horizontalScrollHandle->getBoundsPositionX()
					- this->horizontalScrollTrack->getBoundsPositionX())
				/ std::max(1.0f,
					(float) this->horizontalScrollTrack->getBoundsDimensionsX()
					- (float) this->horizontalScrollHandle->getBoundsDimensionsX());

			AlloyApplicationContext()->requestPack();
			return true;
		}
		return false;
	}
	void Composite::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
		Region::pack(pos, dims, dpmm, pixelRatio);
		box2px bounds = getBounds(false);

		if (verticalScrollTrack.get() == nullptr && isScrollEnabled()) {
			verticalScrollTrack = std::shared_ptr<ScrollTrack>(
				new ScrollTrack("Vert Track", Orientation::Vertical));
			verticalScrollTrack->position = CoordPercent(1.0f, 0.0f);
			verticalScrollTrack->dimensions =
				CoordPerPX(0.0, 1.0f, scrollBarSize, 0.0f);
			verticalScrollTrack->setOrigin(Origin::TopRight);
			verticalScrollTrack->parent = parent;
			verticalScrollHandle = std::shared_ptr<ScrollHandle>(
				new ScrollHandle("Vert Handle", Orientation::Vertical));
			verticalScrollHandle->position = CoordPX(0.0f, 0.0f);
			verticalScrollHandle->dimensions =
				CoordPerPX(1.0f, 0.0f, 0.0f, scrollBarSize);
			verticalScrollHandle->parent = verticalScrollTrack.get();
			verticalScrollHandle->setDragEnabled(true);

			verticalScrollTrack->onMouseDown =
				[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					this->verticalScrollHandle->setDragOffset(event.cursor, this->verticalScrollHandle->getBoundsDimensions() * 0.5f);
					context->setDragObject(verticalScrollHandle.get());
					this->scrollPosition.y = (this->verticalScrollHandle->getBoundsPositionY() - this->verticalScrollTrack->getBoundsPositionY()) /
						std::max(1.0f, (float)this->verticalScrollTrack->getBoundsDimensionsY() - (float)this->verticalScrollHandle->getBoundsDimensionsY());
					context->requestPack();
					return true;
				}
				return false;
			};
			verticalScrollHandle->onMouseDrag =
				[this](AlloyContext* context, const InputEvent& event) {
				if (context->isLeftMouseButtonDown()) {
					this->verticalScrollHandle->setDragOffset(event.cursor, context->getRelativeCursorDownPosition());
					this->scrollPosition.y = (this->verticalScrollHandle->getBoundsPositionY() - this->verticalScrollTrack->getBoundsPositionY()) /
						std::max(1.0f, (float)this->verticalScrollTrack->getBoundsDimensionsY() - (float)this->verticalScrollHandle->getBoundsDimensionsY());
					return true;
				}
				return false;
			};
			horizontalScrollTrack = std::shared_ptr<ScrollTrack>(
				new ScrollTrack("Horiz Track", Orientation::Horizontal));
			horizontalScrollTrack->position = CoordPercent(0.0f, 1.0f);
			horizontalScrollTrack->dimensions =
				CoordPerPX(1.0, 0.0f, 0.0f, scrollBarSize);
			horizontalScrollTrack->setOrigin(Origin::BottomLeft);
			verticalScrollTrack->parent = parent;
			horizontalScrollHandle = std::shared_ptr<ScrollHandle>(
				new ScrollHandle("Horiz Handle", Orientation::Horizontal));
			horizontalScrollHandle->position = CoordPX(0.0f, 0.0f);
			horizontalScrollHandle->dimensions =
				CoordPerPX(0.0f, 1.0f, scrollBarSize, 0.0f);
			horizontalScrollHandle->parent = horizontalScrollTrack.get();
			horizontalScrollHandle->setDragEnabled(true);

			horizontalScrollTrack->onMouseDown =
				[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					this->horizontalScrollHandle->setDragOffset(event.cursor, this->horizontalScrollHandle->getBoundsDimensions() * 0.5f);
					context->setDragObject(horizontalScrollHandle.get());
					this->scrollPosition.x = (this->horizontalScrollHandle->getBoundsPositionX() - this->horizontalScrollTrack->getBoundsPositionX()) /
						std::max(1.0f, (float)this->horizontalScrollTrack->getBoundsDimensionsX() - (float)this->horizontalScrollHandle->getBoundsDimensionsX());
					context->requestPack();
					return true;
				}
				return false;
			};
			horizontalScrollHandle->onMouseDrag =
				[this](AlloyContext* context, const InputEvent& event) {
				if (context->isLeftMouseButtonDown()) {
					this->horizontalScrollHandle->setDragOffset(event.cursor, context->getRelativeCursorDownPosition());
					this->scrollPosition.x = (this->horizontalScrollHandle->getBoundsPositionX() - this->horizontalScrollTrack->getBoundsPositionX()) /
						std::max(1.0f, (float)this->horizontalScrollTrack->getBoundsDimensionsX() - (float)this->horizontalScrollHandle->getBoundsDimensionsX());
					return true;
				}
				return false;
			};
			Application::addListener(this);
		}
		pixel2 offset = cellPadding;
		scrollExtent = pixel2(0, 0);
		for (std::shared_ptr<Region>& region : children) {
			if (!region->isVisible()) {
				continue;
			}
			if (orientation == Orientation::Vertical) {
				pixel2 pix = region->position.toPixels(bounds.dimensions, dpmm,
					pixelRatio);
				region->position = CoordPX(pix.x, offset.y);
			}
			if (orientation == Orientation::Horizontal) {
				pixel2 pix = region->position.toPixels(bounds.dimensions, dpmm,
					pixelRatio);
				region->position = CoordPX(offset.x, pix.y);
			}
			region->pack(bounds.position, bounds.dimensions, dpmm, pixelRatio);
			box2px cbounds = region->getBounds();
			if (orientation == Orientation::Horizontal) {
				offset.x += cellSpacing.x + cbounds.dimensions.x;

			}
			if (orientation == Orientation::Vertical) {
				offset.y += cellSpacing.y + cbounds.dimensions.y;
			}
			scrollExtent = aly::max(cbounds.dimensions + cbounds.position - bounds.position - this->drawOffset(), scrollExtent);
		}
		if (!isScrollEnabled()) {
			if (orientation == Orientation::Horizontal)
				this->bounds.dimensions.x = bounds.dimensions.x = std::max(
					bounds.dimensions.x,
					offset.x - cellSpacing.x + cellPadding.x);
			if (orientation == Orientation::Vertical)
				this->bounds.dimensions.y = bounds.dimensions.y = std::max(
					bounds.dimensions.y,
					offset.y - cellSpacing.y + cellPadding.y);
		}

		if (verticalScrollTrack.get() != nullptr) {
			bool  showY = scrollExtent.y > bounds.dimensions.y || alwaysShowVerticalScrollBar;
			bool  showX = scrollExtent.x > bounds.dimensions.x || alwaysShowHorizontalScrollBar;
			float nudge =
				(showX&&showY) ?
				-scrollBarSize : 0;

			verticalScrollTrack->dimensions =
				CoordPerPX(0.0f, 1.0f, scrollBarSize, nudge);
			verticalScrollHandle->dimensions =
				CoordPerPX(1.0f, 0.0f, 0.0f,
					std::max(scrollBarSize,
						(verticalScrollTrack->getBoundsDimensionsY()
							* bounds.dimensions.y)
						/ scrollExtent.y));

			verticalScrollTrack->pack(bounds.position, bounds.dimensions, dpmm,
				pixelRatio);
			verticalScrollHandle->pack(verticalScrollTrack->getBoundsPosition(),
				verticalScrollTrack->getBoundsDimensions(), dpmm, pixelRatio,
				true);

			horizontalScrollTrack->dimensions =
				CoordPerPX(1.0f, 0.0f, nudge, scrollBarSize);
			horizontalScrollHandle->dimensions =
				CoordPerPX(0.0f, 1.0f,
					std::max(scrollBarSize,
						(horizontalScrollTrack->getBoundsDimensionsX()
							* bounds.dimensions.x)
						/ scrollExtent.x), 0.0f);

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
				if (iter->get() == nullptr)
					continue;
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

	void BorderComposite::setNorth(const std::shared_ptr<Region>& region,
		const AUnit1D& fraction) {
		if (region->parent != nullptr)
			throw std::runtime_error(
				MakeString() << "Cannot add child node [" << region->name
				<< "] to [" << name
				<< "] because it already has a parent ["
				<< region->parent->name << "].");
		northRegion = region;
		northRegion->parent = this;
		northFraction = fraction;
	}
	void BorderComposite::setSouth(const std::shared_ptr<Region>& region,
		const AUnit1D& fraction) {
		if (region->parent != nullptr)
			throw std::runtime_error(
				MakeString() << "Cannot add child node [" << region->name
				<< "] to [" << name
				<< "] because it already has a parent ["
				<< region->parent->name << "].");
		southRegion = region;
		southRegion->parent = this;
		southFraction = fraction;
	}
	void BorderComposite::setEast(const std::shared_ptr<Region>& region,
		const AUnit1D& fraction) {
		if (region->parent != nullptr)
			throw std::runtime_error(
				MakeString() << "Cannot add child node [" << region->name
				<< "] to [" << name
				<< "] because it already has a parent ["
				<< region->parent->name << "].");
		eastRegion = region;
		eastRegion->parent = this;
		eastFraction = fraction;
	}
	void BorderComposite::setWest(const std::shared_ptr<Region>& region,
		const AUnit1D& fraction) {
		if (region->parent != nullptr)
			throw std::runtime_error(
				MakeString() << "Cannot add child node [" << region->name
				<< "] to [" << name
				<< "] because it already has a parent ["
				<< region->parent->name << "].");
		westRegion = region;
		westRegion->parent = this;
		westFraction = fraction;
	}
	void BorderComposite::setCenter(const std::shared_ptr<Region>& region) {
		if (region->parent != nullptr)
			throw std::runtime_error(
				MakeString() << "Cannot add child node [" << region->name
				<< "] to [" << name
				<< "] because it already has a parent ["
				<< region->parent->name << "].");
		centerRegion = region;
		centerRegion->parent = this;
	}
	void BorderComposite::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		if (isScrollEnabled()) {
			pushScissor(nvg, getCursorBounds());
		}
		if (backgroundColor->a > 0) {
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
				nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth);
			}
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}

		for (std::shared_ptr<Region>& region : children) {
			if (region.get() == nullptr)
				continue;
			if (region->isVisible()) {
				region->draw(context);
			}
		}
		if (borderColor->a > 0) {

			nvgLineJoin(nvg, NVG_ROUND);
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
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
		Region(name), northRegion(children[0]), southRegion(children[1]), eastRegion(
			children[2]), westRegion(children[3]), centerRegion(children[4]) {
		northFraction = UnitPX(0.0f);
		southFraction = UnitPX(0.0f);
		eastFraction = UnitPX(0.0f);
		westFraction = UnitPX(0.0f);

	}
	BorderComposite::BorderComposite(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Region(name, pos, dims), northRegion(children[0]), southRegion(
			children[1]), eastRegion(children[2]), westRegion(children[3]), centerRegion(
				children[4]) {
		northFraction = UnitPX(0.0f);
		southFraction = UnitPX(0.0f);
		eastFraction = UnitPX(0.0f);
		westFraction = UnitPX(0.0f);
	}
	void BorderComposite::update(CursorLocator* cursorLocator) {
		if (!ignoreCursorEvents)
			cursorLocator->add(this);
		for (std::shared_ptr<Region>& region : children) {
			if (region.get() == nullptr)
				continue;
			region->update(cursorLocator);
		}
	}

	void BorderComposite::drawDebug(AlloyContext* context) {
		drawBoundsLabel(context, name, context->getFontHandle(FontType::Bold));
		for (std::shared_ptr<Region>& region : children) {
			if (region.get() == nullptr)
				continue;
			region->drawDebug(context);
		}
	}

	void BorderComposite::pack(const pixel2& pos, const pixel2& dims,
		const double2& dpmm, double pixelRatio, bool clamp) {
		Region::pack(pos, dims, dpmm, pixelRatio);
		box2px bounds = getBounds(false);
		pixel north = northFraction.toPixels(bounds.dimensions.y, dpmm.y,
			pixelRatio);
		pixel south = southFraction.toPixels(bounds.dimensions.y, dpmm.y,
			pixelRatio);
		pixel west = westFraction.toPixels(bounds.dimensions.x, dpmm.x, pixelRatio);
		pixel east = eastFraction.toPixels(bounds.dimensions.x, dpmm.x, pixelRatio);

		if (northRegion.get() != nullptr) {
			northRegion->pack(bounds.position, pixel2(bounds.dimensions.x, north),
				dpmm, pixelRatio);
		}
		if (southRegion.get() != nullptr) {
			southRegion->pack(
				bounds.position + pixel2(0, bounds.dimensions.y - south),
				pixel2(bounds.dimensions.x, south), dpmm, pixelRatio);
		}
		if (westRegion.get() != nullptr)
			westRegion->pack(bounds.position + pixel2(0.0f, north),
				pixel2(west, bounds.dimensions.y - north - south), dpmm,
				pixelRatio);
		if (eastRegion.get() != nullptr)
			eastRegion->pack(
				bounds.position + pixel2(bounds.dimensions.x - east, north),
				pixel2(east, bounds.dimensions.y - north - south), dpmm,
				pixelRatio);
		if (centerRegion.get() != nullptr)
			centerRegion->pack(bounds.position + pixel2(west, north),
				pixel2(bounds.dimensions.x - east - west,
					bounds.dimensions.y - north - south), dpmm, pixelRatio);

		for (std::shared_ptr<Region>& region : children) {
			if (region.get() == nullptr)
				continue;
			if (region->onPack)
				region->onPack();
		}
		if (onPack)
			onPack();
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
		}
		else if (orientation == Orientation::Horizontal) {
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
		}
		else if (orientation == Orientation::Horizontal) {
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
		Region(name), cellPadding(0, 0), cellSpacing(5, 5) {

	}
	Composite::Composite(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Region(name, pos, dims), cellPadding(0, 0), cellSpacing(5, 5) {
	}
	bool Composite::onEventHandler(AlloyContext* context, const InputEvent& event) {
		if (isVisible() && event.type == InputType::Scroll && isScrollEnabled()) {
			box2px bounds = getBounds();
			if (bounds.contains(event.cursor)) {
				if (event.scroll.y != 0 && verticalScrollHandle.get() != nullptr
					&& verticalScrollHandle->isVisible()) {
					verticalScrollHandle->addDragOffset(
						pixel2(-10.0f * event.scroll));
					this->scrollPosition.y =
						(this->verticalScrollHandle->getBoundsPositionY()
							- this->verticalScrollTrack->getBoundsPositionY())
						/ std::max(1.0f,
							(float) this->verticalScrollTrack->getBoundsDimensionsY()
							- (float) this->verticalScrollHandle->getBoundsDimensionsY());
					return true;
				}
				if (event.scroll.x != 0 && horizontalScrollHandle.get() != nullptr
					&& horizontalScrollHandle->isVisible()) {
					horizontalScrollHandle->addDragOffset(
						pixel2(-10.0f * event.scroll));
					this->scrollPosition.x =
						(this->horizontalScrollHandle->getBoundsPositionX()
							- this->horizontalScrollTrack->getBoundsPositionX())
						/ std::max(1.0f,
							(float) this->horizontalScrollTrack->getBoundsDimensionsX()
							- (float) this->horizontalScrollHandle->getBoundsDimensionsX());

					return true;
				}
			}
		}
		return Region::onEventHandler(context, event);
	}
	void Composite::update(CursorLocator* cursorLocator) {
		if (!ignoreCursorEvents)
			cursorLocator->add(this);
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
		if (!ignoreCursorEvents)
			cursorLocator->add(this);
	}
	void Region::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {

		pixel2 computedPos = position.toPixels(dims, dpmm, pixelRatio);
		//pixel2 xy = pos + dragOffset + computedPos;
		pixel2 xy = pos + computedPos;
		pixel2 d = dimensions.toPixels(dims, dpmm, pixelRatio);
		if (aspectRatio < 0) {
			aspectRatio = d.x / std::max((float)d.y, 0.0f);
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
			bounds.dimensions = aly::clamp(bounds.dimensions, pixel2(0, 0),
				parent->bounds.dimensions);
		}

		switch (origin) {
		case Origin::TopLeft:
			bounds.position = xy;
			break;
		case Origin::BottomRight:
			bounds.position = xy - bounds.dimensions;
			break;
		case Origin::MiddleCenter:
			bounds.position = xy - bounds.dimensions / (pixel)2;
			break;
		case Origin::TopRight:
			bounds.position = xy - pixel2(bounds.dimensions.x, 0);
			break;
		case Origin::BottomLeft:
			bounds.position = xy - pixel2(0, bounds.dimensions.y);
			break;
		case Origin::MiddleLeft:
			bounds.position = xy - pixel2(0, bounds.dimensions.y / (pixel)2);
			break;
		case Origin::MiddleRight:
			bounds.position = xy
				- pixel2(bounds.dimensions.x, bounds.dimensions.y / (pixel)2);
			break;
		case Origin::TopCenter:
			bounds.position = xy - pixel2(bounds.dimensions.x / (pixel)2, 0);
			break;
		case Origin::BottomCenter:
			bounds.position = xy
				- pixel2(bounds.dimensions.x / (pixel)2, bounds.dimensions.y);
			break;
		}

		if (clamp && parent != nullptr && !parent->isScrollEnabled()) {
			pixel2 ppos = parent->getBoundsPosition();
			pixel2 dims = parent->bounds.dimensions;
			bounds.position = aly::clamp(bounds.position, ppos,
				ppos + dims - bounds.dimensions);
		}
	}

	void Composite::add(const std::shared_ptr<Region>& region) {
		children.push_back(region);
		if (region->parent != nullptr)
			throw std::runtime_error(
				MakeString() << "Cannot add child node [" << region->name
				<< "] to [" << name
				<< "] because it already has a parent ["
				<< region->parent->name << "].");
		region->parent = this;
	}

	void Composite::add(Region* region) {
		add(std::shared_ptr<Region>(region));
	}
	pixel2 TextLabel::getTextDimensions(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();

		float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		nvgFontSize(nvg, th);
		nvgFontFaceId(nvg, context->getFontHandle(fontType));
		float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
		return pixel2(tw, th);
	}
	void TextLabel::draw(AlloyContext* context) {
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		if (backgroundColor->a > 0) {
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
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
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
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
		textStart = 0;
		moveCursorTo((int)text.size());
	}

	void TextField::erase() {
		int lo = std::min(cursorEnd, cursorStart);
		int hi = std::max(cursorEnd, cursorStart);
		if (hi != lo) {
			value.erase(value.begin() + lo, value.begin() + hi);
		}
		cursorEnd = cursorStart = lo;
		textStart = clamp(cursorStart - 1, 0, textStart);
	}

	void TextField::dragCursorTo(int index) {
		if (index < 0 || index >(int) value.size())
			throw std::runtime_error(
				MakeString() << name << ": Cursor position out of range.");
		cursorStart = index;
		textStart = clamp(cursorStart - 1, 0, textStart);
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
		Composite(name), label(name), value("") {
		Application::addListener(this);
		lastTime = std::chrono::high_resolution_clock::now();
	}
	TextField::TextField(const std::string& name, const AUnit2D& position,
		const AUnit2D& dimensions) :
		Composite(name, position, dimensions), label(name), value("") {
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
				if (cursorStart < (int)value.size()) {
					moveCursorTo(cursorStart + 1, e.isShiftDown());
				}
				else {
					moveCursorTo((int)value.size(), e.isShiftDown());
				}
				break;
			case GLFW_KEY_LEFT:
				if (cursorStart > 0) {
					moveCursorTo(cursorStart - 1, e.isShiftDown());
				}
				else {
					moveCursorTo(0, e.isShiftDown());
				}
				break;
			case GLFW_KEY_END:
				moveCursorTo((int)value.size(), e.isShiftDown());
				break;
			case GLFW_KEY_HOME:
				moveCursorTo(0, e.isShiftDown());
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
				else if (cursorStart < (int)value.size())
					value.erase(value.begin() + cursorStart);
				showDefaultLabel = false;
				if (onKeyInput)
					onKeyInput(this);
				break;
			case GLFW_KEY_ENTER:
				if (onTextEntered) {
					onTextEntered(this);
				}
				break;
			}
		}
	}

	void TextField::handleMouseInput(AlloyContext* context, const InputEvent& e) {
		FontPtr fontFace = context->getFont(FontType::Bold);
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			if (e.isDown()) {
				showCursor = true;
				showDefaultLabel = false;
				int shift = (int)(e.cursor.x - textOffsetX);
				int cursorPos = fontFace->getCursorPosition(value, fontSize, shift);
				moveCursorTo(cursorPos);
				textStart = 0;
				dragging = true;
			}
			else {
				dragging = false;
			}
		}
	}
	void TextField::handleCursorInput(AlloyContext* context, const InputEvent& e) {
		FontPtr fontFace = context->getFont(FontType::Bold);
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
			case InputType::Unspecified:
				break;
			case InputType::Scroll:
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
		bool hover = context->isMouseOver(this);
		float x = bounds.position.x;
		float y = bounds.position.y;
		float w = bounds.dimensions.x;
		float h = bounds.dimensions.y;
		if (backgroundColor->a > 0) {
			if (hover) {
				nvgBeginPath(nvg);
				nvgRoundedRect(nvg, x, y, w, h, context->theme.CORNER_RADIUS);
				nvgFillColor(nvg, *backgroundColor);
				nvgFill(nvg);

			}
			else {
				nvgBeginPath(nvg);
				nvgRoundedRect(nvg, x + 1, y + 1, w - 2, h - 2,
					context->theme.CORNER_RADIUS);
				nvgFillColor(nvg, *backgroundColor);
				nvgFill(nvg);
			}
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

		pushScissor(nvg, x + PADDING, y, w - 2 * PADDING, h);

		nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		positions.resize(value.size() + 1);
		nvgTextGlyphPositions(nvg, 0, textY, value.data(),
			value.data() + value.size(), positions.data(),
			(int)positions.size());
		float fwidth = (w - 3.0f * PADDING - 2.0f * lineWidth);
		if (cursorStart > 0) {
			if (positions[cursorStart - 1].maxx - positions[textStart].minx
				> fwidth) {
				while (positions[cursorStart - 1].maxx
					> positions[textStart].minx + fwidth) {
					if (textStart >= (int)positions.size() - 1)
						break;
					textStart++;
				}
			}
		}
		if (!showDefaultLabel) {
			textOffsetX = textOffsetX - positions[textStart].minx;
		}
		float cursorOffset = textOffsetX
			+ (cursorStart ? positions[cursorStart - 1].maxx - 1 : 0);
		bool isFocused = context->isFocused(this);

		if (cursorEnd != cursorStart && isFocused) {
			int lo = std::min(cursorEnd, cursorStart);
			int hi = std::max(cursorEnd, cursorStart);
			float x0 = textOffsetX + (lo ? positions[lo - 1].maxx - 1 : 0);
			float x1 = textOffsetX + (hi ? positions[hi - 1].maxx - 1 : 0);
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
		}
		else {
			nvgFillColor(nvg, *textColor);
			nvgText(nvg, textOffsetX, textY + h / 2, value.c_str(), NULL);
		}
		if (isFocused && showCursor) {
			nvgBeginPath(nvg);

			nvgMoveTo(nvg, cursorOffset, textY + h / 2 - lineh / 2 + PADDING);
			nvgLineTo(nvg, cursorOffset, textY + h / 2 + lineh / 2 - PADDING);
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
	FileField::FileField(const std::string& name, const AUnit2D& position,
		const AUnit2D& dimensions) :
		TextField(name, position, dimensions) {
		showDefaultLabel = true;
		selectionBox = SelectionBoxPtr(new SelectionBox(label));
		selectionBox->setDetached(true);
		selectionBox->setVisible(false);
		selectionBox->position = CoordPerPX(0.0f, 0.0f, 2.0f, 0.0f);
		selectionBox->dimensions = CoordPerPX(1.0f, 0.8f, -4.0f, 0.0f);
		selectionBox->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
		selectionBox->borderColor = MakeColor(
			AlloyApplicationContext()->theme.HIGHLIGHT);
		selectionBox->borderWidth = UnitPX(1.0f);
		selectionBox->textColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT_TEXT);
		selectionBox->textAltColor = MakeColor(
			AlloyApplicationContext()->theme.DARK_TEXT);
		add(selectionBox);
		selectionBox->onSelect = [this](SelectionBox* box) {
			selectionBox->setVisible(false);
			AlloyApplicationContext()->removeOnTopRegion(box);
			std::string path = GetParentDirectory(lastValue);
			this->setValue(path + box->getSelection(box->getSelectedIndex()));
			if (onTextEntered) {
				onTextEntered(this);
			}
			return false;
		};
	}
	void FileField::setValue(const std::string& text) {
		const int PREFERRED_TEXT_WIDTH = 32;
		if (text != this->value) {
			this->value = text;
			if (text.size() == 0) {
				label = name;
			}
			else {
				if (text.size() > PREFERRED_TEXT_WIDTH) {
					this->label = std::string("..") + ALY_PATH_SEPARATOR + GetFileName(text);
				}
				else {
					this->label = text;
				}
			}
			segmentedPath = SplitPath(value);
			textStart = 0;
			moveCursorTo((int)value.size());
		}
	}
	void FileField::updateSuggestionBox(AlloyContext* context, bool forceValue) {
		showCursor = true;
		std::string root = GetParentDirectory(value);
		std::vector<std::string> listing = GetDirectoryListing(root);
		std::vector<std::string> suggestions = AutoComplete(value, listing);
		if (suggestions.size() == 1 && forceValue) {
			if (IsDirectory(suggestions[0])) {
				this->setValue(
					RemoveTrailingSlash(suggestions[0]) + ALY_PATH_SEPARATOR);
			}
			else {
				this->setValue(suggestions[0]);
			}
			context->removeOnTopRegion(selectionBox.get());
			selectionBox->setVisible(false);
		}
		else {
			std::vector<std::string>& labels = selectionBox->options;
			labels.clear();
			for (std::string f : suggestions) {
				if (IsDirectory(f)) {
					labels.push_back(
						GetFileName(f) + ALY_PATH_SEPARATOR);
				}
				else {
					labels.push_back(GetFileName(f));
				}
			}
			if (labels.size() > 0) {
				context->setOnTopRegion(selectionBox.get());
				lastValue = this->getValue();
				box2px bounds = getBounds(false);
				selectionBox->pack(bounds.position,
					bounds.dimensions, context->dpmm,
					context->pixelRatio);
				selectionBox->setVisible(true);
				selectionBox->setSelectionOffset(0);
				selectionBox->setSelectedIndex(0);
			}
			else {
				context->removeOnTopRegion(selectionBox.get());
				selectionBox->setVisible(false);
			}
		}
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
				showTimer.reset();
				if (showTimer.get() == nullptr) {
					showTimer = std::shared_ptr<Timer>(new Timer([this, context] {
						updateSuggestionBox(context, false);
					}, nullptr, 500, 30));
				}
				showTimer->execute();
				break;
			case InputType::Key:
				if (e.isDown()) {
					if (e.key == GLFW_KEY_TAB) {
						updateSuggestionBox(context, true);
						break;
					}
					else if (e.key == GLFW_KEY_ENTER) {
						selectionBox->setVisible(false);
						showTimer.reset();
					}
				}
				handleKeyInput(context, e);
				if (e.isDown()) {
					if (e.key == GLFW_KEY_BACKSPACE) {
						showTimer.reset();
						if (showTimer.get() == nullptr) {
							showTimer = std::shared_ptr<Timer>(
								new Timer([this, context] {
								updateSuggestionBox(context, false);
							}, nullptr, 500, 30));
						}
						showTimer->execute();
					}
				}
				break;
			case InputType::Cursor:
				handleCursorInput(context, e);
				break;
			case InputType::Unspecified:
				break;
			case InputType::Scroll:
				break;
			}
			segmentedPath = SplitPath(value);
		}
		return Region::onEventHandler(context, e);
	}
	void FileField::draw(AlloyContext* context) {

		float ascender, descender, lineh;
		std::vector<NVGglyphPosition> positions(value.size());
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		bool hover = context->isMouseOver(this);
		float x = bounds.position.x;
		float y = bounds.position.y;
		float w = bounds.dimensions.x;
		float h = bounds.dimensions.y;
		bool isFocused = context->isFocused(this);
		if (!isFocused) {
			showDefaultLabel = true;
		}
		if (backgroundColor->a > 0) {
			if (hover) {
				nvgBeginPath(nvg);
				nvgRoundedRect(nvg, x, y, w, h, context->theme.CORNER_RADIUS);
				nvgFillColor(nvg, *backgroundColor);
				nvgFill(nvg);

			}
			else {
				nvgBeginPath(nvg);
				nvgRoundedRect(nvg, x + 1, y + 1, w - 2, h - 2,
					context->theme.CORNER_RADIUS);
				nvgFillColor(nvg, *backgroundColor);
				nvgFill(nvg);
			}
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
		pushScissor(nvg, x + PADDING, y, w - 2 * PADDING, h);
		nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		positions.resize(value.size() + 1);
		nvgTextGlyphPositions(nvg, 0, textY, value.data(),
			value.data() + value.size(), positions.data(),
			(int)positions.size());

		float fwidth = (w - 3.0f * PADDING - 2.0f * lineWidth);
		if (cursorStart > 0) {
			if (positions[cursorStart - 1].maxx - positions[textStart].minx
				> fwidth) {
				while (positions[cursorStart - 1].maxx
					> positions[textStart].minx + fwidth) {
					if (textStart >= (int)positions.size() - 1)
						break;
					textStart++;
				}
			}
		}
		if (!showDefaultLabel) {
			textOffsetX = textOffsetX - positions[textStart].minx;
		}

		if (cursorEnd != cursorStart && isFocused) {
			int lo = std::min(cursorEnd, cursorStart);
			int hi = std::max(cursorEnd, cursorStart);
			float x0 = textOffsetX + (lo ? positions[lo - 1].maxx - 1 : 0);
			float x1 = textOffsetX + (hi ? positions[hi - 1].maxx - 1 : 0);
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
			nvgFillColor(nvg, context->theme.DARK);
			nvgText(nvg, textOffsetX, textY + h / 2, label.c_str(), NULL);
		}
		else {
			float xOffset = textOffsetX;
			std::stringstream path;
			for (std::string comp : segmentedPath) {
				path << comp;
				if (comp == ALY_PATH_SEPARATOR) {
					nvgFillColor(nvg, context->theme.DARK);
				}
				else {
					if (FileExists(path.str())) {
						nvgFillColor(nvg, context->theme.LINK);
					}
					else {
						nvgFillColor(nvg, context->theme.DARK);
					}
				}
				nvgText(nvg, xOffset, textY + h / 2, comp.c_str(), NULL);
				xOffset += nvgTextBounds(nvg, 0, textY, comp.c_str(), nullptr,
					nullptr);
			}
		}

		if (isFocused && showCursor) {
			nvgBeginPath(nvg);
			float xOffset = textOffsetX
				+ (cursorStart ? positions[cursorStart - 1].maxx - 1 : 0);
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
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
				nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth);
			}
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}
		if (glyph.get() != nullptr) {
			box2px b = bounds;
			b.position.x = bounds.position.x + lineWidth * 0.5f;
			b.position.y = bounds.position.y + lineWidth * 0.5f;
			b.dimensions.x = bounds.dimensions.x - lineWidth;
			b.dimensions.y = bounds.dimensions.y - lineWidth;
			glyph->draw(b, *foregroundColor, *backgroundColor, context);
		}

		if (borderColor->a > 0) {

			nvgLineJoin(nvg, NVG_ROUND);
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
			}
			else {
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

	std::shared_ptr<GlyphRegion> MakeGlyphRegion(
		const std::shared_ptr<AwesomeGlyph>& glyph, const AUnit2D& position,
		const AUnit2D& dimensions, const Color& bgColor, const Color& fgColor,
		const Color& borderColor, const AUnit1D& borderWidth) {
		std::shared_ptr<GlyphRegion> region = std::shared_ptr<GlyphRegion>(
			new GlyphRegion(glyph->name));
		region->glyph = glyph;
		region->position = position;
		region->dimensions = dimensions;
		region->backgroundColor = MakeColor(bgColor);
		region->foregroundColor = MakeColor(fgColor);
		region->borderColor = MakeColor(borderColor);
		region->borderWidth = borderWidth;
		region->setAspectRule(AspectRule::FixedHeight);
		region->setAspectRatio(glyph->width / (float)glyph->height);
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
		region->position = position;
		region->dimensions = dimensions;
		region->backgroundColor = MakeColor(bgColor);
		region->foregroundColor = MakeColor(fgColor);
		region->borderColor = MakeColor(borderColor);
		region->borderWidth = borderWidth;
		region->setAspectRule(aspectRatio);
		region->setAspectRatio(glyph->width / (float)glyph->height);
		return region;
	}
	std::shared_ptr<TextLabel> MakeTextLabel(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const FontType& fontType, const AUnit1D& fontSize,
		const Color& fontColor, const HorizontalAlignment& halign,
		const VerticalAlignment& valign) {
		std::shared_ptr<TextLabel> region = std::shared_ptr<TextLabel>(
			new TextLabel(name));
		region->position = position;
		region->dimensions = dimensions;
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
		region->position = position;
		region->dimensions = dimensions;
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
		region->position = position;
		region->dimensions = dimensions;
		region->backgroundColor = MakeColor(bgColor);
		region->borderColor = MakeColor(borderColor);
		region->borderWidth = borderWidth;
		return region;
	}
	std::shared_ptr<Composite> MakeComposite(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const Color& bgColor, const Color& borderColor,
		const AUnit1D& borderWidth, const Orientation& orientation) {
		std::shared_ptr<Composite> composite = std::shared_ptr<Composite>(
			new Composite(name));
		composite->position = position;
		composite->dimensions = dimensions;
		composite->backgroundColor = MakeColor(bgColor);
		composite->borderColor = MakeColor(borderColor);
		composite->borderWidth = borderWidth;
		composite->setOrientation(orientation);
		return composite;
	}

	box2px SelectionBox::getBounds(bool includeBounds) const {
		box2px bounds = Region::getBounds(includeBounds);
		AlloyContext* context = AlloyApplicationContext().get();
		int elements =
			(maxDisplayEntries > 0) ?
			std::min(maxDisplayEntries, (int)options.size()) :
			(int)options.size();
		float entryHeight = std::min(context->height() / (float)elements,
			bounds.dimensions.y);
		float boxHeight = (elements)* entryHeight;
		float parentHeight =
			(parent != nullptr) ? parent->getBoundsDimensionsY() : 0.0f;
		float yOffset = std::min(bounds.position.y + boxHeight + parentHeight,
			(float)context->height()) - boxHeight;
		box2px bbox;

		bbox.position = pixel2(bounds.position.x, yOffset);
		bbox.dimensions = pixel2(bounds.dimensions.x, boxHeight);
		return bbox;
	}
	void SelectionBox::draw(AlloyContext* context) {

		context->setDragObject(this);
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		box2px sbounds = bounds;
		sbounds.position.x += TextField::PADDING;
		sbounds.dimensions.x -= 2 * TextField::PADDING;

		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		int elements =
			(maxDisplayEntries > 0) ?
			std::min(maxDisplayEntries, (int)options.size()) :
			(int)options.size();

		float entryHeight = bounds.dimensions.y / elements;
		if (backgroundColor->a > 0) {
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}

		float th = entryHeight - TextField::PADDING;
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

		int N = (int)options.size();

		if (maxDisplayEntries >= 0) {
			N = std::min(selectionOffset + maxDisplayEntries, (int)options.size());
		}
		int newSelectedIndex = -1;
		for (index = selectionOffset; index < N; index++) {
			if (context->isMouseContainedIn(bounds.position + offset,
				pixel2(bounds.dimensions.x, entryHeight))) {
				newSelectedIndex = index;
				break;
			}
			offset.y += entryHeight;
		}
		if (newSelectedIndex >= 0) {
			selectedIndex = newSelectedIndex;
		}
		offset = pixel2(0, 0);

		for (index = selectionOffset; index < N; index++) {
			std::string& label = options[index];
			if (index == selectedIndex) {
				nvgBeginPath(nvg);
				nvgRect(nvg, bounds.position.x + offset.x,
					bounds.position.y + offset.y, bounds.dimensions.x,
					entryHeight);
				nvgFillColor(nvg, context->theme.NEUTRAL);
				nvgFill(nvg);
				nvgFillColor(nvg, *textAltColor);
			}
			else {
				nvgFillColor(nvg, *textColor);
			}
			pushScissor(nvg, sbounds);
			nvgText(nvg,
				bounds.position.x + offset.x + lineWidth + TextField::PADDING,
				bounds.position.y + entryHeight / 2 + offset.y, label.c_str(),
				nullptr);
			popScissor(nvg);
			offset.y += entryHeight;
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

		box2px downArrowBox(
			pixel2(
				bounds.position.x + bounds.dimensions.x
				- downArrow->width / 2,
				bounds.position.y + bounds.dimensions.y
				- downArrow->height / 2),
			pixel2(downArrow->width, downArrow->height));
		box2px upArrowBox(
			pixel2(
				bounds.position.x + bounds.dimensions.x
				- downArrow->width / 2,
				bounds.position.y - downArrow->height / 2),
			pixel2(downArrow->width, downArrow->height));

		if (maxDisplayEntries >= 0 && (int)options.size() > maxDisplayEntries) {
			if (selectionOffset > 0) {
				nvgBeginPath(nvg);
				nvgFillColor(nvg, context->theme.DARK);
				nvgCircle(nvg, bounds.position.x + bounds.dimensions.x,
					bounds.position.y, upArrow->height / 2);
				nvgFill(nvg);
				upArrow->draw(upArrowBox, context->theme.HIGHLIGHT, COLOR_NONE,
					context);
				if (upArrowBox.contains(
					AlloyApplicationContext()->cursorPosition)) {
					if (selectionOffset > 0) {
						selectionOffset--;
					}
				}
			}

			if (selectionOffset < (int)options.size() - maxDisplayEntries) {
				nvgBeginPath(nvg);
				nvgFillColor(nvg, context->theme.DARK);
				nvgCircle(nvg, bounds.position.x + bounds.dimensions.x,
					bounds.position.y + bounds.dimensions.y,
					downArrow->height / 2);
				nvgFill(nvg);
				downArrow->draw(downArrowBox, context->theme.HIGHLIGHT, COLOR_NONE,
					context);
				if (downArrowBox.contains(
					AlloyApplicationContext()->cursorPosition)) {
					if (selectionOffset
						< (int)options.size() - maxDisplayEntries) {
						selectionOffset++;
					}
				}
			}
		}

	}
	void SelectionBox::setSelectedIndex(int index) {
		selectedIndex = index;
		if (index < 0) {
			label = name;
			selectionOffset = 0;
		}
		else {
			label = options[selectedIndex];

		}
	}
	SelectionBox::SelectionBox(const std::string& name,
		const std::vector<std::string>& labels) :
		Region(name), label(name), options(labels) {

		downArrow = AlloyApplicationContext()->createAwesomeGlyph(0xf0ab,
			FontStyle::Normal, 14);
		upArrow = AlloyApplicationContext()->createAwesomeGlyph(0xf0aa,
			FontStyle::Normal, 14);
		onEvent =
			[this](AlloyContext* context, const InputEvent& event) {
			if (context->isOnTop(this) && this->isVisible()) {
				if (event.type == InputType::Key&&event.isDown()) {
					if (event.key == GLFW_KEY_UP) {
						if (selectedIndex < 0) {
							this->setSelectedIndex((int)options.size() - 1);
						}
						else {
							this->setSelectedIndex(std::max(0, selectedIndex - 1));
						}
						if (maxDisplayEntries >= 0 && selectedIndex>0 && (selectedIndex < selectionOffset || selectedIndex >= selectionOffset + maxDisplayEntries)) {
							selectionOffset = std::max(0, selectedIndex + 1 - maxDisplayEntries);
						}
						return true;
					}
					else if (event.key == GLFW_KEY_DOWN) {
						if (selectedIndex < 0) {
							this->setSelectedIndex(0);
						}
						else {
							this->setSelectedIndex(std::min((int)options.size() - 1, selectedIndex + 1));
						}
						if (maxDisplayEntries >= 0 && selectedIndex>0 && (selectedIndex < selectionOffset || selectedIndex >= selectionOffset + maxDisplayEntries)) {
							selectionOffset = std::max(0, selectedIndex + 1 - maxDisplayEntries);
						}
						return true;
					}
					else if (event.key == GLFW_KEY_PAGE_UP) {
						if (maxDisplayEntries > 0) {
							selectionOffset = 0;
							scrollingUp = false;
							return true;
						}
					}
					else if (event.key == GLFW_KEY_PAGE_DOWN) {
						if (maxDisplayEntries > 0) {
							selectionOffset = (int)options.size() - maxDisplayEntries;
							scrollingDown = false;
							return true;
						}
					}
					else if (event.key == GLFW_KEY_ESCAPE) {
						setSelectedIndex(-1);
						AlloyApplicationContext()->removeOnTopRegion(this);
						this->setVisible(false);
					}
					else if (event.key == GLFW_KEY_ENTER) {
						if (selectedIndex >= 0) {
							if (this->onSelect) {
								return this->onSelect(this);
							}
						}
					}
				}
				else if (event.type == InputType::MouseButton&&event.isDown() && event.button == GLFW_MOUSE_BUTTON_LEFT) {
					if (AlloyApplicationContext()->isMouseOver(this)) {
						if (selectedIndex >= 0) {
							if (this->onSelect) {
								return this->onSelect(this);
							}
						}
						return true;
					}
					else {
						setSelectedIndex(-1);
						AlloyApplicationContext()->removeOnTopRegion(this);
						this->setVisible(false);
					}
				}
				else if (event.type == InputType::MouseButton&&event.isDown() && event.button == GLFW_MOUSE_BUTTON_RIGHT) {
					setSelectedIndex(-1);
					AlloyApplicationContext()->removeOnTopRegion(this);
					this->setVisible(false);
				}
				else if (event.type == InputType::Scroll) {
					if (maxDisplayEntries >= 0) {
						if ((int)options.size() > maxDisplayEntries) {
							if (downTimer.get() != nullptr) {
								scrollingDown = false;
								downTimer.reset();
							}
							if (upTimer.get() != nullptr) {
								scrollingUp = false;
								upTimer.reset();
							}
							selectionOffset = aly::clamp(selectionOffset - (int)event.scroll.y, 0, (int)options.size() - maxDisplayEntries);
							return true;
						}
					}
				}
				else if (event.type == InputType::Cursor && (int)options.size() > maxDisplayEntries) {
					box2px bounds = this->getBounds();
					int elements =
						(maxDisplayEntries > 0) ? std::min(maxDisplayEntries, (int)options.size()) : (int)options.size();
					float entryHeight = bounds.dimensions.y / elements;

					box2px lastBounds = bounds, firstBounds = bounds;
					lastBounds.position.y = bounds.position.y + bounds.dimensions.y - entryHeight;
					lastBounds.dimensions.y = entryHeight;
					firstBounds.dimensions.y = entryHeight;
					if (lastBounds.contains(event.cursor)) {
						if (downTimer.get() == nullptr) {
							downTimer = std::shared_ptr<Timer>(new Timer([this] {
								double deltaT = 200;
								scrollingDown = true;
								while (scrollingDown&&selectionOffset < (int)options.size() - maxDisplayEntries) {
									this->selectionOffset++;
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
					if (firstBounds.contains(event.cursor)) {
						if (upTimer.get() == nullptr) {
							upTimer = std::shared_ptr<Timer>(new Timer([this] {
								double deltaT = 200;
								scrollingUp = true;
								while (scrollingUp&&selectionOffset > 0) {
									this->selectionOffset--;
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
			}

			return false;
		};
		Application::addListener(this);
	}


	box2px Menu::getBounds(bool includeBounds) const {
		box2px bounds = Region::getBounds(includeBounds);
		AlloyContext* context = AlloyApplicationContext().get();
		int elements =
			(maxDisplayEntries > 0) ?
			std::min(maxDisplayEntries, (int)options.size()) :
			(int)options.size();
		float entryHeight = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y, context->pixelRatio);
		float boxHeight = (elements)* entryHeight;
		float yOffset = std::min(bounds.position.y + boxHeight, (float)context->height()) - boxHeight;
		box2px bbox;
		bbox.position = pixel2(bounds.position.x, yOffset);
		bbox.dimensions = pixel2(bounds.dimensions.x, boxHeight);
		return bbox;
	}
	void Menu::draw(AlloyContext* context) {
		context->setDragObject(this);
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		box2px sbounds = bounds;
		sbounds.position.x += TextField::PADDING;
		sbounds.dimensions.x -= 2 * TextField::PADDING;
		pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		int elements =
			(maxDisplayEntries > 0) ?
			std::min(maxDisplayEntries, (int)options.size()) :
			(int)options.size();

		float entryHeight = bounds.dimensions.y / elements;
		if (backgroundColor->a > 0) {
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}

		float th = entryHeight - TextField::PADDING;
		nvgFontSize(nvg, th);
		nvgFillColor(nvg, *textColor);

		float tw = 0.0f;
		for (MenuItemPtr& label : options) {
			tw = std::max(tw,
				nvgTextBounds(nvg, 0, 0, label->name.c_str(), nullptr, nullptr));
		}
		nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		pixel2 offset(0, 0);

		nvgFillColor(nvg, context->theme.DARK);
		int index = 0;
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));

		int N = (int)options.size();

		if (maxDisplayEntries >= 0) {
			N = std::min(selectionOffset + maxDisplayEntries, (int)options.size());
		}
		int newSelectedIndex = -1;
		for (index = selectionOffset; index < N; index++) {
			std::shared_ptr<MenuItem> current = options[index];
			if (context->isMouseContainedIn(bounds.position + offset,
				pixel2(bounds.dimensions.x, entryHeight))) {
				newSelectedIndex = index;
				if (current->isMenu()) {
					current->position = CoordPX(offset.x + bounds.dimensions.x, offset.y);
				}
			}
			offset.y += entryHeight;
		}
		if (newSelectedIndex >= 0) {
			setVisibleItem(options[newSelectedIndex]);
			selectedIndex = newSelectedIndex;
		}
		else {
			currentSelected = nullptr;
		}
		offset = pixel2(0, 0);
		const std::string rightArrow = CodePointToUTF8(0xf0da);
		for (index = selectionOffset; index < N; index++) {
			MenuItemPtr& label = options[index];
			if (index == selectedIndex) {
				nvgBeginPath(nvg);
				nvgRect(nvg, bounds.position.x + offset.x,
					bounds.position.y + offset.y, bounds.dimensions.x,
					entryHeight);
				nvgFillColor(nvg, context->theme.DARK);
				nvgFill(nvg);

				nvgFillColor(nvg, *textAltColor);
			}
			else {
				nvgFillColor(nvg, *textColor);
			}
			pushScissor(nvg, sbounds);
			nvgText(nvg,
				bounds.position.x + offset.x + lineWidth + TextField::PADDING,
				bounds.position.y + entryHeight / 2 + offset.y, label->name.c_str(),
				nullptr);
			popScissor(nvg);
			if (label->isMenu()) {
				nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
				nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
				nvgText(nvg,
					bounds.position.x + bounds.dimensions.x - lineWidth - TextField::PADDING + offset.x,
					bounds.position.y + entryHeight / 2 + offset.y, rightArrow.c_str(),
					nullptr);
				nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));
			}
			//Right carret 0xf0da
			offset.y += entryHeight;
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

		box2px downArrowBox(
			pixel2(
				bounds.position.x + bounds.dimensions.x
				- downArrow->width / 2,
				bounds.position.y + bounds.dimensions.y
				- downArrow->height / 2),
			pixel2(downArrow->width, downArrow->height));
		box2px upArrowBox(
			pixel2(
				bounds.position.x + bounds.dimensions.x
				- downArrow->width / 2,
				bounds.position.y - downArrow->height / 2),
			pixel2(downArrow->width, downArrow->height));

		if (maxDisplayEntries >= 0 && (int)options.size() > maxDisplayEntries) {
			if (selectionOffset > 0) {
				nvgBeginPath(nvg);
				nvgFillColor(nvg, context->theme.DARK);
				nvgCircle(nvg, bounds.position.x + bounds.dimensions.x,
					bounds.position.y, upArrow->height / 2);
				nvgFill(nvg);
				upArrow->draw(upArrowBox, context->theme.HIGHLIGHT, COLOR_NONE,
					context);
				if (upArrowBox.contains(
					AlloyApplicationContext()->cursorPosition)) {
					if (selectionOffset > 0) {
						selectionOffset--;
					}
				}
			}

			if (selectionOffset < (int)options.size() - maxDisplayEntries) {
				nvgBeginPath(nvg);
				nvgFillColor(nvg, context->theme.DARK);
				nvgCircle(nvg, bounds.position.x + bounds.dimensions.x,
					bounds.position.y + bounds.dimensions.y,
					downArrow->height / 2);
				nvgFill(nvg);
				downArrow->draw(downArrowBox, context->theme.HIGHLIGHT, COLOR_NONE,
					context);
				if (downArrowBox.contains(
					AlloyApplicationContext()->cursorPosition)) {
					if (selectionOffset
						< (int)options.size() - maxDisplayEntries) {
						selectionOffset++;
					}
				}
			}
		}
		for (std::shared_ptr<Region>& region : children) {
			if (region->isVisible()) {
				region->draw(context);
			}
		}
	}
	void MenuItem::setVisibleItem(const std::shared_ptr<MenuItem>& item, bool forceShow) {
		std::lock_guard<std::mutex> lockMe(showLock);
		currentSelected = item;
		if (item.get() != requestedSelected.get()) {
			if (forceShow) {
				showTimer.reset();
				if (currentVisible.get() != nullptr&&currentVisible.get() != item.get()) {
					currentVisible->setVisible(false);
				}
				item->setVisible(true);
				currentVisible = item;
				AlloyApplicationContext()->requestPack();
			}
			else {
				showTimer = std::shared_ptr<Timer>(new Timer([=] {
					std::lock_guard<std::mutex> lockMe(showLock);
					if (currentSelected.get() == item.get()) {
						if (currentVisible.get() != nullptr&&currentVisible.get() != item.get()) {
							currentVisible->setVisible(false);
						}
						item->setVisible(true);
						currentVisible = item;
					}
					if (item.get() == requestedSelected.get()) {
						requestedSelected = nullptr;
					}
					AlloyApplicationContext()->requestPack();
				}, [=]() {
					if (item.get() == requestedSelected.get()) {
						requestedSelected = nullptr;
					}
				}, MENU_DISPLAY_DELAY, 30));
				requestedSelected = item;
				showTimer->execute();
			}
		}
	}
	void MenuBar::setVisibleItem(const std::shared_ptr<MenuItem>& item, bool forceShow) {
		std::lock_guard<std::mutex> lockMe(showLock);
		currentSelected = item;
		if (item.get() != requestedSelected.get()) {
			if (forceShow) {
				showTimer.reset();
				if (currentVisible.get() != nullptr&&currentVisible.get() != item.get()) {
					AlloyApplicationContext()->removeOnTopRegion(currentVisible.get());
				}
				AlloyApplicationContext()->setOnTopRegion(item.get());
				item->setVisible(true);
				currentVisible = item;
				AlloyApplicationContext()->requestPack();
			}
			else {
				showTimer = std::shared_ptr<Timer>(new Timer([=] {
					std::lock_guard<std::mutex> lockMe(showLock);
					if (currentSelected.get() == item.get()) {
						if (currentVisible.get() != nullptr&&currentVisible.get() != item.get()) {
							AlloyApplicationContext()->removeOnTopRegion(currentVisible.get());
						}
						AlloyApplicationContext()->setOnTopRegion(item.get());
						item->setVisible(true);
						currentVisible = item;
						AlloyApplicationContext()->requestPack();
					}
					if (item.get() == requestedSelected.get()) {
						requestedSelected = nullptr;
					}
				}, [=]() {
					if (item.get() == requestedSelected.get()) {
						requestedSelected = nullptr;
					}
				}, MENU_DISPLAY_DELAY, 30));
				requestedSelected = item;
				showTimer->execute();
			}
		}
	}
	void Menu::addItem(const std::shared_ptr<MenuItem>& selection) {
		options.push_back(selection);
		if (selection->isMenu()) {
			Composite::add(selection);
		}
	}
	void Menu::setSelectedIndex(int index) {
		selectedIndex = index;
		if (index < 0) {
			label = MenuItemPtr();
			selectionOffset = 0;
		}
		else {
			label = options[selectedIndex];
		}
	}
	void Menu::fireEvent(int selectedIndex) {
		if (selectedIndex >= 0) {
			if (options[selectedIndex]->onSelect) {
				options[selectedIndex]->onSelect();
			}
		}
	}
	void MenuItem::setVisible(bool visible) {
		if (!visible) {
			if (isVisible()) {
				currentVisible = nullptr;
				currentSelected = nullptr;
				requestedSelected = nullptr;
				if (parent != nullptr) {
					MenuItem* mitem = dynamic_cast<MenuItem*>(parent);
					if (mitem) {
						if (mitem->currentVisible.get() == this) {
							mitem->currentVisible = nullptr;
						}
						if (mitem->requestedSelected.get() == this) {
							mitem->requestedSelected = nullptr;
						}
					}
				}
			}
		}
		else if (!isVisible()) {
			currentVisible = nullptr;
			currentSelected = nullptr;
			requestedSelected = nullptr;
		}
		Composite::setVisible(visible);
	}
	void Menu::setVisible(bool visible) {
		if (!visible) {
			setSelectedIndex(-1);
			for (MenuItemPtr& item : options) {
				if (item->isMenu()) {
					item->setVisible(false);
				}
			}
		}
		MenuItem::setVisible(visible);
	}

	Menu::Menu(const std::string& name,float menuWidth,const std::vector<std::shared_ptr<MenuItem>>& labels) :
		MenuItem(name, CoordPerPX(0.0f, 0.0f, 0.0f, 0.0f), CoordPerPX(0.0f, 0.0f, menuWidth, 0.0f)), options(labels) {
		setDetached(true);
		setVisible(false);
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
		borderColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
		borderWidth = UnitPX(1.0f);
		textColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
		textAltColor = MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT);
		downArrow = AlloyApplicationContext()->createAwesomeGlyph(0xf0ab,
			FontStyle::Normal, 14);
		upArrow = AlloyApplicationContext()->createAwesomeGlyph(0xf0aa,
			FontStyle::Normal, 14);
		onRemoveFromOnTop = [=] {
			setVisible(false);
		};
		onEvent =
			[this](AlloyContext* context, const InputEvent& event) {
			if (context->isOnTop(this)&&this->isVisible()) {
				if (event.type == InputType::MouseButton&&event.isDown() && event.button == GLFW_MOUSE_BUTTON_LEFT) {
					if (getSelectedIndex()>=0) {
						fireEvent(selectedIndex);
						return true;
					}
					else {
						setSelectedIndex(-1);
						AlloyApplicationContext()->removeOnTopRegion(this);
					}
				}
				else if (event.type == InputType::MouseButton&&event.isDown() && event.button == GLFW_MOUSE_BUTTON_RIGHT) {
					setSelectedIndex(-1);
					AlloyApplicationContext()->removeOnTopRegion(this);
				}
				else if (event.type == InputType::Cursor && (int)options.size() > maxDisplayEntries) {
					box2px bounds = this->getBounds();
					int elements =
						(maxDisplayEntries > 0) ? std::min(maxDisplayEntries, (int)options.size()) : (int)options.size();
					float entryHeight = bounds.dimensions.y / elements;

					box2px lastBounds = bounds, firstBounds = bounds;
					lastBounds.position.y = bounds.position.y + bounds.dimensions.y - entryHeight;
					lastBounds.dimensions.y = entryHeight;
					firstBounds.dimensions.y = entryHeight;
					if (lastBounds.contains(event.cursor)) {
						if (downTimer.get() == nullptr) {
							downTimer = std::shared_ptr<Timer>(new Timer([this] {
								double deltaT = 200;
								scrollingDown = true;
								while (scrollingDown&&selectionOffset < (int)options.size() - maxDisplayEntries) {
									this->selectionOffset++;
									std::this_thread::sleep_for(std::chrono::milliseconds((long)deltaT));
									deltaT = std::max(30.0, 0.75*deltaT);
								}
							}, nullptr, MENU_DISPLAY_DELAY, 30));
							downTimer->execute();
						}
					}
					else {
						if (downTimer.get() != nullptr) {
							scrollingDown = false;
							downTimer.reset();
						}
					}
					if (firstBounds.contains(event.cursor)) {
						if (upTimer.get() == nullptr) {
							upTimer = std::shared_ptr<Timer>(new Timer([this] {
								double deltaT = 200;
								scrollingUp = true;
								while (scrollingUp&&selectionOffset > 0) {
									this->selectionOffset--;
									std::this_thread::sleep_for(std::chrono::milliseconds((long)deltaT));
									deltaT = std::max(30.0, 0.75*deltaT);
								}
							}, nullptr, MENU_DISPLAY_DELAY, 30));
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
				else if (event.type == InputType::Cursor) {
					if (!context->isMouseOver(this)) {
						setSelectedIndex(-1);
					}
				}
			}

			return false;
		};
		Application::addListener(this);
	}
	MenuBar::MenuBar(const std::string& name, const AUnit2D& position, const AUnit2D& dimensions) :MenuItem(name, position, dimensions) {

		active = false;
		barRegion = CompositePtr(new Composite("Bar Region", CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)));
		Composite::add(barRegion);
		barRegion->setOrientation(Orientation::Horizontal);
		//barRegion->cellSpacing.x = 2;
		//barRegion->cellPadding.x = 2;
		this->backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	}
	void MenuBar::add(const std::shared_ptr<Menu>& menu) {
		MenuHeaderPtr header = MenuHeaderPtr(new MenuHeader(menu, CoordPerPX(0.0f, 1.0f, 0.0f, -30.0f), CoordPX(100, 30)));

		menu->position = CoordPX(0.0f, 30.0f);
		menu->setDetached(true);
		Composite::add(menu);

		headers.push_back(header);
		barRegion->add(header);
		header->onMouseOver = [=](AlloyContext* context, const InputEvent& e) {
			if(currentVisible.get()!=nullptr){
				setVisibleItem(menu);
			}
			return true;
		};
		header->onMouseDown = [=](AlloyContext* context, const InputEvent& e) {
			setVisibleItem(menu, true);
			return true;
		};
	}
	MenuItem::MenuItem(const std::string& name) :Composite(name) {

	}
	MenuItem::MenuItem(const std::string& name, const AUnit2D& position, const AUnit2D& dimensions) : Composite(name, position, dimensions) {
	}

	MenuHeader::MenuHeader(const std::shared_ptr<Menu>& menu, const AUnit2D& position, const AUnit2D& dimensions) : MenuItem(menu->name, position, dimensions), menu(menu) {
		backgroundAltColor = MakeColor(AlloyApplicationContext()->theme.HIGHLIGHT);
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		textAltColor = MakeColor(AlloyApplicationContext()->theme.DARK_TEXT);
		textColor = MakeColor(AlloyApplicationContext()->theme.LIGHT_TEXT);
		borderColor = MakeColor(COLOR_NONE);
		borderWidth = UnitPX(0.0f);
		fontSize = UnitPerPX(1.0f, -10);
		this->aspectRule = AspectRule::FixedHeight;
	}

	void MenuHeader::draw(AlloyContext* context) {
		bool hover = context->isMouseOver(this) || menu->isVisible();
		bool down = context->isMouseDown(this);
		NVGcontext* nvg = context->nvgContext;
		box2px bounds = getBounds();
		menu->position = CoordPX(bounds.position.x, bounds.position.y + bounds.dimensions.y);

		int xoff = 0;
		int yoff = 0;
		int vshift = 5;
		if (down) {
			xoff = 2;
			yoff = 2;
		}
		if (hover) {
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff + vshift,
				bounds.dimensions.x, bounds.dimensions.y);
			nvgFillColor(nvg, *backgroundAltColor);
			nvgFill(nvg);

		}
		else {
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x + 1, bounds.position.y + 1 + vshift,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2);
			nvgFillColor(nvg, *backgroundColor);
			nvgFill(nvg);
		}



		float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
		nvgFontSize(nvg, th);
		if (hover) {
			nvgFillColor(nvg, *textAltColor);
		}
		else {
			nvgFillColor(nvg, *textColor);
		}
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		float tw = nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr);
		this->aspectRatio = (tw + 10.0f) / (th + 10.0f);
		nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
		pixel2 offset(0, 0);
		nvgText(nvg, bounds.position.x + bounds.dimensions.x / 2 + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff + vshift, name.c_str(),
			nullptr);
	}
};

