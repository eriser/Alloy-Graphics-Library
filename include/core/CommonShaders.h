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
	ImageShader(std::shared_ptr<AlloyContext> context=AlloyDefaultContext());
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const box2px& bounds,bool flip=false) {
		begin().set("flip",flip?1:0).set("textureImage", imageTexture, 0).set("bounds",
				box2px(float2(30.0f, 30.0f), float2(300.0f, 200.0f))).set(
				"viewport", context->viewport).draw(imageTexture).end();
	}
	template<class T, int C, ImageType I> void draw(
			const GLTexture<T, C, I>& imageTexture, const float2& location,
			const float2& dimensions,bool flip=false) {
		begin().set("flip",flip?1:0).set("textureImage", imageTexture, 0).set("bounds",
				box2px(location, dimensions)).set("viewport", context->viewport).draw(
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
	void draw(const Mesh& mesh, VirtualCamera& camera, const box2px& bounds);
	void draw(const Mesh& mesh, VirtualCamera& camera);
};

class EdgeDepthAndNormalShader: public GLShader {
public:
	EdgeDepthAndNormalShader(std::shared_ptr<AlloyContext> contex=AlloyDefaultContext());
	void draw(const Mesh& mesh, VirtualCamera& camera, const box2px& bounds);
	void draw(const Mesh& mesh, VirtualCamera& camera);
};
class DistanceFieldShader: public GLShader {
public:
	DistanceFieldShader(std::shared_ptr<AlloyContext> contex=AlloyDefaultContext());
	void draw(const GLTextureRGBAf& imageTexture,int distance, const box2px& bounds);
	void draw(const GLTextureRGBAf& imageTexture,int distance, const float2& location,const float2& dimensions);
};
}

#endif /* COMMONSHADERS_H_ */