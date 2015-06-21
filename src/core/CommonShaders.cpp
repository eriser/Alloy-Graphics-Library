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

#include "CommonShaders.h"
#include "AlloyMesh.h"
#include "AlloyVirtualCamera.h"
namespace aly {
MatcapShader::MatcapShader(const std::string& textureImage,
		std::shared_ptr<AlloyContext> context) :
		GLShader(context), matcapTexture(context) {
	matcapTexture.load(textureImage);
	initialize(std::vector<std::string> { "vp", "vn" },
			R"(#version 330
					layout(location = 0) in vec3 vp; // positions from mesh
					layout(location = 1) in vec3 vn; // normals from mesh
					uniform mat4 ProjMat, ViewMat, ModelMat,ViewModelMat,NormalMat; // proj, view, model matrices
					out vec3 normal;
					void main () {
					  normal = vec3 (NormalMat * vec4 (vn, 0.0));
					  gl_Position = ProjMat * ViewModelMat * vec4 (vp, 1.0);
					})",
			R"(#version 330
					in vec3 normal;
					uniform sampler2D matcapTexture;
					void main() {
					   vec3 normalized_normal = normalize(normal);
					   vec4 c=texture2D(matcapTexture,0.5f*normalized_normal.xy+0.5f);
					   c.w=1.0;
					   gl_FragColor=c;
					 }
					)");
}
void MatcapShader::draw(const Mesh& mesh, VirtualCamera& camera,
		const box2px& bounds) {
	begin().set(camera, bounds).set("matcapTexture", matcapTexture, 0).draw(
			mesh.gl).end();
}
void MatcapShader::draw(const Mesh& mesh, VirtualCamera& camera) {
	draw(mesh, camera, getContext()->getViewport());
}
ImageShader::ImageShader(std::shared_ptr<AlloyContext> context) :
		GLShader(context) {

	initialize(std::vector<std::string> { "vp", "vt" },
			R"(
		 #version 330
		 layout(location = 0) in vec3 vp; 
layout(location = 1) in vec2 vt; 
		 uniform vec4 bounds;
		 uniform ivec4 viewport;
uniform int flip;
out vec2 uv;
		 void main() {
if(flip!=0)uv=vec2(vt.x,1.0-vt.y); else uv=vt;
		 vec2 pos=vp.xy*bounds.zw+bounds.xy;
		 gl_Position = vec4(2*pos.x/viewport.z-1.0,1.0-2*pos.y/viewport.w,0,1);
	})",
			R"(
		 #version 330
in vec2 uv;
		 uniform sampler2D textureImage;

		 void main() {
		 vec4 rgba=texture2D(textureImage,uv);
		 gl_FragColor=rgba;
		 })");
}

}

