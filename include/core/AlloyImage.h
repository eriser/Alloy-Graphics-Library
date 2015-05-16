#ifndef ALLOYIMAGE2D_H_INCLUDE_GUARD
#define ALLOYIMAGE2D_H_INCLUDE_GUARD
#include "AlloyMath.h"
#include <vector>
#include <functional>
namespace aly{
	bool SANITY_CHECK_IMAGE();
	template<class T,int C> struct Image{
	private:
		std::vector<vec<T,C>> storage;
	public:
		std::vector<vec<T,C>>& data;
		int width;
		int height;
		int x,y;
		uint64_t id;
		static const int channels = C;
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
			for(T& x:data){
				x=vec<T,C>(val[offset]);
			}
		}
		void set(vec<T,C>* val){
			if(val==nullptr)return;
			size_t offset=0;
			for(vec<T,C>& x:data){
				x=val[offset++];
			}
		}
		aly::Image<T, C> operator=(Image<T, C>& img){
			return Image<T, C>(&data[0],width,height,x,y,id);
		}
		aly::Image<T, C> operator=(const Image<T, C>& img){
			return Image<T, C>(&data[0],width,height,x,y,id);
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
		template<class F> void apply(F f){
			size_t sz=size();
	#pragma omp parallel for
			for(size_t offset=0;offset<sz;offset++){
				f(offset,data[offset]);
			}
		}
	};

	template<class T, int C> void Transform(Image<T,C>& im1,Image<T,C>& im2,const std::function<void(vec<T,C>&,vec<T,C>&)>& func){
			if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
			size_t sz=im1.size();
	#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(im1.data[offset],im2.data[offset]);
				}
			}
	template<class T, int C> void Transform(Image<T,C>& im1,const std::function<void(vec<T,C>&)>& func){
			size_t sz=im1.size();
	#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(im1.data[offset]);
				}
			}
	template<class T, int C> void Transform(Image<T,C>& im1,const Image<T,C>& im2,const std::function<void(vec<T,C>&,const vec<T,C>&)>& func){
			if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
			size_t sz=im1.size();
	#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(im1.data[offset],im2.data[offset]);
				}
			}
	template<class T, int C> void Transform(Image<T,C>& im1,const Image<T,C>& im2,const Image<T,C>& im3,const std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)>& func){
			if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
			size_t sz=im1.size();
	#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(im1.data[offset],im2.data[offset],im3.data[offset]);
				}
			}
	template<class T, int C> void Transform(Image<T,C>& im1,Image<T,C>& im2,const std::function<void(int i,int j,vec<T,C>& val1,vec<T,C>& val2)>& func){
		if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
		#pragma omp parallel for
				for(int j=0;j<im1.height;j++){
					for(int i=0;i<im1.width;i++){
						size_t offset=i+j*im1.width;
						func(i,j,im1.data[offset],im2.data[offset]);
					}
				}
			}
	template<class T, int C> void Transform(Image<T,C>& im1,Image<T,C>& im2,const std::function<void(size_t offset,vec<T,C>& val1,vec<T,C>& val2)>& func){
			if(im1.dimensions()!=im2.dimensions())throw std::runtime_error(MakeString()<<"Image dimensions do not match. "<<im1.dimensions()<<"!="<<im2.dimensions());
			size_t sz=im1.size();
		#pragma omp parallel for
				for(size_t offset=0;offset<sz;offset++){
					func(offset,im1.data[offset],im2.data[offset]);
				}
			}
	   template<class T,class L,class R,int C> std::basic_ostream<L,R> & operator << (std::basic_ostream<L,R> & ss, const Image<T,C> & A) {
	    	ss<<"Image: "<<A.id<<" Position: ("<<A.x<<","<<A.y<<") Dimensions: ["<<A.width<<","<<A.height<<"]\n";
	    	return ss;
	    }
		template<class T, int C> Image<T, C> operator+(const vec<T,C>& scalar,const Image<T, C>& img) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=scalar+val2;};
			Transform(out,img,f);
			return out;
		}

		template<class T, int C> Image<T, C> operator-(const vec<T,C>& scalar,const Image<T, C>& img) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=scalar-val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator*(const vec<T,C>& scalar,const Image<T, C>& img) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=scalar*val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator/(const vec<T,C>& scalar,const Image<T, C>& img) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=scalar/val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator+(const Image<T, C>& img,const vec<T,C>& scalar) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=val2+scalar;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator-(const Image<T, C>& img,const vec<T,C>& scalar) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=val2-scalar;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator*(const Image<T, C>& img,const vec<T,C>& scalar) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=val2*scalar;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator/(const Image<T, C>& img,const vec<T,C>& scalar) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=val2/scalar;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator-(const Image<T, C>& img) {
			Image<T, C> out(img.width, img.height,img.x,img.y);
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1=-val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator+=(Image<T, C>& out,const Image<T, C>& img) {
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1+=val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator-=(Image<T, C>& out,const Image<T, C>& img) {
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1-=val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator*=(Image<T, C>& out,const Image<T, C>& img) {
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1*=val2;};
			Transform(out,img,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator/=(Image<T, C>& out,const Image<T, C>& img) {
			std::function<void(vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2){val1/=val2;};
			Transform(out,img,f);
			return out;
		}

		template<class T, int C> Image<T, C> operator+=(Image<T, C>& out,const vec<T, C>& scalar) {
			std::function<void(vec<T,C>&)> f=[=](vec<T,C>& val1){val1+=scalar;};
			Transform(out,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator-=(Image<T, C>& out,const vec<T, C>& scalar) {
			std::function<void(vec<T,C>&)> f=[=](vec<T,C>& val1){val1-=scalar;};
			Transform(out,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator*=(Image<T, C>& out,const vec<T, C>& scalar) {
			std::function<void(vec<T,C>&)> f=[=](vec<T,C>& val1){val1*=scalar;};
			Transform(out,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator/=(Image<T, C>& out,const vec<T, C>& scalar) {
			std::function<void(vec<T,C>&)> f=[=](vec<T,C>& val1){val1/=scalar;};
			Transform(out,f);
			return out;
		}

		template<class T, int C> Image<T, C> operator+(const Image<T, C>& img1,const Image<T, C>& img2) {
			Image<T, C> out(img1.width, img1.height);
			std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)> f=[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3){val1=val2+val3;};
			Transform(out,img1,img2,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator-(const Image<T, C>& img1,const Image<T, C>& img2) {
			Image<T, C> out(img1.width, img1.height);
			std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)> f=
					[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3){val1=val2-val3;};
			Transform(out,img1,img2,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator*(const Image<T, C>& img1,const Image<T, C>& img2) {
			Image<T, C> out(img1.width, img1.height);
			std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)> f=
					[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3){val1=val2*val3;};
			Transform(out,img1,img2,f);
			return out;
		}
		template<class T, int C> Image<T, C> operator/(const Image<T, C>& img1,const Image<T, C>& img2) {
			Image<T, C> out(img1.width, img1.height);
			std::function<void(vec<T,C>&,const vec<T,C>&,const vec<T,C>&)> f=
					[=](vec<T,C>& val1,const vec<T,C>& val2,const vec<T,C>& val3){val1=val2/val3;};
			Transform(out,img1,img2,f);
			return out;
		}
	typedef Image<uint8_t, 4> ImageRGBA;
	typedef Image<int, 4> ImageRGBAi;
	typedef Image<float, 4> ImageRGBAf;

	typedef Image<uint8_t, 3> ImageRGB;
	typedef Image<int, 3> ImageRGBi;
	typedef Image<float, 3> ImageRGBf;

	typedef Image<uint8_t, 1> ImageA;
	typedef Image<int, 1> ImageAi;
	typedef Image<float, 1> ImageAf;

	typedef Image<uint8_t, 4> Image4b;
	typedef Image<uint16_t, 4> Image4us;
	typedef Image<int16_t, 4> Image4s;
	typedef Image<int, 4> Image4i;
	typedef Image<uint32_t, 4> Image4ui;
	typedef Image<float, 4> Image4f;

	typedef Image<uint8_t, 3> Image3b;
	typedef Image<uint16_t, 3> Image3us;
	typedef Image<int16_t, 3> Image3s;
	typedef Image<int, 3> Image3i;
	typedef Image<uint32_t, 3> Image3ui;
	typedef Image<float, 3> Image3f;

	typedef Image<uint8_t, 2> Image2b;
	typedef Image<uint16_t, 2> Image2us;
	typedef Image<int16_t, 2> Image2s;
	typedef Image<int, 2> Image2i;
	typedef Image<uint32_t, 2> Image2ui;
	typedef Image<float, 2> Image2f;

	typedef Image<uint8_t, 1> Image1b;
	typedef Image<uint16_t, 1> Image1us;
	typedef Image<int16_t, 1> Image1s;
	typedef Image<int, 1> Image1i;
	typedef Image<uint32_t, 1> Image1ui;
	typedef Image<float, 1> Image1f;
};


#endif
