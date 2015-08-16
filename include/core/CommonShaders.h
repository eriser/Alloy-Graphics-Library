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
#include <initializer_list>
namespace aly {
class VirtualCamera;
class DepthAndNormalShader: public GLShader {
public:
	DepthAndNormalShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	void draw(const std::initializer_list<const Mesh*>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer,
			bool flatShading = false);
	void draw(
			const std::initializer_list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer,
			bool flatShading = false);
	void draw(const std::list<const Mesh*>& meshes, VirtualCamera& camera,
			GLFrameBuffer& framebuffer, bool flatShading = false);
	void draw(const std::list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer,
			bool flatShading = false);

	void draw(const Mesh& mesh, VirtualCamera& camera,
			GLFrameBuffer& framebuffer, bool flatShading = false) {
		draw( { &mesh }, camera, framebuffer, flatShading);
	}

};
class ParticleIdShader: public GLShader {
private:
	GLFrameBuffer framebuffer;
public:
	GLFrameBuffer& getFrameBuffer() {
		return framebuffer;
	}
	ParticleIdShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	void initialize(int w, int h);
	void draw(const std::initializer_list<const Mesh*>& meshes,
			VirtualCamera& camera, Image2i& faceIdMap, int faceIdOffset = 0,
			int objectIdOffset = 0, float radius = 1.0f);
	void draw(
			const std::initializer_list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, Image2i& faceIdMap, int faceIdOffset = 0,
			int objectIdOffset = 0, float radius = 1.0f);
	void draw(const std::list<const Mesh*>& meshes, VirtualCamera& camera,
			Image2i& faceIdMap, int faceIdOffset = 0, int objectIdOffset = 0,
			float radius = 1.0f);
	void draw(const std::list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, Image2i& faceIdMap, int faceIdOffset = 0,
			int objectIdOffset = 0, float radius = 1.0f);
	void draw(const Mesh& mesh, VirtualCamera& camera, Image2i& faceIdMap,
			int faceIdOffset = 0, int objectIdOffset = 0, float radius = 1.0f) {
		draw( { &mesh }, camera, faceIdMap, faceIdOffset, objectIdOffset,
				radius);
	}
};
class ParticleDepthShader: public GLShader {
public:
	ParticleDepthShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	void draw(const std::initializer_list<const Mesh*>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer, float radius =
					1.0f);
	void draw(
			const std::initializer_list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer, float radius =
					1.0f);
	void draw(const std::list<const Mesh*>& meshes, VirtualCamera& camera,
			GLFrameBuffer& framebuffer, float radius = 1.0f);
	void draw(const std::list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer, float radius =
					1.0f);
	void draw(const Mesh& mesh, VirtualCamera& camera,
			GLFrameBuffer& framebuffer, float radius = 1.0f) {
		draw( { &mesh }, camera, framebuffer, radius);
	}
};
class DepthAndTextureShader: public GLShader {
public:
	DepthAndTextureShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	void draw(const std::initializer_list<const Mesh*>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer,
			bool flatShading = false);
	void draw(
			const std::initializer_list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer,
			bool flatShading = false);

	void draw(const Mesh& mesh, VirtualCamera& camera,
			GLFrameBuffer& framebuffer, bool flatShading = false) {
		draw( { &mesh }, camera, framebuffer, flatShading);
	}

};
class FaceIdShader: public GLShader {
private:
	GLFrameBuffer framebuffer;
public:
	GLFrameBuffer& getFrameBuffer() {
		return framebuffer;
	}
	FaceIdShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	void initialize(int w, int h);
	int draw(const std::initializer_list<const Mesh*>& meshes,
			VirtualCamera& camera, Image2i& faceIdMap, int faceIdOffset = 0,
			int objectIdOffset = 0);
	int draw(
			const std::initializer_list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, Image2i& faceIdMap, int faceIdOffset = 0,
			int objectIdOffset = 0);
	int draw(const std::list<const Mesh*>& meshes, VirtualCamera& camera,
			Image2i& faceIdMap, int faceIdOffset = 0, int objectIdOffset = 0);
	int draw(const std::list<std::pair<const Mesh*, float4x4>>& meshes,
			VirtualCamera& camera, Image2i& faceIdMap, int faceIdOffset = 0,
			int objectIdOffset = 0);

	int draw(const Mesh& mesh, VirtualCamera& camera, Image2i& faceIdMap,
			int faceIdOffset = 0, int objectIdOffset = 0) {
		return draw( { &mesh }, camera, faceIdMap, faceIdOffset, objectIdOffset);
	}
};

class MatcapShader: public GLShader {
private:
	GLTextureRGBA matcapTexture;
public:
	MatcapShader(const std::string& textureImage,
			const std::shared_ptr<AlloyContext>& context =
					AlloyDefaultContext());
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, VirtualCamera& camera,
			const box2px& bounds, const box2px& viewport) {
		begin().set("matcapTexture", matcapTexture, 0).set("textureImage",
				imageTexture, 1).set("bounds", bounds).set("viewport", viewport).draw(
				imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, VirtualCamera& camera,
			const float2& location, const float2& dimensions,
			const box2px& viewport) {
		begin().set("matcapTexture", matcapTexture, 0).set("textureImage",
				imageTexture, 1).set("bounds", box2px(location, dimensions)).set(
				"viewport", viewport).draw(imageTexture).end();

	}
};
class CompositeShader: public GLShader {
public:
	CompositeShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& sourceImageTexture,
			const GLTexture<T, C, I>& sourceDepthTexture,
			const GLTexture<T, C, I>& targetImageTexture,
			const GLTexture<T, C, I>& targetDepthTexture, const box2px& bounds,
			float sourceAlpha = 1.0f, float targetAlpha = 1.0f) {
		begin().set("sourceImage", sourceImageTexture, 0).set("sourceDepth",
				sourceDepthTexture, 1).set("targetImage", targetImageTexture, 2).set(
				"targetDepth", targetDepthTexture, 3).set("bounds", bounds).set(
				"sourceAlpha", sourceAlpha).set("targetAlpha", targetAlpha).set(
				"viewport", context->getViewport())

		.draw(targetImageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& sourceImageTexture,
			const GLTexture<T, C, I>& sourceDepthTexture,
			const GLTexture<T, C, I>& targetImageTexture,
			const GLTexture<T, C, I>& targetDepthTexture,
			const float2& location, const float2& dimensions,
			float sourceAlpha = 1.0f, float targetAlpha = 1.0f) {
		begin().set("sourceImage", sourceImageTexture, 0).set("sourceDepth",
				sourceDepthTexture, 1).set("targetImage", targetImageTexture, 2).set(
				"targetDepth", targetDepthTexture, 3).set("bounds",
				box2px(location, dimensions)).set("sourceAlpha", sourceAlpha).set(
				"targetAlpha", targetAlpha).set("viewport",
				context->getViewport()).draw(targetImageTexture).end();
	}
};
class ImageShader: public GLShader {
public:
	enum class Filter {
		NONE, FXAA, SMALL_BLUR, MEDIUM_BLUR, LARGE_BLUR
	};
	ImageShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext(), const Filter& filter = Filter::NONE);
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const box2px& bounds,
			float alpha = 1.0f, bool flip = false) {
		begin().set("flip", flip ? 1 : 0).set("textureImage", imageTexture, 0).set(
				"bounds", bounds).set("alpha", alpha).set("viewport",
				context->getViewport()).draw(imageTexture).end();
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
	EdgeEffectsShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const box2px& bounds) {
		begin().set("KERNEL_SIZE", 4).set("textureImage", imageTexture, 0).set(
				"bounds", bounds).set("imageSize",
				imageTexture.bounds.dimensions).set("viewport",
				context->getViewport()).draw(imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const float2& location,
			const float2& dimensions) {
		draw(imageTexture, box2px(location, dimensions));
	}
};

class NormalColorShader: public GLShader {
public:
	NormalColorShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const box2px& bounds) {
		begin().set("textureImage", imageTexture, 0).set("bounds", bounds).set(
				"viewport", context->getViewport()).draw(imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const float2& location,
			const float2& dimensions) {
		draw(imageTexture, box2px(location, dimensions));
	}
};

class DepthColorShader: public GLShader {
public:
	DepthColorShader(const std::shared_ptr<AlloyContext>& context =
			AlloyDefaultContext());

	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, float2 zRange,
			const box2px& bounds) {
		begin().set("textureImage", imageTexture, 0).set("zMin", zRange.x), set(
				"zMax", zRange.y).set("bounds", bounds).set("viewport",
				context->getViewport()).draw(imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, float2 zRange,
			const float2& location, const float2& dimensions) {
		draw(imageTexture, zRange, box2px(location, dimensions));
	}
};

class EdgeDepthAndNormalShader: public GLShader {
public:
	EdgeDepthAndNormalShader(const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	void draw(const std::initializer_list<const Mesh*>& meshes,
			VirtualCamera& camera, GLFrameBuffer& framebuffer);
	void draw(const Mesh& mesh, VirtualCamera& camera,
			GLFrameBuffer& framebuffer) {
		draw( { &mesh }, camera, framebuffer);
	}
	;

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
	PhongShader(int numLights, const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	PhongShader(const SimpleLight& light,
			const std::shared_ptr<AlloyContext>& context =
					AlloyDefaultContext()) :
			PhongShader(1, context) {
		lights[0] = light;
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, VirtualCamera& camera,
			const box2px& bounds, const box2px& viewport) {
		begin();
		set("textureImage", imageTexture, 0).set("MIN_DEPTH",
				camera.getNearPlane()).set("MAX_DEPTH", camera.getFarPlane()).set(
				"focalLength", camera.getFocalLength()).set("bounds", bounds).set(
				"viewport", context->getViewport());

		std::vector<Color> ambientColors;
		std::vector<Color> diffuseColors;
		std::vector<Color> lambertianColors;
		std::vector<Color> specularColors;

		std::vector<float> specularWeights;
		std::vector<float3> lightDirections;
		std::vector<float3> lightPositions;

		for (SimpleLight& light : lights) {
			ambientColors.push_back(light.ambientColor);
			diffuseColors.push_back(light.diffuseColor);
			specularColors.push_back(light.specularColor);
			lambertianColors.push_back(light.lambertianColor);
			specularWeights.push_back(light.specularPower);
			if (light.moveWithCamera) {
				float3 pt = (camera.View * light.position.xyzw()).xyz();
				lightPositions.push_back(pt);
				float3 norm = normalize(
						(camera.NormalView * float4(light.direction, 0)).xyz());
				lightDirections.push_back(norm);
			} else {
				lightPositions.push_back(light.position);
				lightDirections.push_back(light.direction);
			}
		}
		set("ambientColors", ambientColors).set("diffuseColors", diffuseColors).set(
				"lambertianColors", lambertianColors).set("specularColors",
				specularColors).set("specularWeights", specularWeights).set(
				"lightPositions", lightPositions).set("lightDirections",
				lightDirections).draw(imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, VirtualCamera& camera,
			const float2& location, const float2& dimensions,
			const box2px& viewport) {
		draw(imageTexture, camera, box2px(location, dimensions), viewport);

	}

};
class WireframeShader: public GLShader {
private:
	float lineWidth;
	bool scaleInvariant;
	Color edgeColor;
	Color faceColor;
public:
	inline void setEdgeColor(const Color& c) {
		edgeColor = c;
	}
	inline void setFaceColor(const Color& c) {
		faceColor = c;
	}
	void setLineWidth(float w, bool scaleInvariant) {
		this->lineWidth = w;
		this->scaleInvariant = scaleInvariant;
	}
	WireframeShader(const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& edgeTexture,
			const GLTexture<T, C, I>& depthTexture, float2 zRange,
			const box2px& bounds, const box2px& viewport) {
		begin().set("textureImage", edgeTexture, 0).set("depthImage",
				depthTexture, 1).set("LINE_WIDTH", lineWidth).set("edgeColor",
				edgeColor).set("faceColor", faceColor).set("scaleInvariant",
				(scaleInvariant) ? 1 : 0).set("zMin", zRange.x), set("zMax",
				zRange.y).set("bounds", bounds).set("viewport", viewport).draw(
				edgeTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& edgeTexture,
			const GLTexture<T, C, I>& depthTexture, float2 zRange,
			const float2& location, const float2& dimensions,
			const box2px& viewport) {
		draw(edgeTexture, depthTexture, zRange, box2px(location, dimensions),
				viewport);
	}
};
class AmbientOcclusionShader: public GLShader {
private:
	float sampleRadius;
	std::vector<float3> sampleNormals;
public:
	AmbientOcclusionShader(const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const box2px& bounds,
			const box2px viewport, VirtualCamera& camera) {

		begin().set("textureImage", imageTexture, 0).set("MIN_DEPTH",
				camera.getNearPlane()).set("MAX_DEPTH", camera.getFarPlane()).set(
				"focalLength", camera.getFocalLength()).set("bounds", bounds).set(
				"viewport", viewport).set("u_radius", sampleRadius).set(
				"u_kernel", sampleNormals).draw(imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const float2& location,
			const float2& dimensions, const box2px viewport,
			VirtualCamera& camera) {
		draw(imageTexture, box2px(location, dimensions), viewport, camera);
	}
};
class DistanceFieldShader: public GLShader {
private:
	int kernelSize;
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
	DistanceFieldShader(const std::shared_ptr<AlloyContext>& contex =
			AlloyDefaultContext());

	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const box2px& bounds,
			const box2px& viewport) {
		begin().set("KERNEL_SIZE", kernelSize).set("innerColor", innerGlowColor).set(
				"outerColor", outerGlowColor).set("edgeColor", edgeColor).set(
				"textureImage", imageTexture, 0).set("bounds", bounds).set(
				"imageSize", imageTexture.bounds.dimensions).set("viewport",
				viewport).draw(imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const float2& location,
			const float2& dimensions, const box2px& viewport) {
		draw(imageTexture, box2px(location, dimensions), viewport);
	}
};
}

#endif /* COMMONSHADERS_H_ */
