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
	std::cout<<"Mesh "<<" "<<mesh.mTriIndexes.size()<<" "<<mesh.mVertexes.size()<<std::endl;
	box3f bbox=mesh.getBoundingBox();
	box3f renderBBox=box3f(float3(-0.5,-0.5,-0.5),float3(0.5,0.5,0.5));
    camera.setSpeed(0.1, 0.002, 0.2);
    camera.setNearFarPlanes(0.01f,1000.0f);
    camera.lookAt(float3(0,0,0),1.0);
	camera.setPose(MakeTransform(bbox,renderBBox));
	std::cout<<"Bounding Box "<<camera.getPose()<<std::endl;

	matcapShader.initialize(std::vector<std::string>{"vp","vn"},
			ReadTextFile(getFullPath("shaders/matcap.vert")),
			ReadTextFile(getFullPath("shaders/matcap.frag")));
	mesh.updateGL();
	addListener(&camera);
	return true;
}
void MeshViewer::draw(const aly::DrawEvent3D& event) {
	matcapShader.begin();
	camera.aim(getContext()->viewport,matcapShader);
	mesh.draw();
	matcapShader.end();
}

