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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "AlloyMath.h"
#include "AlloyVector.h"
#include "AlloyImage.h"
#include <vector>
namespace aly {
struct GLMesh{
public:
	GLuint mVao;
	GLuint mVertexBuffer;
	GLuint mNormalBuffer;
	GLuint mColorBuffer;
	GLuint mTriIndexBuffer;
	GLuint mQuadIndexBuffer;
	GLuint mTriangleCount;
	GLuint mQuadCount;
	GLuint mTriangleIndexCount;
	GLuint mQuadIndexCount;

	GLMesh():
		mVao(0),
		mVertexBuffer(0),
		mNormalBuffer(0),
		mColorBuffer(0),
		mTriIndexBuffer(0),
		mQuadIndexBuffer(0),
		mTriangleCount(0),
		mQuadCount(0),
		mTriangleIndexCount(0),
		mQuadIndexCount(0){

	}
};
class Mesh{
	private:
	GLMesh glMesh;
	float4x4 mPose;
	box3f mBoundingBox;
	public:
		enum PrimitiveType {QUADS=4,TRIANGLES=3};


		Vector3f mVertexes;
		Vector3f mVertexNormals;
		Vector4f mVertexColors;

		Vector4ui mQuadIndexes;
		Vector3ui mTriIndexes;

		Vector2f uvMap;
		Image4f textureImage;
		Mesh();
		inline box3f getBoundingBox() const {return mBoundingBox;}
		box3f updateBoundingBox();
		void draw();
		void scale(float sc);
		float estimateVoxelSize(int stride=1);
		inline void setPose(float4x4& pose){
			mPose=pose;
		}
		inline float4x4& getPose(){
			return mPose;
		}
		void updateGL();
		void reset();
		void updateVertexNormals(int SMOOTH_ITERATIONS=0,float DOT_TOLERANCE=0.75f);
		void mapIntoBoundingBox(float voxelSize);
		void mapOutOfBoundingBox(float voxelSize);
		bool openMesh(const std::string& file);
		bool save(const std::string& file);
		~Mesh();
};
}
#endif /* MESH_H_ */
