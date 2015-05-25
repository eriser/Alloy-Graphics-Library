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
namespace aly{
	bool SANITY_CHECK_UI();


    struct Region {
    protected:
    	static uint64_t REGION_COUNTER;
    public:
    	AUnit2D position;
    	AUnit2D dimensions;
    	box2px bounds;
    	const std::string name;
    	AspectRatio aspectRatio=AspectRatio::Unspecified;
    	double aspect=-1.0;//Less than zero indicates undetermined. Will be computed at next pack() event.
    	Region* parent=nullptr;
    	Region(const std::string& name=MakeString()<<"r"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++));
    	virtual void pack(const pixel2& pos,const pixel2& dims,const double2& dpmm,double pixelRatio);
    	virtual void draw(AlloyContext* context)=0;
    	virtual void drawDebug(AlloyContext* context);
    	virtual inline ~Region(){};
    };


    struct Composite : public Region{
    	std::vector<std::shared_ptr<Region>> children;
    	Orientation orientation=Orientation::Unspecified;
		ColorPtr bgColor=MakeColor(COLOR_NONE);
	    Composite(const std::string& name=MakeString()<<"c"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++)):Region(name){};
		virtual void draw(AlloyContext* context) override;
    	virtual void drawDebug(AlloyContext* context) override;
		void pack(const pixel2& pos,const pixel2& dims,const double2& dpmm,double pixelRatio) override;
		void pack(AlloyContext* context);
		Composite& add(const std::shared_ptr<Region>& region);
		Composite& add(Region* region);//After add(), composite will own region and be responsible for destroying it.
		void pack();
	    void draw();
    };
    inline std::shared_ptr<Composite> MakeComposite(const std::string& name,const AUnit2D& position,const AUnit2D& dimensions,
    		RGBA bgColor=COLOR_NONE,Orientation orientation=Orientation::Unspecified){
    	std::shared_ptr<Composite> composite=std::shared_ptr<Composite>(new Composite(name));
    	composite->position=position;
    	composite->bgColor=MakeColor(bgColor);
    	composite->dimensions=dimensions;
    	composite->orientation=orientation;
        return composite;
    }
    struct GlyphRegion : public Region{
    	ColorPtr bgColor=MakeColor(COLOR_NONE);
    	ColorPtr fgColor=MakeColor(COLOR_NONE);
    	ColorPtr borderColor=MakeColor(COLOR_NONE);
    	AUnit1D borderWidth=UnitPX(2);
    	std::shared_ptr<Glyph> glyph;
    	GlyphRegion(const std::string& name=MakeString()<<"g"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++)):
    		Region(name)
    	{
    		aspectRatio=AspectRatio::FixedHeight;
    	};
    	virtual void drawDebug(AlloyContext* context) override;
    	void draw(AlloyContext* context);
    };
    inline std::shared_ptr<GlyphRegion> MakeGlyphRegion(
    		const std::shared_ptr<ImageGlyph>& glyph,
    		const AUnit2D& position,
    		const AUnit2D& dimensions,
    		AspectRatio aspectRatio=AspectRatio::Unspecified,
    		RGBA bgColor=COLOR_NONE,
    		RGBA fgColor=COLOR_NONE,
    		RGBA borderColor=COLOR_NONE,
    		const AUnit1D& borderWidth=UnitPX(2)
    		){
    	std::shared_ptr<GlyphRegion> region=std::shared_ptr<GlyphRegion>((glyph->name.length()>0)?new GlyphRegion(glyph->name):new GlyphRegion());
    	region->glyph=glyph;
    	region->position=position;
    	region->dimensions=dimensions;
    	region->bgColor=MakeColor(bgColor);
    	region->fgColor=MakeColor(fgColor);
    	region->borderColor=MakeColor(borderColor);
    	region->borderWidth=borderWidth;
    	region->aspectRatio=aspectRatio;
    	region->aspect=glyph->width/(float)glyph->height;
    	return region;
    }
    inline std::shared_ptr<GlyphRegion> MakeGlyphRegion(
    		const std::shared_ptr<AwesomeGlyph>& glyph,
    		const AUnit2D& position,
    		const AUnit2D& dimensions,
    		RGBA bgColor=COLOR_NONE,
    		RGBA fgColor=COLOR_NONE,
    		RGBA borderColor=COLOR_NONE,
    		const AUnit1D& borderWidth=UnitPX(2)
    		){
    	std::shared_ptr<GlyphRegion> region=std::shared_ptr<GlyphRegion>(new GlyphRegion(glyph->name));
    	region->glyph=glyph;
    	region->position=position;
    	region->dimensions=dimensions;
    	region->bgColor=MakeColor(bgColor);
    	region->fgColor=MakeColor(fgColor);
    	region->borderColor=MakeColor(borderColor);
    	region->borderWidth=borderWidth;
    	region->aspectRatio=AspectRatio::FixedHeight;
    	region->aspect=glyph->width/(float)glyph->height;
    	return region;
    }

    struct TextLabel : public Region{
        HorizontalAlignment horizontalAlignment=HorizontalAlignment::Left;
    	VerticalAlignment verticalAlignment=VerticalAlignment::Top;

    	FontType fontType=FontType::Normal;
    	AUnit1D fontSize=UnitPT(14);
    	ColorPtr fontColor=MakeColor(COLOR_WHITE);
    	TextLabel(const std::string& name=MakeString()<<"t"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++)):
    		Region(name){};
    	void draw(AlloyContext* context);
    };
    inline std::shared_ptr<TextLabel> MakeLabel(const std::string& name,const AUnit2D& position,const AUnit2D& dimensions,
    		FontType fontType,const AUnit1D& fontSize=UnitPT(14.0f),
    		RGBA fontColor=COLOR_WHITE,HorizontalAlignment halign=HorizontalAlignment::Left,VerticalAlignment valign=VerticalAlignment::Top){
    	std::shared_ptr<TextLabel> region=std::shared_ptr<TextLabel>(new TextLabel(name));
    	region->position=position;
    	region->dimensions=dimensions;
    	region->fontColor=MakeColor(fontColor);
    	region->fontType=fontType;
    	region->fontSize=fontSize;
    	region->horizontalAlignment=halign;
    	region->verticalAlignment=valign;
    	return region;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Region & region) {
        	ss<<"Region: "<<region.name<<std::endl;
        	ss<<"\tRelative Position: "<<region.position<<std::endl;
        	ss<<"\tRelative Dimensions: "<<region.dimensions<<std::endl;
        	ss<<"\tBounds: "<<region.bounds<<std::endl;
        	ss<<"\tAspect Ratio: "<<region.aspectRatio<<std::endl;
        	if(region.parent!=nullptr)ss<<"\tParent: "<<region.parent->name<<std::endl;
        	return ss;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const GlyphRegion & region) {
        	ss<<"Glyph Region: "<<region.name<<std::endl;
        	if(region.glyph.get()!=nullptr)ss<<"\t"<<*region.glyph<<std::endl;
        	ss<<"\tRelative Position: "<<region.position<<std::endl;
        	ss<<"\tRelative Dimensions: "<<region.dimensions<<std::endl;
        	ss<<"\tBounds: "<<region.bounds<<std::endl;
        	ss<<"\tAspect Ratio: "<<region.aspectRatio<<" ("<<region.aspect<<")"<<std::endl;
        	if(region.parent!=nullptr)ss<<"\tParent: "<<region.parent->name<<std::endl;
        	return ss;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const TextLabel & region) {
    	ss<<"Text Label: "<<region.name<<std::endl;
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
    typedef std::shared_ptr<TextLabel> TextLabelPtr;
    typedef std::shared_ptr<Composite> CompositePtr;
    typedef std::shared_ptr<GlyphRegion> GlyphRegionPtr;
    typedef std::shared_ptr<Region> RegionPtr;
}
#endif /* ALLOYUI_H_ */
