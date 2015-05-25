/*
 * AlloyAnimator.cpp
 *
 *  Created on: May 25, 2015
 *      Author: blake
 */
#include "AlloyAnimator.h"
namespace aly{
void Animator::add(const std::shared_ptr<Tween>& tween){
	tweens[parity].push_back(tween);
}
void Animator::reset(){
	for(std::shared_ptr<Tween>& tween:tweens[parity]){
		tween->reset();
	}
}
bool Animator::step(double dt){
	std::list<std::shared_ptr<Tween>>& current=tweens[parity];
	std::list<std::shared_ptr<Tween>>& next=tweens[1-parity];
	if(current.size()==0)return false;
	for(std::shared_ptr<Tween>& tween:current){
		double t=tween->step(dt);
		if(t<1.0){
			next.push_back(tween);
		}
	}
	current.clear();
	parity=1-parity;
	return true;
}
}


