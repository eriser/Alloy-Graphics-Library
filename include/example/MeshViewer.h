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

#ifndef MESHVIEWER_H_
#define MESHVIEWER_H_

#include "Alloy.h"
class MeshViewer: public aly::Application {
private:
	aly::Mesh mesh;
	aly::Mesh particles;
	aly::Mesh mesh2;
	aly::Camera camera;
	aly::FaceIdShader faceShader;
	aly::MatcapShader matcapShader;
	aly::ImageShader imageShader;
	aly::ImageShader imageOffscreenShader;
	aly::EdgeEffectsShader effectsShader;
	aly::ParticleDepthShader particleDepthShader;
	aly::EdgeDepthAndNormalShader edgeDepthAndNormalShader;
	aly::DepthAndNormalShader depthAndNormalShader;
	aly::ParticleIdShader particleFaceIdShader;
	aly::AmbientOcclusionShader ambientOcclusionShader;
	aly::DepthColorShader depthColorShader;
	aly::DepthAndTextureShader depthAndTextureShader;
	aly::NormalColorShader normalColorShader;
	aly::DistanceFieldShader DistanceFieldShader;
	aly::WireframeShader wireframeShader;
	aly::PhongShader phongShader;
	aly::PhongShader phongShader2;
	aly::ColorVertexShader colorVertexShader;
	aly::CompositeShader compositeShader;
	aly::TextureMeshShader texMeshShader;

	aly::ParticleMatcapShader particleMatcapShader;
	
	aly::GLFrameBuffer occlusionFrameBuffer;
	aly::GLFrameBuffer flatDepthFrameBuffer;
	aly::GLFrameBuffer smoothDepthFrameBuffer2;
	aly::GLFrameBuffer smoothDepthFrameBuffer1;
	aly::GLFrameBuffer colorBuffer2;
	aly::GLFrameBuffer colorBuffer1;
	aly::GLFrameBuffer textureFrameBuffer;
	aly::GLFrameBuffer particleFrameBuffer;
	aly::GLFrameBuffer outlineFrameBuffer;
	aly::GLFrameBuffer wireframeFrameBuffer;
	aly::GLFrameBuffer edgeFrameBuffer;
	aly::GLTextureRGBAf texImage;
	aly::GLTextureRGBA exampleImage;

	aly::Image2i faceIdMap;
	float voxelSize;

public:
	MeshViewer();
	bool init(aly::Composite& rootNode) override;
	void draw(aly::AlloyContext* context) override;
};

#endif /* ALLOYEXAMPLEUI_H_ */
