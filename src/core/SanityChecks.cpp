/*
 * AlloyMath.cpp
 *
 *  Created on: May 15, 2015
 *      Author: blake
 */
#include "AlloyMath.h"
#include "AlloyImage.h"
#include "AlloyVector.h"
#include "AlloyFileUtil.h"
#include "AlloyUI.h"
#include <iostream>
namespace aly {

bool SANITY_CHECK_MATH() {
	try {
		std::cout << "Sanity Check .." << std::endl;
		float4 pt(0.1, 5.3, 2.4, 6.7);
		float4x4 M1(1.0f);
		float4x4 M2( { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 });
		float4x4 M3 = { { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, { 8, 9, 10, 11 }, { 12,
				13, 14, 15 } };
		float4x4 Id = float4x4::identity();
		M1 = ALY_PI * M1;
		pt = M2 * pt;
		M2 *= 1.0f / 16.0f;

		M2 += transpose(M2);

		M2 += float4x4::identity() - float4x4::zero();

		return true;
		float det = determinant(M2);

		M2 = inverse(M2);
		pt = pt.xyz().xy().xyz().xyzw();
		float3 pt2(0.1, 0.4, 0.3);
		float3x4 M3r = SubRowMatrix(M3);
		float4x3 M3c = SubColMatrix(M3);
		float3x3 M3rc = SubMatrix(M3);
		float2x2 M2rc = SubMatrix(M3rc);

		float4x4 Zr = inverse(M2) * M2 - Identity<float, 4, 4>();

		float3 v = pt * M3c * pt2;
		float len = length(v);
		float d = distance(v, float3(0, 2, 1));
		v = floor(v + 0.1f);
		v = ceil(v - 0.2f);
		v = abs(-v);
		float4x4 T = MakeTranslation(float3(1, 2, 3));
		float4x4 S = MakeScale(float3(0.1, 0.2, 0.3));
		float4x4 S2 = MakeScale(0.1f);

		float3 neg = -float3(0.1, 0.2, 0.3);
		float single = float1(0.7);
		float4x4 RX = MakeRotationX(0.3f);
		float4x4 RY = MakeRotationY(0.7f);
		float4x4 RZ = MakeRotationZ(0.5f);
		float4x4 R = MakeRotationMatrix(normalize(float3(1, 1, 0)),
				0.3333f * ALY_PI_2);
		float angle = Angle(float3(0.1, 0.6, 0.2), float3(0, 0, 0),
				SubMatrix(R) * float3(0.1, 0.6, 0.2));
		v = aly::max(v, pt.xyz());
		float3 v2 = mix(v, float3(0, 3, 5), float3(0.1, 0.2, 0.5));
		float3 v3 = v2 * 2.0f - 1.0f;
		v = mix(v2, v3, 0.7f);
		return true;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}

}
bool SANITY_CHECK_IMAGE() {
	try {
		std::cout << "Sanity check image ..." << std::endl;
		Image4f im1(320, 240);
		Image4f im2;
		im2.resize(320, 240);
		im2.setZero();
		std::cout << im1 << std::endl;
		std::cout << im2 << std::endl;
		Image4f out = float4(1, 2, 3, 1) + im2;
		Image4f im3 = -out;
		std::cout << out << std::endl;
		out += im1;
		std::cout << out << std::endl;
		out /= float4(0.3f);
		out += float4(1.0f);
		out = im2 - im1;
		float4 val2 = im1(37.1f, 32.2f) + im2(float2(21.4f, 56.2f));

		float4 val3 = im1(37, 32) + im2(float2(21, 56));
		return true;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
}
bool SANITY_CHECK_LINALG() {
	try {
		std::cout << "Sanity check vector ..." << std::endl;
		Vector4f im1(32);
		Vector4f im2;
		im2.resize(32);
		im2.apply(
				[](size_t offset,float4& val) {val=float4(offset%8,0,-offset,1);});
		im1.apply(
				[](size_t offset,float4& val) {val=float4(offset%8,(offset+7)%5,offset%3,1);});
		std::cout << im1 << std::endl;
		std::cout << im2 << std::endl;
		Vector4f im3 = -im1;
		float d1 = dot(im1, im2) + length(im1) + lengthL1(im2) + max(im1)
				- min(im1);
		float4 d2 = dotVec(im1, im2) + lengthVec(im2) + lengthVecL1(im1)
				+ maxVec(im1) - minVec(im1);
		Vector4f out = float4(1, 2, 3, 1) + im2;
		std::cout << "Dots " << d1 << " " << d2 << std::endl;

		out += im1;
		out /= float4(0.3f);
		out += float4(1.0f);
		out = im2 - im1;

		return true;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
}
bool SANITY_CHECK_UI() {
	CoordPercent rel(0.5f, 0.75f);
	CoordDP abs(40, 30);
	CoordIN in(1, 1);
	CoordPerDP pt(0.5f, 0.75f, 40, 20);
	std::cout << rel << std::endl;
	std::cout << abs << std::endl;
	std::cout << in << std::endl;
	std::cout << pt << std::endl;
	TextLabel* r1 = new TextLabel();
	TextLabel* r2 = new TextLabel("region 2");
	Composite comp;
	comp.add(r1).add(r2);
	return true;
}

#ifndef WIN32
bool SANITY_CHECK_FILE_IO() {
	try {
		std::cout << RemoveTrailingSlash("/usr/local/bin/") << "::"
				<< RemoveTrailingSlash("/usr/local/bin/") << std::endl;
		std::vector<std::string> files = GetDirectoryListing("./images/");

		for (std::string f : files) {
			ImageRGB im1;
			ImageRGBA im2;
			std::cout << f << std::endl;
			GetFileExtension(f);
			GetFileNameWithoutExtension(f);
			GetFileWithoutExtension(f);
			GetFileName(f);
			ConcatPath(GetFileDirectoryPath(f), GetFileName(f));
			ReadImageFromFile(f, im1);
			ReadImageFromFile(f, im2);
			WriteImageToFile("/home/blake/" + GetFileName(f) + "_copy_rgb.png",
					im1);
			WriteImageToFile("/home/blake/" + GetFileName(f) + "_copy_rgba.png",
					im2);
		}
		return true;
	} catch (std::exception& e) {
		return false;
	}
}
#endif

}

