/*
 * AlloyMath.cpp
 *
 *  Created on: May 15, 2015
 *      Author: blake
 */
#include <AlloySparseSolve.h>
#include "AlloyMath.h"
#include "AlloyImage.h"
#include "AlloyVector.h"
#include "AlloyFileUtil.h"
#include "AlloyUI.h"
#include "AlloyMesh.h"
#include "AlloySparseMatrix.h"
#include "AlloyVirtualCamera.h"
#include "AlloyMeshIntersector.h"
#include "cereal/archives/xml.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/archives/binary.hpp"
#include <iostream>
#include <fstream>
#include <random>
#ifndef ALY_WINDOWS
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
namespace aly {
bool SANITY_CHECK_ALGO() {
	SparseMatrix<float, 4> A(128, 128);
	Vector4f b(A.rows);
	Vector4f x(A.cols);
	for (int i = 0; i < (int) A.rows; i++) {
		for (int jj = -2; jj <= 2; jj++) {
			A.set(i, i + jj, float4(0.2f));
		}
		b[i] = float4((rand() % 1000) / 1000.0f);
	}
	SolveCG(b, A, x);
	std::ofstream os("matrix.json");
	cereal::JSONOutputArchive archiver(os);
	archiver(A);

	return true;
}
bool SANITY_CHECK_KDTREE() {
	Mesh mesh;
	mesh.load(AlloyDefaultContext()->getFullPath("models/monkey.ply"));
	KDTree kdTree(mesh, 6);
	VirtualCamera camera;
	camera.setNearFarPlanes(0.1f, 2.0f);
	camera.setZoom(0.75f);
	mesh.updateBoundingBox();
	box3f renderBBox = box3f(float3(-0.5f, -0.5f, -0.5f),
			float3(1.0f, 1.0f, 1.0f));
	camera.setPose(MakeTransform(mesh.getBoundingBox(), renderBBox));
	camera.aim(box2px(float2(0, 0), float2(320, 240)));
	Image4f rgba(320, 240);
#pragma omp parallel for
	for (int i = 0; i < rgba.width; i++) {
		for (int j = 0; j < rgba.height; j++) {
			float3 pt1 = camera.transformImageToWorld(float3(i, j, 0.0f),
					rgba.width, rgba.height);
			float3 pt2 = camera.transformImageToWorld(float3(i, j, 1.0f),
					rgba.width, rgba.height);
			float3 v = normalize(pt2 - pt1);
			float3 lastPoint(0.0f);
			double d = kdTree.intersectRayDistance(pt1, v, lastPoint);
			if (d != NO_HIT_DISTANCE) {
				rgba(i, j) = float4(lastPoint, d);
			} else {
				rgba(i, j) = float4(0, 0, 0, 0);
			}
		}
	}
	rgba.writeToXML("depth.xml");
	box3f bbox = mesh.getBoundingBox();
#pragma omp parallel for
	for (int i = 0; i < rgba.width; i++) {
		for (int j = 0; j < rgba.height; j++) {
			float3 pt1 = bbox.position
					+ bbox.dimensions
							* float3(i / (float) rgba.width, 0.5f,
									j / (float) rgba.height);
			float3 lastPoint(0.0f);
			double d = kdTree.closestPoint(pt1, lastPoint);
			if (d != NO_HIT_DISTANCE) {
				rgba(i, j) = float4(lastPoint, d);
			} else {
				rgba(i, j) = float4(0, 0, 0, 0);
			}
		}
	}
	rgba.writeToXML("closest.xml");

#pragma omp parallel for
	for (int i = 0; i < rgba.width; i++) {
		for (int j = 0; j < rgba.height; j++) {
			float3 pt1 = bbox.position
					+ bbox.dimensions
							* float3(i / (float) rgba.width, 0.5f,
									j / (float) rgba.height);
			float3 lastPoint(0.0f);
			double d = kdTree.closestPoint(pt1, 0.1f, lastPoint);
			if (d != NO_HIT_DISTANCE) {
				rgba(i, j) = float4(lastPoint, d);
			} else {
				rgba(i, j) = float4(0, 0, 0, 0);
			}
		}
	}
	rgba.writeToXML("closest_clamped.xml");
	return true;
}
bool SANITY_CHECK_MESH_IO() {
	Mesh tmpMesh;
	tmpMesh.load(AlloyDefaultContext()->getFullPath("models/torus.ply"));
	tmpMesh.updateVertexNormals();
	tmpMesh.textureMap.resize(
			tmpMesh.quadIndexes.size() * 4 + tmpMesh.triIndexes.size() * 3);
	for (int i = 0; i < (int) tmpMesh.textureMap.size(); i++) {
		tmpMesh.textureMap[i] = float2((rand() % 1024) / 1024.0f,
				(rand() % 1024) / 1024.0f);
	}
	WriteMeshToFile("torus2.ply", tmpMesh, true);
	ReadMeshFromFile("torus2.ply", tmpMesh);
	WriteMeshToFile("torus3.ply", tmpMesh, false);
	ReadMeshFromFile("torus3.ply", tmpMesh);

	tmpMesh.load(AlloyDefaultContext()->getFullPath("models/icosahedron.ply"));
	tmpMesh.updateVertexNormals();
	tmpMesh.textureMap.resize(
			tmpMesh.quadIndexes.size() * 4 + tmpMesh.triIndexes.size() * 3);
	for (int i = 0; i < (int) tmpMesh.textureMap.size(); i++) {
		tmpMesh.textureMap[i] = float2((rand() % 1024) / 1024.0f,
				(rand() % 1024) / 1024.0f);
	}

	WriteMeshToFile("icosahedron2.ply", tmpMesh, true);
	ReadMeshFromFile("icosahedron2.ply", tmpMesh);
	tmpMesh.textureImage.resize(640, 480);
	tmpMesh.textureImage.set(RGBAf(1.0f, 0.0, 0.0, 1.0f));
	WriteMeshToFile("icosahedron3.ply", tmpMesh, false);
	ReadMeshFromFile("icosahedron3.ply", tmpMesh);
	return true;
}
bool SANITY_CHECK_SPARSE_SOLVE() {
	SparseMatrix1f A(4, 3);
	SparseMatrix1f B(3, 4);
	float4x3 Ad;
	float3x4 Bd;
	A(0, 0) = float1(1.2f);
	A(3, 2) = float1(3.5f);
	A(2, 2) = float1(1);
	A(1, 1) = float1(3);
	A(0, 2) = float1(4);

	B(0, 0) = float1(1.0f);
	B(2, 1) = float1(2.5f);
	B(2, 2) = float1(7);
	B(1, 1) = float1(0.1);
	B(2, 0) = float1(2);
	for (int i = 0; i < (int)A.rows; i++) {
		for (int j = 0; j < (int)A.cols; j++) {
			Ad(i, j) = A(i, j);
		}
	}
	for (int i = 0; i < (int)B.rows; i++) {
		for (int j = 0; j < (int)B.cols; j++) {
			Bd(i, j) = B(i, j);
		}
	}
	std::cout << A << std::endl;
	std::cout << B << std::endl;
	SparseMatrix1f C;
	C=C.transpose();
	{
		auto Cd = Ad * Bd;
		C = A * B;
		std::cout << "Cd=\n" << Cd << std::endl;
		for (int i = 0; i < (int)C.rows; i++) {
			for (int j = 0; j < (int)C.cols; j++) {
				Cd(i, j) = C(i, j);
			}
		}
		std::cout << "C=\n"<< Cd << std::endl;
	}
	
	{
		C = B * A;
		auto Cd = Bd * Ad;
		std::cout << "Cd=\n" << Cd << std::endl;
		for (int i = 0; i < (int) C.rows; i++) {
			for (int j = 0; j < (int) C.cols; j++) {
				Cd(i, j) = C(i, j);
			}
		}
		std::cout << "C=\n" << Cd << std::endl;
		std::cout << Cd << std::endl;
	}
	return true;
	MeshNeighborTable vertTable;
	Mesh mesh;
	mesh.load(AlloyDefaultContext()->getFullPath("models/monkey.ply"));
	CreateOrderedVertexNeighborTable(mesh, vertTable);
	srand(1023172413L);
	mesh.updateVertexNormals();
	SparseMatrix<float, 1> L(mesh.vertexLocations.size(),
			mesh.vertexLocations.size());
	Vector3f b(L.rows);
	srand(213823);
	for (size_t i = 0; i < L.rows; i++) {

		const float w = 0.9f;
		for (uint32_t v : vertTable[i]) {
			L.set(i, v, float1(-w));
		}
		L.set(i, i, float1(1.0f + (float) w * vertTable[i].size()));
		float3 norm = mesh.vertexNormals[i];
		mesh.vertexLocations[i] += 5.0f * norm
				* (((rand() % 1024) / 1024.0f) - 0.5f);
		b[i] = mesh.vertexLocations[i];
	}
	//WriteMeshToFile("smoothed_before.ply", mesh);
	/*
	 SolveCG(b, L, mesh.vertexLocations, 100, 1E-6f,
	 [this](int iter,double err) {
	 std::cout<<"Iteration "<<iter<<":: "<<err<<std::endl;
	 });
	 */
	SolveBICGStab(b, L, mesh.vertexLocations, 500, 1E-6f,
			[=](int iter,double err) {
				std::cout<<"Iteration "<<iter<<":: "<<err<<std::endl;
			});
	return true;
}
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
	Image4f im2;
	im2 = im1;
	std::srand(10832145);
	for (int i = 0; i < im1.width; i++) {
		for (int j = 0; j < im1.height; j++) {
			im1(i, j) = float4((float) i, (float) j,
					(std::rand() % 256) / (256.0f), 1.0f);
		}
	}
	Vector4f data(16);
	for (int i = 0; i < (int) data.size(); i++) {
		data[i] = float4((float) i, (float) ((i + 32) % 4),
				(std::rand() % 256) / (256.0f), 1.0f);
	}

	Vector4f deserial(16);
	std::string encodedString = EncodeBase64(data.data);
	DecodeBase64(encodedString, deserial.data);
	for (int i = 0; i < (int) data.size(); i++) {
		std::cout << i << ") " << data[i] << " " << deserial[i] << std::endl;
	}

	std::cout << im1.updateHashCode(0, HashMethod::SHA1) << std::endl;
	std::cout << im1.updateHashCode(0, HashMethod::SHA224) << std::endl;
	std::cout << im1.updateHashCode(0, HashMethod::SHA256) << std::endl;
	std::cout << im1.updateHashCode(0, HashMethod::SHA384) << std::endl;
	std::cout << im1.updateHashCode(0, HashMethod::SHA512) << std::endl;

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
bool SANITY_CHECK_SVD() {

	int N = 100;
	std::uniform_real_distribution<float> r(-1.0f, 1.0f);
	std::random_device rd;
	std::mt19937 gen(rd());
	float3x3 M = float3x3::identity();
	float3x3 R = SubMatrix(
			MakeRotationMatrix(normalize(float3(r(gen), r(gen), r(gen))),
					(float) (r(gen) * ALY_PI * 2)));
	float3x3 S = SubMatrix(MakeScale(float3(r(gen), r(gen), r(gen))));
	std::vector<float3> in(N);
	float3 avgIn;
	float3 avgOut;
	for (int n = 0; n < N; n++) {
		float x = 10 * r(gen);
		float y = 10 * r(gen);
		float z = 10 * r(gen);
		float3 pt(x, y, z);
		float3 qt = R * pt - pt;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				M(i, j) += qt[i] * qt[j];
			}
		}
	}
	std::cout << "M=\n" << M << std::endl;
	float3x3 Q, D;
	Eigen(M, Q, D);
	std::cout << "Q=\n" << Q << std::endl;
	std::cout << "R=\n" << R << std::endl;
	std::cout << "D=\n" << D << std::endl;
	float3x3 QDQt = Q * D * transpose(Q);
	std::cout << "QDQt=\n" << QDQt * inverse(M) << std::endl;
	float3x3 U, Vt;
	SVD(M, U, D, Vt);
	std::cout << "U=\n" << U << std::endl;
	std::cout << "D=\n" << D << std::endl;
	std::cout << "Vt=\n" << Vt << std::endl;
	std::cout << "UDVt=\n" << U * D * Vt * inverse(M) << std::endl;
	float3x3 Rest = FactorRotation(M);
	float3x3 A1 = U * D * Vt;
	float3x3 A2 = U * MakeDiagonal(float3(1, 1, -1)) * D * Vt;
	std::cout << "Determinant " << determinant(A1) << " " << determinant(A2)
			<< std::endl;
	std::cout << "Rotation " << Rest << std::endl;
	/*

	 */
	return true;
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
		float d1 = (float) (dot(im1, im2) + length(im1) + lengthL1(im2)
				+ max(im1) - min(im1));
		float4 d2 = float4(dotVec(im1, im2)) + float4(lengthVec(im2))
				+ lengthVecL1(im1) + maxVec(im1) - minVec(im1);
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

