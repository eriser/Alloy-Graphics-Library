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
#include "AlloyApplication.h"
#include "nanovg.h"
#include <iostream>
#include <memory>
#include <list>
namespace aly{
	bool SANITY_CHECK_UI();
	const double MM_TO_PIX=1.0;
	const double DP_TO_PIX=1.0/160.0;

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

	struct placement{
		virtual int2 toPixles(int2 screenSize,double2 dpmm){return int2();};
		virtual inline ~placement(){};
	};
	template<class T> struct placement_t : public placement{
		T value;
		placement_t(const T& t):value(t){}
	};
	struct coord_dp : public placement_t<int2>{
		coord_dp(const int x,const int y):placement_t(int2(x,y)){}
		coord_dp(const int2& t):placement_t(t){}
		int2 toPixles(int2 screenSize,double2 dpmm){
			return int2(std::floor(DP_TO_PIX*dpmm.x*value.x),std::floor(DP_TO_PIX*dpmm.y*value.y));
		}
	};

	struct coord_px : public placement_t<int2>{
		coord_px(const int x,const int y):placement_t(int2(x,y)){}
		coord_px(const int2& t):placement_t(t){}
		int2 toPixles(int2 screenSize,double2 dpmm){
			return value;
		}
	};
	struct coord_mm : public placement_t<float2>{
		coord_mm(const float x,const float y):placement_t(float2(x,y)){}
		coord_mm(const float2& t):placement_t(t){}
		int2 toPixles(int2 screenSize,double2 dpmm){
			return int2(std::floor(MM_TO_PIX*dpmm.x*value.x),std::floor(MM_TO_PIX*dpmm.y*value.y));
		}
	};
	struct percent: public placement_t<float2>{
		percent(const float x,const float y):placement_t(float2(x,y)){}
		percent(const float2& t):placement_t(t){}
		int2 toPixles(int2 screenSize,double2 dpmm){
			return int2(std::floor(screenSize.x*value.x),std::floor(screenSize.y*value.y));
		}
	};
	struct percent_dp : public placement_t<std::pair<percent,coord_dp>>{
		percent_dp(const float2& t,const int2& v):placement_t(std::pair<percent,coord_dp>(t,v)){}
		percent_dp(const float px,const float py,const int dx,const int dy):percent_dp(float2(px,py),int2(dx,dy)){}
		int2 toPixles(int2 screenSize,double2 dpmm){
			return value.first.toPixles(screenSize,dpmm)+value.second.toPixles(screenSize,dpmm);
		}
	};
	struct percent_px : public placement_t<std::pair<percent,coord_px>>{
		percent_px(const float2& t,const int2& v):placement_t(std::pair<percent,coord_px>(t,v)){}
		percent_px(const float px,const float py,const int dx,const int dy):percent_px(float2(px,py),int2(dx,dy)){}
		int2 toPixles(int2 screenSize,double2 dpmm){
			return value.first.toPixles(screenSize,dpmm)+value.second.toPixles(screenSize,dpmm);
		}
	};
	struct percent_mm : public placement_t<std::pair<percent,coord_mm>>{
		percent_mm(const float2& t,const float2& v):placement_t(std::pair<percent,coord_mm>(t,v)){}
		percent_mm(const float px,const float py,const float dx,const float dy):percent_mm(float2(px,py),float2(dx,dy)){}
		int2 toPixles(int2 screenSize,double2 dpmm){
			return value.first.toPixles(screenSize,dpmm)+value.second.toPixles(screenSize,dpmm);
		}
	};
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const coord_dp & v) { return ss << v.value; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const coord_px & v) { return ss << v.value; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const coord_mm & v) { return ss << v.value; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const percent & v) { return ss << v.value; }

    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const percent_dp & v) { return ss <<"["<< v.value.first<<","<<v.value.second<<"]"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const percent_px & v) { return ss <<"["<< v.value.first<<","<<v.value.second<<"]"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const percent_mm & v) { return ss <<"["<< v.value.first<<","<<v.value.second<<"]"; }


    struct Region {
    protected:
    	static uint64_t REGION_COUNTER;
    public:
    	box2i bounds;
    	placement position;
    	placement dimensions;
    	const std::string name;
    	Region(const std::string& name=MakeString()<<"r"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++));
    	virtual void pack(const int2& dims,const double2& dpmm){};
    	virtual void pack(AlloyContext* context){};
    	virtual void draw(AlloyContext* context)=0;
    	virtual inline ~Region(){};
    };
    struct Composite : public Region{
		protected:
			std::list<std::shared_ptr<Region>> children;
		public:
	    	Composite(const std::string& name=MakeString()<<"c"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++)):Region(name){};
			virtual void draw(AlloyContext* context);
			virtual void pack(const int2& dims,const double2& dpmm);
			virtual void pack(AlloyContext* context);
			Composite& add(const std::shared_ptr<Region>& region);
			Composite& add(Region* region);//After add(), composite will own region and be responsible for destroying it.
			void pack();
	    	void draw();
    };
    struct Label : public Region{
        HorizontalAlignment horizontalAlignment;
    	VerticalAlignment verticalAlignment;
    	std::shared_ptr<Font> font;
    	Label(const std::string& name=MakeString()<<"l"<<std::setw(8)<<std::setfill('0')<<(REGION_COUNTER++)):Label(name){};
    	void draw(AlloyContext* context);
    };
    inline std::shared_ptr<Label> MakeLabel(const std::string& name,const placement& position,const placement& dimensions){
    	std::shared_ptr<Label> label=std::shared_ptr<Label>(new Label(name));
    	label->position=position;
    	label->dimensions=dimensions;
    	return label;
    }
    inline std::shared_ptr<Composite> MakeComposite(const std::string& name,const placement& position,const placement& dimensions){
    	std::shared_ptr<Composite> composite=std::shared_ptr<Composite>(new Composite(name));
        composite->position=position;
        composite->dimensions=dimensions;
        return composite;
    }
}
#endif /* ALLOYUI_H_ */
