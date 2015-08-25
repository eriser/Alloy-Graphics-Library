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
#ifndef ALLOYMESH_H_
#define ALLOYMESH_H_

#include <mutex>
#include "GLComponent.h"
#include "AlloyMath.h"
#include "AlloyVector.h"
#include "AlloyImage.h"
#include "AlloyContext.h"
#include <vector>
namespace aly {
struct Mesh;
struct GLMesh: public GLComponent {
public:
	enum class PrimitiveType {
		ALL = 0, QUADS = 4, TRIANGLES = 3, POINTS = 1
	};
	Mesh& mesh;
	GLuint vao;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint colorBuffer;
	GLuint triIndexBuffer;
	GLuint quadIndexBuffer;
	GLuint triVertexBuffer[3];
	GLuint quadVertexBuffer[4];
	GLuint triNormalBuffer[3];
	GLuint quadNormalBuffer[4];
	GLuint triTextureBuffer[3];
	GLuint quadTextureBuffer[4];
	GLuint triCount;
	GLuint quadCount;
	GLuint vertexCount;
	GLuint triIndexCount;
	GLuint quadIndexCount;

	virtual void draw() const override;
	virtual void draw(const PrimitiveType& type) const;
	virtual void update() override;
	GLMesh(Mesh& mesh, std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	virtual ~GLMesh();
};
struct Mesh {
private:
	box3f boundingBox;
	bool dirty = false;
public:
	friend struct GLMesh;

	Vector3f vertexLocations;
	Vector3f vertexNormals;
	Vector4f vertexColors;

	Vector4ui quadIndexes;
	Vector3ui triIndexes;

	Vector2f textureMap;
	Image4f textureImage;

	GLMesh gl;
	float4x4 pose;

	template<class Archive> void serialize(Archive & archive) {
		archive(CEREAL_NVP(pose), CEREAL_NVP(vertexLocations),
				CEREAL_NVP(vertexNormals), CEREAL_NVP(vertexColors),
				CEREAL_NVP(quadIndexes), CEREAL_NVP(triIndexes),
				CEREAL_NVP(textureMap), CEREAL_NVP(textureImage));
	}

	Mesh(std::shared_ptr<AlloyContext>& context = AlloyDefaultContext());
	inline box3f getBoundingBox() const {
		return boundingBox;
	}
	virtual void draw(const GLMesh::PrimitiveType& type) const;
	box3f updateBoundingBox();
	void scale(float sc);
	void transform(const float4x4& M);
	float estimateVoxelSize(int stride = 1);
	void update();
	void clear();
	void setDirty(bool d) {
		this->dirty = d;
	}
	inline bool isDirty() const {
		return dirty;
	}
	bool load(const std::string& file);
	void updateVertexNormals(int SMOOTH_ITERATIONS = 0, float DOT_TOLERANCE =
			0.75f);
	void mapIntoBoundingBox(float voxelSize);
	void mapOutOfBoundingBox(float voxelSize);
	bool save(const std::string& file);
	virtual ~Mesh();
};
void ReadMeshFromFile(const std::string& file, Mesh& mesh);
void WriteMeshToFile(const std::string& file, const Mesh& mesh,bool binary=true);

typedef std::vector<std::list<uint32_t>> MeshNeighborTable;
void CreateVertexNeighborTable(const Mesh& mesh, MeshNeighborTable& vertNbrs,
		bool addDuplicates = false);
void CreateFaceNeighborTable(const Mesh& mesh, MeshNeighborTable& faceNbrs);
}
#endif /* MESH_H_ */
