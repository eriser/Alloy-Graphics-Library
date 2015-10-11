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
#include <set>
using namespace aly;
MeshViewer::MeshViewer() :
	Application(1920, 960, "Mesh Viewer"), matcapShader(
		getFullPath("images/JG_Silver.png")), imageShader(ImageShader::Filter::NONE,true, getContext()), imageOffscreenShader(ImageShader::Filter::NONE,false, getContext()),phongShader(1,true), phongShader2(
				1,true), particleMatcapShader(getFullPath("images/JG_Silver.png"),true), voxelSize(
					0.0f), occlusionFrameBuffer(true),
	depthAndTextureShader(false),
	edgeFrameBuffer(true),
	texMeshShader(false),
	flatDepthFrameBuffer(true),
	smoothDepthFrameBuffer2(true),
	smoothDepthFrameBuffer1(true),
	colorBuffer2(true),
	colorBuffer1(false),
	textureFrameBuffer(false),
	particleFrameBuffer(true),
	outlineFrameBuffer(true),
	wireframeFrameBuffer(true),
	texImage(false),
	exampleImage(true) {
}
bool MeshViewer::init(Composite& rootNode) {

	mesh.load(getFullPath("models/monkey.obj"));
	Subdivide(mesh, SubDivisionScheme::CatmullClark);
	//texImage.load(mesh.textureImage);
	mesh.vertexColors.resize(mesh.vertexLocations.size());
	mesh.scale(100.0f);
	mesh.updateVertexNormals();
	for (int i = 0; i < (int)mesh.vertexLocations.size(); i++) {
		mesh.vertexColors[i] = RGBAf(((rand() % 1024) / 1024.0f),
			((rand() % 1024) / 1024.0f), ((rand() % 1024) / 1024.0f), 1.0f);
	}
	std::cout << mesh << std::endl;
	//WriteMeshToFile("tanya_obj.obj",mesh);
	//WriteMeshToFile("tanya_ply.ply",mesh);
	particles.vertexColors = mesh.vertexColors;
	particles.vertexLocations = mesh.vertexLocations;
	particles.update();

	mesh2.load(getFullPath("models/armadillo.ply"));

	mesh2.updateVertexNormals();

	float4x4 M = MakeTransform(mesh2.getBoundingBox(), mesh.getBoundingBox());
	mesh2.transform(M);
	box3f renderBBox = box3f(float3(-0.5f, -0.5f, -0.5f),
		float3(1.0f, 1.0f, 1.0f));

	camera.setNearFarPlanes(-2.0f, 2.0f);
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

	int w = getContext()->width() / 4;
	int h = getContext()->height() / 2;
	edgeFrameBuffer.initialize(w, h);
	smoothDepthFrameBuffer1.initialize(w, h);
	smoothDepthFrameBuffer2.initialize(w, h);
	flatDepthFrameBuffer.initialize(w, h);
	outlineFrameBuffer.initialize(w, h);
	wireframeFrameBuffer.initialize(w, h);
	occlusionFrameBuffer.initialize(w, h);
	colorBuffer1.initialize(w, h);
	colorBuffer2.initialize(w, h);
	particleFrameBuffer.initialize(w, h);
	faceShader.initialize(w, h);
	particleFaceIdShader.initialize(w, h);
	textureFrameBuffer.initialize(w, h);
	wireframeShader.setLineWidth(3.0f);
	//getContext()->setOffScreenVisible(true);
	mesh.updateVertexNormals();
	addListener(&camera);
	setOnResize([=](const int2& dims) {
		if (!AlloyApplicationContext()->hasDeferredTasks()) {
			AlloyApplicationContext()->addDeferredTask([=] {
				int w = getContext()->width() / 4;
				int h = getContext()->height() / 2;
				edgeFrameBuffer.initialize(w, h);
				smoothDepthFrameBuffer1.initialize(w, h);
				smoothDepthFrameBuffer2.initialize(w, h);
				flatDepthFrameBuffer.initialize(w, h);
				outlineFrameBuffer.initialize(w, h);
				wireframeFrameBuffer.initialize(w, h);
				occlusionFrameBuffer.initialize(w, h);
				colorBuffer1.initialize(w, h);
				colorBuffer2.initialize(w, h);
				particleFrameBuffer.initialize(w, h);
				faceShader.initialize(w, h);
				particleFaceIdShader.initialize(w, h);
				camera.setDirty(true);
			});
		}
	});
	camera.setDirty(true);
	camera.setCameraType(CameraType::Orthographic);
	return true;
}
void MeshViewer::draw(AlloyContext* context) {
	//static bool once = true;
	int w = smoothDepthFrameBuffer1.width();
	int h = smoothDepthFrameBuffer1.height();

	if (camera.isDirty()) {

		glEnable(GL_DEPTH_TEST);
		edgeDepthAndNormalShader.draw({ {&mesh,mesh.pose} }, camera, edgeFrameBuffer);
		depthAndNormalShader.draw(mesh, camera, flatDepthFrameBuffer, true);
		depthAndNormalShader.draw(mesh, camera, smoothDepthFrameBuffer1, false);
		depthAndNormalShader.draw(mesh2, camera, smoothDepthFrameBuffer2, false);	
		particleDepthShader.draw(particles, camera, particleFrameBuffer, 0.1f);
		depthAndTextureShader.draw(mesh, camera, textureFrameBuffer, true);

		wireframeFrameBuffer.begin();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		wireframeShader.draw({ { &mesh,mesh.pose } }, camera,wireframeFrameBuffer.getViewport());
		wireframeFrameBuffer.end();

		/*
		if (once) {
			faceShader.draw(mesh, camera);

			faceShader.read(faceIdMap);
			//faceIdMap.writeToXML("face_id.xml");
			faceShader.draw( { &particles, &mesh }, camera);
			faceShader.read(faceIdMap);
			//faceIdMap.writeToXML("face_particle_id.xml");
			particleFaceIdShader.draw(mesh, camera);
			particleFaceIdShader.read(faceIdMap);
			//faceIdMap.writeToXML("particle_face_id.xml");
			once = false;
		}

		*/
	}
	glDisable(GL_DEPTH_TEST);
	float2 dRange = camera.computeNormalizedDepthRange(mesh);

	depthColorShader.draw(edgeFrameBuffer.getTexture(),
		float2(0.0f, camera.getScale()), float2(0.0f, 0.0f), float2((float)w, (float)h));
	normalColorShader.draw(edgeFrameBuffer.getTexture(), float2(0.0f, (float)h),
		float2((float)w, (float)h));
	depthColorShader.draw(smoothDepthFrameBuffer1.getTexture(), dRange,
		float2((float)w * 2, 0.0f), float2((float)w, (float)h));
	normalColorShader.draw(smoothDepthFrameBuffer1.getTexture(),
		float2((float)w * 3, (float)h), float2((float)w, (float)h));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	matcapShader.draw(smoothDepthFrameBuffer1.getTexture(), camera,
		float2((float)2 * w, (float)h), float2((float)w, (float)h), getContext()->getViewport(),
		RGBAf(1.0f, 1.0f, 1.0f, 1.0f));
	
	glDisable(GL_DEPTH_TEST);
	imageShader.draw(wireframeFrameBuffer.getTexture(), float2((float)2 * w, (float)h),
		float2((float)w, (float)h));
	glEnable(GL_DEPTH_TEST);

	phongShader.draw(smoothDepthFrameBuffer2.getTexture(), camera,
		float2((float)2 * w, (float)h), float2((float)w, (float)h), getContext()->getViewport());
	particleMatcapShader.draw({ &mesh }, camera,
		box2px(float2((float)2 * w, (float)h), float2((float)w, (float)h)),
		getContext()->getViewport(), 2.0);

	glEnable(GL_BLEND);


	/*
	 matcapShader.draw(particleFrameBuffer.getTexture(), camera,
	 float2(2 * w, h), float2(w, h), getContext()->getViewport(),RGBAf(1.0f,0.5f,0.4f,1.0f));
	 */
	glDisable(GL_DEPTH_TEST);
	/*
	if (once) {
		colorBuffer1.getTexture().read().writeToXML("color1.xml");
		colorBuffer2.getTexture().read().writeToXML("color2.xml");
		smoothDepthFrameBuffer1.getTexture().read().writeToXML("depth1.xml");
		smoothDepthFrameBuffer2.getTexture().read().writeToXML("depth2.xml");
		once = false;
	}
*/
	if (camera.isDirty()) {
		outlineFrameBuffer.begin();
		DistanceFieldShader.draw(flatDepthFrameBuffer.getTexture(),
			float2(0.0f, 0.0f), float2((float)w, (float)h),
			outlineFrameBuffer.getViewport());
		outlineFrameBuffer.end();


		occlusionFrameBuffer.begin();
		ambientOcclusionShader.draw(flatDepthFrameBuffer.getTexture(),
			float2(0.0f, 0.0f), float2((float)w, (float)h),
			occlusionFrameBuffer.getViewport(), camera);
		occlusionFrameBuffer.end();

		//colorVertexShader.draw({ &mesh },camera,occlusionFrameBuffer,true);

		/*
		colorBuffer1.begin();
		texMeshShader.draw(textureFrameBuffer.getTexture(), texImage, camera,
			box2px(float2(0.0f, 0.0f), float2((float)w, (float)h)),
			colorBuffer1.getViewport());
		colorBuffer1.end();
		*/
		context->initOffScreenDraw();
		imageOffscreenShader.draw(colorBuffer1.getTexture(), float2((float)0.0f, (float)0.0f),
			float2((float)w, (float)h));
	}


	imageShader.draw(outlineFrameBuffer.getTexture(), float2((float)w, (float)0.0f),
		float2((float)w, (float)h));
	imageShader.draw(wireframeFrameBuffer.getTexture(), float2((float)w, (float)h),
		float2((float)w, (float)h));
	imageShader.draw(occlusionFrameBuffer.getTexture(), float2((float)3 * w, 0.0f),
		float2((float)w, (float)h));

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
