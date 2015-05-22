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


#ifndef ALLOYMATH_H_INCLUDE_GUARD
#define ALLOYMATH_H_INCLUDE_GUARD

#include <stdint.h>
#undef USE_MATH_DEFINES
#include <cmath>
#include <tuple>
#include <iomanip>
#include <iostream>
#include "AlloyCommon.h"
//#include "cereal/cereal.hpp"
#define ALY_PI float(3.1415926535897932384626433832795)
#define ALY_PI_2 float(0.5f*ALY_PI)
#define ALY_PI_4 float(0.25f*ALY_PI)
namespace aly
{
bool SANITY_CHECK_MATH();
    /////////////////////
    // Data structures //
    /////////////////////
template<typename T> T clamp(T val, T min, T max) {
	return std::min(std::max(val, min), max);
}

// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

template<class T, int M> struct vec;
template<class T> struct vec<T,1>
{
    T                           x;

                                vec(T x, T y)                       : x(x) {}
                                vec(T* ptr)							: x(*ptr){}
    explicit                    vec(T s=(T)0)                       : x(s) {}
    template<class U> explicit  vec(const vec<U,2> & r)             : x(T(r.x)) {}
    const T &                   operator [] (int i) const           { return &x; }
    T &                         operator [] (int i)                 { return &x; }
    operator    				T () const 							{ return x; }

   	bool        				operator == (const vec & r) const           { return (x == r.x); }
   	bool        				operator != (const vec & r) const           { return (x!=r.x); }
   	bool        				operator < (const vec & r) const            { return (x < r.x); }
   	bool        				operator > (const vec & r) const            { return (x > r.x); }
	template<class Archive>		void serialize(Archive & archive){			archive(x); }
};
    // A vector with exactly M elements, each of which is an instance of type T. Can also be thought of as an M x 1 matrix.
    template<class T, int M> struct vec;
    template<class T> struct vec<T,2>
    {
        T                           x, y;

                                    vec(T x, T y)                       : x(x), y(y) {}
                                    vec(T* ptr)							: x(ptr[0]),y(ptr[1]){}
        explicit                    vec(T s=(T)0)                       : x(s), y(s) {}
        template<class U> explicit  vec(const vec<U,2> & r)             : x(T(r.x)), y(T(r.y)) {}

        const T &                   operator [] (int i) const           { return (&x)[i]; }
        T &                         operator [] (int i)                 { return (&x)[i]; }
        vec<T,3>					xyz() const;

        bool        operator == (const vec & r) const           { return (x==r.x && y==r.y); }
        bool        operator != (const vec & r) const           { return (x!=r.x || y!=r.y); }
 		bool        operator < (const vec & r) const            { return (std::make_tuple(x, y) < std::make_tuple(r.x, r.y)); }
 		bool        operator > (const vec & r) const            { return (std::make_tuple(x, y) < std::make_tuple(r.x, r.y)); }
 		template<class Archive>		void serialize(Archive & archive){			archive(x, y); }

    };
    template<class T> struct vec<T,3>
    {
        T                           x, y, z;

                                    vec(T x, T y, T z)                  : x(x), y(y), z(z) {}
                                    vec(T* ptr)							: x(ptr[0]),y(ptr[1]),z(ptr[2]){}
                                    vec(vec<T,2> xy, T z)               : x(xy.x), y(xy.y), z(z) {}
        explicit                   	vec(T s=(T)0)                  		: x(s), y(s), z(s) {}
        template<class U> explicit  vec(const vec<U,3> & r)             : x(T(r.x)), y(T(r.y)), z(T(r.z)) {}
        const T &                   operator [] (int i) const           { return (&x)[i]; }
        T &                         operator [] (int i)                 { return (&x)[i]; }
        vec<T,2>					xy() const							{ return vec<T,2>(x,y);}
        vec<T,4>					xyzw()const;

        bool        operator == (const vec & r) const           { return (x==r.x && y==r.y && z==r.z); }
        bool        operator != (const vec & r) const           { return (x!=r.x || y!=r.y || z!=r.z); }
 		bool        operator < (const vec & r) const            { return (std::make_tuple(x, y, z) < std::make_tuple(r.x, r.y, r.z)); }
 		bool        operator > (const vec & r) const            { return (std::make_tuple(x, y, z) < std::make_tuple(r.x, r.y, r.z)); }
 		template<class Archive>		void serialize(Archive & archive){			archive(x, y, z); }

    };
    template<class T> struct vec<T,4>
    {
        T                           x, y, z, w;

                                    vec(T x, T y, T z, T w)             : x(x), y(y), z(z), w(w) {}
                                    vec(T* ptr)							: x(ptr[0]),y(ptr[1]),z(ptr[2]),w(ptr[3]){}
                                    vec(vec<T,3> xyz, T w)              : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
        explicit                    vec(T s=(T)0)                       : x(s), y(s), z(s), w(s) {}
        template<class U> explicit  vec(const vec<U,4> & r)             : x(T(r.x)), y(T(r.y)), z(T(r.z)), w(T(r.w)) {}

        const T &                   operator [] (int i) const           { return (&x)[i]; }
        T &                         operator [] (int i)                 { return (&x)[i]; }
        vec<T,3>					xyz() const							{return vec<T,3>(x,y,z);}

        bool        operator == (const vec & r) const           { return (x==r.x && y==r.y && z==r.z && w==r.w); }
        bool        operator != (const vec & r) const           { return (x!=r.x || y!=r.y || z!=r.z || w!=r.w); }
        bool        operator < (const vec & r) const            { return (std::make_tuple(x, y, z, w) < std::make_tuple(r.x, r.y, r.z, r.w)); }
 		bool        operator > (const vec & r) const            { return (std::make_tuple(x, y, z, w) < std::make_tuple(r.x, r.y, r.z, r.w)); }
 		template<class Archive>		void serialize(Archive & archive){			archive(x, y, z, w); }

    };
    template <class T> vec<T,3> vec<T,2>::xyz() const {
    	return vec<T,3>(x,y,1);
    }
    template <class T> vec<T,4> vec<T,3>::xyzw() const {
      return vec<T,4>(x,y,z,1);
    }


    // A matrix with exactly M rows and N columns, where each element is an instance of type T
    template<class T, int M, int N> struct matrix;


    template<class T, int M, int N> const matrix<T,M,N> Identity() {
    		static matrix<T,M,N> Id;
    		static bool once=true;
    		if(once){
				for(int m=0;m<M;m++){
					for(int n=0;n<N;n++){
						Id(m,n)=(m==n)?1:0;
					}
				}
				once=false;
    		}
        	return Id;
    };
    template<class T, int M, int N> const matrix<T,M,N> Zero() {
    		static matrix<T,M,N> Zero;
    		static bool once=true;
    		if(once){
				for(int m=0;m<M;m++){
					for(int n=0;n<N;n++){
						Zero(m,n)=0;
					}
				}
				once=false;
    		}
        	return Zero;
    };
    template<class T, int M, int N> const matrix<T,M-1,N-1> SubMatrix(matrix<T,M,N> A) {
    		static matrix<T,M-1,N-1> B;
			for(int m=0;m<M-1;m++){
				for(int n=0;n<N-1;n++){
					B(m,n)=A(m,n);
				}
			}
			return B;
    };
    template<class T, int M, int N> const matrix<T,M-1,N> SubRowMatrix(matrix<T,M,N> A) {
    		static matrix<T,M-1,N> B;
			for(int m=0;m<M-1;m++){
				for(int n=0;n<N;n++){
					B(m,n)=A(m,n);
				}
			}
			return B;
    };
    template<class T, int M, int N> const matrix<T,M,N-1> SubColMatrix(matrix<T,M,N> A) {
    		static matrix<T,M,N-1> B;
			for(int m=0;m<M;m++){
				for(int n=0;n<N-1;n++){
					B(m,n)=A(m,n);
				}
			}
			return B;
    };
    template<class T, int M> struct matrix<T,M,2>
    {
        typedef                     vec<T,M> C;
        C                           x, y;
        							matrix(const T (&A)[M*2])	{
        								for(int m=0;m<M;m++){
        									x[m]=A[m*2];
                                            y[m]=A[m*2+1];
                                        }
                                    }
                                    matrix()                               {}
                                    matrix(C x, C y)                       : x(x), y(y) {}
        explicit                    matrix(T s)                            : x(s), y(s) {}
        template<class U> explicit  matrix(const matrix<U,M,2> & r)           : x(C(r.x)), y(C(r.y)) {}
        T*							ptr()								{ return &(x.x);}
        const T*					ptr() const							{ return &(x.x);}
        vec<T,M>*					vecPtr()							{ return &x[0];}
        vec<T,2>                    row(int i) const                    { return {x[i], y[i]}; } 
        const C &                   operator [] (int j) const           { return (&x)[j]; }
        const T &                   operator () (int i, int j) const    { return (*this)[j][i]; }
        C &                         operator [] (int j)                 { return (&x)[j]; }
        T &                         operator () (int i, int j)          { return (*this)[j][i]; }
        static						matrix<T,M,2> zero(){return Zero<T,M,2>();}
        static						matrix<T,M,2> identity(){return Identity<T,M,2>();}
    };

    // Specialization for an M x 3 matrix, with exactly three columns, each of which is an M-element vector
    template<class T, int M> struct matrix<T,M,3>
    {
        typedef                     vec<T,M> C;
        C                           x, y, z;

                                    matrix()                               {}
                                    matrix(const T (&A)[M*3])	{
                                      for(int m=0;m<M;m++){
                                    	  x[m]=A[m*3];
                                          y[m]=A[m*3+1];
                                          z[m]=A[m*3+2];
                                      }
                                    }
                                    matrix(C x, C y, C z)                  : x(x), y(y), z(z) {}
        explicit                    matrix(T s)                            : x(s), y(s), z(s) {}
        template<class U> explicit  matrix(const matrix<U,M,3> & r)           : x(C(r.x)), y(C(r.y)), z(C(r.z)) {}
        T*							ptr()								{ return &(x.x);}
        const T*					ptr() const							{ return &(x.x);}
        vec<T,M>*					vecPtr()							{ return &x[0];}
        vec<T,3>                    row(int i) const                    { return {x[i], y[i], z[i]}; }
        const C &                   operator [] (int j) const           { return (&x)[j]; }
        const T &                   operator () (int i, int j) const    { return (*this)[j][i]; }
        C &                         operator [] (int j)                 { return (&x)[j]; }
        T &                         operator () (int i, int j)          { return (*this)[j][i]; }
        static						matrix<T,M,3> zero(){return Zero<T,M,3>();}
        static						matrix<T,M,3> identity(){return Identity<T,M,3>();}
    };

    // Specialization for an M x 4 matrix, with exactly four columns, each of which is an M-element vector
    template<class T, int M> struct matrix<T,M,4>
    {
        typedef                     vec<T,M> C;
        C                           x, y, z, w;

                                    matrix()                               {}
                                    matrix(const T (&A)[M*4])				   {
                                    	for(int m=0;m<M;m++){
                                    		x[m]=A[m*4];
                                    		y[m]=A[m*4+1];
                                    		z[m]=A[m*4+2];
                                    		w[m]=A[m*4+3];
                                    	}
                                    }
                                    matrix(C x, C y, C z, C w)             : x(x), y(y), z(z), w(w) {}
        explicit                    matrix(T s)                            : x(s), y(s), z(s), w(s) {}
        template<class U> explicit  matrix(const matrix<U,M,4> & r)           : x(C(r.x)), y(C(r.y)), z(C(r.z)), w(C(r.w)) {}
        T*							ptr()								{ return &(x.x);}
        const T*					ptr() const							{ return &(x.x);}
        vec<T,M>*					vecPtr()							{ return &x[0];}
        vec<T,4>                    row(int i) const                    { return {x[i], y[i], z[i], w[i]}; }
        const C &                   operator [] (int j) const           { return (&x)[j]; }
        const T &                   operator () (int i, int j) const    { return (*this)[j][i]; }
        C &                         operator [] (int j)                 { return (&x)[j]; }
        T &                         operator () (int i, int j)          { return (*this)[j][i]; }
        static						matrix<T,M,4> zero(){return Zero<T,M,4>();}
        static						matrix<T,M,4> identity(){return Identity<T,M,4>();}
    };



/////////////
    // Functional helpers //
    ////////////////////////

    // elem_t<T> is a type trait for the type of the underlying elements of a tensor (scalar, vector, or matrix)
    template<class T> struct elem_type { typedef T type; };
    template<class T, int M> struct elem_type<vec<T,M  >> { typedef T type; };
    template<class T, int M, int N> struct elem_type<matrix<T,M,N>> { typedef T type; };
    template<class T> using elem_t = typename elem_type<T>::type;

    // Form a scalar by applying function f to adjacent components of vector or matrix v
    template<class T, class F> T reduce(const vec<T,2> & v, F f) { return f(v.x, v.y); }
    template<class T, class F> T reduce(const vec<T,3> & v, F f) { return f(f(v.x, v.y), v.z); }
    template<class T, class F> T reduce(const vec<T,4> & v, F f) { return f(f(f(v.x, v.y), v.z), v.w); }
    template<class T, int M, class F> T reduce(const matrix<T,M,2> & m, F f) { return f(reduce(m.x, f), reduce(m.y, f)); }
    template<class T, int M, class F> T reduce(const matrix<T,M,3> & m, F f) { return f(f(reduce(m.x, f), reduce(m.y, f)), reduce(m.z, f)); }
    template<class T, int M, class F> T reduce(const matrix<T,M,4> & m, F f) { return f(f(f(reduce(m.x, f), reduce(m.y, f)), reduce(m.z, f)), reduce(m.w, f)); }

    /////////////////////////////////
    // General aggregate functions //
    /////////////////////////////////

    // Return true if any/all elements of vector or matrix v would evaluate to true in a boolean context
    template<class T> bool any(const T & v) { return reduce(v, [](bool a, bool b) { return a || b; }); }
    template<class T> bool all(const T & v) { return reduce(v, [](bool a, bool b) { return a && b; }); }

    // Compute the sum of all elements in vector or matrix v
    template<class T> elem_t<T> sum(const T & v) { return reduce(v, [](elem_t<T> a, elem_t<T> b) { return a + b; }); }
    template<class T> elem_t<T> product(const T & v) { return reduce(v, [](elem_t<T> a, elem_t<T> b) { return a * b; }); }

    // Form a vector or matrix by applying std::abs/ceil/floor/round to each component of vector or matrix v
    template<class T,int M> vec<T,M> abs(const vec<T,M> & v) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::abs(v[m]);return result; }
    template<class T,int M> vec<T,M> ceil(const vec<T,M> & v) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::ceil(v[m]);return result; }
    template<class T,int M> vec<T,M> floor(const vec<T,M> & v) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::floor(v[m]);return result;}
    template<class T,int M> vec<T,M> round(const vec<T,M> & v) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::floor(v[m]+0.5);return result;}
    template<class T,int M> vec<T,M> exp(const vec<T,M> & v) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::exp(v[m]);return result;}
    template<class T,int M> vec<T,M> log(const vec<T,M> & v) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::log(v[m]);return result;}
    template<class T,int M> vec<T,M> sqrt(const vec<T,M> & v) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::sqrt(v[m]);return result;}
    template<class T,int M> vec<T,M> clamp(const vec<T,M> & v,const T& mn,const T& mx) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=clamp(v[m],mn,mx);return result; }

    // Form a vector or matrix by taking the componentwise max/min of two vectors or matrices
    template<class T,int M> vec<T,M> max(const vec<T,M> & l, const vec<T,M> & r) { vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::max(l[m],r[m]);return result; }
    template<class T,int M> vec<T,M> min(const vec<T,M> & l, const vec<T,M> & r) {vec<T,M> result;for(int m=0;m<M;m++)result[m]=std::min(l[m],r[m]);return result; }

    template<class T,int M> vec<T,M> mix(const vec<T,M> & a, const vec<T,M> & b,const T& t) {return (a*(1-t) + b*t); }
    template<class T,int M> vec<T,M> mix(const vec<T,M> & a, const vec<T,M> & b,const vec<T,M>& t) {return (a*(vec<T,M>(1)-t) + b*t); }

    //////////////////////////////
    // Vector algebra functions //
    //////////////////////////////

    // Compute the dot/cross product of two vectors
    template<class T, int N> T dot(const vec<T,N> & l, const vec<T,N> & r) { return sum(l*r); }
    template<class T> vec<T,3> cross(const vec<T,3> & l, const vec<T,3> & r) { return {l.y*r.z-l.z*r.y, l.z*r.x-l.x*r.z, l.x*r.y-l.y*r.x}; }

    // Compute the length/square length of a vector
    template<class T, int N> T lengthSqr(const vec<T,N> & v) { return dot(v,v); }
    template<class T, int N> T length(const vec<T,N> & v) { return std::sqrt(lengthSqr(v)); }

    // Compute the distance/square distance between two points, expressed as vectors
    template<class T, int N> T distance(const vec<T,N> & l, const vec<T,N> & r) { return length(r-l); }
    template<class T, int N> T distanceSqr(const vec<T,N> & l, const vec<T,N> & r) { return length_sq(r-l); }

    // Compute a normalized vector with the same direction as the original vector
    template<class T, int N> vec<T,N> normalize(const vec<T,N> & v,const double eps=1E-6) { return v/std::max(length(v),static_cast<T>(eps)); }

    //////////////////////////////
    // Matrix algebra functions //
    //////////////////////////////

    // Compute the adjugate of a square matrix (equivalent to the transpose of the cofactor matrix)
    template<class T> matrix<T,2,2> adjugate(const matrix<T,2,2> & a) { return {{a.y.y, -a.x.y}, {-a.y.x, a.x.x}}; }
    template<class T> matrix<T,3,3> adjugate(const matrix<T,3,3> & a) { return {{a.y.y*a.z.z - a.z.y*a.y.z, a.z.y*a.x.z - a.x.y*a.z.z, a.x.y*a.y.z - a.y.y*a.x.z},
                                                                          {a.y.z*a.z.x - a.z.z*a.y.x, a.z.z*a.x.x - a.x.z*a.z.x, a.x.z*a.y.x - a.y.z*a.x.x},
                                                                          {a.y.x*a.z.y - a.z.x*a.y.y, a.z.x*a.x.y - a.x.x*a.z.y, a.x.x*a.y.y - a.y.x*a.x.y}}; }
    template<class T> matrix<T,4,4> adjugate(const matrix<T,4,4> & a) { return {{a.y.y*a.z.z*a.w.w + a.w.y*a.y.z*a.z.w + a.z.y*a.w.z*a.y.w - a.y.y*a.w.z*a.z.w - a.z.y*a.y.z*a.w.w - a.w.y*a.z.z*a.y.w,
                                                                           a.x.y*a.w.z*a.z.w + a.z.y*a.x.z*a.w.w + a.w.y*a.z.z*a.x.w - a.w.y*a.x.z*a.z.w - a.z.y*a.w.z*a.x.w - a.x.y*a.z.z*a.w.w,
                                                                           a.x.y*a.y.z*a.w.w + a.w.y*a.x.z*a.y.w + a.y.y*a.w.z*a.x.w - a.x.y*a.w.z*a.y.w - a.y.y*a.x.z*a.w.w - a.w.y*a.y.z*a.x.w,
                                                                           a.x.y*a.z.z*a.y.w + a.y.y*a.x.z*a.z.w + a.z.y*a.y.z*a.x.w - a.x.y*a.y.z*a.z.w - a.z.y*a.x.z*a.y.w - a.y.y*a.z.z*a.x.w},
                                                                          {a.y.z*a.w.w*a.z.x + a.z.z*a.y.w*a.w.x + a.w.z*a.z.w*a.y.x - a.y.z*a.z.w*a.w.x - a.w.z*a.y.w*a.z.x - a.z.z*a.w.w*a.y.x,
                                                                           a.x.z*a.z.w*a.w.x + a.w.z*a.x.w*a.z.x + a.z.z*a.w.w*a.x.x - a.x.z*a.w.w*a.z.x - a.z.z*a.x.w*a.w.x - a.w.z*a.z.w*a.x.x,
                                                                           a.x.z*a.w.w*a.y.x + a.y.z*a.x.w*a.w.x + a.w.z*a.y.w*a.x.x - a.x.z*a.y.w*a.w.x - a.w.z*a.x.w*a.y.x - a.y.z*a.w.w*a.x.x,
                                                                           a.x.z*a.y.w*a.z.x + a.z.z*a.x.w*a.y.x + a.y.z*a.z.w*a.x.x - a.x.z*a.z.w*a.y.x - a.y.z*a.x.w*a.z.x - a.z.z*a.y.w*a.x.x},
                                                                          {a.y.w*a.z.x*a.w.y + a.w.w*a.y.x*a.z.y + a.z.w*a.w.x*a.y.y - a.y.w*a.w.x*a.z.y - a.z.w*a.y.x*a.w.y - a.w.w*a.z.x*a.y.y,
                                                                           a.x.w*a.w.x*a.z.y + a.z.w*a.x.x*a.w.y + a.w.w*a.z.x*a.x.y - a.x.w*a.z.x*a.w.y - a.w.w*a.x.x*a.z.y - a.z.w*a.w.x*a.x.y,
                                                                           a.x.w*a.y.x*a.w.y + a.w.w*a.x.x*a.y.y + a.y.w*a.w.x*a.x.y - a.x.w*a.w.x*a.y.y - a.y.w*a.x.x*a.w.y - a.w.w*a.y.x*a.x.y,
                                                                           a.x.w*a.z.x*a.y.y + a.y.w*a.x.x*a.z.y + a.z.w*a.y.x*a.x.y - a.x.w*a.y.x*a.z.y - a.z.w*a.x.x*a.y.y - a.y.w*a.z.x*a.x.y},
                                                                          {a.y.x*a.w.y*a.z.z + a.z.x*a.y.y*a.w.z + a.w.x*a.z.y*a.y.z - a.y.x*a.z.y*a.w.z - a.w.x*a.y.y*a.z.z - a.z.x*a.w.y*a.y.z,
                                                                           a.x.x*a.z.y*a.w.z + a.w.x*a.x.y*a.z.z + a.z.x*a.w.y*a.x.z - a.x.x*a.w.y*a.z.z - a.z.x*a.x.y*a.w.z - a.w.x*a.z.y*a.x.z,
                                                                           a.x.x*a.w.y*a.y.z + a.y.x*a.x.y*a.w.z + a.w.x*a.y.y*a.x.z - a.x.x*a.y.y*a.w.z - a.w.x*a.x.y*a.y.z - a.y.x*a.w.y*a.x.z,
                                                                           a.x.x*a.y.y*a.z.z + a.z.x*a.x.y*a.y.z + a.y.x*a.z.y*a.x.z - a.x.x*a.z.y*a.y.z - a.y.x*a.x.y*a.z.z - a.z.x*a.y.y*a.x.z}}; }

    // Compute the determinant of a square matrix
    template<class T> T determinant(const matrix<T,2,2> & a) { return a.x.x*a.y.y - a.x.y*a.y.x; }
    template<class T> T determinant(const matrix<T,3,3> & a) { return a.x.x*(a.y.y*a.z.z - a.z.y*a.y.z) + a.x.y*(a.y.z*a.z.x - a.z.z*a.y.x) + a.x.z*(a.y.x*a.z.y - a.z.x*a.y.y); }
    template<class T> T determinant(const matrix<T,4,4> & a) { return a.x.x*(a.y.y*a.z.z*a.w.w + a.w.y*a.y.z*a.z.w + a.z.y*a.w.z*a.y.w - a.y.y*a.w.z*a.z.w - a.z.y*a.y.z*a.w.w - a.w.y*a.z.z*a.y.w)
                                                                 + a.x.y*(a.y.z*a.w.w*a.z.x + a.z.z*a.y.w*a.w.x + a.w.z*a.z.w*a.y.x - a.y.z*a.z.w*a.w.x - a.w.z*a.y.w*a.z.x - a.z.z*a.w.w*a.y.x)
                                                                 + a.x.z*(a.y.w*a.z.x*a.w.y + a.w.w*a.y.x*a.z.y + a.z.w*a.w.x*a.y.y - a.y.w*a.w.x*a.z.y - a.z.w*a.y.x*a.w.y - a.w.w*a.z.x*a.y.y)
                                                                 + a.x.w*(a.y.x*a.w.y*a.z.z + a.z.x*a.y.y*a.w.z + a.w.x*a.z.y*a.y.z - a.y.x*a.z.y*a.w.z - a.w.x*a.y.y*a.z.z - a.z.x*a.w.y*a.y.z); }

    // Compute the inverse of a square matrix
    template<class T, int N> matrix<T,N,N> inverse(const matrix<T,N,N> & a) { return adjugate(a)/determinant(a); }

    // Compute the product of a matrix post-multiplied by a column vector
    template<class T, int M> vec<T,M> mul(const matrix<T,M,2> & a, const vec<T,2> & b) { return a.x*b.x + a.y*b.y; }
    template<class T, int M> vec<T,M> mul(const matrix<T,M,3> & a, const vec<T,3> & b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
    template<class T, int M> vec<T,M> mul(const matrix<T,M,4> & a, const vec<T,4> & b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }

    // Compute the product of two matrices
    template<class T, int M, int N> matrix<T,M,2> mul(const matrix<T,M,N> & a, const matrix<T,N,2> & b) { return {mul(a,b.x), mul(a,b.y)}; }
    template<class T, int M, int N> matrix<T,M,3> mul(const matrix<T,M,N> & a, const matrix<T,N,3> & b) { return {mul(a,b.x), mul(a,b.y), mul(a,b.z)}; }
    template<class T, int M, int N> matrix<T,M,4> mul(const matrix<T,M,N> & a, const matrix<T,N,4> & b) { return {mul(a,b.x), mul(a,b.y), mul(a,b.z), mul(a,b.w)}; }

    // Compute the tranpose of a matrix
    template<class T, int M> matrix<T,M,2> transpose(const matrix<T,2,M> & m) { return {m.row(0), m.row(1)}; }
    template<class T, int M> matrix<T,M,3> transpose(const matrix<T,3,M> & m) { return {m.row(0), m.row(1), m.row(2)}; }
    template<class T, int M> matrix<T,M,4> transpose(const matrix<T,4,M> & m) { return {m.row(0), m.row(1), m.row(2), m.row(3)}; }

    //////////////////////////////////
    // Quaternion algebra functions //
    //////////////////////////////////

    // Compute the conjugate/inverse/produce of a quaternions represented by 4D vectors
    template<class T> vec<T,4> qconjugate(const vec<T,4> & q) { return {-q.x,-q.y,-q.z,q.w}; }
    template<class T> vec<T,4> qinverse(const vec<T,4> & q) { return qconjugate(q)/length_sq(q); }
    template<class T> vec<T,4> qmul(const vec<T,4> & a, const vec<T,4> & b) { return {a.x*b.w+a.w*b.x+a.y*b.z-a.z*b.y, a.y*b.w+a.w*b.y+a.z*b.x-a.x*b.z, a.z*b.w+a.w*b.z+a.x*b.y-a.y*b.x, a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z}; }

    // Compute spatial rotations of 3D vectors via the quaternion product qvq*
    template<class T> vec<T,3> qrotate_x(const vec<T,4> & q) { return {q.w*q.w+q.x*q.x-q.y*q.y-q.z*q.z, (q.x*q.y+q.z*q.w)*2, (q.z*q.x-q.y*q.w)*2}; } // q [1,0,0,0] q*
    template<class T> vec<T,3> qrotate_y(const vec<T,4> & q) { return {(q.x*q.y-q.z*q.w)*2, q.w*q.w-q.x*q.x+q.y*q.y-q.z*q.z, (q.y*q.z+q.x*q.w)*2}; } // q [0,1,0,0] q*
    template<class T> vec<T,3> qrotate_z(const vec<T,4> & q) { return {(q.z*q.x+q.y*q.w)*2, (q.y*q.z-q.x*q.w)*2, q.w*q.w-q.x*q.x-q.y*q.y+q.z*q.z}; } // q [0,0,1,0] q*
    template<class T> vec<T,3> qrotate(const vec<T,4> & q, const vec<T,3> & v) { return qrotate_x(q)*v.x + qrotate_y(q)*v.y + qrotate_z(q)*v.z; } // q [x,y,z,0] q*

	template<class T, int M> vec<T, M>   operator - (const vec<T, M> & a)                         { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=-a[m];};return result; }
	template<class T, int M> vec<T, M>   operator + (const vec<T, M> & a, const vec<T, M> & b)    { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a[m]+b[m];};return result;}
	template<class T, int M> vec<T, M>   operator - (const vec<T, M> & a, const vec<T, M> & b)    { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a[m]-b[m];};return result;}
	template<class T, int M> vec<T, M>   operator * (const vec<T, M> & a, const vec<T, M> & b)    { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a[m]*b[m];};return result;}
	template<class T, int M> vec<T, M>   operator / (const vec<T, M> & a, const vec<T, M> & b)    { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a[m]/b[m];};return result; }

	template<class T, int M> vec<T, M>   operator + (const vec<T, M> & a, const T & b)            { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a[m]+b;};return result; }
	template<class T, int M> vec<T, M>   operator - (const vec<T, M> & a, const T & b)            { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a[m]-b;};return result; }
	template<class T, int M> vec<T, M>   operator * (const vec<T, M> & a, const T & b)            { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a[m]*b;};return result; }
	template<class T, int M> vec<T, M>   operator / (const vec<T, M> & a, const T & b)            { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a[m]/b;};return result; }

	template<class T, int M> vec<T, M>   operator + (const T & a,const vec<T, M> & b)            { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a+b[m];};return result; }
	template<class T, int M> vec<T, M>   operator - (const T & a,const vec<T, M> & b)            { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a-b[m];};return result; }
	template<class T, int M> vec<T, M>   operator * (const T & a,const vec<T, M> & b)            { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a*b[m];};return result; }
	template<class T, int M> vec<T, M>   operator / (const T & a,const vec<T, M> & b)            { vec<T,M> result;for(int m=0;m<M;m++){ result[m]=a/b[m];};return result; }

	template<class T, int M> vec<T, M> & operator += (vec<T, M> & a, const vec<T, M> & b)         { return a = a + b; }
	template<class T, int M> vec<T, M> & operator -= (vec<T, M> & a, const vec<T, M> & b)         { return a = a - b; }
	template<class T, int M> vec<T, M> & operator *= (vec<T, M> & a, const vec<T, M> & b)         { return a = a * b; }
	template<class T, int M> vec<T, M> & operator /= (vec<T, M> & a, const vec<T, M> & b)         { return a = a / b; }
	template<class T, int M> vec<T, M> & operator += (vec<T, M> & a, const T & b)                 { return a = a + b; }
	template<class T, int M> vec<T, M> & operator -= (vec<T, M> & a, const T & b)                 { return a = a - b; }
	template<class T, int M> vec<T, M> & operator *= (vec<T, M> & a, const T & b)                 { return a = a * b; }
	template<class T, int M> vec<T, M> & operator /= (vec<T, M> & a, const T & b)                 { return a = a / b; }


	template<class T, int M, int N> matrix<T,M,N> operator - (const matrix<T,M,N> & a)                       {  matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=-a[i];};return result;  }
	template<class T, int M, int N> matrix<T,M,N> operator + (const matrix<T,M,N> & a, const matrix<T,M,N> & b) { matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a[i]+b[i];};return result;  }
	template<class T, int M, int N> matrix<T,M,N> operator - (const matrix<T,M,N> & a, const matrix<T,M,N> & b) { matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a[i]-b[i];};return result;}
	template<class T, int M, int N> matrix<T,M,N> operator + (const matrix<T,M,N> & a, T b)                  	{ matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a[i]+b;};return result; }
	template<class T, int M, int N> matrix<T,M,N> operator - (const matrix<T,M,N> & a, T b)                  	{ matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a[i]-b;};return result; }
	template<class T, int M, int N> matrix<T,M,N> operator * (const matrix<T,M,N> & a, T b)                  	{ matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a[i]*b;};return result; }
	template<class T, int M, int N> matrix<T,M,N> operator / (const matrix<T,M,N> & a, T b)                  	{ matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a[i]/b;};return result;}
	template<class T, int M, int N> matrix<T,M,N> operator * (T a,const matrix<T,M,N> & b)                  	{ matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a*b[i];};return result; }
	template<class T, int M, int N> matrix<T,M,N> operator / (T a,const matrix<T,M,N> & b)                  	{ matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a/b[i];};return result;}
	template<class T, int M, int N> matrix<T,M,N> operator + (T a,const matrix<T,M,N> & b)                  	{ matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a+b[i];};return result; }
	template<class T, int M, int N> matrix<T,M,N> operator - (T a,const matrix<T,M,N> & b)                  	{ matrix<T,M,N> result;for(int i=0;i<N;i++){ result[i]=a-b[i];};return result;}

	template<class T, int M, int N> matrix<T,M,N> & operator += (matrix<T,M,N> & a, const matrix<T,M,N> & b)    { return a=a+b; }
	template<class T, int M, int N> matrix<T,M,N> & operator -= (matrix<T,M,N> & a, const matrix<T,M,N> & b)    { return a=a-b; }
	template<class T, int M, int N> matrix<T,M,N> & operator *= (matrix<T,M,N> & a, T b)                     { return a=a*b; }
	template<class T, int M, int N> matrix<T,M,N> & operator /= (matrix<T,M,N> & a, T b)                     { return a=a/b; }

	//////////////////////////////////////
	// Matrix multiplication operations //
	//////////////////////////////////////

	template<class T, int M, int N>        vec<T,M>     operator * (const matrix<T,M,N> & a, const vec<T,N> & b)   { return mul(a,b); } // Interpret b as column vector (N x 1 matrix)
	template<class T, int M, int N>        vec<T,N>     operator * (const vec<T,M> & a, const matrix<T,M,N> & b)   { return mul(transpose(b),a); } // Interpret a as row vector (1 x M matrix)
	template<class T, int M, int N, int P> matrix<T,M,P>   operator * (const matrix<T,M,N> & a, const matrix<T,N,P> & b) { return mul(a,b); }
	template<class T, int M>               matrix<T,M,M> & operator *= (matrix<T,M,M> & a, const matrix<T,M,M> & b)      { return a=a*b; }


    template<class C, class R, class T> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const vec<T,1> & v) { return ss << v.x; }
    template<class C, class R, class T> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const vec<T,2> & v) { return ss << '(' << v.x << ',' << v.y << ')'; }
    template<class C, class R, class T> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const vec<T,3> & v) { return ss << '(' << v.x << ',' << v.y << ',' << v.z << ')'; }
    template<class C, class R, class T> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const vec<T,4> & v) { return ss << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')'; }
    template<class C, class R, class T,int M, int N> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const matrix<T,M,N> & A) {
    	ss<<"\n";
    	for(int n=0;n<N;n++){
    		ss<<"[";
    		for(int m=0;m<M;m++){
    			ss<<std::setprecision(10)<<std::setw(16)<<A(n,m)<<((m<M-1)?",":"]\n");
    		}
    	}
    	return ss;
    }
    template <class T> matrix<T,4,4> MakeRotationMatrix(const vec<T,3>& axis, T angle){
    	matrix<T,4,4> M=Identity<T,4,4>();
    	T mag = length(axis);
    	if (mag >= 1E-6f) {
    		mag = 1.0 / mag;
    		T ax = axis[0] * mag;
    		T ay = axis[1] * mag;
    		T az = axis[2] * mag;
    		T sinTheta = (T) sin(angle);
    		T cosTheta = (T) cos(angle);
    		T t = 1.0 - cosTheta;

    		T xz = ax * az;
    		T xy = ax * ay;
    		T yz = ay * az;

    		M(0,0) = t * ax * ax + cosTheta;
    		M(0,1) = t * xy - sinTheta * az;
    		M(0,2) = t * xz + sinTheta * ay;

    		M(1,0) = t * xy + sinTheta * az;
    		M(1,1) = t * ay * ay + cosTheta;
    		M(1,2) = t * yz - sinTheta * ax;

    		M(2,0) = t * xz - sinTheta * ay;
    		M(2,1) = t * yz + sinTheta * ax;
    		M(2,2) = t * az * az + cosTheta;
    	}
    	return M;
    }
    template <class T> matrix<T,4,4> MakeRotationX(T angle){
    	return MakeRotationMatrix(vec<T,3>(1,0,0),angle);
    }
    template <class T> matrix<T,4,4> MakeRotationY(T angle){
    	return MakeRotationMatrix(vec<T,3>(0,1,0),angle);
    }
    template <class T> matrix<T,4,4> MakeRotationZ(T angle){
    	return MakeRotationMatrix(vec<T,3>(0,0,1),angle);
    }
    template <class T> matrix<T,4,4> MakeTranslation(const vec<T,3>& translation){
    	return matrix<T,4,4>(vec<T,4>((T)0),vec<T,4>((T)0),vec<T,4>((T)0),translation.xyzw());
    }
    template <class T> matrix<T,4,4> MakeScale(const vec<T,3>& scale){
    	return matrix<T,4,4>(
    			vec<T,4>(scale.x,0,0,0),
    			vec<T,4>(0,scale.y,0,0),
    			vec<T,4>(0,0,scale.z,0),
    			vec<T,4>(0,0,0,1));
    }
    template <class T> matrix<T,4,4> MakeScale(T scale){
    	return matrix<T,4,4>(
    			vec<T,4>(scale,0,0,0),
    			vec<T,4>(0,scale,0,0),
    			vec<T,4>(0,0,scale,0),
    			vec<T,4>(0,0,0,1));
    }
    template <class T> T Angle(const vec<T,3>& v0, const vec<T,3>& v1, const vec<T,3>& v2){
    	vec<T,3> v = v0 - v1;
    	vec<T,3> w = v2 - v1;
    	float len1 = length(v);
    	float len2 = length(w);
    	return std::acos(dot(v,w) / std::max(1E-8f, len1 * len2));
    }
	template<class K,class T, int C> void ConvertType(vec<T, C> in,vec<K, C>& out){
		for (int c = 0; c < C; c++){
			out[c] = static_cast<K>(in[c]);
		}
	}
	template<class K,class T, int C> vec<K, C> ConvertType(vec<T, C> in){
		vec<K, C> out;
		ConvertType(in,out);
		return out;
	}
    // Geometric Tools LLC, Redmond WA 98052
    // Copyright (c) 1998-2015
    // Distributed under the Boost Software License, Version 1.0.
    // http://www.boost.org/LICENSE_1_0.txt
    // http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
    // File Version: 1.0.0 (2014/08/11)
	//----------------------------------------------------------------------------
	template <class T> matrix<T,2,2> inverse(matrix<T,2,2> const& M)
	{
		matrix<T,2,2> result;
	    T det = M(0, 0)*M(1, 1) - M(0, 1)*M(1, 0);
	    if (det != (T)0)
	    {
	        T invDet = ((T)1) / det;
	        result = matrix<T,2,2>({
	            	 M(1, 1)*invDet,-M(0, 1)*invDet,
	                -M(1, 0)*invDet, M(0, 0)*invDet
	        });
	    }
	    else
	    {
	        result=Zero<T,2,2>();
	        throw std::runtime_error("Could not invert matrix.");
	    }

	    return result;
	}
	template <typename T>
	matrix<T,3,3> inverse(matrix<T,3,3> const& M)
	{
	    matrix<T,3,3> result;
	    T c00 = M(1, 1)*M(2, 2) - M(1, 2)*M(2, 1);
	    T c10 = M(1, 2)*M(2, 0) - M(1, 0)*M(2, 2);
	    T c20 = M(1, 0)*M(2, 1) - M(1, 1)*M(2, 0);
	    T det = determinant(M);
	    if (det != (T)0)
	    {
	        T invDet = ((T)1) / det;
	        result = matrix<T,3,3>
	        ({
	            c00*invDet,
	                (M(0, 2)*M(2, 1) - M(0, 1)*M(2, 2))*invDet,
	                (M(0, 1)*M(1, 2) - M(0, 2)*M(1, 1))*invDet,
	                c10*invDet,
	                (M(0, 0)*M(2, 2) - M(0, 2)*M(2, 0))*invDet,
	                (M(0, 2)*M(1, 0) - M(0, 0)*M(1, 2))*invDet,
	                c20*invDet,
	                (M(0, 1)*M(2, 0) - M(0, 0)*M(2, 1))*invDet,
	                (M(0, 0)*M(1, 1) - M(0, 1)*M(1, 0))*invDet
	        });
	    }
	    else
	    {
	        result=Zero<T,3,3>();
	        throw std::runtime_error("Could not invert matrix.");
	    }
	    return result;
	}

	//----------------------------------------------------------------------------
	template <class T> matrix<T,4,4> inverse(matrix<T,4,4> const& M) {
		matrix<T,4,4> result;
	    T a0 = M(0, 0)*M(1, 1) - M(0, 1)*M(1, 0);
	    T a1 = M(0, 0)*M(1, 2) - M(0, 2)*M(1, 0);
	    T a2 = M(0, 0)*M(1, 3) - M(0, 3)*M(1, 0);
	    T a3 = M(0, 1)*M(1, 2) - M(0, 2)*M(1, 1);
	    T a4 = M(0, 1)*M(1, 3) - M(0, 3)*M(1, 1);
	    T a5 = M(0, 2)*M(1, 3) - M(0, 3)*M(1, 2);
	    T b0 = M(2, 0)*M(3, 1) - M(2, 1)*M(3, 0);
	    T b1 = M(2, 0)*M(3, 2) - M(2, 2)*M(3, 0);
	    T b2 = M(2, 0)*M(3, 3) - M(2, 3)*M(3, 0);
	    T b3 = M(2, 1)*M(3, 2) - M(2, 2)*M(3, 1);
	    T b4 = M(2, 1)*M(3, 3) - M(2, 3)*M(3, 1);
	    T b5 = M(2, 2)*M(3, 3) - M(2, 3)*M(3, 2);
	    T det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
	    if (det != (T)0)
	    {
	        T invDet = ((T)1) / det;
	        result = matrix<T,4,4>({
	        		(+M(1, 1)*b5 - M(1, 2)*b4 + M(1, 3)*b3)*invDet,
	                (-M(0, 1)*b5 + M(0, 2)*b4 - M(0, 3)*b3)*invDet,
	                (+M(3, 1)*a5 - M(3, 2)*a4 + M(3, 3)*a3)*invDet,
	                (-M(2, 1)*a5 + M(2, 2)*a4 - M(2, 3)*a3)*invDet,
	                (-M(1, 0)*b5 + M(1, 2)*b2 - M(1, 3)*b1)*invDet,
	                (+M(0, 0)*b5 - M(0, 2)*b2 + M(0, 3)*b1)*invDet,
	                (-M(3, 0)*a5 + M(3, 2)*a2 - M(3, 3)*a1)*invDet,
	                (+M(2, 0)*a5 - M(2, 2)*a2 + M(2, 3)*a1)*invDet,
	                (+M(1, 0)*b4 - M(1, 1)*b2 + M(1, 3)*b0)*invDet,
	                (-M(0, 0)*b4 + M(0, 1)*b2 - M(0, 3)*b0)*invDet,
	                (+M(3, 0)*a4 - M(3, 1)*a2 + M(3, 3)*a0)*invDet,
	                (-M(2, 0)*a4 + M(2, 1)*a2 - M(2, 3)*a0)*invDet,
	                (-M(1, 0)*b3 + M(1, 1)*b1 - M(1, 2)*b0)*invDet,
	                (+M(0, 0)*b3 - M(0, 1)*b1 + M(0, 2)*b0)*invDet,
	                (-M(3, 0)*a3 + M(3, 1)*a1 - M(3, 2)*a0)*invDet,
	                (+M(2, 0)*a3 - M(2, 1)*a1 + M(2, 2)*a0)*invDet
	        });
	    }
	    else
	    {
	        result=Zero<T,4,4>();
	        throw std::runtime_error("Could not invert matrix.");
	    }
	    return result;
	}
    template<class T,int M> struct box{
    	vec<T,M> position;
    	vec<T,M> dimensions;
    	box():position((T)0),dimensions((T)0){
    	}

       	box(vec<T,M> pt,vec<T,M> dims):position(pt),dimensions(dims){
        }
    	bool contains(const vec<T,M>& qt){
    		for(int m=0;m<M;m++)if(qt[m]<position[m]||qt[m]>=position[m]+dimensions[m])return false;
    		return true;
    	}
    };
    template<class C, class R,class T,int M> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const box<T,M> & v) {
    	return ss << "{min: "<<v.position<<", max: "<<v.position+v.dimensions<<", dimensions: "<<v.dimensions<<"}"; }
    /////////////////////////
    // Convenience aliases //
    /////////////////////////

    	typedef vec<uint8_t,1> ubyte1;typedef vec<int8_t,1> byte1; typedef vec<uint16_t,1> ushort1;typedef vec<int16_t,1> short1;typedef vec<int32_t,1> int1; typedef vec<uint32_t,1> uint1; typedef vec<bool,1> bool1; typedef vec<float,1> float1; typedef vec<double,1> double1;
        typedef vec<uint8_t,2> ubyte2;typedef vec<int8_t,2> byte2; typedef vec<uint16_t,2> ushort2;typedef vec<int16_t,2> short2;typedef vec<int32_t,2> int2; typedef vec<uint32_t,2> uint2; typedef vec<bool,2> bool2; typedef vec<float,2> float2; typedef vec<double,2> double2;
        typedef vec<uint8_t,3> ubyte3;typedef vec<int8_t,3> byte3; typedef vec<uint16_t,3> ushort3;typedef vec<int16_t,3> short3;typedef vec<int32_t,3> int3; typedef vec<uint32_t,3> uint3; typedef vec<bool,3> bool3; typedef vec<float,3> float3; typedef vec<double,3> double3;
        typedef vec<uint8_t,4> ubyte4;typedef vec<int8_t,4> byte4; typedef vec<uint16_t,4> ushort4;typedef vec<int16_t,4> short4;typedef vec<int32_t,4> int4; typedef vec<uint32_t,4> uint4; typedef vec<bool,4> bool4; typedef vec<float,4> float4; typedef vec<double,4> double4;
        typedef matrix<float,2,2> float2x2; typedef matrix<float,2,3> float2x3; typedef matrix<float,2,4> float2x4;
        typedef matrix<float,3,2> float3x2; typedef matrix<float,3,3> float3x3; typedef matrix<float,3,4> float3x4;
        typedef matrix<float,4,2> float4x2; typedef matrix<float,4,3> float4x3; typedef matrix<float,4,4> float4x4;
        typedef matrix<double,2,2> double2x2; typedef matrix<double,2,3> double2x3; typedef matrix<double,2,4> double2x4;
        typedef matrix<double,3,2> double3x2; typedef matrix<double,3,3> double3x3; typedef matrix<double,3,4> double3x4;
        typedef matrix<double,4,2> double4x2; typedef matrix<double,4,3> double4x3; typedef matrix<double,4,4> double4x4;
        typedef float4 RGBAf;
        typedef float3 RGBf;
        typedef int4 RGBAi;
        typedef int3 RGBi;
        typedef ubyte4 RGBA;
        typedef ubyte3 RGB;

        typedef box<float,1> box1f;
        typedef box<float,2> box2f;
        typedef box<float,3> box3f;
        typedef box<float,4> box4f;

        typedef box<int,1> box1i;
        typedef box<int,2> box2i;
        typedef box<int,3> box3i;
        typedef box<int,4> box4i;
};

#endif
