/*
 * AlloyAnimator.h
 *
 *  Created on: May 24, 2015
 *      Author: blake
 */

#ifndef ALLOYANIMATOR_H_
#define ALLOYANIMATOR_H_

#include "AlloyUnits.h"

namespace aly{
struct Interpolant{

};
struct Tween{
	Tween(Tweenable object,Interpolant tween,double duration){

	}
};
class Animator{
private:
	std::list<std::shared_ptr<Tween>> tasks;
public:
	void add(){

	}
	void step(){

	}
};
}

#endif /* ALLOYANIMATOR_H_ */
