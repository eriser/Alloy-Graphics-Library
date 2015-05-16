/*
 * AlloyMath.cpp
 *
 *  Created on: May 15, 2015
 *      Author: blake
 */
#include "../include/core/AlloyMath.h"
#include <iostream>
namespace aly{

	bool SANITY_CHECK_MATH(){
		try {
			std::cout<<"Sanity Check .."<<std::endl;
			float4 pt(0.1,5.3,2.4,6.7);
			float4x4 M1(1.0f);
			float4x4 M2({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15});
			float4x4 M3={{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}};
			float4x4 Id=float4x4::identity();
			M1=ALY_PI*M1;
			pt=M2*pt;
			M2*=1.0f/16.0f;

			M2+=transpose(M2);

			M2+=float4x4::identity()-float4x4::zero();

			return true;
			float det=determinant(M2);

			M2=inverse(M2);
			pt=pt.xyz().xy().xyz().xyzw();
			float3 pt2(0.1,0.4,0.3);
			float3x4 M3r=SubRowMatrix(M3);
			float4x3 M3c=SubColMatrix(M3);
			float3x3 M3rc=SubMatrix(M3);
			float2x2 M2rc=SubMatrix(M3rc);

			float4x4 Zr=inverse(M2)*M2-Identity<float,4,4>();

			float3 v=pt*M3c*pt2;
			float len=length(v);
			float d=distance(v,float3(0,2,1));
			v=floor(v+0.1f);
			v=ceil(v-0.2f);
			v=abs(-v);
			float4x4 T=MakeTranslation(float3(1,2,3));
			float4x4 S=MakeScale(float3(0.1,0.2,0.3));
			float4x4 S2=MakeScale(0.1f);

			float3 neg=-float3(0.1,0.2,0.3);
			float single=float1(0.7);
			float4x4 RX=MakeRotationX(0.3f);
			float4x4 RY=MakeRotationY(0.7f);
			float4x4 RZ=MakeRotationZ(0.5f);
			float4x4 R=MakeRotationMatrix(normalize(float3(1,1,0)),0.3333f*ALY_PI_2);
			float angle=Angle(float3(0.1,0.6,0.2),float3(0,0,0),SubMatrix(R)*float3(0.1,0.6,0.2));
			v=aly::max(v,pt.xyz());
			float3 v2=mix(v,float3(0,3,5),float3(0.1,0.2,0.5));
			float3 v3=v2*2.0f-1.0f;
			v=mix(v2,v3,0.7f);
			return true;
		} catch(std::exception& e){
			std::cout<<e.what()<<std::endl;
			return false;
		}

	}
}



