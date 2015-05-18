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

namespace aly {
class GLShader;
template<class T,int C,ImageType I> class GLTexture: public GLComponent {
public:
	GLuint textureId=0;
protected:
	Image<T,C,I> textureImage;
	static std::unique_ptr<GLShader> defaultShader;
	GLShader* imageShader=false;
	bool enableShader=true;
	GLuint internalFormat = GL_DEPTH_COMPONENT;
	virtual void update(AlloyContext* context) override{
		GLuint& vao=context->globalImage.vao;
		GLuint& positionBuffer=context->globalImage.positionBuffer;
		GLuint& uvBuffer=context->globalImage.uvBuffer;
		if(textureId==0){
			glGenTextures( 1,&textureId);
		}
		glBindTexture( GL_TEXTURE_2D, textureId);
		switch(textureImage.type){
			case ImageType::FLOAT:
				if(textureImage.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32F, textureImage.width, textureImage.height, 0,GL_RGBA,GL_FLOAT, &textureImage[0]);
				} else if(textureImage.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32F, textureImage.width, textureImage.height, 0,GL_RGB,GL_FLOAT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32F, textureImage.width, textureImage.height, 0,GL_R,GL_FLOAT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeString());
				}
				break;
			case ImageType::UBYTE:
				if(textureImage.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA, textureImage.width, textureImage.height, 0,GL_RGBA,GL_UNSIGNED_BYTE, &textureImage[0]);
				} else if(textureImage.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB, textureImage.width, textureImage.height, 0,GL_RGB,GL_UNSIGNED_BYTE, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R, textureImage.width, textureImage.height, 0,GL_R,GL_UNSIGNED_BYTE, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeString());
				}
				break;
			case ImageType::USHORT:
				if(textureImage.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA16UI, textureImage.width, textureImage.height, 0,GL_RGBA,GL_UNSIGNED_SHORT, &textureImage[0]);
				} else if(textureImage.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB16UI, textureImage.width, textureImage.height, 0,GL_RGB,GL_UNSIGNED_SHORT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R16UI, textureImage.width, textureImage.height, 0,GL_R,GL_UNSIGNED_SHORT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeString());
				}
				break;
			case ImageType::UINT:
				if(textureImage.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32UI, textureImage.width, textureImage.height, 0,GL_RGBA,GL_UNSIGNED_INT, &textureImage[0]);
				} else if(textureImage.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32UI, textureImage.width, textureImage.height, 0,GL_RGB,GL_UNSIGNED_INT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32UI, textureImage.width, textureImage.height, 0,GL_R,GL_UNSIGNED_INT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeString());
				}
				break;
			case ImageType::BYTE:
				if(textureImage.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA, textureImage.width, textureImage.height, 0,GL_RGBA,GL_BYTE, &textureImage[0]);
				} else if(textureImage.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB, textureImage.width, textureImage.height, 0,GL_RGB,GL_BYTE, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R, textureImage.width, textureImage.height, 0,GL_R,GL_BYTE, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeString());
				}
				break;
			case ImageType::SHORT:
				if(textureImage.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA16UI, textureImage.width, textureImage.height, 0,GL_RGBA,GL_SHORT, &textureImage[0]);
				} else if(textureImage.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB16UI, textureImage.width, textureImage.height, 0,GL_RGB,GL_SHORT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R16UI, textureImage.width, textureImage.height, 0,GL_R,GL_SHORT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeString());
				}
				break;
			case ImageType::INT:
				if(textureImage.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32UI, textureImage.width, textureImage.height, 0,GL_RGBA,GL_INT, &textureImage[0]);
				} else if(textureImage.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32UI, textureImage.width, textureImage.height, 0,GL_RGB,GL_INT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32UI, textureImage.width, textureImage.height, 0,GL_R,GL_INT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeString());
				}
				break;
			default:
				throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeString());
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture( GL_TEXTURE_2D, 0);

	}
	Image<T,C,I>& read(){
		Application::getContext()->begin();
		if (textureId){
			glBindTexture(GL_TEXTURE_2D, textureId);
			if(textureImage.channel==4){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_RGBA, textureImage.ptr());
			} else if(textureImage.channel==3){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_RGB, textureImage.ptr());
			} else if(textureImage.channel==1){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_R, textureImage.ptr());
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			Application::getContext()->end();
		} else {
			Application::getContext()->end();
			throw std::runtime_error("Count not read image, texture buffer not allocated.");
		}
		return textureImage;
	}
	virtual void draw(AlloyContext* context) override;
public:
	GLShader* getShader();
	inline void setShadeEnabled(bool shade){
		enableShader=shade;
	}
	void setShader(GLShader* shader){
		this->imageShader=shader;
	}
	inline int width(){return textureImage.width;}
	inline int height(){return textureImage.height;}
	vec<T,C>& operator()(const int i,const int j){
		return textureImage(i,j);
	}
	GLTexture():GLComponent(),imageShader(NULL),textureId(0),enableShader(true){
	}
	GLTexture(int x,int y,int width,int height,int imageWidth,int imageHeight):GLComponent(){
		textureImage.resize(imageWidth,imageHeight);
		bounds=box2i({x,y},{width,height});
	}
	GLTexture(const Image<T,C,I>& image):GLComponent(){
		textureImage.set(image);
		bounds=box2i(0,0,textureImage.width,textureImage.height);
	}
	virtual ~GLTexture(){
		Application::getContext()->begin();
		if(textureId){
			glDeleteTextures(1,&textureId);
			textureId=0;
		}

		Application::getContext()->end();
	}
};

}

#endif /* IMAGE_H_ */
