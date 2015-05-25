/*
 * AlloyAnimator.cpp
 *
 *  Created on: May 25, 2015
 *      Author: blake
 */
#include "AlloyAnimator.h"
namespace aly{
void Animator::add(const std::shared_ptr<Tween>& tween){
	tweens.push_back(tween);
}
void Animator::reset(){
	for(std::shared_ptr<Tween>& tween:tweens){
		tween->reset();
	}
}
bool Animator::step(double dt){
	if(tweens.size()==0)return false;
	std::list<std::shared_ptr<Tween>> next;
	for(std::shared_ptr<Tween>& tween:tweens){
		double t=tween->step(dt);
		if(t<1.0){
			next.push_back(tween);
		}
	}
	tweens=next;
	return true;
}
}


