/*
 * Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
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
#include "AlloyMeshPrimitive.h"
namespace aly {
	Box::Box(const box3f& box, std::shared_ptr<AlloyContext>& context) :Mesh(context) {
		float3 minPt = box.min();
		float3 maxPt = box.max();
		vertexLocations.push_back(float3(minPt.x, minPt.y, minPt.z));
		vertexLocations.push_back(float3(maxPt.x, minPt.y, minPt.z));
		vertexLocations.push_back(float3(maxPt.x, maxPt.y, minPt.z));
		vertexLocations.push_back(float3(minPt.x, maxPt.y, minPt.z));
		vertexLocations.push_back(float3(minPt.x, minPt.y, maxPt.z));
		vertexLocations.push_back(float3(maxPt.x, minPt.y, maxPt.z));
		vertexLocations.push_back(float3(maxPt.x, maxPt.y, maxPt.z));
		vertexLocations.push_back(float3(minPt.x, maxPt.y, maxPt.z));


		quadIndexes.push_back(uint4(3, 2, 1, 0));
		quadIndexes.push_back(uint4(6, 7, 4, 5));
		quadIndexes.push_back(uint4(5, 1, 2, 6));
		quadIndexes.push_back(uint4(0, 4, 7, 3));
		quadIndexes.push_back(uint4(6, 2, 3, 7));
		quadIndexes.push_back(uint4(4, 0, 1, 5));

		updateVertexNormals();
		boundingBox = box;
		setDirty(true);
	}
	Sphere::Sphere(float radius,int slices,int stacks,std::shared_ptr<AlloyContext>& context) :Mesh(context) {
		std::vector<std::vector<uint32_t>> rings(stacks - 1, std::vector<uint32_t>(slices));
		vertexLocations.push_back(float3(0.0f, 0.0f, radius));
		vertexNormals.push_back(float3(0.0f, 0.0f, 1.0f));
		uint32_t idx = 1;
		for (int i = 1; i<stacks; i++)
		{
			float cosi = std::cos(ALY_PI*i / (float)(stacks));
			float sini = std::sin(ALY_PI*i / (float)(stacks));
			for (int j = 0; j<slices; j++)
			{
				float cosj = std::cos(2 * ALY_PI*j / (float)slices);
				float sinj = std::sin(2 * ALY_PI*j / (float)slices);
				float x = cosj * sini;
				float y = sinj * sini;
				float z = cosi;
				rings[i-1][j]=idx;
				vertexLocations.push_back(radius*float3(x, y, z));
				vertexNormals.push_back(float3(x,y,z));
				idx++;
			}
		}
		vertexLocations.push_back(float3(0.0f, 0.0f, -radius));
		vertexNormals.push_back(float3(0.0f, 0.0f, -1.0f));
		uint32_t v0, v1, v2, v3;
		for (int j = 0; j < slices; j++)
		{
			v0 = 0;
			v2 = rings[0][(j + 1) % slices];
			v1 = rings[0][j];
			triIndexes.push_back(uint3(v0, v1, v2));
			v0 =idx;
			v2 = rings[stacks - 2][(j + 1) % slices];
			v1 = rings[stacks - 2][j];
			triIndexes.push_back(uint3(v1, v0, v2));
			
			for (int i = 0;i < stacks-2;i++) {
				v0 = rings[i][j];
				v1 = rings[i][(j + 1) % slices];
				v2 = rings[i+1][j];
				v3 = rings[i+1][(j + 1) % slices];
				quadIndexes.push_back(uint4(v0, v2, v3,v1));
			}
		}
		boundingBox = box3f(float3(-radius, -radius, -radius), float3(2 * radius, 2 * radius, 2*radius));
		setDirty(true);
	}
	Icosahedron::Icosahedron(float radius,std::shared_ptr<AlloyContext>& context) :Mesh(context) {
		vertexLocations.push_back(float3(0.0f, 0.0f, -1.0f));
		vertexLocations.push_back(float3(0.7236f, -0.52572f, -0.447215f));
		vertexLocations.push_back(float3(-0.276385f, -0.85064f, -0.447215f));
		vertexLocations.push_back(float3(0.7236f, 0.52572f, -0.447215f));
		vertexLocations.push_back(float3(-0.894425f, 0.0f, -0.447215f));
		vertexLocations.push_back(float3(-0.276385f, 0.85064f, -0.447215f));
		vertexLocations.push_back(float3(-0.7236f, -0.52572f, 0.447215f));
		vertexLocations.push_back(float3(0.894425f, 0.0f, 0.447215f));
		vertexLocations.push_back(float3(0.276385f, -0.85064f, 0.447215f));
		vertexLocations.push_back(float3(-0.7236f, 0.52572f, 0.447215f));
		vertexLocations.push_back(float3(0.276385f, 0.85064f, 0.447215f));
		vertexLocations.push_back(float3(0.0f, 0.0f, 1.0f));
		vertexLocations *= float3(radius);
		triIndexes.push_back(uint3(0, 1, 2));
		triIndexes.push_back(uint3(1, 0, 3));
		triIndexes.push_back(uint3(0, 2, 4));
		triIndexes.push_back(uint3(0, 4, 5));
		triIndexes.push_back(uint3(0, 5, 3));
		triIndexes.push_back(uint3(1, 3, 7));
		triIndexes.push_back(uint3(2, 1, 8));
		triIndexes.push_back(uint3(4, 2, 6));
		triIndexes.push_back(uint3(5, 4, 9));
		triIndexes.push_back(uint3(3, 5, 10));
		triIndexes.push_back(uint3(1, 7, 8));
		triIndexes.push_back(uint3(2, 8, 6));
		triIndexes.push_back(uint3(4, 6, 9));
		triIndexes.push_back(uint3(5, 9, 10));
		triIndexes.push_back(uint3(3, 10, 7));
		triIndexes.push_back(uint3(8, 7, 11));
		triIndexes.push_back(uint3(6, 8, 11));
		triIndexes.push_back(uint3(9, 6, 11));
		triIndexes.push_back(uint3(10, 9, 11));
		triIndexes.push_back(uint3(7, 10, 11));
		updateVertexNormals();

		boundingBox = box3f(float3(-radius, -radius, -radius), float3(2 * radius, 2 * radius, 2 * radius));
		setDirty(true);
	}
}