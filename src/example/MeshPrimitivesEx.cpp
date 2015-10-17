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
#include "../../include/example/MeshPrimitivesEx.h"
using namespace aly;
MeshPrimitivesEx::MeshPrimitivesEx() :
		Application(800, 600, "Mesh Primitives Example"), phongShader(1), imageShader(
				ImageShader::Filter::MEDIUM_BLUR) {
}
bool MeshPrimitivesEx::init(Composite& rootNode) {
	box3f renderBBox = box3f(float3(-0.5f, -0.5f, -0.5f),
			float3(1.0f, 1.0f, 1.0f));
	phongShader[0] = SimpleLight(Color(0.0f, 0.0f, 0.1f, 0.25f),
		Color(0.0f, 0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.8f, 0.5f),
		Color(0.0f, 0.0f, 0.0f, 0.0f), 16.0f, float3(0, 0.0, 2.0),
		float3(0, 1, 0));
	phongShader[0].moveWithCamera = false;
	mesh.load(getFullPath("models/monkey.obj"));
	mesh.updateVertexNormals();
	//Initialize depth buffer to store the render
	int w = getContext()->width();
	int h = getContext()->height();
	depthFrameBuffer.initialize(w, h);
	wireframeFrameBuffer.initialize(w, h);
	//Set up camera
	camera.setNearFarPlanes(-5.0f, 5.0f);
	camera.setZoom(1.0f);
	camera.setCameraType(CameraType::Orthographic);
	//Map object geometry into unit bounding box for draw.
	camera.setPose(MakeTransform(mesh.getBoundingBox(), renderBBox));
	//Add listener to respond to mouse manipulations
	addListener(&camera);
	SelectionPtr selection = SelectionPtr(new Selection("Primitive", CoordPX(5, 5), CoordPX(200, 30),
		std::vector<std::string>{"Cube", "Icosahedron", "Sphere","Plane","Capsule", "Cylinder","Pyramid","Cone","Frustum","Sphere (tri)","Sphere (quad)"}));
	selection->onSelect = [this](int index) {
	};
	selection->setSelectionIndex(3);
	setOnResize([=](const int2& dims) {
		camera.setDirty(true);
	});
	wireframeShader.setFaceColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	wireframeShader.setEdgeColor(Color(1.0f, 0.8f, 0.1f, 1.0f));
	wireframeShader.setLineWidth(1.5f);
	wireframeShader.setSolid(false);
	setOnResize([this](const int2& dims) {
		if(!getContext()->hasDeferredTasks()) {
			getContext()->addDeferredTask([this]() {
						int w=getContext()->width();
						int h=getContext()->height();
						depthFrameBuffer.initialize(w,h);
						wireframeFrameBuffer.initialize(w,h);
						camera.setDirty(true);
					});
		}
	});
	rootNode.add(selection);

	return true;
}
void MeshPrimitivesEx::draw(AlloyContext* context) {
	if (camera.isDirty()) {
		depthAndNormalShader.draw(mesh, camera, depthFrameBuffer,true);
		wireframeFrameBuffer.begin();
		phongShader.draw(depthFrameBuffer.getTexture(), camera, context->getViewport(), context->getViewport());
		wireframeShader.draw(mesh, camera, context->getViewport());
		wireframeFrameBuffer.end();
	}
	imageShader.draw(wireframeFrameBuffer.getTexture(), context->getViewport(),1.0f, false);
	camera.setDirty(false);
}

