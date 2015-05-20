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
template<class T,int C,ImageType I> class GLTexture: public GLComponent {
protected:
	Image<T,C,I> textureImage;
	bool enableShader=true;
	GLuint internalFormat = GL_DEPTH_COMPONENT;
public:
	GLuint textureId=0;
	virtual void draw() override{
		GLuint& vao=context->globalImage.vao;
		GLuint& positionBuffer=context->globalImage.positionBuffer;
		GLuint& uvBuffer=context->globalImage.uvBuffer;
		glBindVertexArray (vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		if(enableShader){
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glDrawArrays(GL_QUADS,0,4);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}

	virtual void update() override{
		GLuint& vao=context->globalImage.vao;
		GLuint& positionBuffer=context->globalImage.positionBuffer;
		GLuint& uvBuffer=context->globalImage.uvBuffer;
		if(textureId==0){
			glGenTextures( 1,&textureId);
		}
		glBindTexture( GL_TEXTURE_2D, textureId);
		switch(textureImage.type){
			case ImageType::FLOAT:
				if(textureImage.channels==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32F, textureImage.width, textureImage.height, 0,GL_RGBA,GL_FLOAT, &textureImage[0]);
				} else if(textureImage.channels==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32F, textureImage.width, textureImage.height, 0,GL_RGB,GL_FLOAT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32F, textureImage.width, textureImage.height, 0,GL_R,GL_FLOAT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeName());
				}
				break;
			case ImageType::UBYTE:
				if(textureImage.channels==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA, textureImage.width, textureImage.height, 0,GL_RGBA,GL_UNSIGNED_BYTE, &textureImage[0]);
				} else if(textureImage.channels==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB, textureImage.width, textureImage.height, 0,GL_RGB,GL_UNSIGNED_BYTE, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R, textureImage.width, textureImage.height, 0,GL_R,GL_UNSIGNED_BYTE, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeName());
				}
				break;
			case ImageType::USHORT:
				if(textureImage.channels==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA16UI, textureImage.width, textureImage.height, 0,GL_RGBA,GL_UNSIGNED_SHORT, &textureImage[0]);
				} else if(textureImage.channels==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB16UI, textureImage.width, textureImage.height, 0,GL_RGB,GL_UNSIGNED_SHORT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R16UI, textureImage.width, textureImage.height, 0,GL_R,GL_UNSIGNED_SHORT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeName());
				}
				break;
			case ImageType::UINT:
				if(textureImage.channels==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32UI, textureImage.width, textureImage.height, 0,GL_RGBA,GL_UNSIGNED_INT, &textureImage[0]);
				} else if(textureImage.channels==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32UI, textureImage.width, textureImage.height, 0,GL_RGB,GL_UNSIGNED_INT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32UI, textureImage.width, textureImage.height, 0,GL_R,GL_UNSIGNED_INT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeName());
				}
				break;
			case ImageType::BYTE:
				if(textureImage.channels==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA, textureImage.width, textureImage.height, 0,GL_RGBA,GL_BYTE, &textureImage[0]);
				} else if(textureImage.channels==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB, textureImage.width, textureImage.height, 0,GL_RGB,GL_BYTE, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R, textureImage.width, textureImage.height, 0,GL_R,GL_BYTE, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeName());
				}
				break;
			case ImageType::SHORT:
				if(textureImage.channels==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA16UI, textureImage.width, textureImage.height, 0,GL_RGBA,GL_SHORT, &textureImage[0]);
				} else if(textureImage.channels==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB16UI, textureImage.width, textureImage.height, 0,GL_RGB,GL_SHORT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R16UI, textureImage.width, textureImage.height, 0,GL_R,GL_SHORT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeName());
				}
				break;
			case ImageType::INT:
				if(textureImage.channels==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32UI, textureImage.width, textureImage.height, 0,GL_RGBA,GL_INT, &textureImage[0]);
				} else if(textureImage.channels==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32UI, textureImage.width, textureImage.height, 0,GL_RGB,GL_INT, &textureImage[0]);
				} else if(textureImage.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32UI, textureImage.width, textureImage.height, 0,GL_R,GL_INT, &textureImage[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeName());
				}
				break;
			default:
				throw std::runtime_error(MakeString()<<"Texture format not supported "<<textureImage.getTypeName());
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture( GL_TEXTURE_2D, 0);

	}
	Image<T,C,I>& read(){
		context->begin();
		if (textureId){
			glBindTexture(GL_TEXTURE_2D, textureId);
			if(textureImage.channels==4){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_RGBA, textureImage.ptr());
			} else if(textureImage.channels==3){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_RGB, textureImage.ptr());
			} else if(textureImage.channels==1){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_R, textureImage.ptr());
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			context->end();
		} else {
			context->end();
			throw std::runtime_error("Count not read image, texture buffer not allocated.");
		}
		return textureImage;
	}
public:
	inline int width(){return textureImage.width;}
	inline int height(){return textureImage.height;}
	vec<T,C>& operator()(const int i,const int j){
		return textureImage(i,j);
	}
	GLTexture(std::shared_ptr<AlloyContext>& context):GLComponent(context),textureId(0){
	}
	GLTexture(int x,int y,int width,int height,int imageWidth,int imageHeight,std::shared_ptr<AlloyContext>& context):GLComponent(context){
		textureImage.resize(imageWidth,imageHeight);
		bounds=box2i({x,y},{width,height});
	}
	GLTexture(const Image<T,C,I>& image,std::shared_ptr<AlloyContext>& context):GLComponent(context){
		textureImage.set(image);
		bounds=box2i({0,0},{textureImage.width,textureImage.height});
	}
	GLTexture(int x,int y,int width,int height,int imageWidth,int imageHeight);
	GLTexture(const Image<T,C,I>& image);
	virtual ~GLTexture(){
		context->begin();
		if(textureId){
			glDeleteTextures(1,&textureId);
			textureId=0;
		}

		context->end();
	}
};
	typedef GLTexture<uint8_t,4,ImageType::UBYTE> GLTextureRGBA;
	typedef GLTexture<uint8_t,3,ImageType::UBYTE> GLTextureRGB;
	typedef GLTexture<float,4,ImageType::FLOAT> GLTextureRGBAf;
	typedef GLTexture<float,3,ImageType::FLOAT> GLTextureRGBf;

}

#endif /* IMAGE_H_ */
