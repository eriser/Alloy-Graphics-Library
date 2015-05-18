/*
 * Copyright(C) 2014, Blake C. Lucas, Ph.D. (img.science@gmail.com)
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

#ifndef IMAGE_H_
#define IMAGE_H_
#include "AlloyContext.h"
#include "AlloyMath.h"
#include "AlloyImage.h"
#include "GLComponent.h"
#include "GLShader.h"
namespace aly {
template<class T,int C,ImageType I> class GLImage:public GLComponent {
protected:
	Image<T,C,I> mData;
	static std::unique_ptr<GLShader> defaultShader;
	GLShader* imageShader;
	bool mShadeEnabled;
	static GLuint vao;//Only need one per context
	static GLuint mPositionBuffer;
	static GLuint mUVBuffer;
	GLuint mTextureId;
	virtual void update(AlloyContext* context) override;
	virtual void draw(AlloyContext* context) override;
public:
	GLShader* getShader();
	static const float2 TextureCoords[6];
	static const float3 PositionCoords[6];
	std::vector<RGBA>& data(){
		return mData;
	}
	inline void setShadeEnabled(bool shade){
		mShadeEnabled=shade;
	}
	void setShader(GLShader* shader){
		this->imageShader=shader;
	}
	inline int width(){return mData.width;}
	inline int height(){return mData.height;}
	unsigned int textureId(){return mTextureId;}
	vec<T,C>& operator()(const int i,const int j){
		return mData(i,j);
	}
	GLImage():GLComponent(),imageShader(NULL),mTextureId(0),mShadeEnabled(true){
	}
	GLImage(int _x,int _y,int _width,int _height,int imageWidth,int imageHeight);
	GLImage(const Image<T,C,I>& image);
	static GLImage* read(const std::string& file);
	bool write(const std::string& file);
	bool writeTexture(const std::string& file);
	virtual ~GLImage();
};

}

#endif /* IMAGE_H_ */
