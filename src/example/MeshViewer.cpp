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

#include "Alloy.h"
#include "../../include/example/MeshViewer.h"
using namespace aly;
MeshViewer::MeshViewer() :
		Application(1280, 720, "Mesh Viewer"),bgTexture(getContext()),matcapTexture(getContext()) {
}
bool MeshViewer::init(Composite& rootNode) {
	mesh.openMesh(getFullPath("models/armadillo.ply"));
	box3f bbox=mesh.getBoundingBox();
	box3f renderBBox=box3f(float3(-0.5f,-0.5f,-0.5f),float3(1.0f,1.0f,1.0f));
	camera.setPose(MakeTransform(bbox,renderBBox)*MakeRotationY((float)M_PI));
	bgTexture.load(getFullPath("images/sfsunset.png"));
	matcapTexture.load(getFullPath("images/JG_Gold.png"));
	matcapShader.initialize(std::vector<std::string>{"vp","vn"},
			ReadTextFile(getFullPath("shaders/matcap.vert")),
			ReadTextFile(getFullPath("shaders/matcap.frag")));

	 imageShader.initialize(std::vector<std::string>{"vp","vn"},
	 R"(
	 #version 330
	 in vec3 vp; 
	 uniform vec4 bounds;
	 uniform ivec4 viewport;
	 out vec3 pos3d;
	 void main() {
	 pos3d=vp;
	 vec2 pos=vp.xy*bounds.zw+bounds.xy;
	 gl_Position = vec4(2*pos.x/viewport.z-1.0,1.0-2*pos.y/viewport.w,0,1);
})",
	 R"(
	 #version 330
	 in vec3 pos3d;
	 uniform sampler2D textureImage;
	 void main() {
	 vec4 rgba=texture2D(textureImage,pos3d.xy);
	 gl_FragColor=rgba;
	 })");
	mesh.updateVertexNormals();
	mesh.updateGL();
	addListener(&camera);
	return true;
}
void MeshViewer::draw(const aly::DrawEvent3D& event) {
	matcapShader
		.begin()
		.set(camera,getContext()->getViewport())
		.set("matcapTexture",matcapTexture,0);
	mesh.draw();
	matcapShader.end();
}
void MeshViewer::draw(const aly::DrawEvent2D& event) {
	imageShader
			.begin()
			.set("textureImage",bgTexture,0)
			.set("bounds",box2px(float2(30.0f,30.0f),float2(300.0f,200.0f)))
			.set("viewport",getContext()->viewport);
	bgTexture.draw();
	imageShader.end();
}
