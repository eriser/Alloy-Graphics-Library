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
#ifndef ALLOYMESHPRIMITIVE_H_
#define ALLOYMESHPRIMITIVE_H_
#include "AlloyMesh.h"
#include "AlloyMath.h"
namespace aly {
	class Box: public Mesh {
	public:
		Box(const box3f& box,std::shared_ptr<AlloyContext>& context = AlloyDefaultContext());
	};
	class Icosahedron : public Mesh {
	public:
		Icosahedron(float radius,std::shared_ptr<AlloyContext>& context = AlloyDefaultContext());
	};
	class Sphere : public Mesh {
	public:
		Sphere(float r, int slices, int stacks, std::shared_ptr<AlloyContext>& context = AlloyDefaultContext());
	};
}
#endif /* MESH_H_ */
