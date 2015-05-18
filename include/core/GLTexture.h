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
template<class T,int C,ImageType I> class GLTexture:public GLComponent {
protected:
	Image<T,C,I> textureImage;
	static std::unique_ptr<GLShader> defaultShader;
	GLShader* imageShader=false;
	bool enableShader=true;
	GLuint vao=0;
	GLuint positionBuffer=0;
	GLuint uvBuffer=0;
	GLuint textureId=0;
	GLuint internalFormat = GL_DEPTH_COMPONENT;
	virtual void update(AlloyContext* context) override{
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
		if(vao==0){
			glGenVertexArrays (1, &vao);
			if(positionBuffer==0){
				glGenBuffers(1, &positionBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) *3* 4,PositionCoords, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindTexture( GL_TEXTURE_2D, 0);
			}
			if(uvBuffer==0){
				glGenBuffers(1, &uvBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2* 4,TextureCoords, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}
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
	virtual void draw(AlloyContext* context) override{
		glBindVertexArray (vao);
		GLShader* shader=NULL;
		if(enableShader){
			shader=getShader();
			shader->begin();
			glEnable(GL_BLEND);
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(shader->GetProgramHandle(),"textureImage"),0);;
			glUniform2f(glGetUniformLocation(shader->GetProgramHandle(),"IMG_POS"),bounds.pt.x,bounds.pt.y);
			glUniform2f(glGetUniformLocation(shader->GetProgramHandle(),"IMG_DIMS"),bounds.dims.x,bounds.dims.y);
			glUniform2f(glGetUniformLocation(shader->GetProgramHandle(),"SCREEN_DIMS"),context->width(),context->height());
			glUniform1i(glGetUniformLocation(shader->GetProgramHandle(),"showBackground"),0);
			glBindTexture(GL_TEXTURE_2D,textureId);
		}
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
		if(enableShader){
			glBindTexture(GL_TEXTURE_2D, 0);
			shader->end();
		}
	}
public:
	GLShader* getShader(){
		if(imageShader==NULL){
			if(defaultShader.get()==nullptr){
				std::vector<std::string> attrib={"vp","uv"};
				defaultShader=std::unique_ptr<GLShader>(new GLShader(
R"(
#version 330
in vec3 vp; 
uniform vec2 IMG_POS;
uniform vec2 IMG_DIMS;
uniform vec2 SCREEN_DIMS;
out vec3 pos3d;
void main() {
	pos3d=vp;
	vec2 pos=(vp.xy*IMG_DIMS+IMG_POS);
	pos.x=2*pos.x/SCREEN_DIMS.x-1.0;
	pos.y=1.0-2*pos.y/SCREEN_DIMS.y;
  gl_Position = vec4(pos.x,pos.y,0,1);
})",
R"(
#version 330
in vec3 pos3d;
uniform sampler2D textureImage;
uniform vec2 IMG_DIMS;
void main() {
	vec4 rgba=texture2D(textureImage,pos3d.xy);
	gl_FragColor=rgba;
})","",attrib));
			}
			imageShader=defaultShader.get();
		}
		return imageShader;
	}

	static const float2 TextureCoords[4];
	static const float3 PositionCoords[4];
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
		if(vao){
			glDeleteVertexArrays(1,&vao);
		}
		if(uvBuffer){
			glDeleteBuffers(1,&uvBuffer);
		}
		if(positionBuffer){
			glDeleteBuffers(1,&positionBuffer);
		}
		Application::getContext()->end();
	}
};

}

#endif /* IMAGE_H_ */
