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
};
struct Linear : public Interpolant
{
	inline double operator()(double t) const {
	    return t;
    }
};

struct Smoothstep : public Interpolant
{
    inline double operator()(double t) const {
	    auto scale = t * t * (3.f - 2.f * t);
	    return scale * 1.0f;
    }
};

struct SineIn : public Interpolant
{
    inline double operator()(double t) const {
	    return 1.0f - std::cos(t * ALY_PI / 2.f);
    }
};

struct SineOut : public Interpolant
{
    inline double operator()(double t) const {
	    return (t < 0.5f) ? (1.0f - std::cos(t * ALY_PI)) * 0.5f : 1.0f - 1.0f - std::cos((1.0f-t) * ALY_PI) * 0.5f;
    }
};

struct ExponentialIn: public Interpolant
{
    inline double operator()(double t) const {
	    return powf(2.0f, 10.f * (t - 1.0f));
    }
 };
struct ExponentialOut: public Interpolant
{
	inline double operator()(double t) const {
 	    return (t < 0.5f) ? std::pow(2.0f, 10.f * (2.0f*t - 1.0f)) * 0.5f : 1.0f - std::pow(2.0f, 10.f * (1.0f-2.0f*t)) * 0.5f;
    }
};
struct QuadraticIn: public Interpolant
{
    inline double operator()(double t) const {
	    return t * t;
    }
};
struct QuadraticOut: public Interpolant
{
    inline double operator()(double t) const {
        return (t < 0.5f) ? (t*t * 2.0f) * 0.5f : 1.0f - (2.0f - t*t * 2.0f) * 0.5f;
    }
};
struct CubicPulse : public Interpolant
{
    inline double operator()(double t) const {
        double c = 1.0, w = 0.9f;
	    double x = std::abs(t - c);
	    if (x > w) return 0.0f;
	    x /= w;
	    return 1.0f - x*x*(3.0f - 2.0f*x);
    }
};

struct Tween{
	std::shared_ptr<Tweenable> object;
	Interpolant interp;
	double duration;
	Tween(const std::shared_ptr<Tweenable>& object,const Interpolant& interp,double duration):object(object),interp(interp),duration(duration){
	}
};
class Animator{
private:
	std::list<std::shared_ptr<Tween>> tweens[2];
	int parity=0;
public:
	void add(const std::shared_ptr<Tween>& tween);
	void reset();
	bool step(double dt);
};
}

#endif /* ALLOYANIMATOR_H_ */
