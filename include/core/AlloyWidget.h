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
class Widget: public Composite{
public:
	Widget(const std::string& name=""):Composite(name){}
	virtual void onMouseOver(Region* region,const InputEvent& event){}
	virtual void onMouseClick(Region* region,const InputEvent& event){}
	virtual void onMouseDrag(Region* region,const InputEvent& event,const pixel2& lastDragPosition){}
	virtual void add(const std::shared_ptr<Region>& region) override;
};
class Button: public Widget{
private:
	AColor textColor;
	AUnit1D fontSize;
public:
	Button(const std::string& label,const AUnit2D& position,const AUnit2D& dimensions);
	virtual void  draw(AlloyContext* context) override;
	virtual inline ~Button(){};
};
class ScrollHandle: public Region{
public:
	ScrollHandle(const std::string& name):Region(name){}
	virtual void  draw(AlloyContext* context) override;
};
class ScrollTrack: public Composite{
public:
	ScrollTrack(const std::string& name):Composite(name){}

	virtual void  draw(AlloyContext* context) override;
};
class HorizontalSlider: public Widget{
private:
	AColor textColor;
	AUnit1D fontSize;
public:
	HorizontalSlider(const std::string& label,const AUnit2D& position,const AUnit2D& dimensions);
	virtual void  draw(AlloyContext* context) override;
	virtual inline ~HorizontalSlider(){};
};

typedef std::shared_ptr<Button> ButtonPtr;
typedef std::shared_ptr<HorizontalSlider> HSliderPtr;

}

#endif /* ALLOYWIDGET_H_ */
