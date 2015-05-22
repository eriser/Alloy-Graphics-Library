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
namespace aly{
	uint64_t Region::REGION_COUNTER=0;

	Region::Region(const std::string& name):position(CoordPX(0,0)),dimensions(Percent(1,1)),name(name){

	}

	void Composite::draw(AlloyContext* context){
		if(bgColor.w>0){
			NVGcontext* nvg=context->nvgContext;
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x, bounds.dimensions.y);
			nvgFillColor(nvg, Color(bgColor));
			nvgFill(nvg);
		}
		for(std::shared_ptr<Region>& region:children){
			region->draw(context);
		}
	}
	void Composite::pack(){
		pack(Application::getContext().get());
	}
	void Composite::draw(){
		draw(Application::getContext().get());
	}
	void Composite::pack(const int2& pos,const int2& dims,const double2& dpmm){
		bounds.position=position.toPixels(dims,dpmm);
		bounds.dimensions=dimensions.toPixels(dims,dpmm);
		for(std::shared_ptr<Region>& region:children){
			region->pack(bounds.position,bounds.dimensions,dpmm);
		}
	}
	void Region::pack(const int2& pos,const int2& dims,const double2& dpmm){
		bounds.position=pos+position.toPixels(dims,dpmm);
		bounds.dimensions=dimensions.toPixels(dims,dpmm);
	}
	void Region::pack(AlloyContext* context){
		bounds.position=position.toPixels(context->viewport.dimensions,context->dpmm);
		bounds.dimensions=dimensions.toPixels(context->viewport.dimensions,context->dpmm);
	}

	void Composite::pack(AlloyContext* context){
		bounds.position=position.toPixels(context->viewport.dimensions,context->dpmm);
		bounds.dimensions=dimensions.toPixels(context->viewport.dimensions,context->dpmm);
		for(std::shared_ptr<Region>& region:children){
			region->pack(bounds.position,bounds.dimensions,context->dpmm);
		}
	}

	Composite& Composite::add(const std::shared_ptr<Region>& region){
		children.push_back(region);
		if(region->parent!=nullptr)throw std::runtime_error("Cannot add child node because it already has a parent.");
		region->parent=this;
		return *this;
	}
	Composite& Composite::add(Region* region){
		children.push_back(std::shared_ptr<Region>(region));
		return *this;
	}

	void Label::draw(AlloyContext* context){
		NVGcontext* nvg=context->nvgContext;
		nvgFontSize(nvg, fontSize);
		nvgFillColor(nvg,Color(fontColor));
		nvgFontFaceId(nvg,context->getFontHandle(fontType));
		nvgTextAlign(nvg,static_cast<int>(horizontalAlignment)|static_cast<int>(verticalAlignment));
		nvgText(nvg,bounds.position.x, bounds.position.y,name.c_str(),nullptr);
	}


};



