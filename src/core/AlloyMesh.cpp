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
#include "AlloyFileUtil.h"
#include <vector>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AlloyPLY.h"
#include <stddef.h>
#ifndef ALY_WINDOWS
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif
namespace aly {
	using namespace std;
	using namespace ply;

	void GLMesh::draw() const {
		draw(PrimitiveType::ALL);
	}
	void GLMesh::draw(const PrimitiveType& type) const {
		if (mesh.isDirty()) {
			mesh.update();
			mesh.setDirty(false);
		}
		context->begin();
		glBindVertexArray(vao);
		CHECK_GL_ERROR();
		if ((type == GLMesh::PrimitiveType::ALL
			|| type == GLMesh::PrimitiveType::POINTS) && vertexCount > 0) {
			if (vertexBuffer > 0) {
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}
			if (normalBuffer > 0) {
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			}
			if (colorBuffer > 0) {
				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
				glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
			}
			glDrawArrays(GL_POINTS, 0, vertexCount);
		}
		if ((type == GLMesh::PrimitiveType::ALL
			|| type == GLMesh::PrimitiveType::QUADS) && quadIndexCount > 0) {
			for (int n = 0; n < 4; n++) {
				if (quadVertexBuffer[n] > 0) {
					glEnableVertexAttribArray(3 + n);
					glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer[n]);
					glVertexAttribPointer(3 + n, 3, GL_FLOAT, GL_FALSE, 0, 0);
				}

			}
			for (int n = 0; n < 4; n++) {
				if (quadNormalBuffer[n] > 0) {
					glEnableVertexAttribArray(7 + n);
					glBindBuffer(GL_ARRAY_BUFFER, quadNormalBuffer[n]);
					glVertexAttribPointer(7 + n, 3, GL_FLOAT, GL_FALSE, 0, 0);
				}
			}
			for (int n = 0; n < 4; n++) {
				if (quadTextureBuffer[n] > 0) {
					glEnableVertexAttribArray(11 + n);
					glBindBuffer(GL_ARRAY_BUFFER, quadTextureBuffer[n]);
					glVertexAttribPointer(11 + n, 2, GL_FLOAT, GL_FALSE, 0, 0);
				}
			}
			glDrawArrays(GL_POINTS, 0, quadIndexCount);
		}
		CHECK_GL_ERROR();
		if ((type == GLMesh::PrimitiveType::ALL
			|| type == GLMesh::PrimitiveType::TRIANGLES) && triIndexCount > 0) {
			for (int n = 0; n < 3; n++) {
				if (triVertexBuffer[n] > 0) {
					glEnableVertexAttribArray(3 + n);
					glBindBuffer(GL_ARRAY_BUFFER, triVertexBuffer[n]);
					glVertexAttribPointer(3 + n, 3, GL_FLOAT, GL_FALSE, 0, 0);
				}
			}
			for (int n = 0; n < 3; n++) {
				if (triNormalBuffer[n] > 0) {
					glEnableVertexAttribArray(7 + n);
					glBindBuffer(GL_ARRAY_BUFFER, triNormalBuffer[n]);
					glVertexAttribPointer(7 + n, 3, GL_FLOAT, GL_FALSE, 0, 0);
				}
			}
			for (int n = 0; n < 3; n++) {
				if (triTextureBuffer[n] > 0) {
					glEnableVertexAttribArray(11 + n);
					glBindBuffer(GL_ARRAY_BUFFER, triTextureBuffer[n]);
					glVertexAttribPointer(11 + n, 2, GL_FLOAT, GL_FALSE, 0, 0);
				}
			}
			glDrawArrays(GL_POINTS, 0, triIndexCount);
		}
		for (int i = 0; i < 14; i++) {
			glDisableVertexAttribArray(i);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		CHECK_GL_ERROR();

		context->end();
	}
	GLMesh::GLMesh(Mesh& mesh, std::shared_ptr<AlloyContext>& context) :
		GLComponent(context), mesh(mesh), vao(0), vertexBuffer(0), normalBuffer(
			0), colorBuffer(0), triIndexBuffer(0), quadIndexBuffer(0), triCount(
				0), quadCount(0), vertexCount(0), triIndexCount(0), quadIndexCount(
					0) {

		for (int n = 0; n < 4; n++)
			quadVertexBuffer[n] = 0;
		for (int n = 0; n < 3; n++)
			triVertexBuffer[n] = 0;
		for (int n = 0; n < 4; n++)
			quadNormalBuffer[n] = 0;
		for (int n = 0; n < 3; n++)
			triNormalBuffer[n] = 0;
		for (int n = 0; n < 4; n++)
			quadTextureBuffer[n] = 0;
		for (int n = 0; n < 3; n++)
			triTextureBuffer[n] = 0;
	}
	GLMesh::~GLMesh() {
		if (context.get() == nullptr)
			return;
		context->begin();
		if (glIsBuffer(vertexBuffer) == GL_TRUE)
			glDeleteBuffers(1, &vertexBuffer);
		if (glIsBuffer(normalBuffer) == GL_TRUE)
			glDeleteBuffers(1, &normalBuffer);
		if (glIsBuffer(colorBuffer) == GL_TRUE)
			glDeleteBuffers(1, &colorBuffer);
		if (glIsBuffer(triIndexBuffer) == GL_TRUE)
			glDeleteBuffers(1, &triIndexBuffer);
		if (glIsBuffer(quadIndexBuffer) == GL_TRUE)
			glDeleteBuffers(1, &quadIndexBuffer);

		for (int n = 0; n < 4; n++)
			if (glIsBuffer(quadVertexBuffer[n]) == GL_TRUE)
				glDeleteBuffers(1, &quadVertexBuffer[n]);
		for (int n = 0; n < 3; n++)
			if (glIsBuffer(triVertexBuffer[n]) == GL_TRUE)
				glDeleteBuffers(1, &triVertexBuffer[n]);

		for (int n = 0; n < 4; n++)
			if (glIsBuffer(quadNormalBuffer[n]) == GL_TRUE)
				glDeleteBuffers(1, &quadNormalBuffer[n]);
		for (int n = 0; n < 3; n++)
			if (glIsBuffer(triNormalBuffer[n]) == GL_TRUE)
				glDeleteBuffers(1, &triNormalBuffer[n]);

		for (int n = 0; n < 4; n++)
			if (glIsBuffer(quadTextureBuffer[n]) == GL_TRUE)
				glDeleteBuffers(1, &quadTextureBuffer[n]);
		for (int n = 0; n < 3; n++)
			if (glIsBuffer(triTextureBuffer[n]) == GL_TRUE)
				glDeleteBuffers(1, &triTextureBuffer[n]);

		if (vao != 0)
			glDeleteVertexArrays(1, &vao);
		context->end();
	}
	void GLMesh::update() {
		if (context.get() == nullptr)
			return;
		context->begin();
		quadCount = 0;
		triCount = 0;
		triIndexCount = 0;
		quadIndexCount = 0;
		vertexCount = 0;
		if (vao == 0)
			glGenVertexArrays(1, &vao);

		if (mesh.vertexLocations.size() > 0) {

			if (glIsBuffer(vertexBuffer) == GL_TRUE)
				glDeleteBuffers(1, &vertexBuffer);
			glGenBuffers(1, &vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			if (glIsBuffer(vertexBuffer) == GL_FALSE)
				throw std::runtime_error("Error: Unable to create vertex buffer");
			glBufferData(GL_ARRAY_BUFFER,
				sizeof(GLfloat) * 3 * mesh.vertexLocations.size(),
				mesh.vertexLocations.ptr(), GL_STATIC_DRAW);
			vertexCount = (uint32_t)mesh.vertexLocations.size();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		if (mesh.vertexNormals.size() > 0) {
			if (glIsBuffer(normalBuffer) == GL_TRUE)
				glDeleteBuffers(1, &normalBuffer);
			glGenBuffers(1, &normalBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
			if (glIsBuffer(normalBuffer) == GL_FALSE)
				throw std::runtime_error("Error: Unable to create vertex buffer");
			glBufferData(GL_ARRAY_BUFFER,
				sizeof(GLfloat) * 3 * mesh.vertexNormals.size(),
				mesh.vertexNormals.ptr(), GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		if (mesh.vertexColors.size() > 0) {
			if (glIsBuffer(colorBuffer) == GL_TRUE)
				glDeleteBuffers(1, &colorBuffer);
			glGenBuffers(1, &colorBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
			if (glIsBuffer(colorBuffer) == GL_FALSE)
				throw std::runtime_error("Error: Unable to create color buffer");
			glBufferData(GL_ARRAY_BUFFER,
				sizeof(GLfloat) * 4 * mesh.vertexColors.size(),
				mesh.vertexColors.ptr(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		if (mesh.triIndexes.size() > 0) {
			int offset = 0;

			std::vector<float3> tris[3];
			for (int n = 0; n < 3; n++) {
				tris[n].resize(mesh.triIndexes.size());
				if (glIsBuffer(triVertexBuffer[n]) == GL_TRUE)
					glDeleteBuffers(1, &triVertexBuffer[n]);
				glGenBuffers(1, &triVertexBuffer[n]);
			}
			for (uint3 face : mesh.triIndexes.data) {
				for (int n = 0; n < 3; n++) {
					tris[n][offset] = mesh.vertexLocations[face[n]];
				}
				offset++;
			}
			for (int n = 0; n < 3; n++) {
				if (glIsBuffer(triVertexBuffer[n]) == GL_TRUE)
					glDeleteBuffers(1, &triVertexBuffer[n]);
				glGenBuffers(1, &triVertexBuffer[n]);
				glBindBuffer(GL_ARRAY_BUFFER, triVertexBuffer[n]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * tris[n].size(),
					tris[n].data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			CHECK_GL_ERROR();

			triIndexCount = (GLuint)mesh.triIndexes.size();

		}
		if (mesh.quadIndexes.size() > 0) {
			int offset = 0;
			std::vector<float3> quads[4];
			for (int n = 0; n < 4; n++) {
				quads[n].resize(mesh.quadIndexes.size());
				if (glIsBuffer(quadVertexBuffer[n]) == GL_TRUE)
					glDeleteBuffers(1, &quadVertexBuffer[n]);
				glGenBuffers(1, &quadVertexBuffer[n]);
			}
			for (uint4 face : mesh.quadIndexes.data) {
				for (int n = 0; n < 4; n++) {
					quads[n][offset] = mesh.vertexLocations[face[n]];
				}
				offset++;
			}
			for (int n = 0; n < 4; n++) {
				if (glIsBuffer(quadVertexBuffer[n]) == GL_TRUE)
					glDeleteBuffers(1, &quadVertexBuffer[n]);
				glGenBuffers(1, &quadVertexBuffer[n]);
				glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer[n]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * quads[n].size(),
					quads[n].data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			CHECK_GL_ERROR();

			quadIndexCount = (GLuint)mesh.quadIndexes.size();
		}
		if (mesh.vertexNormals.size() > 0) {

			if (glIsBuffer(normalBuffer) == GL_TRUE)
				glDeleteBuffers(1, &normalBuffer);

			glGenBuffers(1, &normalBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
			if (glIsBuffer(normalBuffer) == GL_FALSE)
				throw std::runtime_error("Error: Unable to create normal buffer");

			glBufferData(GL_ARRAY_BUFFER,
				sizeof(GLfloat) * 3 * mesh.vertexNormals.size(),
				mesh.vertexNormals.ptr(),
				GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			if (mesh.quadIndexes.size() > 0) {
				int offset = 0;
				std::vector<float3> quads[4];
				for (int n = 0; n < 4; n++) {
					quads[n].resize(mesh.quadIndexes.size());
					if (glIsBuffer(quadNormalBuffer[n]) == GL_TRUE)
						glDeleteBuffers(1, &quadNormalBuffer[n]);
					glGenBuffers(1, &quadNormalBuffer[n]);
				}
				for (uint4 face : mesh.quadIndexes.data) {
					for (int n = 0; n < 4; n++) {
						quads[n][offset] = mesh.vertexNormals[face[n]];
					}
					offset++;
				}
				for (int n = 0; n < 4; n++) {
					if (glIsBuffer(quadNormalBuffer[n]) == GL_TRUE)
						glDeleteBuffers(1, &quadNormalBuffer[n]);
					glGenBuffers(1, &quadNormalBuffer[n]);
					glBindBuffer(GL_ARRAY_BUFFER, quadNormalBuffer[n]);
					glBufferData(GL_ARRAY_BUFFER,
						sizeof(GLfloat) * 3 * quads[n].size(), quads[n].data(),
						GL_STATIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				CHECK_GL_ERROR();
			}
			if (mesh.triIndexes.size() > 0) {
				int offset = 0;
				std::vector<float3> tris[3];
				for (int n = 0; n < 3; n++) {
					tris[n].resize(mesh.triIndexes.size());
					if (glIsBuffer(triNormalBuffer[n]) == GL_TRUE)
						glDeleteBuffers(1, &triNormalBuffer[n]);
					glGenBuffers(1, &triNormalBuffer[n]);
				}
				for (uint3 face : mesh.triIndexes.data) {
					for (int n = 0; n < 3; n++) {
						tris[n][offset] = mesh.vertexNormals[face[n]];
					}
					offset++;
				}
				for (int n = 0; n < 3; n++) {
					if (glIsBuffer(triNormalBuffer[n]) == GL_TRUE)
						glDeleteBuffers(1, &triNormalBuffer[n]);
					glGenBuffers(1, &triNormalBuffer[n]);
					glBindBuffer(GL_ARRAY_BUFFER, triNormalBuffer[n]);
					glBufferData(GL_ARRAY_BUFFER,
						sizeof(GLfloat) * 3 * tris[n].size(), tris[n].data(),
						GL_STATIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				CHECK_GL_ERROR();
			}
		}
		if (mesh.textureMap.size() > 0) {
			if (mesh.quadIndexes.size() > 0) {
				int offset = 0;
				size_t idx = 0;
				std::vector<float2> quads[4];
				for (int n = 0; n < 4; n++) {
					quads[n].resize(mesh.quadIndexes.size());
					if (glIsBuffer(quadTextureBuffer[n]) == GL_TRUE)
						glDeleteBuffers(1, &quadTextureBuffer[n]);
					glGenBuffers(1, &quadTextureBuffer[n]);
				}
				for (offset = 0; offset < (int)mesh.quadIndexes.size(); offset++) {
					for (int n = 0; n < 4; n++) {
						quads[n][offset] = mesh.textureMap[idx++];
					}
				}
				for (int n = 0; n < 4; n++) {
					if (glIsBuffer(quadTextureBuffer[n]) == GL_TRUE)
						glDeleteBuffers(1, &quadTextureBuffer[n]);
					glGenBuffers(1, &quadTextureBuffer[n]);
					glBindBuffer(GL_ARRAY_BUFFER, quadTextureBuffer[n]);
					glBufferData(GL_ARRAY_BUFFER,
						sizeof(GLfloat) * 2 * quads[n].size(), quads[n].data(),
						GL_STATIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				CHECK_GL_ERROR();
			}
			if (mesh.triIndexes.size() > 0) {
				int offset = 0;
				size_t idx = 0;
				std::vector<float2> tris[3];
				for (int n = 0; n < 3; n++) {
					tris[n].resize(mesh.triIndexes.size());
					if (glIsBuffer(triTextureBuffer[n]) == GL_TRUE)
						glDeleteBuffers(1, &triTextureBuffer[n]);
					glGenBuffers(1, &triTextureBuffer[n]);
				}
				for (offset = 0; offset < (int)mesh.triIndexes.size(); offset++) {
					for (int n = 0; n < 3; n++) {
						tris[n][offset] = mesh.textureMap[idx++];
					}
					offset++;
				}
				for (int n = 0; n < 3; n++) {
					if (glIsBuffer(triTextureBuffer[n]) == GL_TRUE)
						glDeleteBuffers(1, &triTextureBuffer[n]);
					glGenBuffers(1, &triTextureBuffer[n]);
					glBindBuffer(GL_ARRAY_BUFFER, triTextureBuffer[n]);
					glBufferData(GL_ARRAY_BUFFER,
						sizeof(GLfloat) * 2 * tris[n].size(), tris[n].data(),
						GL_STATIC_DRAW);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				CHECK_GL_ERROR();
			}
		}
		context->end();
	}
	Mesh::Mesh(std::shared_ptr<AlloyContext>& context) :
		gl(*this, context), pose(float4x4::identity()) {
	}
	bool Mesh::load(const std::string& file) {
		try {
			ReadMeshFromFile(file, *this);
			return true;
		}
		catch (const exception& e) {
			std::cout << e.what() << std::endl;
			return false;
		}
	}
	void Mesh::draw(const GLMesh::PrimitiveType& type) const {
		gl.draw(type);
	}
	void Mesh::clear() {
		vertexLocations.clear();
		vertexNormals.clear();
		quadIndexes.clear();
		triIndexes.clear();
		textureMap.clear();
		textureImage.clear();
		dirty = true;
	}
	bool Mesh::save(const std::string& file) {
		try {
			WriteMeshToFile(file, *this);
			return true;
		}
		catch (exception&) {
			return false;
		}
	}
	void WriteMeshToFile(const std::string& file, const Mesh& mesh, bool binary) {
		std::vector<std::string> elemNames = { "vertex", "face" };
		int i, j, idx;
		bool hasTexture = (mesh.textureMap.size() > 0);
		// Get input and check data
		PLYReaderWriter ply;
		ply.openForWriting(file, elemNames, (binary) ? FileFormat::BINARY_LE : FileFormat::ASCII); //

		// compute colors, if any
		int numPts = (int)(mesh.vertexLocations.size());

		int numPolys = (int)(mesh.quadIndexes.size() + mesh.triIndexes.size());

		std::vector<unsigned char> pointColors;

		if (mesh.vertexColors.size() > 0) {
			size_t inc = 0;
			pointColors.resize(3 * mesh.vertexColors.size());
			for (i = 0; i < numPts; i++) {
				float4 d = mesh.vertexColors[i];
				pointColors[inc++] = (unsigned char)clamp(d[0] * 255.0f, 0.0f,
					255.0f);
				pointColors[inc++] = (unsigned char)clamp(d[1] * 255.0f, 0.0f,
					255.0f);
				pointColors[inc++] = (unsigned char)clamp(d[2] * 255.0f, 0.0f,
					255.0f);
			}
		}
		// describe what properties go into the vertex and face elements
		ply.elementCount("vertex", numPts);
		ply.describeProperty("vertex", &MeshVertProps[0]);
		ply.describeProperty("vertex", &MeshVertProps[1]);
		ply.describeProperty("vertex", &MeshVertProps[2]);

		if (mesh.vertexNormals.size() > 0) {
			ply.describeProperty("vertex", &MeshVertProps[3]);
			ply.describeProperty("vertex", &MeshVertProps[4]);
			ply.describeProperty("vertex", &MeshVertProps[5]);
		}
		if (mesh.vertexColors.size() > 0) {
			ply.describeProperty("vertex", &MeshVertProps[9]);
			ply.describeProperty("vertex", &MeshVertProps[10]);
			ply.describeProperty("vertex", &MeshVertProps[11]);
		}
		ply.elementCount("face", numPolys);

		if (hasTexture) {
			ply.describeProperty("face", &MeshFaceProps[2]);
			ply.describeProperty("face", &MeshFaceProps[3]);
		}
		else {
			ply.describeProperty("face", &MeshFaceProps[0]);
		}

		// write a comment and an object information field
		ply.appendComment("PLY File");
		if (mesh.textureImage.size() > 0) {
			std::string textureFile = GetFileNameWithoutExtension(file) + ".png";
			std::string comment = "TextureFile " + textureFile;
			ply.appendComment(comment);
			std::string root = RemoveTrailingSlash(GetParentDirectory(file));
			if (root.size() > 0) {
				WriteImageToFile(root + ALY_PATH_SEPARATOR + textureFile, mesh.textureImage);
			}
			else {
				WriteImageToFile(textureFile, mesh.textureImage);
			}
		}
		ply.appendObjInfo("ImageSci");
		// complete the header
		ply.headerComplete();

		// set up and write the vertex elements
		plyVertex vert;
		ply.putElementSetup("vertex");

		for (i = 0; i < numPts; i++) {
			float3 pt = mesh.vertexLocations[i];
			vert.x[0] = pt[0];
			vert.x[1] = pt[1];
			vert.x[2] = pt[2];
			if (mesh.vertexNormals.size() > 0) {
				float3 n = mesh.vertexNormals[i];
				vert.n[0] = n[0];
				vert.n[1] = n[1];
				vert.n[2] = n[2];
			}
			if (pointColors.size() > 0) {
				idx = 3 * i;
				vert.red = pointColors[idx];
				vert.green = pointColors[idx + 1];
				vert.blue = pointColors[idx + 2];
			}
			ply.putElement(&vert);
		}
		// set up and write the face elements
		plyFace face;
		plyFaceTexture faceT;
		int verts[256];
		float2 uvs[4];
		float velocity[3];
		face.verts = verts;
		faceT.verts = verts;
		faceT.velocity = velocity;
		faceT.uvs = (float*)uvs;
		ply.putElementSetup("face");
		if (hasTexture) {
			int sz = (int)(mesh.quadIndexes.size());
			for (int i = 0; i < sz; i++) {
				faceT.nverts = 4;
				faceT.uvcount = 8;
				for (j = 0; j < 4; j++) {
					faceT.verts[j] = mesh.quadIndexes[i][j];
					uvs[j] = mesh.textureMap[4 * i + j];
				}
				ply.putElement(&faceT);
			}
			sz = (int)(mesh.triIndexes.size());
			for (int i = 0; i < sz; i++) {
				faceT.nverts = 3;
				faceT.uvcount = 6;
				for (j = 0; j < 3; j++) {
					faceT.verts[j] = mesh.triIndexes[i][j];
					uvs[j] = mesh.textureMap[3 * i + j];
				}
				ply.putElement(&faceT);
			}
		}
		else {
			int sz = (int)(mesh.quadIndexes.size());
			for (int i = 0; i < sz; i++) {
				for (j = 0; j < 4; j++) {
					face.nverts = 4;
					face.verts[j] = mesh.quadIndexes[i][j];
				}
				ply.putElement(&face);
			}
			sz = (int)(mesh.triIndexes.size());
			for (int i = 0; i < sz; i++) {
				for (j = 0; j < 3; j++) {
					face.nverts = 3;
					face.verts[j] = mesh.triIndexes[i][j];
				}
				ply.putElement(&face);
			}
		}
	}
	void Mesh::updateVertexNormals(int SMOOTH_ITERATIONS, float DOT_TOLERANCE) {
		uint32_t sz = (uint32_t)triIndexes.size();
		float3 pt;
		vertexNormals.clear();
		vertexNormals.resize(vertexLocations.size(), float3(0.0f));
		for (uint32_t i = 0; i < sz; i++) {
			uint3 verts = triIndexes[i];
			float3 v1 = vertexLocations[verts.x];
			float3 v2 = vertexLocations[verts.y];
			float3 v3 = vertexLocations[verts.z];
			float3 norm = cross((v3 - v1), (v2 - v1));
			vertexNormals[verts.x] += norm;
			vertexNormals[verts.y] += norm;
			vertexNormals[verts.z] += norm;
		}
		sz = (uint32_t)quadIndexes.size();
		for (uint32_t i = 0; i < sz; i++) {
			uint4 verts = quadIndexes[i];
			float3 v1 = vertexLocations[verts.x];
			float3 v2 = vertexLocations[verts.y];
			float3 v3 = vertexLocations[verts.z];
			float3 v4 = vertexLocations[verts.w];

			vertexNormals[verts.x] += cross((v4 - v1), (v2 - v1));
			vertexNormals[verts.y] += cross((v1 - v2), (v3 - v2));
			vertexNormals[verts.z] += cross((v2 - v3), (v4 - v3));
			vertexNormals[verts.w] += cross((v3 - v4), (v1 - v4));
		}
#pragma omp parallel for
		for (int n = 0; n < (int)vertexNormals.size(); n++) {
			vertexNormals[n] = normalize(vertexNormals[n]);
		}
		if (SMOOTH_ITERATIONS > 0) {
			int vertCount = (int)vertexLocations.size();
			std::vector<float3> tmp(vertCount);
			std::vector<std::list<int>> vertNbrs(vertCount);
			int indexCount = (int)quadIndexes.size();

			for (int i = 0; i < indexCount; i++) {
				uint4 verts = quadIndexes[i];
				int v1 = verts.x;
				int v2 = verts.y;
				int v3 = verts.z;
				int v4 = verts.w;

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
					float3 norm = vertexNormals[i];
					float3 avg = float3(0.0f);
					for (int nbr : vertNbrs[i]) {
						float3 nnorm = vertexNormals[nbr];
						;
						if (dot(norm, nnorm) > DOT_TOLERANCE) {
							avg += nnorm;
						}
						else {
							avg += norm;
						}
					}
					tmp[i] = normalize(avg);
				}
				vertexNormals = tmp;
			}
		}
		dirty = true;
	}
	float Mesh::estimateVoxelSize(int stride) {
		int count = 0;
		//float maxLength = 0.0f;
		int sz = (int)triIndexes.size();
		float mEstimatedVoxelSize = 0.0f;
		for (int i = 0; i < sz; i += stride) {
			uint3 verts = triIndexes[i];
			float3 v1 = vertexLocations[verts.x];
			float3 v2 = vertexLocations[verts.y];
			float3 v3 = vertexLocations[verts.z];
			float e1 = length(v1 - v2);
			float e2 = length(v1 - v3);
			float e3 = length(v2 - v3);
			//maxLength = std::max(std::max(e1, e2), std::max(maxLength, e3));
			mEstimatedVoxelSize += e1 + e2 + e3;
		}
		count = sz / stride;
		sz = (int)(quadIndexes.size());
		for (int i = 0; i < sz; i += stride) {
			uint4 verts = quadIndexes[i];
			float3 v1 = vertexLocations[verts.x];
			float3 v2 = vertexLocations[verts.y];
			float3 v3 = vertexLocations[verts.z];
			float3 v4 = vertexLocations[verts.w];
			float e1 = length(v1 - v2);
			float e2 = length(v2 - v3);
			float e3 = length(v3 - v4);
			float e4 = length(v4 - v1);
			//maxLength = std::max(maxLength,std::max(std::max(e1, e2), std::max(e3, e4)));
			mEstimatedVoxelSize += e1 + e2 + e3 + e4;
		}
		count += sz / stride;
		mEstimatedVoxelSize /= count;
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
		int SZ = (int)vertexLocations.size();
		int batchSize = (SZ % BATCHES == 0) ? SZ / BATCHES : SZ / BATCHES + 1;
#pragma omp for
		for (int b = 0; b < BATCHES; b++) {
			int sz = std::min(SZ, batchSize * (b + 1));
			for (uint32_t idx = b * batchSize; idx < (uint32_t)sz; idx++) {
				float3& pt = vertexLocations[idx];
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
		boundingBox = box3f(minPt, maxPt - minPt);
		return boundingBox;
	}
	void Mesh::scale(float sc) {
#pragma omp parallel for
		for (int i = 0; i < (int)vertexLocations.size(); i++) {
			vertexLocations[i] *= sc;
		}
		boundingBox.dimensions = sc * boundingBox.dimensions;
		boundingBox.position = sc * boundingBox.position;
		dirty = true;
	}
	void Mesh::transform(const float4x4& M) {
#pragma omp parallel for
		for (int i = 0; i < (int)vertexLocations.size(); i++) {
			float4 pt = M * vertexLocations[i].xyzw();
			vertexLocations[i] = pt.xyz() / pt.w;
		}

		if (vertexNormals.size() > 0) {
			float3x3 NM = transpose(inverse(SubMatrix(M)));
#pragma omp parallel for
			for (int i = 0; i < (int)vertexLocations.size(); i++) {
				vertexNormals[i] = normalize(NM * vertexNormals[i]);
			}
		}
		updateBoundingBox();
		dirty = true;
	}
	void Mesh::mapIntoBoundingBox(float voxelSize) {
		float3 minPt = boundingBox.min();
#pragma omp parallel for
		for (int i = 0; i < (int)vertexLocations.size(); i++) {
			float3& pt = vertexLocations[i];
			pt = (pt - minPt) / voxelSize;
		}
		dirty = true;
	}
	void Mesh::mapOutOfBoundingBox(float voxelSize) {
		float3 minPt = boundingBox.min();
#pragma omp for
		for (int i = 0; i < (int)vertexLocations.size(); i++) {
			float3& pt = vertexLocations[i];
			pt = pt * voxelSize + minPt;
		}
		dirty = true;
	}

	void Mesh::update() {
		gl.update();
	}
	Mesh::~Mesh() {
		// TODO Auto-generated destructor stub
	}

	void ReadMeshFromFile(const std::string& file, Mesh &mesh) {
		int i, j;
		int numPts = 0, numPolys = 0;
		PLYReaderWriter ply;
		ply.openForReading(file);
		// Check to make sure that we can read geometry
		PlyElement *elem;
		int index;
		if ((elem = ply.findElement("vertex")) == nullptr
			|| ply.findProperty(elem, "x", &index) == nullptr
			|| ply.findProperty(elem, "y", &index) == nullptr
			|| ply.findProperty(elem, "z", &index) == nullptr
			|| (elem = ply.findElement("face")) == nullptr
			|| ply.findProperty(elem, "vertex_indices", &index) == nullptr) {
			throw std::runtime_error(
				MakeString() << "Could not read geometry [" << file << "]");
		}

		// Check for optional attribute data. We can handle intensity; and the
		// triplet red, green, blue.
		bool RGBPointsAvailable = false;
		bool hasNormals = false;

		mesh.triIndexes.clear();
		mesh.quadIndexes.clear();
		mesh.vertexLocations.clear();
		mesh.vertexNormals.clear();
		mesh.vertexColors.clear();
		mesh.textureMap.clear();
		mesh.textureImage.clear();
		if ((elem = ply.findElement("vertex")) != nullptr
			&& ply.findProperty(elem, "red", &index) != nullptr
			&& ply.findProperty(elem, "green", &index) != nullptr
			&& ply.findProperty(elem, "blue", &index) != nullptr) {
			RGBPointsAvailable = true;
		}
		if ((elem = ply.findElement("vertex")) != nullptr
			&& ply.findProperty(elem, "nx", &index) != nullptr
			&& ply.findProperty(elem, "ny", &index) != nullptr
			&& ply.findProperty(elem, "nz", &index) != nullptr) {
			hasNormals = true;
		}
		bool hasTexture = false;

		if ((elem = ply.findElement("face")) != nullptr &&
			ply.findProperty(elem, "texcoord", &index) != nullptr)
		{
			hasTexture = true;
			std::string textureFile;
			for (string comment : ply.getComments())
			{
				const string keyName("TextureFile");
				int offset = (int)comment.find(keyName, 0);
				if (offset >= 0)
				{
					textureFile = comment.substr(offset + keyName.size() + 1);
					break;
				}

			}
			std::string texturePath = RemoveTrailingSlash(GetParentDirectory(file)) + ALY_PATH_SEPARATOR + GetFileName(textureFile);
			if (textureFile.size() > 0 && FileExists(texturePath)) {
				ReadImageFromFile(texturePath, mesh.textureImage);
			}
		}

		int verts[256];
		float velocity[3];
		float uvs[8];

		plyVertex vertex;
		plyFace face;
		face.verts = verts;
		face.velocity = velocity;

		plyFaceTexture faceTex;
		faceTex.uvs = uvs;
		faceTex.verts = verts;
		faceTex.velocity = velocity;

		memset(verts, 0, sizeof(verts));
		std::vector<std::string> elist = ply.getElementNames();
		std::string elemName;
		int numElems, nprops;
		// Okay, now we can grab the data
		for (i = 0; i < ply.getNumberOfElements(); i++) {
			//get the description of the first element */
			elemName = elist[i];
			ply.getElementDescription(elemName, &numElems, &nprops);
			// if we're on vertex elements, read them in
			if (elemName == "vertex") {
				// Create a list of points
				numPts = numElems;
				mesh.vertexLocations.resize(numPts, float3(0.0f));
				// Setup to read the PLY elements
				ply.getProperty(elemName, &MeshVertProps[0]);
				ply.getProperty(elemName, &MeshVertProps[1]);
				ply.getProperty(elemName, &MeshVertProps[2]);
				if (hasNormals) {
					mesh.vertexNormals.resize(numPts);
					ply.getProperty(elemName, &MeshVertProps[3]);
					ply.getProperty(elemName, &MeshVertProps[4]);
					ply.getProperty(elemName, &MeshVertProps[5]);
				}
				if (RGBPointsAvailable) {
					mesh.vertexColors.resize(numPts);
					ply.getProperty(elemName, &MeshVertProps[9]);
					ply.getProperty(elemName, &MeshVertProps[10]);
					ply.getProperty(elemName, &MeshVertProps[11]);
				}
				for (j = 0; j < numPts; j++) {
					ply.getElement(&vertex);
					mesh.vertexLocations[j] = float3(vertex.x[0], vertex.x[1],
						vertex.x[2]);

					if (RGBPointsAvailable) {
						mesh.vertexColors[j] = float4(vertex.red / 255.0f,
							vertex.green / 255.0f, vertex.blue / 255.0f, 1.0f);
					}
					if (hasNormals) {
						mesh.vertexNormals[j] = float3(vertex.n[0], vertex.n[1],
							vertex.n[2]);
					}
				}
			}			//if vertex
			else if (elemName == "face") {
				// Create a polygonal array
				numPolys = numElems;
				// Get the face properties
				if (hasTexture) {
					ply.getProperty(elemName, &MeshFaceProps[2]);
					ply.getProperty(elemName, &MeshFaceProps[3]);
					for (j = 0; j < numPolys; j++) {
						ply.getElement(&faceTex);
						if (faceTex.nverts == 4) {
							mesh.quadIndexes.append(
								uint4(faceTex.verts[0], faceTex.verts[1], faceTex.verts[2],
									faceTex.verts[3]));
						}
						else if (faceTex.nverts == 3) {
							mesh.triIndexes.append(
								uint3(faceTex.verts[0], faceTex.verts[1], faceTex.verts[2]));
						}
						for (int i = 0;i < faceTex.nverts;i++) {
							mesh.textureMap.append(float2(faceTex.uvs[2 * i], faceTex.uvs[2 * i + 1]));
						}
					}
				}
				else {
					ply.getProperty(elemName, &MeshFaceProps[0]);
					for (j = 0; j < numPolys; j++) {
						ply.getElement(&face);
						if (face.nverts == 4) {
							mesh.quadIndexes.append(
								uint4(face.verts[0], face.verts[1], face.verts[2],
									face.verts[3]));
						}
						else if (face.nverts == 3) {
							mesh.triIndexes.append(
								uint3(face.verts[0], face.verts[1], face.verts[2]));
						}
					}
				}
			}							//if face
		} //for all elements of the PLY file
		if (mesh.vertexLocations.size() > 0) {
			mesh.updateBoundingBox();
		}
		mesh.setDirty(true);
	}

	void CreateVertexNeighborTable(const Mesh& mesh,
		std::vector<std::list<uint32_t>>& vertNbrs, bool addDuplicates) {
		vertNbrs.resize(mesh.vertexLocations.size());
		for (const uint3& face : mesh.triIndexes.data) {
			vertNbrs[face.x].push_back(face.y);
			vertNbrs[face.y].push_back(face.z);
			vertNbrs[face.z].push_back(face.x);
			if (addDuplicates) {
				vertNbrs[face.z].push_back(face.y);
				vertNbrs[face.y].push_back(face.x);
				vertNbrs[face.x].push_back(face.z);
			}
		}
		for (const uint4& face : mesh.quadIndexes.data) {
			vertNbrs[face.x].push_back(face.y);
			vertNbrs[face.y].push_back(face.z);
			vertNbrs[face.z].push_back(face.w);
			vertNbrs[face.w].push_back(face.x);
			if (addDuplicates) {
				vertNbrs[face.w].push_back(face.z);
				vertNbrs[face.z].push_back(face.y);
				vertNbrs[face.y].push_back(face.x);
				vertNbrs[face.x].push_back(face.w);
			}
		}
	}
	inline uint64_t faceHashCode(const uint2& val) {
		return ((uint64_t)val.y) << 32 | ((uint64_t)val.x);
	}
	void CreateFaceNeighborTable(const Mesh& mesh,
		std::vector<std::list<uint32_t>>& faceNbrs) {
		faceNbrs.resize(mesh.triIndexes.size() + mesh.quadIndexes.size());
		std::map<uint64_t, std::list<uint>> edgeTable;
		uint2 edge;
		uint fid = 0;
		for (const uint3& face : mesh.triIndexes.data) {
			edge = (face.x < face.y) ?
				uint2(face.x, face.y) : uint2(face.y, face.x);
			edgeTable[faceHashCode(edge)].push_back(fid);
			edge = (face.y < face.z) ?
				uint2(face.y, face.z) : uint2(face.z, face.y);
			edgeTable[faceHashCode(edge)].push_back(fid);
			edge = (face.z < face.x) ?
				uint2(face.z, face.x) : uint2(face.x, face.z);
			edgeTable[faceHashCode(edge)].push_back(fid);
			fid++;
		}
		for (const uint4& face : mesh.quadIndexes.data) {
			edge = (face.x < face.y) ?
				uint2(face.x, face.y) : uint2(face.y, face.x);
			edgeTable[faceHashCode(edge)].push_back(fid);
			edge = (face.y < face.z) ?
				uint2(face.y, face.z) : uint2(face.z, face.y);
			edgeTable[faceHashCode(edge)].push_back(fid);
			edge = (face.z < face.w) ?
				uint2(face.z, face.w) : uint2(face.w, face.z);
			edgeTable[faceHashCode(edge)].push_back(fid);
			edge = (face.w < face.x) ?
				uint2(face.w, face.x) : uint2(face.x, face.w);
			edgeTable[faceHashCode(edge)].push_back(fid);
			fid++;
		}
		for (std::pair<const uint64_t, std::list<uint>>& edgeList : edgeTable) {
			if (edgeList.second.size() == 2) {
				uint fid1 = edgeList.second.front();
				uint fid2 = edgeList.second.back();
				if (fid1 != fid2) {
					faceNbrs[fid1].push_back(fid2);
					faceNbrs[fid2].push_back(fid1);
				}
			}
		}
	}

} /* namespace imagesci */
