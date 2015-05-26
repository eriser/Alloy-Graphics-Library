/*
 * AlloyAnimator.cpp
 *
 *  Created on: May 25, 2015
 *      Author: blake
 */
#include "AlloyAnimator.h"
namespace aly{
std::shared_ptr<Tween>& Animator::add(const std::shared_ptr<Tween>& tween){
	tweens[parity].push_back(tween);
	return tweens[parity].back();
}
void Animator::reset(){
	for(std::shared_ptr<Tween>& tween:tweens[parity]){
		tween->reset();
	}
}
bool Animator::step(double dt){
	std::list<std::shared_ptr<Tween>>& current=tweens[parity];
	std::list<std::shared_ptr<Tween>>& next=tweens[1-parity];
	std::list<std::shared_ptr<Tween>> finished;
	if(current.size()==0)return false;
	for(std::shared_ptr<Tween>& tween:current){
		if(tween->step(dt)){
			next.push_back(tween);
		} else {
			finished.push_back(tween);
		}
	}
	current.clear();
	parity=1-parity;
	for(std::shared_ptr<Tween> tween:finished){
		tween->firePostEvents();
	}
	return true;
}
}


