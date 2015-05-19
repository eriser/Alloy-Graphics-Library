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
#include <iostream>
namespace aly{
	bool SANITY_CHECK_UI();
	template<class T> struct screen_unit{
		T position;
		screen_unit(const T& t):position(t){}
		virtual int2 toPixles(int screenWidth,int screenHeight,double dpi)=0;
		virtual inline ~screen_unit(){};
	};
	struct coord_dp : public screen_unit<int2>{
		coord_dp(const int x,const int y):screen_unit(int2(x,y)){}
		coord_dp(const int2& t):screen_unit(t){}
		int2 toPixles(int screenWidth,int screenHeight,double dpi){
			return int2();
		}
	};

	struct coord_px : public screen_unit<int2>{
		coord_px(const int x,const int y):screen_unit(int2(x,y)){}
		coord_px(const int2& t):screen_unit(t){}
		int2 toPixles(int screenWidth,int screenHeight,double dpi){
			return int2();
		}
	};
	struct coord_mm : public screen_unit<float2>{
		coord_mm(const float x,const float y):screen_unit(float2(x,y)){}
		coord_mm(const float2& t):screen_unit(t){}
		int2 toPixles(int screenWidth,int screenHeight,double dpi){
			return int2();
		}
	};
	struct percent: public screen_unit<float2>{
		percent(const float x,const float y):screen_unit(float2(x,y)){}
		percent(const float2& t):screen_unit(t){}
		int2 toPixles(int screenWidth,int screenHeight,double dpi){
			return int2();
		}
	};
	struct percent_dp : public screen_unit<std::pair<percent,coord_dp>>{
		percent_dp(const float2& t,const int2& v):screen_unit(std::pair<percent,coord_dp>(t,v)){}
		percent_dp(const float px,const float py,const int dx,const int dy):percent_dp(float2(px,py),int2(dx,dy)){}
		int2 toPixles(int screenWidth,int screenHeight,double dpi){
			return int2();
		}
	};
	struct percent_px : public screen_unit<std::pair<percent,coord_px>>{
		percent_px(const float2& t,const int2& v):screen_unit(std::pair<percent,coord_px>(t,v)){}
		percent_px(const float px,const float py,const int dx,const int dy):percent_px(float2(px,py),int2(dx,dy)){}
		int2 toPixles(int screenWidth,int screenHeight,double dpi){
			return int2();
		}
	};
	struct percent_mm : public screen_unit<std::pair<percent,coord_mm>>{
		percent_mm(const float2& t,const float2& v):screen_unit(std::pair<percent,coord_mm>(t,v)){}
		percent_mm(const float px,const float py,const float dx,const float dy):percent_mm(float2(px,py),float2(dx,dy)){}

		int2 toPixles(int screenWidth,int screenHeight,double dpi){
			return int2();
		}
	};
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const coord_dp & v) { return ss << v.position; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const coord_px & v) { return ss << v.position; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const coord_mm & v) { return ss << v.position; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const percent & v) { return ss << v.position; }

    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const percent_dp & v) { return ss <<"["<< v.position.first<<","<<v.position.second<<"]"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const percent_px & v) { return ss <<"["<< v.position.first<<","<<v.position.second<<"]"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const percent_mm & v) { return ss <<"["<< v.position.first<<","<<v.position.second<<"]"; }

}
#endif /* ALLOYUI_H_ */
