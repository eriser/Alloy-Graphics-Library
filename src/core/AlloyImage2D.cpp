/*
 * AlloyImage2D.cpp
 *
 *  Created on: May 16, 2015
 *      Author: blake
 */

#include "../include/core/AlloyImage2D.h"
#include <iostream>
namespace aly{
	bool SANITY_CHECK_IMAGE(){
		try{
			std::cout<<"Sanity check image ..."<<std::endl;
			Image4f im1(320,240);
			Image4f im2;
			im2.resize(320,240);

			std::cout<<im1<<std::endl;
			std::cout<<im2<<std::endl;
			Image4f out=float4(1,2,3,1)+im2;

			std::cout<<out<<std::endl;
			out+=im1;
			std::cout<<out<<std::endl;
			out/=float4(0.3f);
			out+=float4(1.0f);
			out=im2-im1;
			return true;
		} catch(std::exception& e){
			std::cout<<e.what()<<std::endl;
			return false;
		}
	}
}


