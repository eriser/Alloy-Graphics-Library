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

#ifndef ALLOYUI_H_
#define ALLOYUI_H_
#include "AlloyMath.h"
#include "AlloyContext.h"
#include "AlloyUnits.h"
#include "nanovg.h"
#include  "GLTexture.h"
#include <iostream>
#include <memory>
#include <list>
#include <vector>
namespace aly {
bool SANITY_CHECK_UI();

struct Composite;
struct BorderComposite;
struct Region: public EventHandler {
private:
	box2px bounds;
protected:
	void drawBoundsLabel(AlloyContext* context, const std::string& name,
			int font);
	Region* mouseOverRegion = nullptr;
	Region* mouseDownRegion = nullptr;
	static uint64_t REGION_COUNTER;
	bool visible = true;
	bool dragEnabled = false;
	bool ignoreCursorEvents=false;
	Origin origin = Origin::TopLeft;
	pixel2 dragOffset = pixel2(0, 0);

	AspectRule aspectRule = AspectRule::Unspecified;
	double aspectRatio = -1.0; //Less than zero indicates undetermined. Will be computed at next pack() event.
	AUnit2D position = CoordPercent(0.0f, 0.0f);
	AUnit2D dimensions = CoordPercent(1.0f, 1.0f);
	bool roundCorners=false;
	bool detached=false;
public:
	friend struct Composite;
	friend struct BorderComposite;
	void setIgnoreCursorEvents(bool ignore){
		ignoreCursorEvents=ignore;
	}
	void setDetacted(bool enable){
		detached=enable;
	}
	inline void setRoundCorners(bool round){
		this->roundCorners=round;
	}
	inline bool hasParent(Region* region) const {
		return (parent!=nullptr&&(parent==region||parent->hasParent(region)));
	}
	inline bool isDetached() const {
		return (parent!=nullptr&&parent->isDetached())||detached;
	}
	std::function<bool(AlloyContext*, const InputEvent& event)> onEvent;
	const std::string name;
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
	virtual inline bool isScrollEnabled() const {
		return false;
	}
	virtual inline pixel2 drawOffset() const {
		if (parent != nullptr) {
			return parent->drawOffset();
		} else {
			return pixel2(0, 0);
		}
	}
	virtual Region* locate(const pixel2& cursor);
	inline void setPosition(const AUnit2D& pt) {
		position = pt;
	}
	inline void setDimensions(const AUnit2D& dims) {
		dimensions = dims;
	}
	inline void setAspectRule(const AspectRule& aspect) {
		aspectRule = aspect;
	}
	inline void setAspectRatio(double val) {
		aspectRatio = val;
	}
	inline void setBounds(const AUnit2D& pt, const AUnit2D& dim) {
		setPosition(pt);
		setDimensions(dim);
	}
	inline void setBounds(const pixel2& pt, const pixel2& dim) {
		bounds.position=pt;
		bounds.dimensions=dim;
	}
	inline void setBounds(const box2px& bbox) {
		bounds=bbox;
	}
	AColor backgroundColor = MakeColor(COLOR_NONE);
	AColor borderColor = MakeColor(COLOR_NONE);
	AUnit1D borderWidth = UnitPX(2);
	std::function<void()> onPack;
	std::function<void()> onRemoveFromOnTop;
	std::function<bool(AlloyContext* context, const InputEvent& event)> onMouseDown;
	std::function<bool(AlloyContext* context, const InputEvent& event)> onMouseUp;
	std::function<bool(AlloyContext* context, const InputEvent& event)> onMouseOver;
	std::function<bool(AlloyContext* context, const InputEvent& event)> onScroll;
	std::function<bool(AlloyContext* context, const InputEvent& event)> onMouseDrag;
	void setDragOffset(const pixel2& cursor, const pixel2& delta);
	void addDragOffset(const pixel2& delta);

	inline void setEnableDrag(bool enabled) {
		dragEnabled = enabled;
	}
	inline void setOrigin(const Origin& org) {
		origin = org;
	}
	bool isDragEnabled() const {
		return dragEnabled;
	}
	AUnit2D& getPosition() {
		return position;
	}
	AUnit2D& getDimensions() {
		return dimensions;
	}
	virtual box2px getBounds(bool includeOffset=true) const;
	virtual box2px getCursorBounds(bool includeOffset = true) const;
	pixel2 getBoundsPosition(bool includeOffset = true) const {
		return getBounds(includeOffset).position;
	}
	pixel2 getBoundsDimensions(bool includeOffset = true) const {
		return getBounds(includeOffset).dimensions;
	}
	pixel2 getDragOffset() const {
		return dragOffset;
	}
	pixel getBoundsPositionX(bool includeOffset = true) const {
		return getBounds(includeOffset).position.x;
	}
	pixel getBoundsDimensionsX(bool includeOffset = true) const {
		return getBounds(includeOffset).dimensions.x;
	}
	pixel getBoundsPositionY(bool includeOffset = true) const {
		return getBounds(includeOffset).position.y;
	}
	pixel getBoundsDimensionsY(bool includeOffset = true) const {
		return getBounds(includeOffset).dimensions.y;
	}
	const AUnit2D& getPosition() const {
		return position;
	}
	const AUnit2D& getDimensions() const {
		return dimensions;
	}
	void setVisible(bool vis);
	Region* parent = nullptr;
	Region(
			const std::string& name = MakeString() << "r" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++));
	Region(const std::string& name, const AUnit2D& pos, const AUnit2D& dims);
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false);
	virtual void draw(AlloyContext* context);
	virtual void update(CursorLocator* cursorLocator);
	virtual void drawDebug(AlloyContext* context);
	bool isVisible();
	virtual ~Region(){

	}
};
struct ScrollHandle: public Region {
public:
	const Orientation orientation;
	ScrollHandle(const std::string& name, Orientation orient) :
			Region(name), orientation(orient) {
	}
	virtual void draw(AlloyContext* context) override;
};
struct ScrollTrack: public Region {
public:
	const Orientation orientation;
	ScrollTrack(const std::string& name, Orientation orient) :
			Region(name), orientation(orient) {
	}
	virtual void draw(AlloyContext* context) override;
};
struct Composite: public Region {
protected:
	Orientation orientation = Orientation::Unspecified;
	bool scrollEnabled = false;

	static const float scrollBarSize;
	pixel2 scrollExtent = pixel2(0, 0);
	float horizontalScrollExtent = 0;
	pixel2 scrollPosition = pixel2(0, 0);
	std::shared_ptr<ScrollTrack> verticalScrollTrack, horizontalScrollTrack;
	std::shared_ptr<ScrollHandle> verticalScrollHandle, horizontalScrollHandle;
public:

	std::vector<std::shared_ptr<Region>> children;
	Composite(
			const std::string& name = MakeString() << "c" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++));
	Composite(const std::string& name, const AUnit2D& pos, const AUnit2D& dims);
	virtual Region* locate(const pixel2& cursor) override;

	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
	inline void setOrientation(const Orientation& orient) {
		orientation = orient;
	}
	virtual inline bool isScrollEnabled() const override {
		return scrollEnabled;
	}
	void setScrollEnabled(bool enabled) {
		scrollEnabled = enabled;
	}

	virtual inline pixel2 drawOffset() const {
		pixel2 offset(0,0);
		if (isScrollEnabled()) {
			offset = -scrollPosition* aly::max(pixel2(0, 0), scrollExtent - getBoundsDimensions());
		}
		if (parent != nullptr)offset += parent->drawOffset();
		return offset;
	}

	virtual void draw(AlloyContext* context) override;
	virtual void drawDebug(AlloyContext* context) override;
	virtual void update(CursorLocator* cursorLocator) override;
	void pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
			double pixelRatio, bool clamp = false) override;
	void pack(AlloyContext* context);
	virtual void add(const std::shared_ptr<Region>& region);
	void add(Region* region); //After add(), composite will own region and be responsible for destroying it.
	void pack();
	void draw();
};

struct BorderComposite: public Region {
protected:
	std::array<std::shared_ptr<Region>,5> children;
	std::shared_ptr<Region>& northRegion;
	std::shared_ptr<Region>& southRegion;
	std::shared_ptr<Region>& eastRegion;
	std::shared_ptr<Region>& westRegion;
	float northFraction=0,southFraction=0,eastFraction=0,westFraction=0;
	std::shared_ptr<Region>& centerRegion;
public:
	BorderComposite(const std::string& name = MakeString() << "c" << std::setw(8)<< std::setfill('0') << (REGION_COUNTER++));
	BorderComposite(const std::string& name, const AUnit2D& pos, const AUnit2D& dims);
	virtual Region* locate(const pixel2& cursor) override;
	virtual void draw(AlloyContext* context) override;
	virtual void drawDebug(AlloyContext* context) override;
	virtual void update(CursorLocator* cursorLocator) override;
	void pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,double pixelRatio, bool clamp = false) override;
	void pack(AlloyContext* context);
	inline void setNorth(const std::shared_ptr<Region>& region,float fraction){
		northRegion=region;
		northFraction=fraction;
	}
	inline void setSouth(const std::shared_ptr<Region>& region,float fraction){
		southRegion=region;
		southFraction=fraction;
	}
	inline void setEast(const std::shared_ptr<Region>& region,float fraction){
		eastRegion=region;
		eastFraction=fraction;
	}
	inline void setWest(const std::shared_ptr<Region>& region,float fraction){
		westRegion=region;
		westFraction=fraction;
	}
	inline void setCenter(const std::shared_ptr<Region>& region){
		centerRegion=region;
	}

	void pack();
	void draw();
};

struct GlyphRegion: public Region {
	AColor foregroundColor = MakeColor(Theme::Default.LIGHT_TEXT);
	std::shared_ptr<Glyph> glyph;
	GlyphRegion(
			const std::string& name = MakeString() << "g" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++)) :
			Region(name) {
		aspectRule = AspectRule::FixedHeight;
	}
	;
	virtual void drawDebug(AlloyContext* context) override;
	virtual void draw(AlloyContext* context) override;
};

struct TextLabel: public Region {
public:
	pixel2 getTextDimensions(AlloyContext* context);
	HorizontalAlignment horizontalAlignment = HorizontalAlignment::Left;
	VerticalAlignment verticalAlignment = VerticalAlignment::Top;
	FontStyle fontStyle = FontStyle::Normal;
	FontType fontType = FontType::Normal;
	AUnit1D fontSize = UnitPX(24);
	AColor textColor = MakeColor(Theme::Default.LIGHT_TEXT);
	AColor textAltColor = MakeColor(Theme::Default.DARK_TEXT);
	std::string label;
	TextLabel(
			const std::string& name = MakeString() << "t" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++)) :
			Region(name), label(name) {
	}
	virtual void draw(AlloyContext* context) override;
};
struct TextField: public Composite {
protected:
	bool showDefaultLabel = true;
	std::string label;
	std::string value;
	float fontSize = 0;
	float textOffsetX = 0;
	bool showCursor = false;
	std::chrono::high_resolution_clock::time_point lastTime;
	void clear();
	void erase();
	void handleCursorInput(AlloyContext* context, const InputEvent& e);
	void handleMouseInput(AlloyContext* context, const InputEvent& e);
	void handleKeyInput(AlloyContext* context, const InputEvent& e);
	void handleCharacterInput(AlloyContext* context, const InputEvent& e);
	void moveCursorTo(int index, bool isShiftHeld = false);
	void dragCursorTo(int index);
	int cursorStart = 0, cursorEnd = 0;
	bool dragging = false;

public:

	static const float PADDING;
	AColor textColor = MakeColor(Theme::Default.LIGHT_TEXT);
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
	virtual inline ~TextField(){}
	TextField(
			const std::string& name = MakeString() << "t" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++));
	TextField(const std::string& name,const AUnit2D& position,const AUnit2D& dimensions);
	virtual void draw(AlloyContext* context) override;
	virtual void setValue(const std::string& value);
	std::string getValue() const {
		return value;
	}
	std::function<void(TextField*)> onTextEntered;
	std::function<void(TextField*)> onKeyInput;
};
struct SelectionBox: public Region {
protected:
	int selectedIndex = -1;
	std::string label;
	int maxDisplayEntries=-1;
	int selectionOffset=0;
	void updateBox(AlloyContext* context);

public:
	std::mutex updateLock;
	FontStyle fontStyle = FontStyle::Normal;
	FontType fontType = FontType::Normal;
	AColor textColor = MakeColor(COLOR_WHITE);
	AColor textAltColor = MakeColor(COLOR_BLACK);
	std::function<bool(SelectionBox*)> onSelect;
	std::vector<std::string> options;
	void setMaxDisplayEntries(int mx){
		maxDisplayEntries=mx;
	}

	virtual box2px getBounds(bool includeBounds=true) const override;
	virtual box2px getCursorBounds(bool includeBounds = true) const override;
	std::string getSelection(int index) {
		return (selectedIndex >= 0) ? options[selectedIndex] : name;
	}
	int getSelectedIndex() const {
		return selectedIndex;
	}
	inline void setSelectionOffset(bool offset){
		selectionOffset=offset;
	}
	void setSelectedIndex(int index) {
		selectedIndex = index;
		if(index<0){
			label=name;
			selectionOffset=0;
		} else {
			label=options[selectedIndex];
		}
	}
	void draw(AlloyContext* context) override;

	SelectionBox(const std::string& name,
			const std::vector<std::string>& options=std::vector<std::string>());
};

struct FileField: public TextField {
protected:
	std::vector<std::string> segmentedPath;
	std::shared_ptr<SelectionBox> selectionBox;
public:
	AColor textColor = MakeColor(Theme::Default.LIGHT_TEXT);
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
	virtual inline ~FileField(){}

	FileField(const std::string& name,const AUnit2D& position,const AUnit2D& dimensions);
	virtual void draw(AlloyContext* context) override;
	virtual void setValue(const std::string& value) override;
};
std::shared_ptr<Composite> MakeComposite(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const Color& bgColor = COLOR_NONE, const Orientation& orientation =
				Orientation::Unspecified);
std::shared_ptr<GlyphRegion> MakeGlyphRegion(
		const std::shared_ptr<ImageGlyph>& glyph, const AUnit2D& position,
		const AUnit2D& dimensions, const AspectRule& aspectRatio =
				AspectRule::Unspecified, const Color& bgColor = COLOR_NONE,
		const Color& fgColor = COLOR_NONE,
		const Color& borderColor = COLOR_NONE, const AUnit1D& borderWidth =
				UnitPX(2));
std::shared_ptr<GlyphRegion> MakeGlyphRegion(
		const std::shared_ptr<AwesomeGlyph>& glyph, const AUnit2D& position,
		const AUnit2D& dimensions, const Color& bgColor = COLOR_NONE,
		const Color& fgColor = COLOR_NONE,
		const Color& borderColor = COLOR_NONE, const AUnit1D& borderWidth =
				UnitPX(2));
std::shared_ptr<TextLabel> MakeTextLabel(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const FontType& fontType, const AUnit1D& fontSize = UnitPT(14.0f),
		const Color& fontColor = COLOR_WHITE,
		const HorizontalAlignment& halign = HorizontalAlignment::Left,
		const VerticalAlignment& valign = VerticalAlignment::Top);
std::shared_ptr<TextField> MakeTextField(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const Color& bgColor = Theme::Default.DARK, const Color& textColor =
				Theme::Default.LIGHT_TEXT, const std::string& value = "");
std::shared_ptr<Region> MakeRegion(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const Color& bgColor = COLOR_NONE, const Color& lineColor = COLOR_WHITE,
		const AUnit1D& lineWidth = UnitPX(2.0f));
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Region & region) {
	ss << "Region: " << region.name << std::endl;
	ss << "\tOrigin: " << region.origin << std::endl;
	ss << "\tRelative Position: " << region.getPosition() << std::endl;
	ss << "\tRelative Dimensions: " << region.getDimensions() << std::endl;
	ss << "\tBounds: " << region.bounds << std::endl;
	ss << "\tAspect Ratio: " << region.aspectRule << std::endl;
	ss << "\tBackground Color: " << region.backgroundColor << std::endl;
	if (region.parent != nullptr)
		ss << "\tParent: " << region.parent->name << std::endl;
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const GlyphRegion & region) {
	ss << "Glyph Region: " << region.name << std::endl;
	if (region.glyph.get() != nullptr)
		ss << "\t" << *region.glyph << std::endl;
	ss << "\tOrigin: " << region.origin << std::endl;
	ss << "\tRelative Position: " << region.getPosition() << std::endl;
	ss << "\tRelative Dimensions: " << region.getDimensions() << std::endl;
	ss << "\tBounds: " << region.getBounds() << std::endl;
	ss << "\tAspect Ratio: " << region.aspectRule << " (" << region.aspectRatio
			<< ")" << std::endl;
	if (region.parent != nullptr)
		ss << "\tParent: " << region.parent->name << std::endl;
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const TextLabel & region) {
	ss << "Text Label: " << region.name << std::endl;
	ss << "\tOrigin: " << region.origin << std::endl;
	ss << "\tHorizontal Alignment: " << region.horizontalAlignment << std::endl;
	ss << "\tVertical Alignment: " << region.verticalAlignment << std::endl;
	ss << "\tRelative Position: " << region.getPosition() << std::endl;
	ss << "\tRelative Dimensions: " << region.getDimensions() << std::endl;
	ss << "\tBounds: " << region.getBounds() << std::endl;
	ss << "\tFont Type: " << region.fontType << std::endl;
	ss << "\tFont Size: " << region.fontSize << std::endl;
	ss << "\tFont Color: " << region.textColor << std::endl;
	if (region.parent != nullptr)
		ss << "\tParent: " << region.parent->name << std::endl;
	return ss;
}

template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Composite& region) {
	ss << "Composite: " << region.name << std::endl;
	ss << "\tOrigin: " << region.origin << std::endl;
	ss << "\tOrientation: " << region.orientation << std::endl;
	ss << "\tRelative Position: " << region.getPosition() << std::endl;
	ss << "\tRelative Dimensions: " << region.getDimensions() << std::endl;
	ss << "\tBackground Color: " << region.backgroundColor << std::endl;
	ss << "\tBounds: " << region.getBounds() << std::endl;
	int counter = 0;
	for (const std::shared_ptr<Region>& child : region.children) {
		ss << "\tChild[" << counter << "]: " << child->name << " "
				<< child->getBounds() << std::endl;
	}
	return ss;
}
typedef std::shared_ptr<TextLabel> TextLabelPtr;
typedef std::shared_ptr<TextField> TextFieldPtr;
typedef std::shared_ptr<Composite> CompositePtr;
typedef std::shared_ptr<SelectionBox> SelectionBoxPtr;
typedef std::shared_ptr<BorderComposite> BorderCompositePtr;
typedef std::shared_ptr<GlyphRegion> GlyphRegionPtr;
typedef std::shared_ptr<Region> RegionPtr;
}
#endif /* ALLOYUI_H_ */
