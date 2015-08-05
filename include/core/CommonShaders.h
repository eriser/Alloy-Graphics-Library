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

#ifndef COMMONSHADERS_H_
#define COMMONSHADERS_H_

#include "GLTexture.h"
#include "GLShader.h"
#include "GLFrameBuffer.h"
#include "AlloyImage.h"
#include "AlloyUnits.h"
namespace aly {
class VirtualCamera;
class MatcapShader: public GLShader {
private:
	GLTextureRGBA matcapTexture;
public:
	MatcapShader(const std::string& textureImage,
			const std::shared_ptr<AlloyContext>& context = AlloyDefaultContext());
	void draw(const Mesh& mesh, VirtualCamera& camera, const box2px& bounds);
	void draw(const Mesh& mesh, VirtualCamera& camera);
};
class CompositeShader : public GLShader {
public:
	CompositeShader(const std::shared_ptr<AlloyContext>& context = AlloyDefaultContext());
	template<class T, int C, ImageType I> void draw(
		const GLTexture<T, C, I>& sourceImageTexture,
		const GLTexture<T, C, I>& sourceDepthTexture, 
		const GLTexture<T, C, I>& targetImageTexture,
		const GLTexture<T, C, I>& targetDepthTexture,
		const box2px& bounds,
		float sourceAlpha=1.0f,
		float targetAlpha = 1.0f) {
		begin()
		.set("sourceImage", sourceImageTexture, 0)
		.set("sourceDepth", sourceDepthTexture, 1)
		.set("targetImage", targetImageTexture, 2)
		.set("targetDepth", targetDepthTexture, 3)
		.set("bounds", bounds)
		.set("sourceAlpha", sourceAlpha)
		.set("targetAlpha", targetAlpha)
		.set("viewport", context->getViewport())

		.draw(targetImageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
		const GLTexture<T, C, I>& sourceImageTexture,
		const GLTexture<T, C, I>& sourceDepthTexture,
		const GLTexture<T, C, I>& targetImageTexture,
		const GLTexture<T, C, I>& targetDepthTexture,
		const float2& location,
		const float2& dimensions,
		float sourceAlpha = 1.0f,
		float targetAlpha = 1.0f) {
		begin()
			.set("sourceImage", sourceImageTexture, 0)
			.set("sourceDepth", sourceDepthTexture, 1)
			.set("targetImage", targetImageTexture, 2)
			.set("targetDepth", targetDepthTexture, 3)
			.set("bounds", box2px(location, dimensions))
			.set("sourceAlpha", sourceAlpha)
			.set("targetAlpha", targetAlpha)
			.set("viewport", context->getViewport())
			.draw(targetImageTexture).end();
	}
};
class ImageShader: public GLShader {
public:
	enum class Filter {
		NONE, FXAA, SMALL_BLUR, MEDIUM_BLUR, LARGE_BLUR
	};
	ImageShader(const std::shared_ptr<AlloyContext>& context = AlloyDefaultContext(),
			const Filter& filter = Filter::NONE);
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const box2px& bounds,
			bool flip = false) {
		begin().set("flip", flip ? 1 : 0).set("textureImage", imageTexture, 0).set(
				"bounds", bounds).set("viewport", context->getViewport()).draw(
				imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const float2& location,
			const float2& dimensions, bool flip = false) {
		begin().set("flip", flip ? 1 : 0).set("textureImage", imageTexture, 0).set(
				"bounds", box2px(location, dimensions)).set("viewport",
				context->getViewport()).draw(imageTexture).end();

	}
};
class EdgeEffectsShader: public GLShader {
public:
	EdgeEffectsShader(std::shared_ptr<AlloyContext> context =
			AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, const box2px& bounds);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,
			const float2& dimensions);
};

class NormalColorShader: public GLShader {
public:
	NormalColorShader(std::shared_ptr<AlloyContext> context =
			AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, const box2px& bounds);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,
			const float2& dimensions);
};

class DepthColorShader: public GLShader {
public:
	DepthColorShader(std::shared_ptr<AlloyContext> context =
			AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, float2 zRange,
			const box2px& bounds);
	void draw(const GLTextureRGBAf& imageTexture, float2 zRange,
			const float2& location, const float2& dimensions);
};

class DepthAndNormalShader: public GLShader {
public:
	DepthAndNormalShader(const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	void draw(const Mesh& mesh, VirtualCamera& camera,
			GLFrameBuffer& framebuffer, bool flatShading = false);
};

class EdgeDepthAndNormalShader: public GLShader {
public:
	EdgeDepthAndNormalShader(
		const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	void draw(const Mesh& mesh, VirtualCamera& camera,
			GLFrameBuffer& framebuffer);
};
struct SimpleLight {
	Color ambientColor;
	Color diffuseColor;
	Color lambertianColor;
	Color specularColor;
	float specularPower;
	float3 position;
	float3 direction;
	float2 focalLength;
	bool moveWithCamera;
	bool castShadow;
	bool orthographic;
	bool attenuate;
	SimpleLight(const Color& ambientColor, const Color& diffuseColor,
			const Color& lambertianColor, const Color& specularColor,
			const float& specularPower, const float3& position,
			const float3& direction, const float2& focalLength = float2(0, 0),
			bool moveWithCamera = false, bool castShadow = false,
			bool orthographic = false, bool attenuate = false) :
			ambientColor(ambientColor), diffuseColor(diffuseColor), lambertianColor(
					lambertianColor), specularColor(specularColor), specularPower(
					specularPower), position(position), direction(direction), focalLength(
					focalLength), moveWithCamera(moveWithCamera), castShadow(
					castShadow), orthographic(orthographic), attenuate(
					attenuate) {

	}
	SimpleLight() :
			ambientColor(COLOR_NONE), diffuseColor(COLOR_NONE), lambertianColor(
					COLOR_NONE), specularColor(COLOR_NONE), specularPower(0.0f), position(
					0.0f), direction(0.0f), focalLength(0.0f), moveWithCamera(
					false), castShadow(false), orthographic(false), attenuate(
					false) {

	}
};
class PhongShader: public GLShader {

	std::vector<SimpleLight> lights;
public:
	inline SimpleLight& getLight(int i) {
		if (i >= (int) lights.size()) {
			throw std::runtime_error("Light index out of range.");
		}
		return lights[i];
	}
	const SimpleLight & operator [](int i) const {
		return lights[i];
	}
	SimpleLight & operator [](int i) {
		return lights[i];
	}
	inline int size() const {
		return (int) lights.size();
	}
	PhongShader(int numLights,
		const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	PhongShader(const SimpleLight& light,
			const std::shared_ptr<AlloyContext>& context = AlloyDefaultContext()) :
			PhongShader(1, context) {
		lights[0] = light;
	}
	void draw(const GLTextureRGBAf& imageTexture, const box2px& bounds,
			VirtualCamera& camera, const box2px& viewport);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,
			const float2& dimensions, VirtualCamera& camera,
			const box2px& viewport);
};
class WireframeShader: public GLShader {
private:
	float lineWidth;
	Color edgeColor;
	Color faceColor;
public:
	inline void setEdgeColor(const Color& c) {
		edgeColor = c;
	}
	inline void setFaceColor(const Color& c) {
		faceColor = c;
	}
	WireframeShader(const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, float2 zRange,
			const box2px& bounds, const box2px& viewport);
	void draw(const GLTextureRGBAf& imageTexture, float2 zRange,
			const float2& location, const float2& dimensions,
			const box2px& viewport);
};
class AmbientOcclusionShader: public GLShader {
private:
	float sampleRadius;
	std::vector<float3> sampleNormals;
public:
	AmbientOcclusionShader(
		const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, const box2px& bounds,
			const box2px viewport, VirtualCamera& camera);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,
			const float2& dimensions, const box2px viewport,
			VirtualCamera& camera);
};
class OutlineShader: public GLShader {
private:
	int kernelSize ;
	aly::Color innerGlowColor;
	aly::Color outerGlowColor;
	aly::Color edgeColor;
public:
	inline void setInnerGlowColor(const Color& c) {
		innerGlowColor = c;
	}
	inline void setOuterGlowColor(const Color& c) {
		outerGlowColor = c;
	}
	inline void setEdgeColor(const Color& c) {
		edgeColor = c;
	}
	inline void setExtent(int distance) {
		kernelSize = distance;
	}
	OutlineShader(
		const std::shared_ptr<AlloyContext>& contex = AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, const box2px& bounds,
			const box2px& viewport);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,
			const float2& dimensions, const box2px& viewport);
};
}

#endif /* COMMONSHADERS_H_ */
