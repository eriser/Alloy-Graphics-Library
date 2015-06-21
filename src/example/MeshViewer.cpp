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
		Application(1280, 720, "Mesh Viewer"), mesh(getContext()), exampleImage(
				getContext()) {
}
bool MeshViewer::init(Composite& rootNode) {
	mesh.load(getFullPath("models/armadillo.ply"));
	box3f renderBBox = box3f(float3(-0.5f, -0.5f, -0.5f),
			float3(1.0f, 1.0f, 1.0f));
	camera.setPose(MakeTransform(mesh.getBoundingBox(), renderBBox));
	matcapShader = std::shared_ptr<MatcapShader>(
			new MatcapShader(getFullPath("images/JG_Gold.png"), getContext()));
	exampleImage.load(getFullPath("images/sfsunset.png"), true);

	imageShader = std::shared_ptr<ImageShader>(new ImageShader(getContext()));
	mesh.updateVertexNormals();
	mesh.transform(MakeRotationY((float) M_PI));
	addListener(&camera);
	return true;
}
void MeshViewer::draw(const aly::DrawEvent3D& event) {
	matcapShader->draw(mesh, camera);
}
void MeshViewer::draw(const aly::DrawEvent2D& event) {
	imageShader->draw(exampleImage, float2(30.0f, 30.0f),
			float2(300.0f, 200.0f));
}
