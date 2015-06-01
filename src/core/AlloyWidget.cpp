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
#include "AlloyWidget.h"

using namespace std;
namespace aly{
	void Widget::add(const std::shared_ptr<Region>& region){
		Composite::add(region);
		region->onMouseOver=[this,region](AlloyContext* context,const InputEvent& e){this->onMouseOver(context,region.get(),e);};
		region->onMouseClick=[this,region](AlloyContext* context,const InputEvent& e){this->onMouseClick(context,region.get(),e);};
		region->onMouseDrag=[this,region](AlloyContext* context,const InputEvent& e,const pixel2& lastDragPosition){this->onMouseDrag(context,region.get(),e,lastDragPosition);};
	}

	Button::Button(const std::string& label,const AUnit2D& position,const AUnit2D& dimensions):Widget(label){
		this->position=position;
		this->dimensions=dimensions;
		backgroundColor=MakeColor(Color(255,255,255,255));
		textColor=MakeColor(Color(0,0,0,255));
		borderColor=MakeColor(Color(255,255,255,255));
		this->aspectRatio=AspectRatio::FixedHeight;
	}
	void ScrollTrack::draw(AlloyContext* context){
		NVGcontext* nvg=context->nvgContext;

		nvgBeginPath(nvg);
		nvgMoveTo(nvg,bounds.position.x+15,bounds.position.y+bounds.dimensions.y*0.5f);
		nvgLineTo(nvg,bounds.position.x-15+bounds.dimensions.x,bounds.position.y+bounds.dimensions.y*0.5f);
		nvgStrokeColor(nvg,Color(255,255,255,255));
		nvgStrokeWidth(nvg,8.0f);
		nvgLineCap(nvg,NVG_ROUND);
		nvgStroke(nvg);
		for(std::shared_ptr<Region> ptr:children){
			ptr->draw(context);
		}
	}
	void ScrollHandle::draw(AlloyContext* context){
		NVGcontext* nvg=context->nvgContext;
		if(context->isMouseOver(this)||context->isMouseDown(this)){
			nvgBeginPath(nvg);
			nvgCircle(nvg,bounds.position.x+bounds.dimensions.x*0.5f,bounds.position.y+bounds.dimensions.y*0.5f,bounds.dimensions.y*0.5f);
			nvgFillColor(nvg,Color(220,220,220,128));
			nvgFill(nvg);
		}
		nvgBeginPath(nvg);
		nvgCircle(nvg,bounds.position.x+bounds.dimensions.x*0.5f,bounds.position.y+bounds.dimensions.y*0.5f,bounds.dimensions.y*0.25f);
		nvgFillColor(nvg,Color(255,255,255,255));
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgStrokeWidth(nvg,2.0f);
		nvgStrokeColor(nvg,Color(128,128,128,128));
		nvgCircle(nvg,bounds.position.x+bounds.dimensions.x*0.5f,bounds.position.y+bounds.dimensions.y*0.5f,bounds.dimensions.y*0.25f);
		nvgStroke(nvg);

	}
	HorizontalSlider::HorizontalSlider(const std::string& label,const AUnit2D& position,const AUnit2D& dimensions,const Number& min,const Number& max,const Number& value):Widget(label),minValue(min),maxValue(max),value(value){
		this->position=position;
		this->dimensions=dimensions;
		textColor=MakeColor(Color(0,0,0,255));
		borderColor=MakeColor(Color(255,255,255,255));
		scrollHandle =std::shared_ptr<ScrollHandle>(new ScrollHandle("Scroll Handle"));

		scrollHandle->setPosition(CoordPercent(0.0, 0.0));
		scrollHandle->setDimensions(CoordPX(30, 30));
		scrollHandle->backgroundColor=MakeColor(255, 128, 64, 255);
		scrollHandle->setEnableDrag(true);

		scrollTrack = std::shared_ptr<ScrollTrack>(new ScrollTrack("Scroll Track"));
		scrollTrack->setPosition(CoordPerPX(0.0f, 0.5f,0.0f,-5.0f));
		scrollTrack->setDimensions(CoordPerPX(1.0f, 0.0f,0.0f,30.0f));
		scrollTrack->backgroundColor=MakeColor(128, 128, 128, 255);
		scrollTrack->add(scrollHandle);
		scrollTrack->backgroundColor=MakeColor(255,0,0);

		add(MakeTextLabel("Label",CoordPerPX(0.0f,0.0f,10,0),CoordPerPX(0.5f,1.0f,0,-21),FontType::Bold,UnitPX(26),RGBA(255,255,255,255),HorizontalAlignment::Left,VerticalAlignment::Bottom));
		add(valueLabel=MakeTextLabel("0.12345",CoordPercent(0.0f,0.0f),CoordPerPX(1.0f,1.0f,-10,-21),FontType::Normal,UnitPX(24),RGBA(255,255,255,255),HorizontalAlignment::Right,VerticalAlignment::Bottom));
		add(scrollTrack);
	}
	void HorizontalSlider::onMouseClick(AlloyContext* context,Region* region,const InputEvent& event){
		if(region==scrollTrack.get()){
			scrollHandle->setDragOffset(event.cursor,scrollHandle->getBoundsDimensions()*0.5f);
			context->requestPack();
		}
	}
void HorizontalSlider::draw(AlloyContext* context){
		double interp=(scrollHandle->getBoundsPositionX()-scrollTrack->getBoundsPositionX())/(double)(scrollTrack->getBoundsDimensionsX()-scrollHandle->getBoundsDimensionsX());
		double val=(1.0-interp)*minValue.toDouble()+interp*maxValue.toDouble();
		value.setValue(val);
		valueLabel->label=labelFormatter(value);
		NVGcontext* nvg=context->nvgContext;
		float cornerRadius=5.0f;
		nvgBeginPath(nvg);
			NVGpaint shadowPaint = nvgBoxGradient(nvg,
									bounds.position.x+1,
									bounds.position.y,
									bounds.dimensions.x-2,
									bounds.dimensions.y,cornerRadius,8,Color(0,0,0,255),Color(255,255,255,0));
			nvgFillPaint(nvg, shadowPaint);
			nvgRoundedRect(nvg,
					bounds.position.x+1,
					bounds.position.y+4,
					bounds.dimensions.x,
					bounds.dimensions.y,4.0f);
			nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x,
				bounds.position.y,
				bounds.dimensions.x,
				bounds.dimensions.y,cornerRadius);
		nvgFillColor(nvg,Color(64,64,64,255));
		nvgFill(nvg);
		Composite::draw(context);

	}
	void Button::draw(AlloyContext* context){
		bool hover =context->isMouseOver(this);
		bool down = context->isMouseDown(this);
		NVGcontext* nvg=context->nvgContext;
		float cornerRadius=5.0f;
		float lineWidth=2.0f;

		int  xoff=0;
		int yoff=0;
		if(down){
			xoff=2;
			yoff=2;
		}

		if(hover||down){
			if(!down){
			nvgBeginPath(nvg);
				NVGpaint shadowPaint = nvgBoxGradient(nvg,
										bounds.position.x+1,
										bounds.position.y,
										bounds.dimensions.x-2,
										bounds.dimensions.y,cornerRadius,8,Color(0,0,0,255),Color(255,255,255,0));
				nvgFillPaint(nvg, shadowPaint);
				nvgRoundedRect(nvg,
						bounds.position.x+1,
						bounds.position.y+4,
						bounds.dimensions.x,
						bounds.dimensions.y,4.0f);
				nvgFill(nvg);
			}

			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x+xoff,
					bounds.position.y+yoff,
					bounds.dimensions.x,
					bounds.dimensions.y,cornerRadius);
			nvgFillColor(nvg,Color(255,255,255,255));
			nvgFill(nvg);

		} else {
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, bounds.position.x+1,
					bounds.position.y+1,
					bounds.dimensions.x-2,
					bounds.dimensions.y-2,cornerRadius);
			nvgFillColor(nvg,Color(255,255,255,255));
			nvgFill(nvg);
		}

		if(hover){

			nvgBeginPath(nvg);
			NVGpaint hightlightPaint = nvgBoxGradient(nvg,
									bounds.position.x+xoff,
									bounds.position.y+yoff,
									bounds.dimensions.x,
									bounds.dimensions.y,
									cornerRadius,4,Color(255,255,255,0),Color(64,64,64,255));
			nvgFillPaint(nvg, hightlightPaint);
			nvgRoundedRect(nvg,
					bounds.position.x+xoff,
					bounds.position.y+yoff,
					bounds.dimensions.x,
					bounds.dimensions.y,cornerRadius);
			nvgFill(nvg);
		}

		float th=std::max(10.0f,(float)(bounds.dimensions.y*0.75f));
		nvgFontSize(nvg,th);
		nvgFillColor(nvg, *textColor);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		float tw=nvgTextBounds(nvg,0,0,name.c_str(),nullptr,nullptr);
		this->aspect=tw/th;

		nvgTextAlign(nvg,NVG_ALIGN_MIDDLE|NVG_ALIGN_CENTER);
		pixel2 offset(0,0);

		nvgText(nvg, bounds.position.x+bounds.dimensions.x/2+xoff, bounds.position.y+bounds.dimensions.y/2+yoff, name.c_str(), nullptr);


		/*

*/


	}
}



