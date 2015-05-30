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
		region->onMouseOver=[this,region](const InputEvent& e){this->onMouseOver(region.get(),e);};
		region->onMouseClick=[this,region](const InputEvent& e){this->onMouseClick(region.get(),e);};
		region->onMouseDrag=[this,region](const InputEvent& e,const pixel2& lastDragPosition){this->onMouseDrag(region.get(),e,lastDragPosition);};
	}

	Button::Button(const std::string& label,const AUnit2D& position,const AUnit2D& dimensions):Widget(label){
		this->position=position;
		this->dimensions=dimensions;
		backgroundColor=MakeColor(Color(255,255,255,255));
		textColor=MakeColor(Color(0,0,0,255));
		borderColor=MakeColor(Color(255,255,255,255));
		this->aspectRatio=AspectRatio::FixedHeight;
	}
	void Button::draw(AlloyContext* context){

		bool hover=(context->mouseOverRegion==this);
		bool down=(context->mouseDownRegion==this);
		NVGcontext* nvg=context->nvgContext;
		float cornerRadius=5.0f;
		float lineWidth=2.0f;

		int  xoff=0;
		int yoff=0;
		if(down){
			xoff=2;
			yoff=2;
		}

		if(hover){
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



