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
#include <iostream>
#include <memory>
#include <list>
#include <vector>
namespace aly{
	bool SANITY_CHECK_UI();

	inline NVGcolor Color(RGB color){
		return nvgRGB(color.x,color.y,color.z);
	}
	inline NVGcolor Color(RGBA color){
		return nvgRGBA(color.x,color.y,color.z,color.w);
	}
	inline NVGcolor Color(RGBi color){
		return nvgRGB(color.x,color.y,color.z);
	}
	inline NVGcolor Color(RGBAi color){
		return nvgRGBA(color.x,color.y,color.z,color.w);
	}
	inline NVGcolor Color(RGBAf color){
		return nvgRGBAf(color.x,color.y,color.z,color.w);
	}
	inline NVGcolor Color(RGBf color){
		return nvgRGBf(color.x,color.y,color.z);
	}
	inline NVGcolor Color(float r,float g,float b,float a){
		return nvgRGBAf(r,g,b,a);
	}
	inline NVGcolor Color(float r,float g,float b){
		return nvgRGBf(r,g,b);
	}
	inline NVGcolor Color(uint8_t r,uint8_t g,uint8_t b,uint8_t a){
		return nvgRGBA(r,g,b,a);
	}
	inline NVGcolor Color(uint8_t r,uint8_t g,uint8_t b){
		return nvgRGB(r,g,b);
	}
	inline NVGcolor Color(int r,int g,int b,int a){
		return nvgRGBA(r,g,b,a);
	}
	inline NVGcolor Color(int r,int g,int b){
		return nvgRGB(r,g,b);
	}


    struct Region {
    protected:
    	static uint64_t REGION_COUNTER;
    public:
    	AUnit2D position;
    	AUnit2D dimensions;
    	box2i bounds;
    	const std::string name;
    	Region* parent=nullptr;
    	Region(const std::string& name=MakeString()<<"r"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++));
    	virtual void pack(const int2& pos,const int2& dims,const double2& dpmm,double pixelRatio);
    	virtual void pack(AlloyContext* context);
    	virtual void draw(AlloyContext* context)=0;
    	virtual inline ~Region(){};
    };


    struct Composite : public Region{
    	std::vector<std::shared_ptr<Region>> children;
    	Orientation orientation;
		RGBA bgColor;
	    Composite(const std::string& name=MakeString()<<"c"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++)):Region(name),bgColor(0,0,0,0),orientation(Orientation::Unspecified){};
		virtual void draw(AlloyContext* context) override;
		virtual void pack(const int2& pos,const int2& dims,const double2& dpmm,double pixelRatio) override;
		virtual void pack(AlloyContext* context) override;
		Composite& add(const std::shared_ptr<Region>& region);
		Composite& add(Region* region);//After add(), composite will own region and be responsible for destroying it.
		void pack();
	    void draw();
    };



    struct Label : public Region{
        HorizontalAlignment horizontalAlignment;
    	VerticalAlignment verticalAlignment;
    	FontType fontType;
    	AUnit1D fontSize;
    	RGBA fontColor;
    	Label(const std::string& name=MakeString()<<"l"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++)):Region(name),fontType(FontType::Normal),horizontalAlignment(HorizontalAlignment::Left),verticalAlignment(VerticalAlignment::Top),fontColor(255,255,255,255){};
    	void draw(AlloyContext* context);
    };

    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Region & region) {
        	ss<<"Region: "<<region.name<<std::endl;
        	ss<<"\tRelative Position: "<<region.position<<std::endl;
        	ss<<"\tRelative Dimensions: "<<region.dimensions<<std::endl;
        	ss<<"\tBounds: "<<region.bounds<<std::endl;
        	if(region.parent!=nullptr)ss<<"\tParent: "<<region.parent->name<<std::endl;
        	return ss;
        }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Label & region) {
    	ss<<"Label: "<<region.name<<std::endl;
    	ss<<"\tRelative Position: "<<region.position<<std::endl;
    	ss<<"\tRelative Dimensions: "<<region.dimensions<<std::endl;
    	ss<<"\tBounds: "<<region.bounds<<std::endl;
    	ss<<"\tFont Type: "<<region.fontType<<std::endl;
    	ss<<"\tFont Size: "<<region.fontSize<<std::endl;
    	ss<<"\tFont Color: "<<region.fontColor<<std::endl;
    	ss<<"\tHorizontal Alignment: "<<region.horizontalAlignment<<std::endl;
    	ss<<"\tVertical Alignment: "<<region.verticalAlignment<<std::endl;
    	if(region.parent!=nullptr)ss<<"\tParent: "<<region.parent->name<<std::endl;
    	return ss;
    }

    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss,const Composite& region) {
    	ss<<"Composite: "<<region.name<<std::endl;
    	ss<<"\tRelative Position: "<<region.position<<std::endl;
    	ss<<"\tRelative Dimensions: "<<region.dimensions<<std::endl;
    	ss<<"\tBackground Color: "<<region.bgColor<<std::endl;
    	ss<<"\tBounds: "<<region.bounds<<std::endl;
    	ss<<"\tOrientation: "<<region.orientation<<std::endl;
    	int counter=0;
    	for(const std::shared_ptr<Region>& child:region.children){
    		ss<<"\tChild["<<counter<<"]: "<<child->name<<" "<<child->bounds<<std::endl;
    	}
    	return ss;
    }
    typedef std::shared_ptr<Label> LabelPtr;
    typedef std::shared_ptr<Composite> CompositePtr;
    typedef std::shared_ptr<Region> RegionPtr;
    inline std::shared_ptr<Label> MakeLabel(const std::string& name,const AUnit2D& position,const AUnit2D& dimensions,FontType fontType,const AUnit1D& fontSize=UnitPT(14.0f),RGBA fontColor=RGBA(255,255,255,255),HorizontalAlignment halign=HorizontalAlignment::Left,VerticalAlignment valign=VerticalAlignment::Top){
    	std::shared_ptr<Label> label=std::shared_ptr<Label>(new Label(name));
    	label->position=position;
    	label->dimensions=dimensions;
    	label->fontColor=fontColor;
    	label->fontType=fontType;
    	label->fontSize=fontSize;
    	label->horizontalAlignment=halign;
    	label->verticalAlignment=valign;
    	return label;
    }

    inline std::shared_ptr<Composite> MakeComposite(const std::string& name,const AUnit2D& position,const AUnit2D& dimensions,RGBA bgColor=RGBA(0,0,0,0),Orientation orientation=Orientation::Unspecified){
    	std::shared_ptr<Composite> composite=std::shared_ptr<Composite>(new Composite(name));
    	composite->position=position;
    	composite->bgColor=bgColor;
    	composite->dimensions=dimensions;
    	composite->orientation=orientation;
        return composite;
    }
}
#endif /* ALLOYUI_H_ */
