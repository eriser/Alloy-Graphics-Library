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
				getFullPath("images/JG_Silver.png")), imageShader(getContext(),
				ImageShader::Filter::MEDIUM_BLUR), phongShader(1), phongShader2(
				1), particleMatcapShader(getFullPath("images/JG_Silver.png")), voxelSize(
				0.0f) {
}
bool MeshViewer::init(Composite& rootNode) {
	Mesh tmpMesh;
	tmpMesh.load(getFullPath("models/torus.ply"));
	tmpMesh.updateVertexNormals();
	tmpMesh.textureMap.resize(
			tmpMesh.quadIndexes.size() * 4 + tmpMesh.triIndexes.size() * 3);
	for (int i = 0; i < (int) tmpMesh.textureMap.size(); i++) {
		tmpMesh.textureMap[i] = float2((rand() % 1024) / 1024.0f,
				(rand() % 1024) / 1024.0f);
	}
	WriteMeshToFile("torus2.ply", tmpMesh, true);
	ReadMeshFromFile("torus2.ply", tmpMesh);
	WriteMeshToFile("torus3.ply", tmpMesh, false);
	ReadMeshFromFile("torus3.ply", tmpMesh);

	tmpMesh.load(getFullPath("models/icosahedron.ply"));
	tmpMesh.updateVertexNormals();
	tmpMesh.textureMap.resize(
			tmpMesh.quadIndexes.size() * 4 + tmpMesh.triIndexes.size() * 3);
	for (int i = 0; i < (int) tmpMesh.textureMap.size(); i++) {
		tmpMesh.textureMap[i] = float2((rand() % 1024) / 1024.0f,
				(rand() % 1024) / 1024.0f);
	}

	WriteMeshToFile("icosahedron2.ply", tmpMesh, true);
	ReadMeshFromFile("icosahedron2.ply", tmpMesh);
	tmpMesh.textureImage.resize(640, 480);
	tmpMesh.textureImage.set(RGBAf(1.0f, 0.0, 0.0, 1.0f));
	WriteMeshToFile("icosahedron3.ply", tmpMesh, false);
	ReadMeshFromFile("icosahedron3.ply", tmpMesh);
	mesh.load(getFullPath("models/monkey.ply"));
	mesh.vertexColors.resize(mesh.vertexLocations.size());
	//WriteMeshToFile("monkey2.ply", mesh, true);
	//WriteMeshToFile("monkey3.ply", mesh, false);

	mesh.scale(100.0f);
	//mesh.save("monkey2.ply");
	//mesh.vertexColors.resize(mesh.vertexLocations.size());
	//mesh.vertexColors.set(float4(1.0f, 0.0f, 0.0f, 1.0f));
	MeshNeighborTable vertTable;
	CreateVertexNeighborTable(mesh, vertTable, false);

	srand(1023172413L);
	mesh.updateVertexNormals();

	/*
	 SparseMatrix<float, 3> L(mesh.vertexLocations.size(), mesh.vertexLocations.size());
	 for (size_t i = 0;i < L.rows;i++) {
	 for(uint32_t v:vertTable[i]){
	 L.insert(i, v, float3(-w));
	 }
	 L.insert(i,i,float3(1.0f+(float)w* vertTable[i].size()));
	 float3 norm = mesh.vertexNormals[i];
	 mesh.vertexLocations[i] +=5.0f*norm*(((rand() % 1024) / 1024.0f)-0.5f);
	 b[i] = mesh.vertexLocations[i];
	 }
	 */

	SparseMatrix<float, 1> L(mesh.vertexLocations.size(),
			mesh.vertexLocations.size());
	Vector3f b(L.rows);
	srand(213823);

	for (size_t i = 0; i < L.rows; i++) {

		const float w = 0.9f;
		for (uint32_t v : vertTable[i]) {
			L.set(i, v, float1(-w));
		}
		L.set(i, i, float1(1.0f + (float) w * vertTable[i].size()));
		float3 norm = mesh.vertexNormals[i];
		mesh.vertexLocations[i] += 5.0f * norm
				* (((rand() % 1024) / 1024.0f) - 0.5f);
		b[i] = mesh.vertexLocations[i];
		mesh.vertexColors[i] = RGBAf(((rand() % 1024) / 1024.0f),
				((rand() % 1024) / 1024.0f), ((rand() % 1024) / 1024.0f), 1.0f);
	}
	//WriteMeshToFile("smoothed_before.ply", mesh);
	/*
	 SolveCG(b, L, mesh.vertexLocations,100,1E-6f,
	 [this](int iter,double err) {
	 std::cout<<"Iteration "<<iter<<":: "<<err<<std::endl;
	 });
	 */
	//WriteMeshToFile("smoothed_cg.ply", mesh);
	SolveBICGStab(b, L, mesh.vertexLocations, 100, 1E-6f,
			[this](int iter,double err) {
				std::cout<<"Iteration "<<iter<<":: "<<err<<std::endl;
			});

	WriteMeshToFile("smoothed_bicg.ply", mesh);

	mesh.updateVertexNormals();
	particles.vertexLocations = mesh.vertexLocations;
	particles.update();
	//WriteMeshToFile("smoothed_after.ply", mesh);

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

	mesh.updateVertexNormals();
	addListener(&camera);
	setOnResize([=](const int2& dims) {
		if(!AlloyApplicationContext()->hasDeferredTasks()) {
			AlloyApplicationContext()->addDeferredTask([=] {
						int w=getContext()->width()/4;
						int h=getContext()->height()/2;
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
	static bool once = true;
	glEnable(GL_DEPTH_TEST);
	if (camera.isDirty()) {
		edgeDepthAndNormalShader.draw(mesh, camera, edgeFrameBuffer);
		depthAndNormalShader.draw(mesh, camera, flatDepthFrameBuffer, true);
		depthAndNormalShader.draw(mesh, camera, smoothDepthFrameBuffer1, false);
		depthAndNormalShader.draw(mesh2, camera, smoothDepthFrameBuffer2,
				false);
		particleDepthShader.draw(particles, camera, particleFrameBuffer, 0.75f);

		if (once) {
			faceShader.draw(mesh, camera);

			faceShader.read(faceIdMap);
			faceIdMap.writeToXML("face_id.xml");
			faceShader.draw( { &particles, &mesh }, camera);
			faceShader.read(faceIdMap);
			faceIdMap.writeToXML("face_particle_id.xml");
			particleFaceIdShader.draw(mesh, camera);
			particleFaceIdShader.read(faceIdMap);
			faceIdMap.writeToXML("particle_face_id.xml");
			once = false;
		}

	}
	glDisable(GL_DEPTH_TEST);
	float2 dRange = camera.computeNormalizedDepthRange(mesh);
	int w = smoothDepthFrameBuffer1.width();
	int h = smoothDepthFrameBuffer1.height();
	depthColorShader.draw(edgeFrameBuffer.getTexture(),
			float2(0.0f, camera.getScale()), float2(0.0f, 0.0f), float2(w, h));
	normalColorShader.draw(edgeFrameBuffer.getTexture(), float2(0.0f, h),
			float2(w, h));
	depthColorShader.draw(smoothDepthFrameBuffer1.getTexture(), dRange,
			float2(w * 2, 0.0f), float2(w, h));
	normalColorShader.draw(smoothDepthFrameBuffer1.getTexture(),
			float2(w * 3, h), float2(w, h));
	glEnable(GL_DEPTH_TEST);
	wireframeShader.draw(edgeFrameBuffer.getTexture(),
			smoothDepthFrameBuffer1.getTexture(),
			float2(0.0f, camera.getScale()), float2(2 * w, h), float2(w, h),
			getContext()->getViewport());

	phongShader.draw(smoothDepthFrameBuffer2.getTexture(), camera,
			float2(2 * w, h), float2(w, h), getContext()->getViewport());

	matcapShader.draw(smoothDepthFrameBuffer1.getTexture(), camera,
			float2(2 * w, h), float2(w, h), getContext()->getViewport(),
			RGBAf(1.0f, 0.8f, 0.2f, 1.0f));

	particleMatcapShader.draw( { &mesh }, camera,
			box2px(float2(2 * w, h), float2(w, h)),
			getContext()->getViewport());

	/*
	 matcapShader.draw(particleFrameBuffer.getTexture(), camera,
	 float2(2 * w, h), float2(w, h), getContext()->getViewport(),RGBAf(1.0f,0.5f,0.4f,1.0f));
	 */
	glDisable(GL_DEPTH_TEST);
	if (once) {
		colorBuffer1.getTexture().read().writeToXML("color1.xml");
		colorBuffer2.getTexture().read().writeToXML("color2.xml");
		smoothDepthFrameBuffer1.getTexture().read().writeToXML("depth1.xml");
		smoothDepthFrameBuffer2.getTexture().read().writeToXML("depth2.xml");
		once = false;
	}

	if (camera.isDirty()) {
		outlineFrameBuffer.begin();
		DistanceFieldShader.draw(flatDepthFrameBuffer.getTexture(),
				float2(0.0f, 0.0f), float2(w, h),
				outlineFrameBuffer.getViewport());
		outlineFrameBuffer.end();

		wireframeFrameBuffer.begin();
		wireframeShader.draw(edgeFrameBuffer.getTexture(),
				smoothDepthFrameBuffer1.getTexture(),
				float2(0.0f, camera.getScale()), float2(0.0f, 0.0f),
				float2(w, h), wireframeFrameBuffer.getViewport());
		wireframeFrameBuffer.end();

		occlusionFrameBuffer.begin();
		ambientOcclusionShader.draw(smoothDepthFrameBuffer1.getTexture(),
				float2(0.0f, 0.0f), float2(w, h),
				occlusionFrameBuffer.getViewport(), camera);
		occlusionFrameBuffer.end();
	}
	imageShader.draw(outlineFrameBuffer.getTexture(), float2(w, 0.0f),
			float2(w, h));
	imageShader.draw(wireframeFrameBuffer.getTexture(), float2(w, h),
			float2(w, h));
	imageShader.draw(occlusionFrameBuffer.getTexture(), float2(3 * w, 0.0f),
			float2(w, h));

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
