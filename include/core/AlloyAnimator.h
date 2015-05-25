/*
 * AlloyAnimator.h
 *
 *  Created on: May 24, 2015
 *      Author: blake
 */

#ifndef ALLOYANIMATOR_H_
#define ALLOYANIMATOR_H_
#include "AlloyUnits.h"
#include <memory>
#include <list>
namespace aly{
struct Interpolant{
	virtual double operator()(double t) const =0;
	virtual inline ~Interpolant(){};
};
struct Linear : public Interpolant
{
	virtual inline double operator()(double t) const override {
	    return t;
    }
};

struct Smoothstep : public Interpolant
{
	virtual inline double operator()(double t) const override {
	    auto scale = t * t * (3.f - 2.f * t);
	    return scale * 1.0f;
    }
};

struct SineIn : public Interpolant
{
	virtual inline double operator()(double t) const override {
	    return 1.0f - std::cos(t * ALY_PI / 2.f);
    }
};

struct SineOut : public Interpolant
{
	virtual inline double operator()(double t) const override {
	    return (t < 0.5f) ? (1.0f - std::cos(t * ALY_PI)) * 0.5f : 1.0f - 1.0f - std::cos((1.0f-t) * ALY_PI) * 0.5f;
    }
};

struct ExponentialIn: public Interpolant
{
	virtual inline double operator()(double t) const override {
	    return powf(2.0f, 10.f * (t - 1.0f));
    }
 };
struct ExponentialOut: public Interpolant
{
	virtual inline double operator()(double t) const override {
 	    return (t < 0.5f) ? std::pow(2.0f, 10.f * (2.0f*t - 1.0f)) * 0.5f : 1.0f - std::pow(2.0f, 10.f * (1.0f-2.0f*t)) * 0.5f;
    }
};
struct QuadraticIn: public Interpolant
{
	virtual inline double operator()(double t) const override {
	    return t * t;
    }
};
struct QuadraticOut: public Interpolant
{
	virtual inline double operator()(double t) const override {
        return (t < 0.5f) ? (t*t * 2.0f) * 0.5f : 1.0f - (2.0f - t*t * 2.0f) * 0.5f;
    }
};
struct CubicPulse : public Interpolant
{
	virtual inline double operator()(double t) const override {
        double c = 1.0, w = 0.9f;
	    double x = std::abs(t - c);
	    if (x > w) return 0.0f;
	    x /= w;
	    return 1.0f - x*x*(3.0f - 2.0f*x);
    }
};
class Tween{
private:
	std::unique_ptr<Interpolant> interpolant;
public:
	double time=0;
	Tweenable* object;
	double duration;
	template<class A> Tween(Tweenable* object,double duration,const A& a):object(object),duration(duration){
		interpolant=std::unique_ptr<Interpolant>(new A(a));
	}
	void reset(){
		object->reset();
		time=0;
	}
	double step(double dt){
		time+=dt;
		object->setTweenValue((*interpolant)(time/duration));
		return object->getTweenValue();
	}
};
class Animator{
private:
	std::list<std::shared_ptr<Tween>> tweens;
	int parity=0;
public:
	void add(const std::shared_ptr<Tween>& tween);
	void reset();
	bool step(double dt);
	template<class A> void add(AColor& out,const Color& start,const Color& end,double duration,const A& a=Linear()){
		ColorTween* ctween=new ColorTween(start,end);
		out=std::shared_ptr<ColorTween>(ctween);
		add(std::shared_ptr<Tween>(new Tween(ctween,duration,a)));
	}
	template<class A> void add(AUnit2D& out,const AUnit2D& start,const AUnit2D& end,double duration,const A& a=Linear()){
		CoordTween* ctween=new CoordTween(start,end);
		out=AUnit2D(ctween);
		add(std::shared_ptr<Tween>(new Tween(ctween,duration,a)));
	}
	template<class A> void add(AUnit1D& out,const AUnit1D& start,const AUnit1D& end,double duration,const A& a=Linear()){
		UnitTween* utween=new UnitTween(start,end);
		out=AUnit1D(utween);
		add(std::shared_ptr<Tween>(new Tween(utween,duration,a)));
	}
};
}

#endif /* ALLOYANIMATOR_H_ */
