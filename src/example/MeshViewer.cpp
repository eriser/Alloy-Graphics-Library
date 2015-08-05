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
		Application(1920, 960, "Mesh Viewer"), matcapShader(
				getFullPath("images/JG_Gold.png")), imageShader(getContext(),
				ImageShader::Filter::MEDIUM_BLUR), voxelSize(0.0f), phongShader(
				1), phongShader2(1) {
}
bool MeshViewer::init(Composite& rootNode) {
	mesh.load(getFullPath("models/monkey.ply"));
	mesh.scale(10.0f);
	mesh2.load(getFullPath("models/armadillo.ply"));
	mesh2.updateVertexNormals();
	float4x4 M=MakeTransform(mesh2.getBoundingBox(), mesh.getBoundingBox());
	mesh2.transform(M);
	//
	//mesh.transform(MakeRotationY((float)(0.2f*M_PI))*MakeRotationX((float)(-0.5f*M_PI)));
	box3f renderBBox = box3f(float3(-0.5f, -0.5f, -0.5f),
			float3(1.0f, 1.0f, 1.0f));

	camera.setNearFarPlanes(0.01f, 10.0f);
	camera.setZoom(0.5f);
	camera.setPose(MakeTransform(mesh.getBoundingBox(), renderBBox));

	voxelSize = mesh.estimateVoxelSize(2);
	phongShader[0] = SimpleLight(Color(0.5f, 0.5f, 0.5f, 0.25f),
			Color(1.0f, 1.0f, 1.0f, 0.25f), Color(0.8f, 0.0f, 0.0f, 0.5f),
			Color(0.8f, 0.0f, 0.0f, 0.5f), 16.0f, float3(0, 0.0, 2.0),
			float3(0, 1, 0));
	phongShader[0].moveWithCamera = false;

	phongShader2[0] = SimpleLight(Color(0.5f, 0.5f, 0.5f, 0.25f),
		Color(1.0f, 1.0f, 1.0f, 0.25f), Color(0.0f, 0.0f, 0.8f, 1.0f),
		Color(0.0f, 0.0f, 0.8f, 1.0f), 16.0f, float3(0, 0.0, 2.0),
		float3(0, 1, 0));
	phongShader2[0].moveWithCamera = false;

	exampleImage.load(getFullPath("images/sfsunset.png"), true);
	edgeFrameBuffer.initialize(480, 480);
	smoothDepthFrameBuffer1.initialize(480, 480);
	smoothDepthFrameBuffer2.initialize(480, 480);
	flatDepthFrameBuffer.initialize(480, 480);
	outlineFrameBuffer.initialize(480, 480);
	wireframeFrameBuffer.initialize(480, 480);
	occlusionFrameBuffer.initialize(480, 480);
	colorBuffer1.initialize(480, 480);
	colorBuffer2.initialize(480, 480);
	mesh.updateVertexNormals();
	addListener(&camera);

	camera.setDirty(true);
	return true;
}
void MeshViewer::draw(const aly::DrawEvent3D& event) {
	if (camera.isDirty()) {
		edgeDepthAndNormalShader.draw(mesh, camera, edgeFrameBuffer);
		depthAndNormalShader.draw(mesh, camera, flatDepthFrameBuffer, true);
		depthAndNormalShader.draw(mesh, camera, smoothDepthFrameBuffer1, false);
		depthAndNormalShader.draw(mesh2, camera, smoothDepthFrameBuffer2, false);
	}
}
void MeshViewer::draw(const aly::DrawEvent2D& event) {
	float2 dRange = camera.computeNormalizedDepthRange(mesh);
	depthColorShader.draw(edgeFrameBuffer.getTexture(),
			float2(0.0f, camera.getScale()), float2(0.0f, 0.0f),
			float2(480, 480));
	normalColorShader.draw(edgeFrameBuffer.getTexture(), float2(0.0f, 480.0f),
			float2(480, 480));

	depthColorShader.draw(flatDepthFrameBuffer.getTexture(), dRange,
			float2(960.0f, 0.0f), float2(480, 480));

	normalColorShader.draw(smoothDepthFrameBuffer1.getTexture(),
			float2(1440.0f, 480.0f), float2(480, 480));

	
	colorBuffer1.begin();
	phongShader.draw(smoothDepthFrameBuffer1.getTexture(),
		getContext()->getViewport(), camera,
		colorBuffer1.getViewport());
	colorBuffer1.end();

	colorBuffer2.begin();
	phongShader2.draw(smoothDepthFrameBuffer2.getTexture(),
		getContext()->getViewport(), camera,
		colorBuffer2.getViewport());
	colorBuffer2.end();

	
	static bool once = true;
	
	if(once){
	colorBuffer1.getTexture().read().writeToXML("color1.xml");
	colorBuffer2.getTexture().read().writeToXML("color2.xml");
	smoothDepthFrameBuffer1.getTexture().read().writeToXML("depth1.xml");
	smoothDepthFrameBuffer2.getTexture().read().writeToXML("depth2.xml");
	once=false;
	}
	
	compositeShader.draw(
		colorBuffer1.getTexture(), smoothDepthFrameBuffer1.getTexture(),
		colorBuffer2.getTexture(), smoothDepthFrameBuffer2.getTexture(), 
		float2(960.0f, 480.0f), float2(480, 480),0.5f,1.0f);

	if (camera.isDirty()) {
		outlineFrameBuffer.begin();
		outlineShader.draw(flatDepthFrameBuffer.getTexture(), float2(0.0f, 0.0f),
				float2(480, 480), outlineFrameBuffer.getViewport());
		outlineFrameBuffer.end();

		wireframeFrameBuffer.begin();
		wireframeShader.draw(edgeFrameBuffer.getTexture(),
				float2(0.0f, 0.25f * camera.getScale()), float2(0.0f, 0.0f),
				float2(480, 480), wireframeFrameBuffer.getViewport());
		wireframeFrameBuffer.end();

		occlusionFrameBuffer.begin();
		ambientOcclusionShader.draw(flatDepthFrameBuffer.getTexture(),
				float2(0.0f, 0.0f), float2(480, 480),
				occlusionFrameBuffer.getViewport(), camera);
		occlusionFrameBuffer.end();
	}
	imageShader.draw(outlineFrameBuffer.getTexture(), float2(480.0f, 0.0f),
			float2(480, 480));
	imageShader.draw(wireframeFrameBuffer.getTexture(), float2(480.0f, 480.0f),
			float2(480, 480));
	imageShader.draw(occlusionFrameBuffer.getTexture(), float2(1440.0f, 0.0f),
			float2(480, 480));
	
	/*
	static bool once = true;
	 if(once){
	 wireframeFrameBuffer.getTexture().read().writeToXML("/home/blake/tmp/wire.xml");
	 outlineFrameBuffer.getTexture().read().writeToXML("/home/blake/tmp/outline.xml");
	 depthFrameBuffer.getTexture().read().writeToXML("/home/blake/tmp/depth.xml");
	 occlusionFrameBuffer.getTexture().read().writeToXML("/home/blake/tmp/occlusion.xml");

	 once=false;
	 }
	 */
	//imageShader.draw(exampleImage, float2(1280-310.0f, 10.0f),float2(300.0f, 200.0f),true);
	camera.setDirty(false);
}
