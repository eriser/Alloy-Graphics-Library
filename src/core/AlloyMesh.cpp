/*
 * Copyright(C) 2014, Blake C. Lucas, Ph.D. (img.science@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "AlloyMath.h"
#include "AlloyMesh.h"
#include <vector>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ply_io.h"
namespace aly {
using namespace std;
PlyProperty MeshVertProps[] = { // property information for a vertex
		{ "x", Float32, Float32, static_cast<int>(offsetof(plyVertex, x)), 0, 0,0, 0 },
		{ "y", Float32, Float32, static_cast<int>(offsetof(plyVertex, x) + sizeof(float)), 0, 0,0, 0 },
		{ "z", Float32, Float32, static_cast<int>(offsetof(plyVertex, x) + sizeof(float)+ sizeof(float)), 0, 0, 0, 0 },
		{ "nx", Float32, Float32, static_cast<int>(offsetof(plyVertex, n)), 0, 0,0, 0 },
		{ "ny", Float32, Float32, static_cast<int>(offsetof(plyVertex, n) + sizeof(float)), 0, 0,0, 0 },
		{ "nz", Float32, Float32, static_cast<int>(offsetof(plyVertex, n) + sizeof(float)+ sizeof(float)), 0, 0, 0, 0 },
		{ "vx", Float32, Float32, static_cast<int>(offsetof(plyVertex, vel)), 0, 0,0, 0 },
		{ "vy", Float32, Float32, static_cast<int>(offsetof(plyVertex, vel) + sizeof(float)), 0, 0,0, 0 },
		{ "vz", Float32, Float32, static_cast<int>(offsetof(plyVertex, vel) + sizeof(float)+ sizeof(float)), 0, 0, 0, 0 },
		{ "red", Uint8, Uint8, static_cast<int>(offsetof(plyVertex, red)), 0, 0, 0, 0 },
		{"green", Uint8, Uint8, static_cast<int>(offsetof(plyVertex,green)), 0, 0, 0, 0 },
		{ "blue", Uint8, Uint8,static_cast<int>(offsetof(plyVertex, blue)), 0, 0, 0, 0 },
		{"alpha", Uint8, Uint8, static_cast<int>(offsetof(plyVertex,alpha)), 0, 0, 0, 0 }, };

PlyProperty MeshFaceProps[] = { // property information for a face
		{ "vertex_indices", Int32, Int32, static_cast<int>(offsetof(plyFace,verts)), 1, Uint8, Uint8, static_cast<int>(offsetof(plyFace,nverts)) },
		{ "velocities", Float32, Float32, static_cast<int>(offsetof(plyFace,velocity)), 1, Uint8, Uint8, static_cast<int>(offsetof(plyFace,nvels)) },
		{ "vertex_indices", Int32, Int32, static_cast<int>(offsetof(plyFaceTexture, verts)), 1, Uint8,Uint8, static_cast<int>(offsetof(plyFaceTexture, nverts)) },
		{ "texcoord", Float32, Float32, static_cast<int>(offsetof(plyFaceTexture, uvs)), 1, Uint8, Uint8,static_cast<int>(offsetof(plyFaceTexture, uvcount)) },
		{ "velocities", Float32, Float32, static_cast<int>(offsetof(plyFaceTexture,velocity)), 1, Uint8, Uint8, static_cast<int>(offsetof(plyFaceTexture,nvels)) }, };
Mesh::Mesh() :
		mGL(), mPose(float4x4::identity()) {
}
void Mesh::reset() {
	mVertexes.clear();
	mVertexNormals.clear();
	mQuadIndexes.clear();
	mTriIndexes.clear();
	uvMap.clear();
	textureImage.clear();
}
bool Mesh::save(const std::string& f) {
	if (mVertexes.size() == 0)
		return false;
	char* elemNames[] = { "vertex", "face", "normal" };

	std::cout << "Saving " << f << " ... ";
	int i, j, idx;
	const char* fileName = f.c_str();
	bool usingTexture = (uvMap.size() > 0);

	PlyFile *ply;

	// Get input and check data
	ply = open_for_writing_ply(fileName, 2, elemNames,PLY_BINARY_LE);//

	if (ply == NULL) {
		std::cout << "Failed. " << std::endl;
		return false;
	}

	// compute colors, if any
	int numPts = mVertexes.size();

	int numPolys = mQuadIndexes.size() / 4 + mTriIndexes.size() / 3;

	std::vector<unsigned char> pointColors;

	if (mVertexColors.size() > 0) {
		size_t inc = 0;
		pointColors.resize(3 * mVertexColors.size());
		for (i = 0; i < numPts; i++) {
			float4 d = mVertexColors[i];
			pointColors[inc++] = (unsigned char) clamp(d[0] * 255.0f, 0.0f,
					255.0f);
			pointColors[inc++] = (unsigned char) clamp(d[1] * 255.0f, 0.0f,
					255.0f);
			pointColors[inc++] = (unsigned char) clamp(d[2] * 255.0f, 0.0f,
					255.0f);
		}
	}
	// describe what properties go into the vertex and face elements
	element_count_ply(ply, "vertex", numPts);
	ply_describe_property(ply, "vertex", &MeshVertProps[0]);
	ply_describe_property(ply, "vertex", &MeshVertProps[1]);
	ply_describe_property(ply, "vertex", &MeshVertProps[2]);

	if(mVertexNormals.size()>0){
		ply_describe_property(ply, "vertex", &MeshVertProps[3]);
		ply_describe_property(ply, "vertex", &MeshVertProps[4]);
		ply_describe_property(ply, "vertex", &MeshVertProps[5]);
	}
	if (mVertexColors.size() > 0) {
		ply_describe_property(ply, "vertex", &MeshVertProps[9]);
		ply_describe_property(ply, "vertex", &MeshVertProps[10]);
		ply_describe_property(ply, "vertex", &MeshVertProps[11]);
	}
	element_count_ply(ply, "face", numPolys);

	if (usingTexture) {
		ply_describe_property(ply, "face", &MeshFaceProps[2]);
		ply_describe_property(ply, "face", &MeshFaceProps[3]);
	} else {
		ply_describe_property(ply, "face", &MeshFaceProps[0]);
	}

	// write a comment and an object information field
	append_comment_ply(ply, "PLY File");
	if (usingTexture) {
		std::string comment = "TextureFile texture.png";
		append_comment_ply(ply, (char*) comment.c_str());
	}
	append_obj_info_ply(ply, "ImageSci");

	// complete the header
	header_complete_ply(ply);

	// set up and write the vertex elements
	plyVertex vert;
	put_element_setup_ply(ply, "vertex");

	for (i = 0; i < numPts; i++) {
		float3 pt = mVertexes[i];
		vert.x[0] = pt[0];
		vert.x[1] = pt[1];
		vert.x[2] = pt[2];
		if(mVertexNormals.size()>0){
			float3 n=mVertexNormals[i];
			vert.n[0]=n[0];
			vert.n[1]=n[1];
			vert.n[2]=n[2];
		}
		if (pointColors.size() > 0) {
			idx = 3 * i;
			vert.red = pointColors[idx];
			vert.green = pointColors[idx + 1];
			vert.blue = pointColors[idx + 2];
		}
		put_element_ply(ply, (void *) &vert);
	}
	// set up and write the face elements
	plyFace face;
	plyFaceTexture faceT;
	int verts[256];
	float2 uvs[3];
	float vel[3];
	face.verts = verts;
	faceT.verts = verts;
	faceT.uvs = (float*) uvs;
	put_element_setup_ply(ply, "face");
	if (usingTexture) {
		int sz = mQuadIndexes.size() / 4;
		for (int i = 0; i < sz; i++) {
			faceT.nverts = 4;
			faceT.uvcount = 8;
			for (j = 0; j < 4; j++) {
				faceT.verts[j] = mQuadIndexes[i][j];
				uvs[j] = uvMap[4 * i + j];
			}
			put_element_ply(ply, (void *) &faceT);
		}
		sz = mTriIndexes.size() / 3;
		for (int i = 0; i < sz; i++) {
			faceT.nverts = 3;
			faceT.uvcount = 6;
			for (j = 0; j < 3; j++) {
				faceT.verts[j] = mTriIndexes[i][j];
				uvs[j] = uvMap[3 * i + j];
			}
			put_element_ply(ply, (void *) &faceT);
		}
	} else {
		int sz = mQuadIndexes.size() / 4;
		for (int i = 0; i < sz; i++) {
			for (j = 0; j < 4; j++) {
				face.nverts = 4;
				face.verts[j] = mQuadIndexes[i][j];
			}
			put_element_ply(ply, (void *) &face);
		}
		sz = mTriIndexes.size() / 3;
		for (int i = 0; i < sz; i++) {
			for (j = 0; j < 3; j++) {
				face.nverts = 3;
				face.verts[j] = mTriIndexes[i][j];
			}
			put_element_ply(ply, (void *) &face);
		}
	}
	// close the PLY file
	close_ply(ply);
	free_ply(ply);
	std::cout << "Done." << std::endl;
	return true;
}
bool Mesh::openMesh(const std::string& file) {
	int i, j, k;
	int numPts = 0, numPolys = 0;
	// open a PLY file for reading
	PlyFile *ply;
	int nelems = 3, fileType = PLY_BINARY_LE, numElems, nprops;
	char** elist = NULL;
	char* elemName;
	float version;
	if (!(ply = ply_open_for_reading(file.c_str(), &nelems, &elist, &fileType,
			&version))) {
		std::cout << "Could not open ply file. [" << file << "]" << std::endl;
		free_ply(ply);
		return false;
	}
	// Check to make sure that we can read geometry
	PlyElement *elem;
	int index;
	if ((elem = find_element(ply, "vertex")) == NULL
			|| find_property(elem, "x", &index) == NULL
			|| find_property(elem, "y", &index) == NULL
			|| find_property(elem, "z", &index) == NULL
			|| (elem = find_element(ply, "face")) == NULL
			|| find_property(elem, "vertex_indices", &index) == NULL) {
		std::cerr << "Cannot read geometry [" << file << "]" << std::endl;
		close_ply(ply);
		free_ply(ply);
		return false;
	}

	// Check for optional attribute data. We can handle intensity; and the
	// triplet red, green, blue.
	bool RGBPointsAvailable = false;
	bool hasParticleVelocity=false;
	bool hasNormals=false;
	bool hasVertexVelocity=false;
	this->mTriIndexes.clear();
	this->mQuadIndexes.clear();
	this->mVertexes.clear();
	this->mVertexNormals.clear();
	this->mVertexColors.clear();
	if ((elem = find_element(ply, "vertex")) != NULL
			&& find_property(elem, "red", &index) != NULL
			&& find_property(elem, "green", &index) != NULL
			&& find_property(elem, "blue", &index) != NULL) {
		RGBPointsAvailable = true;
	}
	if ((elem = find_element(ply, "vertex")) != NULL
			&& find_property(elem, "nx", &index) != NULL
			&& find_property(elem, "ny", &index) != NULL
			&& find_property(elem, "nz", &index) != NULL) {
		hasNormals = true;
	}
	if ((elem = find_element(ply, "vertex")) != NULL
			&& find_property(elem, "vx", &index) != NULL
			&& find_property(elem, "vy", &index) != NULL
			&& find_property(elem, "vz", &index) != NULL) {
		hasVertexVelocity = true;
	}
	if ((elem = find_element(ply, "face")) != NULL&& find_property(elem, "velocities", &index) != NULL) {
		hasParticleVelocity = true;
	}
	int verts[256];
	float velocity[3];
	plyFace face;
	plyVertex vertex;
	face.verts=verts;
	face.velocity=velocity;
	memset(verts, 0, sizeof(verts));
	// Okay, now we can grab the data
	for (i = 0; i < nelems; i++) {
		//get the description of the first element */
		elemName = elist[i];
		get_element_description_ply(ply, elemName, &numElems, &nprops);
		// if we're on vertex elements, read them in
		if (elemName && !strcmp("vertex", elemName)) {
			// Create a list of points
			numPts = numElems;
			this->mVertexes.resize(numPts, float3(0.0f));
			// Setup to read the PLY elements
			ply_get_property(ply, elemName, &MeshVertProps[0]);
			ply_get_property(ply, elemName, &MeshVertProps[1]);
			ply_get_property(ply, elemName, &MeshVertProps[2]);
			if (hasNormals) {
				this->mVertexNormals.resize(numPts);
				ply_get_property(ply, elemName, &MeshVertProps[3]);
				ply_get_property(ply, elemName, &MeshVertProps[4]);
				ply_get_property(ply, elemName, &MeshVertProps[5]);
			}
			if (RGBPointsAvailable) {
				this->mVertexColors.resize(numPts);
				ply_get_property(ply, elemName, &MeshVertProps[9]);
				ply_get_property(ply, elemName, &MeshVertProps[10]);
				ply_get_property(ply, elemName, &MeshVertProps[11]);
			}
			for (j = 0; j < numPts; j++) {
				get_element_ply(ply, &vertex);
				this->mVertexes[j] = float3(vertex.x[0], vertex.x[1],
						vertex.x[2]);

				if (RGBPointsAvailable) {
					this->mVertexColors[j] = float4(vertex.red / 255.0f,
							vertex.green / 255.0f, vertex.blue / 255.0f,1.0f);
				}
				if(hasNormals){
					this->mVertexNormals[j]=float3(vertex.n[0],vertex.n[1],vertex.n[2]);
				}
			}
		}			//if vertex
		else if (elemName && !strcmp("face", elemName)) {
			// Create a polygonal array
			numPolys = numElems;
			// Get the face properties
			ply_get_property(ply, elemName, &MeshFaceProps[0]);
			for (j = 0; j < numPolys; j++) {
				get_element_ply(ply, &face);
				if (face.nverts == 4) {
					this->mQuadIndexes.append(uint4(face.verts[0],face.verts[1],face.verts[2],face.verts[3]));
				} else if (face.nverts == 3) {
					for (k = 0; k < face.nverts; k++) {
						this->mTriIndexes.append(uint3(face.verts[0],face.verts[1],face.verts[2]));
					}
				}
			}
		}							//if face
	} //for all elements of the PLY file
	  //free(elist); //allocated by ply_open_for_reading
	close_ply(ply);
	free_ply(ply);
	if (this->mVertexes.size() > 0) {
		this->updateBoundingBox();
		return true;
	} else {
		return false;
	}
}
void Mesh::updateVertexNormals(int SMOOTH_ITERATIONS, float DOT_TOLERANCE) {
	uint32_t sz = mTriIndexes.size();
	float3 pt;
	mVertexNormals.resize(mVertexes.size(), float3(0.0f));
	for (uint32_t i = 0; i < sz; i ++) {
		uint3 verts=mTriIndexes[i];
		float3 v1 = mVertexes[verts.x];
		float3 v2 = mVertexes[verts.y];
		float3 v3 = mVertexes[verts.z];
		float3 norm = cross((v2 - v1),(v3 - v1));
		mVertexNormals[verts.x] += norm;
		mVertexNormals[verts.y] += norm;
		mVertexNormals[verts.z] += norm;
	}
	sz = mQuadIndexes.size();
	for (int i = 0; i < sz; i ++) {
		uint4 verts=mQuadIndexes[i];
		float3 v1 = mVertexes[verts.x];
		float3 v2 = mVertexes[verts.y];
		float3 v3 = mVertexes[verts.z];
		float3 v4 = mVertexes[verts.w];
		float3 norm = cross((v1 - pt),(v2 - pt));
		norm += cross((v2 - pt),(v3 - pt));
		norm += cross((v3 - pt),(v4 - pt));
		norm += cross((v4 - pt),(v1 - pt));
		mVertexNormals[verts.x] += norm;
		mVertexNormals[verts.y] += norm;
		mVertexNormals[verts.z] += norm;
		mVertexNormals[verts.w] += norm;
	}
#pragma omp for
	for (size_t n=0;n<mVertexNormals.size();n++) {
		mVertexNormals[n]=normalize(mVertexNormals[n]);
	}
	if (SMOOTH_ITERATIONS > 0) {
		int vertCount = mVertexes.size();
		std::vector<float3> tmp(vertCount);
		std::vector<std::list<int>> vertNbrs(vertCount);
		int indexCount = mQuadIndexes.size();
		int v1, v2, v3, v4;

		for (int i = 0; i < indexCount; i ++) {
			uint4 verts=mQuadIndexes[i];
			int v1=verts.x;
			int v2=verts.y;
			int v3=verts.z;
			int v4=verts.w;

			vertNbrs[v1].push_back(v2);
			vertNbrs[v2].push_back(v3);
			vertNbrs[v3].push_back(v1);

			vertNbrs[v3].push_back(v4);
			vertNbrs[v4].push_back(v1);
			vertNbrs[v1].push_back(v3);
		}
		for (int iter = 0; iter < SMOOTH_ITERATIONS; iter++) {
#pragma omp for
			for (int i = 0; i < vertCount; i++) {
				float3 norm = mVertexNormals[i];
				float3 avg = float3(0.0f);
				for (int nbr : vertNbrs[i]) {
					float3 nnorm = mVertexNormals[nbr];
					;
					if (dot(norm,nnorm) > DOT_TOLERANCE) {
						avg += nnorm;
					} else {
						avg += norm;
					}
				}
				tmp[i] = normalize(avg);
			}
			mVertexNormals = tmp;
		}
	}
}
float Mesh::estimateVoxelSize(int stride) {
	int count = 0;
	//float maxLength = 0.0f;
	int sz = mTriIndexes.size();
	float mEstimatedVoxelSize = 0.0f;
	for (int i = 0; i < sz; i +=  stride) {
		uint3 verts=mTriIndexes[i];
		float3 v1 = mVertexes[verts.x];
		float3 v2 = mVertexes[verts.y];
		float3 v3 = mVertexes[verts.z];
		float e1 = length(v1 - v2);
		float e2 = length(v1 - v3);
		float e3 = length(v2 - v3);
		//maxLength = std::max(std::max(e1, e2), std::max(maxLength, e3));
		mEstimatedVoxelSize += e1 + e2 + e3;
	}
	count = sz / stride;
	sz = mQuadIndexes.size();
	for (int i = 0; i < sz; i +=  stride) {
		uint4 verts=mQuadIndexes[i];
		float3 v1 = mVertexes[verts.x];
		float3 v2 = mVertexes[verts.y];
		float3 v3 = mVertexes[verts.z];
		float3 v4 = mVertexes[verts.w];
		float e1 = length(v1 - v2);
		float e2 = length(v2 - v3);
		float e3 = length(v3 - v4);
		float e4 = length(v4 - v1);
		//maxLength = std::max(maxLength,std::max(std::max(e1, e2), std::max(e3, e4)));
		mEstimatedVoxelSize += e1 + e2 + e3 + e4;
	}
	count += sz / stride;
	mEstimatedVoxelSize /= count;

	std::cout << "Estimated voxel size =" << mEstimatedVoxelSize << std::endl;
	return mEstimatedVoxelSize;
}
box3f Mesh::updateBoundingBox() {
	const int BATCHES = 32;
	float3 minPt(std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max());
	std::vector<float3> minPtBatch(BATCHES,
			float3(std::numeric_limits<float>::max(),
					std::numeric_limits<float>::max(),
					std::numeric_limits<float>::max()));
	float3 maxPt(std::numeric_limits<float>::min(),
			std::numeric_limits<float>::min(),
			std::numeric_limits<float>::min());
	std::vector<float3> maxPtBatch(BATCHES,
			float3(std::numeric_limits<float>::min(),
					std::numeric_limits<float>::min(),
					std::numeric_limits<float>::min()));
	int SZ = mVertexes.size();
	int batchSize = (SZ % BATCHES == 0) ? SZ / BATCHES : SZ / BATCHES + 1;
#pragma omp for
	for (int b = 0; b < BATCHES; b++) {
		int sz = std::min(SZ, batchSize * (b + 1));
		for (uint32_t idx = b * batchSize; idx < sz; idx++) {
			float3& pt = mVertexes[idx];
			minPtBatch[b][0] = std::min(minPtBatch[b][0], pt[0]);
			minPtBatch[b][1] = std::min(minPtBatch[b][1], pt[1]);
			minPtBatch[b][2] = std::min(minPtBatch[b][2], pt[2]);

			maxPtBatch[b][0] = std::max(maxPtBatch[b][0], pt[0]);
			maxPtBatch[b][1] = std::max(maxPtBatch[b][1], pt[1]);
			maxPtBatch[b][2] = std::max(maxPtBatch[b][2], pt[2]);
		}
	}

	for (int b = 0; b < BATCHES; b++) {
		minPt[0] = std::min(minPtBatch[b][0], minPt[0]);
		minPt[1] = std::min(minPtBatch[b][1], minPt[1]);
		minPt[2] = std::min(minPtBatch[b][2], minPt[2]);

		maxPt[0] = std::max(maxPtBatch[b][0], maxPt[0]);
		maxPt[1] = std::max(maxPtBatch[b][1], maxPt[1]);
		maxPt[2] = std::max(maxPtBatch[b][2], maxPt[2]);
	}
	mBoundingBox=box3f(minPt, maxPt);
	return mBoundingBox;
}
void Mesh::scale(float sc) {
#pragma omp for
	for (size_t i = 0; i < mVertexes.size(); i++) {
		mVertexes[i] *= sc;
	}
	mBoundingBox.dimensions = sc*mBoundingBox.dimensions;
	mBoundingBox.position = sc*mBoundingBox.position;

}
void Mesh::mapIntoBoundingBox(float voxelSize) {
	float3 minPt = mBoundingBox.min();
#pragma omp for
	for (size_t i = 0; i < mVertexes.size(); i++) {
		float3& pt = mVertexes[i];
		pt = (pt - minPt) / voxelSize;
	}
}
void Mesh::mapOutOfBoundingBox(float voxelSize) {
	float3 minPt = mBoundingBox.min();
#pragma omp for
	for (size_t i = 0; i < mVertexes.size(); i++) {
		float3& pt = mVertexes[i];
		pt = pt * voxelSize + minPt;
	}
}
void Mesh::draw() {
	glBindVertexArray(mGL.mVao);
	if (mGL.mVertexBuffer > 0) {
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mGL.mVertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (mGL.mNormalBuffer > 0) {
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, mGL.mNormalBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (mGL.mColorBuffer > 0) {
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, mGL.mColorBuffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (mGL.mQuadIndexCount > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGL.mQuadIndexBuffer);
		glDrawElements(GL_TRIANGLES, mGL.mQuadIndexCount, GL_UNSIGNED_INT, NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else if (mGL.mQuadCount > 0) {
		glDrawArrays(GL_TRIANGLES, 0, mGL.mQuadCount);
	}
	if (mGL.mTriangleIndexCount > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGL.mTriIndexBuffer);
		glDrawElements(GL_TRIANGLES, mGL.mTriangleIndexCount, GL_UNSIGNED_INT,
				NULL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else if (mGL.mTriangleCount > 0) {
		glDrawArrays(GL_TRIANGLES, 0, mGL.mTriangleCount);
	}
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::updateGL() {
	mGL.mQuadCount = 0;
	mGL.mTriangleCount = 0;
	mGL.mTriangleIndexCount = 0;
	mGL.mQuadIndexCount = 0;
	if (mGL.mVao == 0)
		glGenVertexArrays(1, &mGL.mVao);

	if (mVertexes.size() > 0) {
		if (glIsBuffer(mGL.mVertexBuffer) == GL_TRUE)
			glDeleteBuffers(1, &mGL.mVertexBuffer);
		glGenBuffers(1, &mGL.mVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mGL.mVertexBuffer);
		if (glIsBuffer(mGL.mVertexBuffer) == GL_FALSE)
			throw std::runtime_error("Error: Unable to create vertex buffer");
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mVertexes.size(),
				&mVertexes[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	if (mVertexColors.size() > 0) {
		if (glIsBuffer(mGL.mColorBuffer) == GL_TRUE)
			glDeleteBuffers(1, &mGL.mColorBuffer);

		glGenBuffers(1, &mGL.mColorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mGL.mColorBuffer);
		if (glIsBuffer(mGL.mColorBuffer) == GL_FALSE)
			throw std::runtime_error("Error: Unable to create color buffer");

		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * mVertexColors.size(),
				&mVertexColors[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	if (mTriIndexes.size() > 0) {
		// clear old buffer
		if (glIsBuffer(mGL.mTriIndexBuffer) == GL_TRUE)
			glDeleteBuffers(1, &mGL.mTriIndexBuffer);

		// gen new buffer
		glGenBuffers(1, &mGL.mTriIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGL.mTriIndexBuffer);
		if (glIsBuffer(mGL.mTriIndexBuffer) == GL_FALSE)
			throw std::runtime_error("Error: Unable to create index buffer");

		// upload data
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				sizeof(GLuint) * mTriIndexes.size(), &mTriIndexes[0],
				GL_STATIC_DRAW); // upload data

		mGL.mTriangleIndexCount = mTriIndexes.size();
		// release buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	if (mQuadIndexes.size() > 0) {
		// clear old buffer
		if (glIsBuffer(mGL.mQuadIndexBuffer) == GL_TRUE)
			glDeleteBuffers(1, &mGL.mQuadIndexBuffer);

		// gen new buffer
		glGenBuffers(1, &mGL.mQuadIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGL.mQuadIndexBuffer);
		if (glIsBuffer(mGL.mQuadIndexBuffer) == GL_FALSE)
			throw std::runtime_error("Error: Unable to create index buffer");

		// upload data
		int sz = mQuadIndexes.size();
		std::vector<GLuint> tmp(12 * (mQuadIndexes.size() / 4));
#pragma omp for
		for (unsigned int i = 0; i < sz; i ++) {
			int offset = 12 * i;
			tmp[offset++] = mQuadIndexes[i][1];
			tmp[offset++] = mQuadIndexes[i][2];
			tmp[offset++] = mQuadIndexes[i][0];
			tmp[offset++] = mQuadIndexes[i][2];
			tmp[offset++] = mQuadIndexes[i][3];
			tmp[offset++] = mQuadIndexes[i][1];
			tmp[offset++] = mQuadIndexes[i][0];
			tmp[offset++] = mQuadIndexes[i][1];
			tmp[offset++] = mQuadIndexes[i][3];
			tmp[offset++] = mQuadIndexes[i][3];
			tmp[offset++] = mQuadIndexes[i][0];
			tmp[offset++] = mQuadIndexes[i][2];
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * tmp.size(),
				&tmp[0], GL_STATIC_DRAW); // upload data

		mGL.mQuadIndexCount = tmp.size();
		// release buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	if (mVertexNormals.size() > 0) {
		if (glIsBuffer(mGL.mNormalBuffer) == GL_TRUE)
			glDeleteBuffers(1, &mGL.mNormalBuffer);

		glGenBuffers(1, &mGL.mNormalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mGL.mNormalBuffer);
		if (glIsBuffer(mGL.mNormalBuffer) == GL_FALSE)
			throw std::runtime_error("Error: Unable to create normal buffer");

		glBufferData(GL_ARRAY_BUFFER,
				sizeof(GLfloat) * 3 * mVertexNormals.size(), &mVertexNormals[0],
				GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
Mesh::~Mesh() {
	// TODO Auto-generated destructor stub
}

} /* namespace imagesci */
