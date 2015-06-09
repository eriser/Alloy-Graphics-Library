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

#ifndef ALLOYWIDGET_H_
#define ALLOYWIDGET_H_

#include "AlloyUI.h"
namespace aly {
class Widget: public Composite {
public:
	Widget(const std::string& name = "") :
			Composite(name) {
	}
	Widget(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Composite(name, pos, dims) {

	}

	virtual bool onMouseOver(AlloyContext* context, Region* region,
			const InputEvent& event) {
	}
	virtual bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event) {
	}
	virtual bool onMouseUp(AlloyContext* context, Region* region,
			const InputEvent& event) {
	}
	virtual bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event) {
	}

};
class Button: public Widget {
private:
	AColor textColor;
	AUnit1D fontSize;
	void internalDraw(AlloyContext* context);
public:
	Button(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions);
	virtual void draw(AlloyContext* context) override;
	virtual void drawOnTop(AlloyContext* context) override;
	virtual inline ~Button() {
	}
	;
};
class CheckBox: public Widget {
private:
	TextLabelPtr checkLabel;
	GlyphRegionPtr valueLabel;
	bool checked;
public:
	inline bool getValue() {
		return checked;
	}
	CheckBox(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, bool checked = false);
	virtual void draw(AlloyContext* context) override;
};
class SliderHandle: public Region {
public:
	SliderHandle(const std::string& name) :
			Region(name) {
	}
	virtual void draw(AlloyContext* context) override;
};
class SliderTrack: public Composite {
protected:
	const Orientation orientation;
public:
	SliderTrack(const std::string& name, Orientation orient) :
			Composite(name), orientation(orient) {
	}
	virtual void draw(AlloyContext* context) override;
};
class ProgressBar: public Widget {
private:
	TextLabelPtr textLabel;
	float value;
	std::string label;
public:
	virtual void draw(AlloyContext* context) override;
	inline void setValue(float p) {
		value = clamp(p, 0.0f, 1.0f);
	}
	inline void setValue(const std::string& l) {
		label = l;
	}
	inline void setValue(const std::string& l, float p) {
		label = l;
		value = clamp(p, 0.0f, 1.0f);
	}
	ProgressBar(const std::string& name, const AUnit2D& pt,
			const AUnit2D& dims);
};
class HorizontalSlider: public Widget {
private:
	AColor textColor;
	AUnit1D fontSize;
	Number value;
	Number minValue;
	Number maxValue;

	TextLabelPtr sliderLabel;
	TextLabelPtr valueLabel;
	std::shared_ptr<SliderHandle> sliderHandle;
	std::shared_ptr<SliderTrack> sliderTrack;
	std::function<std::string(const Number& value)> labelFormatter =
			[](const Number& value) {return value.toString();};
	void update();
public:
	void setValue(double value);
	inline void setValue(int value) {
		setValue((double) value);
	}
	inline void setValue(float value) {
		setValue((double) value);
	}
	const Number& getValue() {
		return value;
	}

	virtual bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	virtual bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	inline void setLabelFormatter(
			const std::function<const std::string&(const Number& value)>& func) {
		labelFormatter = func;
	}
	HorizontalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	virtual void draw(AlloyContext* context) override;
	virtual inline ~HorizontalSlider() {
	}
};

class VerticalSlider: public Widget {
private:
	AColor textColor;
	AUnit1D fontSize;
	Number value;
	Number minValue;
	Number maxValue;

	TextLabelPtr sliderLabel;
	TextLabelPtr valueLabel;
	std::shared_ptr<SliderHandle> sliderHandle;
	std::shared_ptr<SliderTrack> sliderTrack;
	std::function<std::string(const Number& value)> labelFormatter =
			[](const Number& value) {return value.toString();};
	void update();
public:
	void setValue(double value);
	inline void setValue(int value) {
		setValue((double) value);
	}
	inline void setValue(float value) {
		setValue((double) value);
	}
	const Number& getValue() {
		return value;
	}

	virtual bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	virtual bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event) override;

	inline void setLabelFormatter(
			const std::function<const std::string&(const Number& value)>& func) {
		labelFormatter = func;
	}
	VerticalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	virtual void draw(AlloyContext* context) override;
	virtual inline ~VerticalSlider() {
	}
};
struct SelectionBox: public Region {
protected:
	int selectedIndex = -1;
	std::string label;
	void updateBox(AlloyContext* context);
public:
	FontStyle fontStyle = FontStyle::Normal;
	FontType fontType = FontType::Normal;
	AUnit1D fontSize = UnitPX(14);
	AColor textColor = MakeColor(COLOR_WHITE);
	AColor textAltColor = MakeColor(COLOR_BLACK);
	std::vector<std::string> options;
	virtual box2px getBounds() const override;
	Region* locate(const pixel2& cursor);
	std::string getSelection(int index) {
		return (selectedIndex >= 0) ? options[selectedIndex] : name;
	}
	int getSelectedIndex() const {
		return selectedIndex;
	}

	void setSelectedIndex(int index) {
		selectedIndex = index;
		label = (index >= 0) ? options[selectedIndex] : name;
	}
	void drawOnTop(AlloyContext* context) override;
	void draw(AlloyContext* context) override;

	SelectionBox(const std::string& name,
			const std::vector<std::string>& options);
};
class Selection: public Widget {
private:
	TextLabelPtr selectionLabel;
	GlyphRegionPtr arrowLabel;
	std::shared_ptr<SelectionBox> selectionBox;
	int selectedIndex = -1;
public:
	inline int getSelectedIndex() const {
		return selectedIndex;
	}
	std::string getSelection() {
		return selectionBox->getSelection(selectedIndex);
	}
	virtual void draw(AlloyContext* context) override;
	Selection(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const std::vector<std::string>& options =
					std::vector<std::string>());
};

struct ColorWheel: public Composite {
protected:
	Color selectedColor;
	HSV hsvColor;
	float2 tBounds[3];
	float2 tPoints[3];
	float rInner, rOuter;
	float2 center;

	bool triangleSelected = false;
	bool circleSelected = false;
	void updateWheel();
public:
	void reset();
	Region* locate(const pixel2& cursor);
	virtual box2px getBounds() const override;
	Color getSelectedColor() const {
		return selectedColor;
	}
	void setColor(const Color& c);
	void setColor(const pixel2& cursor);
	ColorWheel(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	void drawOnTop(AlloyContext* context) override;
};
class ColorSelector: public Widget {
	TextLabelPtr textLabel;
	RegionPtr colorLabel;
	std::shared_ptr<ColorWheel> colorWheel;
public:
	void setColor(const Color& color);
	Color getColor();
	ColorSelector(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	virtual void draw(AlloyContext* context) override;
};
typedef std::shared_ptr<Button> ButtonPtr;
typedef std::shared_ptr<HorizontalSlider> HSliderPtr;
typedef std::shared_ptr<VerticalSlider> VSliderPtr;
typedef std::shared_ptr<ColorSelector> ColorSelectorPtr;
typedef std::shared_ptr<CheckBox> CheckBoxPtr;
typedef std::shared_ptr<Selection> SelectionPtr;
typedef std::shared_ptr<SelectionBox> SelectionBoxPtr;
typedef std::shared_ptr<ProgressBar> ProgressBarPtr;
typedef std::shared_ptr<ColorWheel> ColorWheelPtr;
}

#endif /* ALLOYWIDGET_H_ */
