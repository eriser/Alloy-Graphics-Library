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
#include "nanovg.h"
#include <iostream>
#include <memory>
#include <list>
#include <vector>
namespace aly{
	bool SANITY_CHECK_UI();
	const double MM_TO_PIX=1.0;
	const double DP_TO_PIX=1.0/160.0;
	const double IN_TO_PIX=25.4;

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
	class AUnit1D{
		struct Interface
		{
			virtual int toPixels(int screenSize,double dpmm,double pixelRatio) const =0;
			virtual std::string toString() const = 0;
		};
	public:
		template<class T> struct Impl : public Interface{
			T value;
			Impl(const T& value):value(value){};
			virtual int toPixels(int screenSize,double dpmm,double pixelRatio) const {
				return value.toPixels(screenSize,dpmm,pixelRatio);
			}
			virtual std::string toString() const {
				return MakeString()<<value;
			}
		};
		std::shared_ptr<Interface> impl;

		    // Can construct or assign any type that implements the implicit interface (According to Sterling)
		    AUnit1D() {}
		    AUnit1D(const AUnit1D & r) : impl(r.impl) {}
		    virtual inline ~AUnit1D(){}
		    template<class T> AUnit1D(const T & value) : impl(new Impl<T>{value}) {}
		    AUnit1D & operator = (const AUnit1D & r) { impl = r.impl; return *this; }
		    template<class T> AUnit1D & operator = (const T & value) { return *this = AUnit1D(value); }
		    // Implicit interface
		    int toPixels(int screenSize,double dpmm,double pixelRatio) const { return impl->toPixels(screenSize,dpmm,pixelRatio); }
		    std::string toString() const {return impl->toString(); }
	};

	class AUnit2D{
		struct Interface
		{
			virtual int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const =0;
			virtual std::string toString() const = 0;
		};
		template<class T> struct Impl : public Interface{
			T value;
			Impl(const T& value):value(value){};
			virtual int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
				return value.toPixels(screenSize,dpmm,pixelRatio);
			}
			virtual std::string toString() const {
				return MakeString()<<value;
			}
		};
		std::shared_ptr<Interface> impl;
		public:
		    // Can construct or assign any type that implements the implicit interface (According to Sterling)
		    AUnit2D() {}
		    AUnit2D(const AUnit2D & r) : impl(r.impl) {}
		    virtual inline ~AUnit2D(){}
		    template<class T> AUnit2D(const T & value):impl(new Impl<T>{value}) {}
		    AUnit2D & operator = (const AUnit2D & r) { impl = r.impl; return *this; }
		    template<class T> AUnit2D & operator = (const T & value) { return *this = AUnit2D(value); }
		    // Implicit interface
		    int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const { return impl->toPixels(screenSize,dpmm,pixelRatio); }
		    std::string toString() const {return impl->toString(); }
	};
	struct UnitDP{
		int value;
		UnitDP(int x):value(x){}
		int toPixels(int screenSize,double dpmm,double pixelRatio) const {
			return std::floor(DP_TO_PIX*dpmm*value);
		}
	};

	struct UnitPX{
		int value;
		UnitPX(int x):value(x){}
		int toPixels(int screenSize,double dpmm,double pixelRatio) const {
			return value;
		}
	};

	struct UnitPT{
		float value;
		UnitPT(float x):value(x){}
		int toPixels(int screenSize,double dpmm,double pixelRatio) const {
			return (int)std::floor(value*pixelRatio);
		}
	};
	struct UnitMM{
		float value;
		UnitMM(float x):value(x){}
		int toPixels(int screenSize,double dpmm,double pixelRatio) const {
			return std::floor(MM_TO_PIX*dpmm*value);
		}
	};
	struct UnitIN{
		float value;
		UnitIN(float x):value(x){}
		int toPixels(int screenSize,double dpmm,double pixelRatio) const {
			return std::floor(IN_TO_PIX*dpmm*value);
		}
	};
	struct UnitPercent{
		float value;
		UnitPercent(float x):value(x){}
		int toPixels(int screenSize,double dpmm,double pixelRatio) const {
			return std::floor(screenSize*value);
		}
	};

	struct CoordDP{
		int2 value;
		CoordDP(int x,int y):value(x,y){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return int2(std::floor(DP_TO_PIX*dpmm.x*value.x),std::floor(DP_TO_PIX*dpmm.y*value.y));
		}
	};
	struct CoordPX{
		int2 value;
		CoordPX(int x,int y):value(x,y){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return value;
		}
	};
	struct CoordMM{
		float2 value;
		CoordMM(float x,float y):value(x,y){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return int2(std::floor(MM_TO_PIX*dpmm.x*value.x),std::floor(MM_TO_PIX*dpmm.y*value.y));
		}
	};
	struct CoordPT{
		float2 value;
		CoordPT(float x,float y):value(x,y){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return int2(std::floor(pixelRatio*value.x),std::floor(pixelRatio*value.y));
		}
	};
	struct CoordIN{
		float2 value;
		CoordIN(float x,float y):value(x,y){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return int2(std::floor(IN_TO_PIX*dpmm.x*value.x),std::floor(IN_TO_PIX*dpmm.y*value.y));
		}
	};
	struct Percent{
		float2 value;
		Percent(float x,float y):value(x,y){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return int2(std::floor(screenSize.x*value.x),std::floor(screenSize.y*value.y));
		}
	};
	struct PercentDP{
		std::pair<Percent,CoordDP> value;
		PercentDP(float px,float py,int x,int y):value(Percent(px,py),CoordDP(x,y)){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
	struct PercentPX{
		std::pair<Percent,CoordPX> value;
		PercentPX(float px,float py,int x,int y):value(Percent(px,py),CoordPX(x,y)){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
	struct PercentPT{
		std::pair<Percent,CoordPT> value;
		PercentPT(float px,float py,float x,float y):value(Percent(px,py),CoordPT(x,y)){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
	struct PercentMM{
		std::pair<Percent,CoordMM> value;
		PercentMM(float px,float py,float x,float y):value(Percent(px,py),CoordMM(x,y)){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
	struct PercentIN{
		std::pair<Percent,CoordIN> value;
		PercentIN(float px,float py,float x,float y):value(Percent(px,py),CoordIN(x,y)){}
		int2 toPixels(int2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const UnitDP & v) { return ss<<v.value<<" dp"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const UnitPX & v) { return ss<<v.value<<" px"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const UnitMM & v) { return ss<<v.value<<" mm"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const UnitIN & v) { return ss<<v.value<<" in"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const UnitPT & v) { return ss<<v.value<<" pt"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const UnitPercent & v) { return ss<<v.value*100<<"%"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const AUnit1D& v) { return ss <<v.toString(); }

	template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordDP & v) { return ss <<"("<<v.value.x<<" dp, "<<v.value.y<<" dp)"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordPX & v) { return ss <<"("<<v.value.x<<" px, "<<v.value.y<<" px)"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordPT & v) { return ss <<"("<<v.value.x<<" pt, "<<v.value.y<<" pt)"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordMM & v) { return ss <<"("<<v.value.x<<" mm, "<<v.value.y<<" mm)"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordIN & v) { return ss << "("<<v.value.x<<" in, "<<v.value.y<<" in)"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Percent & v) { return ss << "("<<v.value.x*100<<"%, "<<v.value.y*100<<"%)"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const AUnit2D& v) { return ss <<v.toString(); }

    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const PercentDP & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const PercentPX & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const PercentPT & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const PercentMM & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const PercentIN & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }


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
        	ss<<"\tBounds "<<region.bounds<<std::endl;
        	if(region.parent!=nullptr)ss<<"\tParent "<<region.parent->name<<std::endl;
        	return ss;
        }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Label & region) {
    	ss<<"Label: "<<region.name<<std::endl;
    	ss<<"\tRelative Position: "<<region.position<<std::endl;
    	ss<<"\tRelative Dimensions: "<<region.dimensions<<std::endl;
    	ss<<"\tFont Type: "<<region.fontType<<std::endl;
    	ss<<"\tFont Size: "<<region.fontSize<<std::endl;
    	ss<<"\tFont Color: "<<region.fontColor<<std::endl;
    	ss<<"\tBounds "<<region.bounds<<std::endl;
    	if(region.parent!=nullptr)ss<<"\tParent "<<region.parent->name<<std::endl;
    	return ss;
    }

    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss,const Composite& region) {
    	ss<<"Composite: "<<region.name<<std::endl;
    	ss<<"\tRelative Position: "<<region.position<<std::endl;
    	ss<<"\tRelative Dimensions: "<<region.dimensions<<std::endl;
    	ss<<"\tBounds: "<<region.bounds<<std::endl;
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
