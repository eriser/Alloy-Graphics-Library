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
namespace aly {
class VirtualCamera;
class MatcapShader: public GLShader {
private:
	GLTextureRGBA matcapTexture;
public:
	MatcapShader(const std::string& textureImage,
			std::shared_ptr<AlloyContext> context=AlloyDefaultContext());
	void draw(const Mesh& mesh, VirtualCamera& camera, const box2px& bounds);
	void draw(const Mesh& mesh, VirtualCamera& camera);
};

class ImageShader: public GLShader {
public:
	enum class Filter {
		NONE,FXAA,SMALL_BLUR,MEDIUM_BLUR,LARGE_BLUR
	};
	ImageShader(std::shared_ptr<AlloyContext> context=AlloyDefaultContext(),const Filter& filter=Filter::NONE);
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const box2px& bounds,bool flip=false) {
		begin().set("flip",flip?1:0).set("textureImage", imageTexture, 0).set("bounds",bounds).set(
				"viewport", context->getViewport()).draw(imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const float2& location,
			const float2& dimensions,bool flip=false) {
		begin().set("flip",flip?1:0).set("textureImage", imageTexture, 0).set("bounds",
				box2px(location, dimensions)).set("viewport", context->getViewport()).draw(
				imageTexture).end();

	}
};
class EdgeEffectsShader: public GLShader {
public:
	EdgeEffectsShader(std::shared_ptr<AlloyContext> context=AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, const box2px& bounds);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,const float2& dimensions);
};

class NormalColorShader: public GLShader {
public:
	NormalColorShader(std::shared_ptr<AlloyContext> context=AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, const box2px& bounds);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,const float2& dimensions);
};

class DepthColorShader: public GLShader {
public:
	DepthColorShader(std::shared_ptr<AlloyContext> context=AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, float2 zRange,const box2px& bounds);
	void draw(const GLTextureRGBAf& imageTexture, float2 zRange,const float2& location,const float2& dimensions);
};

class DepthAndNormalShader: public GLShader {
public:
	DepthAndNormalShader(std::shared_ptr<AlloyContext> contex=AlloyDefaultContext());
	void draw(const Mesh& mesh, VirtualCamera& camera, GLFrameBuffer& framebuffer,bool flatShading=false);
};

class EdgeDepthAndNormalShader: public GLShader {
public:
	EdgeDepthAndNormalShader(std::shared_ptr<AlloyContext> contex=AlloyDefaultContext());
	void draw(const Mesh& mesh, VirtualCamera& camera, GLFrameBuffer& framebuffer,bool flatShading=false);
};
class WireframeShader: public GLShader {
private:
	float lineWidth=0.25f;
	aly::Color edgeColor=Color(1.0f,1.0f,1.0f,1.0f);
	aly::Color faceColor=Color(1.0f,0.3f,0.1f,1.0f);
public:
	inline void setEdgeColor(const Color& c){
		edgeColor=c;
	}
	inline void setFaceColor(const Color& c){
		faceColor=c;
	}
	WireframeShader(std::shared_ptr<AlloyContext> contex=AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture, float2 zRange,const box2px& bounds,const box2px& viewport);
	void draw(const GLTextureRGBAf& imageTexture, float2 zRange,const float2& location,const float2& dimensions,const box2px& viewport);
};
class AmbientOcclusionShader: public GLShader {
private:
	float sampleRadius=0.005f;
	std::vector<float3> sampleNormals;
public:
	AmbientOcclusionShader(std::shared_ptr<AlloyContext> contex=AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture,const box2px& bounds, const box2px viewport,VirtualCamera& camera);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,const float2& dimensions, const box2px viewport,VirtualCamera& camera);
};
class OutlineShader: public GLShader {
private:
	int kernelSize=8;
	aly::Color innerGlowColor=Color(1.0f,0.3f,0.1f,1.0f);
	aly::Color outerGlowColor=Color(0.3f,1.0f,0.1f,1.0f);
	aly::Color edgeColor=Color(1.0f,1.0f,1.0f,1.0f);
public:
	inline void setInnerGlowColor(const Color& c){
		innerGlowColor=c;
	}
	inline void setOuterGlowColor(const Color& c){
		outerGlowColor=c;
	}
	inline void setEdgeColor(const Color& c){
		edgeColor=c;
	}
	inline void setExtent(int distance){
		kernelSize=distance;
	}
	OutlineShader(std::shared_ptr<AlloyContext> contex=AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture,const box2px& bounds,const box2px& viewport);
	void draw(const GLTextureRGBAf& imageTexture, const float2& location,const float2& dimensions, const box2px& viewport);
};
}

#endif /* COMMONSHADERS_H_ */
