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
#ifndef ALLOYIMAGE2D_H_INCLUDE_GUARD
#define ALLOYIMAGE2D_H_INCLUDE_GUARD
#include "AlloyCommon.h"
#include "AlloyMath.h"

#include <vector>
#include <functional>
namespace aly{
	bool SANITY_CHECK_IMAGE();
	enum class ImageType{BYTE,UBYTE,SHORT,USHORT,INT,UINT,FLOAT,DOUBLE};
	template<class T,int C,ImageType I> struct Image{
	private:
		std::vector<vec<T,C>> storage;
	public:
		std::vector<vec<T,C>>& data;
		int width;
		int height;
		int x,y;
		uint64_t id;
		const int channels = C;
		const ImageType type = I;

		void set(const T& val){
			for(T& x:data){
				x=vec<T,C>(val);
			}
		}
		void set(const vec<T,C>& val){
			for(vec<T,C>& x:data){
				x=val;
			}
		}

		void set(T* val){
			if(val==nullptr)return;
			size_t offset=0;
			for(vec<T,C>& x:data){
				for(int c=0;c<C;c++){
					x[c]=val[offset++];
				}
			}
		}
		void set(vec<T,C>* val){
			if(val==nullptr)return;
			size_t offset=0;
			for(vec<T,C>& x:data){
				x=val[offset++];
			}
		}
		void set(const Image<T,C,I>& other){
			resize(other.width,other.height);
			id=other.id;
			x=other.x;
			y=other.y;
			set(&other.data[0]);
		}
		std::string getTypeName() const {
			std::string tName;
			switch(type){
				case ImageType::BYTE: tName="byte";break;
				case ImageType::UBYTE: tName="ubyte";break;
				case ImageType::SHORT: tName="short";break;
				case ImageType::USHORT: tName="ushort";break;
				case ImageType::INT: tName="int";break;
				case ImageType::UINT: tName="uint";break;
				case ImageType::FLOAT: tName="float";break;
				case ImageType::DOUBLE: tName="double";break;
			}
			return MakeString()<<tName<<channels;
		}
		const Image<T, C, I>& operator=(Image<T, C, I>& img) const{
			return Image<T, C, I>(&data[0],img.width,img.height,img.x,img.y,img.id);
		}

		const Image<T, C, I>& operator=(const Image<T, C, I>& img) const {
			return Image<T, C, I>(&data[0],img.width,img.height,img.x,img.y,img.id);
		}

		Image(int w,int h,int x=0,int y=0,int id=0):width(w),height(h),x(x),y(x),id(id),data(storage){
			data.resize(w*h);
			data.shrink_to_fit();
		}
		Image(T* ptr,int w,int h,int x=0,int y=0,int id=0):Image(w,h,x,y,id){
			set(ptr);
		}
		Image(vec<T,C>* ptr,int w,int h,int x=0,int y=0,int id=0):Image(w,h,x,y,id){
			set(ptr);
		}
		Image(std::vector<vec<T,C>>& ref,int w,int h,int x=0,int y=0,int id=0):width(w),height(h),x(x),y(y),id(id),data(ref){
		}
		Image():width(0),height(0),x(0),y(0),id(0),data(storage){
		}

		int2 dimensions() const {
			return int2(width,height);
		}
		int2 position() const {
			return int2(x,y);
		}
		size_t size() const {
			return data.size();
		}
		size_t typeSize() const {
			return sizeof(vec<T,C>);
		}
		void resize(int w,int h) {
			data.resize(w*h);
			data.shrink_to_fit();
			width=w;
			height=h;
		}
		vec<T,C>* vecPtr(){
			if(data.size()==0)return nullptr;
			return data.data();
		}
		T* ptr(){
			if(data.size()==0)return nullptr;
			return &(data.data[0][0]);
		}
		void setZero(){
			memset(this->data(),0,sizeof(vec<T,C>)*data.size());
		}
		const vec<T, C>& operator[](const size_t i) const {
			return data[i];
		}
		vec<T, C>& operator[](const size_t i) {
			return data[i];
		}
		vec<T, C>& operator()(const int i, const int j) {
			return data[clamp(i, 0, width - 1) + clamp(j, 0, height - 1)*width];
		}
		vec<T, C>& operator()(const int2 ij) {
			return data[clamp(ij.x, 0, width - 1) + clamp(ij.y, 0, height - 1)*width];
		}
		const vec<T, C>& operator()(const int i, const int j) const {
			return data[clamp(i, 0, width - 1) + clamp(j, 0, height - 1)*width];
		}
		const vec<T, C>& operator()(const int2 ij) const {
			return data[clamp(ij.x, 0, width - 1) + clamp(ij.y, 0, height - 1)*width];
		}
		template<class K> vec<K,C> operator()(const K x, const K y){
			int i = static_cast<int>(std::floor(x));
			int j = static_cast<int>(std::floor(y));
			vec<K, C> rgb00 = ConvertType<K,T,C>(operator()(i, j));
			vec<K, C> rgb10 = ConvertType<K,T,C>(operator()(i + 1, j));
			vec<K, C> rgb11 = ConvertType<K,T,C>(operator()(i + 1, j + 1));
			vec<K, C> rgb01 = ConvertType<K,T,C>(operator()(i, j + 1));
			K dx = x - i;
			K dy = y - j;
			return ((rgb00*(K(1) - dx) + rgb10*dx)*(K(1) - dy) + (rgb01*(K(1) - dx) + rgb11*dx)*dy);
		}
		template<class K> inline vec<K, C> operator()(const vec<K,2>& pt){
			return operator()(pt.x,pt.y);
		}
		template<class F> void apply(F f){
			size_t sz=size();
	#pragma omp parallel for
			for(size_t offset=0;offset<sz;offset++){
				f(offset,data[offset]);
			}
		}
	};

	template<class T, int C,ImageType I> void Transform(Image<T,C,I>& im1,Image<T,C,I>& im2,const std::function<void(vec<T,C>&,vec<T,C>&)>& func){
			if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
			size_t sz=im1.size();
	#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(im1.data[offset],im2.data[offset]);
				}
			}
	template<class T, int C,ImageType I> void Transform(Image<T,C,I>& im1,const std::function<void(vec<T,C>&)>& func){
			size_t sz=im1.size();
	#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(im1.data[offset]);
				}
			}
	template<class T, int C,ImageType I> void Transform(Image<T,C,I>& im1,const Image<T,C,I>& im2,const std::function<void(vec<T,C>&,const vec<T,C>&)>& func){
			if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
			size_t sz=im1.size();
	#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(im1.data[offset],im2.data[offset]);
				}
			}
	template<class T, int C,ImageType I> void Transform(Image<T,C,I>& im1,const Image<T,C,I>& im2,const Image<T,C,I>& im3,const std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)>& func){
			if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
			size_t sz=im1.size();
	#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(im1.data[offset],im2.data[offset],im3.data[offset]);
				}
			}
	template<class T, int C,ImageType I> void Transform(Image<T,C,I>& im1,Image<T,C,I>& im2,const std::function<void(int i,int j,vec<T,C>& val1,vec<T,C>& val2)>& func){
		if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
		#pragma omp parallel for
				for(int j=0;j<im1.height;j++){
					for(int i=0;i<im1.width;i++){
						size_t offset=i+j*im1.width;
						func(i,j,im1.data[offset],im2.data[offset]);
					}
				}
			}
	template<class T, int C,ImageType I> void Transform(Image<T,C,I>& im1,Image<T,C,I>& im2,const std::function<void(size_t offset,vec<T,C>& val1,vec<T,C>& val2)>& func){
			if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
			size_t sz=im1.size();
		#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(offset,im1.data[offset],im2.data[offset]);
				}
			}
	   template<class T,class L,class R,int C,ImageType I> std::basic_ostream<L,R> & operator << (
			   std::basic_ostream<L,R> & ss, const Image<T,C,I> & A) {
	    	ss<<"Image ("<<A.getTypeName()<<"): "<<A.id<<" Position: ("<<A.x<<","<<A.y<<") Dimensions: ["<<A.width<<","<<A.height<<"]\n";
	    	return ss;
	    }
		template<class T, int C,ImageType I> Image<T, C, I> operator+(const vec<T,C>& scalar,const Image<T, C, I>& img) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=scalar+val2;};
			Transform(out,img,f);
			return out;
		}

		template<class T, int C,ImageType I> Image<T, C, I> operator-(const vec<T,C>& scalar,const Image<T, C, I>& img) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=scalar-val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator*(const vec<T,C>& scalar,const Image<T, C, I>& img) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=scalar*val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator/(const vec<T,C>& scalar,const Image<T, C, I>& img) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=scalar/val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator+(const Image<T, C, I>& img,const vec<T,C>& scalar) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=val2+scalar;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator-(const Image<T, C, I>& img,const vec<T,C>& scalar) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=val2-scalar;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator*(const Image<T, C, I>& img,const vec<T,C>& scalar) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=val2*scalar;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator/(const Image<T, C, I>& img,const vec<T,C>& scalar) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=val2/scalar;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator-(const Image<T, C, I>& img) {
			Image<T, C, I> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=-val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator+=(Image<T, C, I>& out,const Image<T, C, I>& img) {
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1+=val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator-=(Image<T, C, I>& out,const Image<T, C, I>& img) {
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1-=val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator*=(Image<T, C, I>& out,const Image<T, C, I>& img) {
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1*=val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator/=(Image<T, C, I>& out,const Image<T, C, I>& img) {
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1/=val2;};
			Transform(out,img,f);
			return out;
		}

		template<class T, int C,ImageType I> Image<T, C, I> operator+=(Image<T, C, I>& out,const vec<T, C>& scalar) {
			std::function<void(vec<T,C>&)> f=[=](vec<T,C>& val1){val1+=scalar;};
			Transform(out,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator-=(Image<T, C, I>& out,const vec<T, C>& scalar) {
			std::function<void(vec<T,C>&)> f=[=](vec<T,C>& val1){val1-=scalar;};
			Transform(out,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator*=(Image<T, C, I>& out,const vec<T, C>& scalar) {
			std::function<void(vec<T,C>&)> f=[=](vec<T,C>& val1){val1*=scalar;};
			Transform(out,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator/=(Image<T, C, I>& out,const vec<T, C>& scalar) {
			std::function<void(vec<T,C>&)> f=[=](vec<T,C>& val1){val1/=scalar;};
			Transform(out,f);
			return out;
		}

		template<class T, int C,ImageType I> Image<T, C, I> operator+(const Image<T, C, I>& img1,const Image<T, C, I>& img2) {
			Image<T, C, I> out(img1.width, img1.height);
			std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3){val1=val2+val3;};
			Transform(out,img1,img2,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator-(const Image<T, C, I>& img1,const Image<T, C, I>& img2) {
			Image<T, C, I> out(img1.width, img1.height);
			std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)> f=
					[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3){val1=val2-val3;};
			Transform(out,img1,img2,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator*(const Image<T, C, I>& img1,const Image<T, C, I>& img2) {
			Image<T, C, I> out(img1.width, img1.height);
			std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)> f=
					[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3){val1=val2*val3;};
			Transform(out,img1,img2,f);
			return out;
		}
		template<class T, int C,ImageType I> Image<T, C, I> operator/(const Image<T, C, I>& img1,const Image<T, C, I>& img2) {
			Image<T, C, I> out(img1.width, img1.height);
			std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)> f=
					[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3){val1=val2/val3;};
			Transform(out,img1,img2,f);
			return out;
		}
	typedef Image<uint8_t, 4,ImageType::UBYTE> ImageRGBA;
	typedef Image<int, 4,ImageType::INT> ImageRGBAi;
	typedef Image<float, 4,ImageType::FLOAT> ImageRGBAf;

	typedef Image<uint8_t, 3,ImageType::UBYTE> ImageRGB;
	typedef Image<int, 3,ImageType::INT> ImageRGBi;
	typedef Image<float, 3,ImageType::FLOAT> ImageRGBf;

	typedef Image<uint8_t, 1,ImageType::UBYTE> ImageA;
	typedef Image<int, 1,ImageType::INT> ImageAi;
	typedef Image<float, 1,ImageType::FLOAT> ImageAf;

	typedef Image<uint8_t, 4,ImageType::UBYTE> Image4b;
	typedef Image<uint16_t, 4,ImageType::USHORT> Image4us;
	typedef Image<int16_t, 4,ImageType::SHORT> Image4s;
	typedef Image<int, 4,ImageType::INT> Image4i;
	typedef Image<uint32_t, 4,ImageType::UINT> Image4ui;
	typedef Image<float, 4,ImageType::FLOAT> Image4f;

	typedef Image<uint8_t, 3,ImageType::UBYTE> Image3b;
	typedef Image<uint16_t, 3,ImageType::USHORT> Image3us;
	typedef Image<int16_t, 3,ImageType::SHORT> Image3s;
	typedef Image<int, 3,ImageType::INT> Image3i;
	typedef Image<uint32_t, 3,ImageType::UINT> Image3ui;
	typedef Image<float, 3,ImageType::FLOAT> Image3f;

	typedef Image<uint8_t, 2,ImageType::UBYTE> Image2b;
	typedef Image<uint16_t, 2,ImageType::USHORT> Image2us;
	typedef Image<int16_t, 2,ImageType::SHORT> Image2s;
	typedef Image<int, 2,ImageType::INT> Image2i;
	typedef Image<uint32_t, 2,ImageType::UINT> Image2ui;
	typedef Image<float, 2,ImageType::FLOAT> Image2f;

	typedef Image<uint8_t, 1,ImageType::UBYTE> Image1b;
	typedef Image<uint16_t, 1,ImageType::USHORT> Image1us;
	typedef Image<int16_t, 1,ImageType::SHORT> Image1s;

	typedef Image<int, 1,ImageType::INT> Image1i;
	typedef Image<uint32_t, 1,ImageType::UINT> Image1ui;
	typedef Image<float, 1,ImageType::FLOAT> Image1f;
};


#endif
