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
		return false;
	}
	virtual bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event) {
		return false;
	}
	virtual bool onMouseUp(AlloyContext* context, Region* region,
			const InputEvent& event) {
		return false;
	}
	virtual bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event) {
		return false;
	}

};
class TextButton: public Widget {
private:
	AColor textColor;
	AUnit1D fontSize;
public:
	TextButton(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions);
	virtual void draw(AlloyContext* context) override;
	virtual inline ~TextButton() {
	}
};
class TextIconButton: public Widget {
private:
	AColor textColor;
	AUnit1D fontSize;
	std::string iconCodeString;
public:
	TextIconButton(const std::string& label, int iconCode,
			const AUnit2D& position, const AUnit2D& dimensions);
	virtual void draw(AlloyContext* context) override;
	virtual inline ~TextIconButton() {
	}
};
class IconButton: public Widget {
private:
	std::shared_ptr<Glyph> iconGlyph;
public:
	IconButton(const std::shared_ptr<Glyph>& glyph, const AUnit2D& position,
			const AUnit2D& dimensions);
	virtual void draw(AlloyContext* context) override;
	virtual inline ~IconButton() {
	}
};
class CheckBox: public Widget {
private:
	TextLabelPtr checkLabel;
	TextLabelPtr valueLabel;
	bool checked;
	bool handleMouseDown(AlloyContext* context, const InputEvent& event);
public:
	std::function<void(bool)> onChange;
	inline bool getValue() {
		return checked;
	}
	void setValue(bool value);
	CheckBox(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, bool checked = false);
	virtual void draw(AlloyContext* context) override;
};
class ToggleBox: public Widget {
private:
	TextLabelPtr toggleLabel;
	TextLabelPtr onLabel;
	TextLabelPtr offLabel;
	CompositePtr clickRegion;
	bool toggledOn;
	bool handleMouseDown(AlloyContext* context,const InputEvent& e);
public:
	inline bool getValue() {
		return toggledOn;
	}
	std::function<void(bool)> onChange;

	void setValue(bool value);
	ToggleBox(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, bool toggledOn = false);
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
	Color startColor, endColor;
	SliderTrack(const std::string& name, Orientation orientColor,
			const Color& st, const Color& ed);
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
class Slider: public Widget {
protected:
	AColor textColor;
	AUnit1D fontSize;
	Number value;
	Number minValue;
	Number maxValue;
	TextLabelPtr sliderLabel;
	TextLabelPtr valueLabel;
	std::shared_ptr<SliderHandle> sliderHandle;
	std::shared_ptr<SliderTrack> sliderTrack;
	std::function<std::string(const Number& value)> labelFormatter;
	std::function<void(const Number& value)> onChangeEvent;
	virtual void update()=0;
public:
	void setSliderColor(const Color& startColor, const Color& endColor) {
		sliderTrack->startColor = startColor;
		sliderTrack->endColor = endColor;
	}
	Slider(const std::string& name, const Number& min, const Number& max,
			const Number& val) :
			Widget(name), minValue(min), maxValue(max), value(val) {
		labelFormatter = [](const Number& value) {return value.toString();};
	}
	Slider(const std::string& name, const AUnit2D& pos, const AUnit2D& dims,
			const Number& min, const Number& max, const Number& val) :
			Widget(name, pos, dims), minValue(min), maxValue(max), value(val) {
		labelFormatter = [](const Number& value) {return value.toString();};
	}
	double getBlendValue() const;
	void setBlendValue(double value);

	virtual void setValue(double value)=0;
	inline void setValue(int value) {
		setValue((double) value);
	}
	inline void setValue(float value) {
		setValue((double) value);
	}
	const Number& getValue() {
		return value;
	}
	inline void setOnChangeEvent(
			const std::function<void(const Number& value)>& func) {
		onChangeEvent = func;
	}
	inline void setLabelFormatter(
			const std::function<std::string(const Number& value)>& func) {
		labelFormatter = func;
	}
};
class HorizontalSlider: public Slider {
protected:
	virtual void update() override;
public:
	virtual void setValue(double value) override;
	virtual bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	virtual bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	HorizontalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	virtual void draw(AlloyContext* context) override;
	virtual inline ~HorizontalSlider() {
	}
};

class VerticalSlider: public Slider {
protected:
	virtual void update() override;
public:
	virtual void setValue(double value) override;
	virtual bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	virtual bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	VerticalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	virtual void draw(AlloyContext* context) override;
	virtual inline ~VerticalSlider() {
	}
};
class Selection: public Widget {
private:
	TextLabelPtr selectionLabel;
	TextLabelPtr arrowLabel;
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
	float2 tBounds[3];
	float2 tPoints[3];
	float rInner, rOuter;
	float2 center;
	HSVA hsvColor;
	bool triangleSelected = false;
	bool circleSelected = false;
	std::function<void(const Color& value)> onChangeEvent;
	void updateWheel();
public:
	inline void setOnChangeEvent(
			const std::function<void(const Color& value)>& func) {
		onChangeEvent = func;
	}
	void reset();
	Color getSelectedColor() const {
		return selectedColor;
	}
	void setColor(const Color& c);
	void setColor(const pixel2& cursor);
	ColorWheel(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	void draw(AlloyContext* context) override;
};

class ColorSelector: public Widget {
private:
	TextLabelPtr textLabel;
	GlyphRegionPtr colorLabel;
	CompositePtr colorSelectionPanel;
	std::shared_ptr<VerticalSlider> redSlider;
	std::shared_ptr<VerticalSlider> greenSlider;
	std::shared_ptr<VerticalSlider> blueSlider;
	std::shared_ptr<VerticalSlider> lumSlider;
	std::shared_ptr<VerticalSlider> alphaSlider;
	std::shared_ptr<ColorWheel> colorWheel;
	void updateColorSliders(const Color& c);
public:
	void setColor(const Color& color);
	Color getColor();
	ColorSelector(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	virtual void draw(AlloyContext* context) override;
};
class ExpandRegion: public Composite {
private:
	std::shared_ptr<TextLabel> selectionLabel;
	std::shared_ptr<TextLabel> arrowIcon;
	std::shared_ptr<Region> contentRegion;
	bool expanded;
public:
	void setExpanded(bool expanded);
	ExpandRegion(const std::string& name, const std::shared_ptr<Region>& region,
			const AUnit2D& pos, const AUnit2D& dims);
};
class ExpandBar: public Widget {
private:
	std::list<std::shared_ptr<ExpandRegion>> regions;
public:
	CompositePtr& add(Region* region, bool expanded);
	CompositePtr& add(const std::shared_ptr<Region>&, bool expanded);

	ExpandBar(const std::string& name, const AUnit2D& pos, const AUnit2D& dims);
};
class FileDialog;
class FileEntry: public Region {
private:
	std::string iconCodeString;
	std::string fileName;
	std::string creationTime;
	std::string lastModifiedTime;
	std::string lastAccessTime;
	std::string fileSize;
	bool selected;
	AUnit1D fontSize;
	FileDialog* dialog;
public:
	FileDescription fileDescription;
	void setSelected(bool selected);
	bool isSelected();
	FileEntry(FileDialog* dialog, const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	virtual void draw(AlloyContext* context) override;
	void setValue(const FileDescription& fileDescription);
};
class FileDialog: public Widget {
private:

	std::list<FileEntry*> lastSelected;
	std::shared_ptr<FileField> fileLocation;
	std::shared_ptr<Composite> directoryTree;
	std::shared_ptr<Composite> directoryList;
	std::shared_ptr<TextIconButton> openButton;
	std::shared_ptr<IconButton> cancelButton;
	std::shared_ptr<BorderComposite> containerRegion;
	std::vector<std::shared_ptr<FileEntry>> fileEntries;
	bool enableMultiSelection = false;
	FileType fileType = FileType::File;
	void setSelectedFile(const std::string& file);
	bool onMouseDown(FileEntry* entry, AlloyContext* context,
			const InputEvent& e);
	bool onMouseOver(FileEntry* entry, AlloyContext* context,
			const InputEvent& e);
	bool onMouseDrag(FileEntry* entry, AlloyContext* context,
			const InputEvent& e);
	bool onMouseUp(FileEntry* entry, AlloyContext* context,
			const InputEvent& e);
public:
	friend class FileEntry;
	virtual void draw(AlloyContext* context) override;
	FileDialog(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	void setEnableMultiSelection(bool enable);
	void setFileSelectionType(FileType type);
	bool isMultiSelectionEnabled();
	void setValue(const std::string& file);
	std::string getValue() const;

};
class FileSelector: public Widget {
private:
	std::shared_ptr<TextLabel> fileLocationLabel;
	std::shared_ptr<TextLabel> fileLabel;
	std::shared_ptr<TextLabel> openIcon;
	std::shared_ptr<FileDialog> fileDialog;
	std::string fileLocation;
public:
	FileSelector(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	void setFileLocation(const std::string& file);
	void openFileDialog(AlloyContext* context,
			const std::string& workingDirectory = ALY_PATH_SEPARATOR);
	virtual void draw(AlloyContext* context) override;
};

typedef std::shared_ptr<TextButton> ButtonPtr;
typedef std::shared_ptr<HorizontalSlider> HSliderPtr;
typedef std::shared_ptr<VerticalSlider> VSliderPtr;
typedef std::shared_ptr<ColorSelector> ColorSelectorPtr;
typedef std::shared_ptr<CheckBox> CheckBoxPtr;
typedef std::shared_ptr<ToggleBox> ToggleBoxPtr;
typedef std::shared_ptr<Selection> SelectionPtr;
typedef std::shared_ptr<ProgressBar> ProgressBarPtr;
typedef std::shared_ptr<ColorWheel> ColorWheelPtr;
typedef std::shared_ptr<ExpandBar> ExpandBarPtr;
typedef std::shared_ptr<ExpandRegion> ExpandRegionPtr;
typedef std::shared_ptr<FileSelector> FileSelectorPtr;
typedef std::shared_ptr<FileDialog> FileDialogPtr;

}

#endif /* ALLOYWIDGET_H_ */
