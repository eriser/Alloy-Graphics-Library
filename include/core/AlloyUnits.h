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

#ifndef ALLOYUNITS_H_
#define ALLOYUNITS_H_
#include "AlloyMath.h"
#include <string>
#include <iostream>
namespace aly{
	const float MM_TO_PIX=1.0;
	const float DP_TO_PIX=1.0/160.0;
	const float IN_TO_PIX=25.4;
	const float PT_TO_PIX=1.333333f;
	typedef int pixel;
	typedef vec<pixel,2> pixel2;
	typedef box<pixel,2> box2px;
	class AUnit1D{
		struct Interface
		{
			virtual pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const =0;
			virtual std::string toString() const = 0;
		};
	public:
		template<class T> struct Impl : public Interface{
			T value;
			Impl(const T& value):value(value){};
			virtual pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const {
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
		    pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const { return impl->toPixels(screenSize,dpmm,pixelRatio); }
		    std::string toString() const {return impl->toString(); }
	};

	class AUnit2D{
		struct Interface
		{
			virtual pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const =0;
			virtual std::string toString() const = 0;
		};
		template<class T> struct Impl : public Interface{
			T value;
			Impl(const T& value):value(value){};
			virtual pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
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
		    pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const { return impl->toPixels(screenSize,dpmm,pixelRatio); }
		    std::string toString() const {return impl->toString(); }
	};
	struct UnitDP{
		int value;
		UnitDP(int x):value(x){}
		pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const {
			return (float)(DP_TO_PIX*dpmm*value);
		}
	};

	struct UnitPX{
		pixel value;
		UnitPX(float x):value((pixel)x){}
		UnitPX(int x):value((pixel)x){}
		pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const {
			return value;
		}
	};

	struct UnitPT{
		float value;
		UnitPT(float x):value(x){}
		pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const {
			return (float)(PT_TO_PIX*value*pixelRatio);
		}
	};
	struct UnitMM{
		float value;
		UnitMM(float x):value(x){}
		pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const {
			return (float)(MM_TO_PIX*dpmm*value);
		}
	};
	struct UnitIN{
		float value;
		UnitIN(float x):value(x){}
		pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const {
			return (float)(IN_TO_PIX*dpmm*value);
		}
	};
	struct UnitPercent{
		float value;
		UnitPercent(float x):value(x){}
		pixel toPixels(pixel screenSize,double dpmm,double pixelRatio) const {
			return (float)(screenSize*value);
		}
	};

	struct CoordDP{
		int2 value;
		CoordDP(int x,int y):value(x,y){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return  pixel2((pixel)(DP_TO_PIX*dpmm.x*value.x),(pixel)(DP_TO_PIX*dpmm.y*value.y));
		}
	};
	struct CoordPX{
		pixel2 value;
		CoordPX(float x,float y):value((pixel)x,(pixel)y){}
		CoordPX(int x,int y):value((pixel)x,(pixel)y){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return value;
		}
	};
	struct CoordMM{
		float2 value;
		CoordMM(float x,float y):value(x,y){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return  pixel2((pixel)(MM_TO_PIX*dpmm.x*value.x),(pixel)(MM_TO_PIX*dpmm.y*value.y));
		}
	};
	struct CoordPT{
		float2 value;
		CoordPT(float x,float y):value(x,y){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return  pixel2((pixel)(PT_TO_PIX*pixelRatio*value.x),(pixel)(PT_TO_PIX*pixelRatio*value.y));
		}
	};
	struct CoordIN{
		float2 value;
		CoordIN(float x,float y):value(x,y){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return  pixel2((pixel)(IN_TO_PIX*dpmm.x*value.x),(pixel)(IN_TO_PIX*dpmm.y*value.y));
		}
	};
	struct CoordPercent{
		float2 value;
		CoordPercent(float x,float y):value(x,y){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return  pixel2((pixel)(screenSize.x*value.x),(pixel)(screenSize.y*value.y));
		}
	};
	struct CoordPerDP{
		std::pair<CoordPercent,CoordDP> value;
		CoordPerDP(float px,float py,int x,int y):value(CoordPercent(px,py),CoordDP(x,y)){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
	struct CoordPerPX{
		std::pair<CoordPercent,CoordPX> value;
		CoordPerPX(float px,float py,int x,int y):value(CoordPercent(px,py),CoordPX(x,y)){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
	struct CoordPerPT{
		std::pair<CoordPercent,CoordPT> value;
		CoordPerPT(float px,float py,float x,float y):value(CoordPercent(px,py),CoordPT(x,y)){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
	struct CoordPerMM{
		std::pair<CoordPercent,CoordMM> value;
		CoordPerMM(float px,float py,float x,float y):value(CoordPercent(px,py),CoordMM(x,y)){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
			return value.first.toPixels(screenSize,dpmm,pixelRatio)+value.second.toPixels(screenSize,dpmm,pixelRatio);
		}
	};
	struct CoordPerIN{
		std::pair<CoordPercent,CoordIN> value;
		CoordPerIN(float px,float py,float x,float y):value(CoordPercent(px,py),CoordIN(x,y)){}
		pixel2 toPixels(pixel2 screenSize,double2 dpmm,double pixelRatio) const {
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
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordPercent & v) { return ss << "("<<v.value.x*100<<"%, "<<v.value.y*100<<"%)"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const AUnit2D& v) { return ss <<v.toString(); }

    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordPerDP & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordPerPX & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordPerPT & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordPerMM & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const CoordPerIN & v) { return ss <<"{"<< v.value.first<<", "<<v.value.second<<"}"; }
}
#endif /* ALLOYUNITS_H_ */
