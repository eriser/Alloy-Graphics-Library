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
	aly::VirtualCamera camera;

	aly::MatcapShader matcapShader;
	aly::ImageShader imageShader;
	aly::EdgeEffectsShader effectsShader;
	aly::EdgeDepthAndNormalShader edgeDepthAndNormalShader;
	aly::DepthAndNormalShader depthAndNormalShader;
	aly::AmbientOcclusionShader ambientOcclusionShader;
	aly::DepthColorShader depthColorShader;

	aly::NormalColorShader normalColorShader;
	aly::OutlineShader outlineShader;
	aly::WireframeShader wireframeShader;
	aly::PhongShader phongShader;
	aly::GLFrameBuffer edgeFrameBuffer;

	aly::GLFrameBuffer occlusionFrameBuffer;
	aly::GLFrameBuffer flatDepthFrameBuffer;
	aly::GLFrameBuffer smoothDepthFrameBuffer;
	aly::GLFrameBuffer outlineFrameBuffer;
	aly::GLFrameBuffer wireframeFrameBuffer;
	aly::GLTextureRGBA exampleImage;
	float voxelSize;

public:
	MeshViewer();
	bool init(aly::Composite& rootNode) override;
	void draw(const aly::DrawEvent3D& event) override;
	void draw(const aly::DrawEvent2D& event) override;
};

#endif /* ALLOYEXAMPLEUI_H_ */
