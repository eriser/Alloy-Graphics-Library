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
	std::list<std::shared_ptr<Tween>>& current=tweens[parity];
	for(std::shared_ptr<Tween>& tween:current){
		tween->object->reset();
	}
}
bool Animator::step(double dt){
	std::list<std::shared_ptr<Tween>>& current=tweens[parity];
	std::list<std::shared_ptr<Tween>>& next=tweens[1-parity];
	next.clear();
	for(std::shared_ptr<Tween>& tween:current){
		(*tween->object)+=dt;
		if(tween->object->getTweenValue()<1.0){
			next.push_back(tween);
		}
	}
	current.clear();
	parity=1-parity;
	return (next.size()>0);
}
}


