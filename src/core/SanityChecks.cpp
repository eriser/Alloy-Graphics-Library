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
#include "AlloyMesh.h"
#include "cereal/archives/xml.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/binary.hpp"
#include <iostream>
#include <fstream>
namespace aly {

bool SANITY_CHECK_MATH() {
	try {
		std::cout << "Sanity Check .." << std::endl;
		float4 pt(0.1f, 5.3f, 2.4f, 6.7f);
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
		float3 pt2(0.1f, 0.4f, 0.3f);
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
		float4x4 S = MakeScale(float3(0.1f, 0.2f, 0.3f));
		float4x4 S2 = MakeScale(0.1f);

		float3 neg = -float3(0.1f, 0.2f, 0.3f);
		float single = float1(0.7f);
		float4x4 RX = MakeRotationX(0.3f);
		float4x4 RY = MakeRotationY(0.7f);
		float4x4 RZ = MakeRotationZ(0.5f);
		float4x4 R = MakeRotationMatrix(normalize(float3(1, 1, 0)),
				0.3333f * ALY_PI_2);
		float angle = Angle(float3(0.1f, 0.6f, 0.2f), float3(0, 0, 0),
				SubMatrix(R) * float3(0.1f, 0.6f, 0.2f));
		v = aly::max(v, pt.xyz());
		float3 v2 = mix(v, float3(0, 3, 5), float3(0.1f, 0.2f, 0.5));
		float3 v3 = v2 * 2.0f - 1.0f;
		v = mix(v2, v3, 0.7f);
		return true;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}

}
bool SANITY_CHECK_CEREAL() {

	float4 v1(1, 2, 3, 4);
	float3 v2(1, 2, 3);
	float2 v3(1, 2);
	float1 v4(1);

	float4x4 MR1 = MakeRotationMatrix(normalize(float3(0.1f, 0.5f, 0.3f)),
			ALY_PI * 0.333f) * MakeTranslation(float4(89, 43, 21, 1));
	float3x3 MR2 = SubMatrix(
			MakeRotationMatrix(normalize(float3(0.1f, 0.5f, 0.3f)),
					ALY_PI * 0.1f) * MakeTranslation(float4(89, 43, 21, 1)));
	float4x3 MR3 = SubColMatrix(
			MakeRotationMatrix(normalize(float3(0.1f, 0.5f, 0.3f)),
					ALY_PI * 0.1f) * MakeTranslation(float4(89, 43, 21, 1)));

	Image4f im1(32, 24);
	std::srand(10832145);
	for (int i = 0; i < im1.width; i++) {
		for (int j = 0; j < im1.height; j++) {
			im1(i, j) = float4((float) i, (float) j,
					(std::rand() % 256) / (256.0f), 1.0f);
		}
	}
	Vector4f data(16);
	for (int i = 0; i < data.size(); i++) {
		data[i] = float4((float) i, (float) ((i + 32) % 4),
				(std::rand() % 256) / (256.0f), 1.0f);
	}

	Vector4f deserial(16);
	std::string encodedString = EncodeBase64(data.data);
	DecodeBase64(encodedString, deserial.data);
	for (int i = 0; i < data.size(); i++) {
		std::cout << i << ") " << data[i] << " " << deserial[i] << std::endl;
	}

	std::cout << im1.updateHashCode(HashMethod::SHA1) << std::endl;
	std::cout << im1.updateHashCode(HashMethod::SHA224) << std::endl;
	std::cout << im1.updateHashCode(HashMethod::SHA256) << std::endl;
	std::cout << im1.updateHashCode(HashMethod::SHA384) << std::endl;
	std::cout << im1.updateHashCode(HashMethod::SHA512) << std::endl;

	Integer value1(4);
	Double value2(3.14159);
	Float value3(3.14f);
	Color c(0.5f, 0.4f, 0.1f, 1.0f);
	Number num = Float(1.222f);
	AUnit1D unit1 = UnitPerPT(0.5f, 3.0f);
	AUnit2D unit2 = CoordPerPX(0.5f, 0.3f, 3.0f, 10.0f);
	AUnit1D unit3 = UnitDP(0.2f);
	AUnit2D unit4 = CoordMM(100, 200);

	{
		std::ofstream os("nums.xml");
		cereal::XMLOutputArchive archiver(os);
		archiver(value1, value2, value3, num, c, unit1, unit2, unit3, unit4);
	}
	{
		std::ofstream os("data.xml");
		cereal::XMLOutputArchive archiver(os);
		archiver(v1, v2, v3, v4, MR1, MR2, MR3, im1, data);
	}
	{
		std::ofstream os("data.json");
		cereal::JSONOutputArchive archiver(os);
		archiver(v1, v2, v3, v4, MR1, MR2, MR3, im1, data);
	}
	{
		std::ofstream os("data.bin");
		cereal::BinaryOutputArchive archiver(os);
		archiver(v1, v2, v3, v4, MR1, MR2, MR3, im1, data);
	}
	{

		Mesh mesh;
		ReadMeshFromFile("assets/models/monkey.ply", mesh);
		std::ofstream os("monkey.json");
		cereal::JSONOutputArchive archiver(os);
		archiver(cereal::make_nvp("monkey", mesh));
	}
	//std::cout << "Hit any key ..." << std::endl;
	//std::cout << "Data Size " << im1.data.size() << std::endl;
	//getchar();
	return true;
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
				[](size_t offset,float4& val) {val=float4((float)(offset%8),0.0f,-(float)offset,1);});
		im1.apply(
				[](size_t offset,float4& val) {val=float4((float)(offset%8), (float)((offset+7)%5),(float)(offset%3),1.0f);});
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
	comp.add(r1);
	comp.add(r2);
	return true;
}

#ifndef WIN32
bool SANITY_CHECK_FILE_IO() {
	try {
		std::cout << RemoveTrailingSlash("/usr/local/bin/") << "::"
				<< RemoveTrailingSlash("/usr/local/bin/") << std::endl;
		std::vector<std::string> files = GetDirectoryFileListing("./images/");

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

