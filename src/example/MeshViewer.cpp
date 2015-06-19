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
		Application(1280, 720, "Mesh Viewer") {
}
bool MeshViewer::init(Composite& rootNode) {
	mesh.openMesh(getFullPath("models/armadillo.ply"));
	box3f bbox=mesh.getBoundingBox();
	box3f renderBBox=box3f(float3(-0.5f,-0.5f,-0.5f),float3(1.0f,1.0f,1.0f));
    camera.setSpeed(0.1, 0.002, 0.2);
    camera.setNearFarPlanes(0.01f,10.0f);
    camera.lookAt(float3(0,0,0),1.0);
	camera.setPose(MakeTransform(bbox,renderBBox));
	ImageRGBA bgImage;
	ReadImageFromFile(getFullPath("images/sfsunset.png"),bgImage);
	bgTexture=std::unique_ptr<GLTextureRGBA>(new GLTextureRGBA(bgImage,getContext()));
	ImageRGBA matcapImage;
	ReadImageFromFile(getFullPath("images/JG_Gold.png"),matcapImage);
	matcapTexture=std::unique_ptr<GLTextureRGBA>(new GLTextureRGBA(matcapImage,getContext()));

	matcapShader.initialize(std::vector<std::string>{"vp","vn"},
			ReadTextFile(getFullPath("shaders/matcap.vert")),
			ReadTextFile(getFullPath("shaders/matcap.frag")));

	 imageShader.initialize(std::vector<std::string>{"vp","vn"},
	 R"(
	 #version 330
	 in vec3 vp; 
	 uniform vec2 imgPosition;
	 uniform vec2 imgDimensions;
	 uniform vec2 viewDimensions;
	 out vec3 pos3d;
	 void main() {
	 pos3d=vp;
	 vec2 pos=(vp.xy*imgDimensions+imgPosition);
	 pos.x=2*pos.x/viewDimensions.x-1.0;
	 pos.y=1.0-2*pos.y/viewDimensions.y;
	 gl_Position = vec4(pos.x,pos.y,0,1);
})",
	 R"(
	 #version 330
	 in vec3 pos3d;
	 uniform sampler2D textureImage;
	 uniform vec2 imgDimensions;
	 void main() {
	 vec4 rgba=texture2D(textureImage,pos3d.xy);
	 gl_FragColor=rgba;
	 })");
	bgTexture->update();
	matcapTexture->update();
	mesh.updateVertexNormals();
	mesh.updateGL();
	addListener(&camera);
	return true;
}
void MeshViewer::draw(const aly::DrawEvent3D& event) {
	matcapShader.begin();
	camera.aim(getContext()->viewport,matcapShader);
	imageShader.set("matcapTexture",*matcapTexture,0);

	mesh.draw();
	matcapShader.end();
}
void MeshViewer::draw(const aly::DrawEvent2D& event) {
	imageShader.begin();
	imageShader.set("textureImage",*bgTexture,0);
	imageShader.set("imgPosition",float2(50,50));
	imageShader.set("imgDimensions",float2(300,200));
	imageShader.set("viewDimensions",float2(getContext()->viewport.dimensions));
	bgTexture->draw();
	imageShader.end();
}
