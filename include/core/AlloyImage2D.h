#ifndef ALLOYIMAGE2D_H_INCLUDE_GUARD
#define ALLOYIMAGE2D_H_INCLUDE_GUARD
#include "AlloyMath.h"
#include <vector>;
namespace aly{
	template<class T,int M> class Image{
		std::vector<vec<T,M>> data;
		int width;
		int height;
		int x,y;
		uint64_t id;
		static const int channels = M;
		Image(int w,int h):width(w),height(h),x(0),y(0),id(0){
			data.resize(w*h);
		}
		Image():width(0),height(0),x(0),y(0),id(0){
		}
		void resize(int w,int h) {
			data.resize(w*h);
		}
		vec<T,M>* vecPtr(){
			if(data.size()==0)return nullptr;
			return data.data();
		}
		T* ptr(){
			if(data.size()==0)return nullptr;
			return &(data.data[0][0]);
		}
		void setZero(){
			memset(this->data(),0,sizeof(vec<T,M>)*data.size());
		}
		void set(T val){
			for(T& x:data){
				x=vec<T,M>(val);
			}
		}
		void set(vec<T,M> val){
			for(vec<T,M>& x:data){
				x=val;
			}
		}
		void fill(T val){set(val);}
		vec<T, M>& operator[](const int i) {
			return data[i];
		}
		const vec<T, M>& operator[](const int i) const {
			return data[i];
		}
		vec<T, M>& operator()(const int i, const int j) {
			return data[clamp(i, 0, width - 1) + clamp(j, 0, height - 1)*width];
		}
		vec<T, M>& operator()(const int2 ij) {
			return data[clamp(ij.x, 0, width - 1) + clamp(ij.y, 0, height - 1)*width];
		}
		const vec<T, M>& operator()(const int i, const int j) const {
			return data[clamp(i, 0, width - 1) + clamp(j, 0, height - 1)*width];
		}
		const vec<T, M>& operator()(const int2 ij) const {
			return data[clamp(ij.x, 0, width - 1) + clamp(ij.y, 0, height - 1)*width];
		}
	};
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
