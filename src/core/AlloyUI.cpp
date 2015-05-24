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

	Region::Region(const std::string& name):position(CoordPX(0,0)),dimensions(CoordPercent(1,1)),name(name){

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
	void Composite::pack(const pixel2& pos,const pixel2& dims,const double2& dpmm,double pixelRatio){
		bounds.position=position.toPixels(dims,dpmm,pixelRatio);
		bounds.dimensions=dimensions.toPixels(dims,dpmm,pixelRatio);
		for(std::shared_ptr<Region>& region:children){
			region->pack(bounds.position,bounds.dimensions,dpmm,pixelRatio);
		}
	}
	void Region::pack(const pixel2& pos,const pixel2& dims,const double2& dpmm,double pixelRatio){
		bounds.position=pos+position.toPixels(dims,dpmm,pixelRatio);
		pixel2 d=dimensions.toPixels(dims,dpmm,pixelRatio);
		if(aspect<0){
			aspect=dims.x/std::max((float)dims.y,0.0f);
		}
		switch(aspectRatio){
			case AspectRatio::FixedWidth:
				bounds.dimensions=pixel2(d.x,d.x/aspect);
				break;
			case AspectRatio::FixedHeight:
				bounds.dimensions=pixel2(d.y*aspect,d.y);
				break;
			case AspectRatio::Unspecified:
			default:
				bounds.dimensions=d;
		}
	}

	void Composite::pack(AlloyContext* context){
		pack(pixel2(context->viewport.position),pixel2(context->viewport.dimensions),context->dpmm,context->pixelRatio);
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

	void TextLabel::draw(AlloyContext* context){
		NVGcontext* nvg=context->nvgContext;
		nvgFontSize(nvg, fontSize.toPixels(context->height(),context->dpmm.y,context->pixelRatio));
		nvgFillColor(nvg,Color(fontColor));
		nvgFontFaceId(nvg,context->getFontHandle(fontType));
		nvgTextAlign(nvg,static_cast<int>(horizontalAlignment)|static_cast<int>(verticalAlignment));
		nvgText(nvg,bounds.position.x, bounds.position.y,name.c_str(),nullptr);
	}
	void GlyphRegion::draw(AlloyContext* context){
		NVGcontext* nvg=context->nvgContext;
		if(bgColor.w>0){
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x, bounds.dimensions.y);
			nvgFillColor(nvg, Color(bgColor));
			nvgFill(nvg);
		}
		if(glyph.get()!=nullptr){
			glyph->draw(bounds,fgColor,context);
		}
		if(borderColor.w>0){
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x, bounds.dimensions.y);
			nvgStrokeColor(nvg, Color(borderColor));
			nvgStrokeWidth(nvg,borderWidth.toPixels(context->height(),context->dpmm.y,context->pixelRatio));
			nvgStroke(nvg);
		}
	}

};



