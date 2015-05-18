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
	GLuint internalFormat = GL_DEPTH_COMPONENT;
	virtual void update(AlloyContext* context) override{
		if(mTextureId==0){
			glGenTextures( 1,&mTextureId);
		}
		glBindTexture( GL_TEXTURE_2D, mTextureId);
		switch(mData.type){
			case ImageType::FLOAT:
				if(mData.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32F, mData.width, mData.height, 0,GL_RGBA,GL_FLOAT, &mData[0]);
				} else if(mData.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32F, mData.width, mData.height, 0,GL_RGB,GL_FLOAT, &mData[0]);
				} else if(mData.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32F, mData.width, mData.height, 0,GL_R,GL_FLOAT, &mData[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<mData.getTypeString());
				}
				break;
			case ImageType::UBYTE:
				if(mData.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA, mData.width, mData.height, 0,GL_RGBA,GL_UNSIGNED_BYTE, &mData[0]);
				} else if(mData.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB, mData.width, mData.height, 0,GL_RGB,GL_UNSIGNED_BYTE, &mData[0]);
				} else if(mData.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R, mData.width, mData.height, 0,GL_R,GL_UNSIGNED_BYTE, &mData[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<mData.getTypeString());
				}
				break;
			case ImageType::USHORT:
				if(mData.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA16UI, mData.width, mData.height, 0,GL_RGBA,GL_UNSIGNED_SHORT, &mData[0]);
				} else if(mData.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB16UI, mData.width, mData.height, 0,GL_RGB,GL_UNSIGNED_SHORT, &mData[0]);
				} else if(mData.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R16UI, mData.width, mData.height, 0,GL_R,GL_UNSIGNED_SHORT, &mData[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<mData.getTypeString());
				}
				break;
			case ImageType::UINT:
				if(mData.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32UI, mData.width, mData.height, 0,GL_RGBA,GL_UNSIGNED_INT, &mData[0]);
				} else if(mData.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32UI, mData.width, mData.height, 0,GL_RGB,GL_UNSIGNED_INT, &mData[0]);
				} else if(mData.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32UI, mData.width, mData.height, 0,GL_R,GL_UNSIGNED_INT, &mData[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<mData.getTypeString());
				}
				break;
			case ImageType::BYTE:
				if(mData.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA, mData.width, mData.height, 0,GL_RGBA,GL_BYTE, &mData[0]);
				} else if(mData.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB, mData.width, mData.height, 0,GL_RGB,GL_BYTE, &mData[0]);
				} else if(mData.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R, mData.width, mData.height, 0,GL_R,GL_BYTE, &mData[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<mData.getTypeString());
				}
				break;
			case ImageType::SHORT:
				if(mData.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA16UI, mData.width, mData.height, 0,GL_RGBA,GL_SHORT, &mData[0]);
				} else if(mData.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB16UI, mData.width, mData.height, 0,GL_RGB,GL_SHORT, &mData[0]);
				} else if(mData.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R16UI, mData.width, mData.height, 0,GL_R,GL_SHORT, &mData[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<mData.getTypeString());
				}
				break;
			case ImageType::INT:
				if(mData.channel==4){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGBA32UI, mData.width, mData.height, 0,GL_RGBA,GL_INT, &mData[0]);
				} else if(mData.channel==3){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_RGB32UI, mData.width, mData.height, 0,GL_RGB,GL_INT, &mData[0]);
				} else if(mData.channels==1){
					glTexImage2D( GL_TEXTURE_2D, 0, internalFormat=GL_R32UI, mData.width, mData.height, 0,GL_R,GL_INT, &mData[0]);
				} else {
					throw std::runtime_error(MakeString()<<"Texture format not supported "<<mData.getTypeString());
				}
				break;
			default:
				throw std::runtime_error(MakeString()<<"Texture format not supported "<<mData.getTypeString());
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture( GL_TEXTURE_2D, 0);
		if(vao==0){
			glGenVertexArrays (1, &vao);
			if(mPositionBuffer==0){
				glGenBuffers(1, &mPositionBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) *3* 4,PositionCoords, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindTexture( GL_TEXTURE_2D, 0);
			}
			if(mUVBuffer==0){
				glGenBuffers(1, &mUVBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2* 4,TextureCoords, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}
	}
	Image<T,C,I>& read(){
		if (mTextureId){
			glBindTexture(GL_TEXTURE_2D, mTextureId);
			if(mData.channel==4){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_RGBA, mData.ptr());
			} else if(mData.channel==3){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_RGB, mData.ptr());
			} else if(mData.channel==1){
				glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, GL_R, mData.ptr());
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		} else
			throw std::runtime_error("Count not read image, texture buffer not allocated.");
		return mData;
	}
	virtual void draw(AlloyContext* context) override{
		glBindVertexArray (vao);
		GLShader* shader=NULL;
		if(mShadeEnabled){
			shader=getShader();
			shader->begin();
			glEnable(GL_BLEND);
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(glGetUniformLocation(shader->GetProgramHandle(),"textureImage"),0);;
			glUniform2f(glGetUniformLocation(shader->GetProgramHandle(),"IMG_POS"),bounds.pt.x,bounds.pt.y);
			glUniform2f(glGetUniformLocation(shader->GetProgramHandle(),"IMG_DIMS"),bounds.dims.x,bounds.dims.y);
			glUniform2f(glGetUniformLocation(shader->GetProgramHandle(),"SCREEN_DIMS"),context->width(),context->height());
			glUniform1i(glGetUniformLocation(shader->GetProgramHandle(),"showBackground"),0);
			glBindTexture(GL_TEXTURE_2D,mTextureId);
		}
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mPositionBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		if(mShadeEnabled){
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glDrawArrays(GL_QUADS,0,4);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		if(mShadeEnabled){
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
	GLImage(int x,int y,int width,int height,int imageWidth,int imageHeight):GLComponent(),imageShader(NULL),mTextureId(0),mShadeEnabled(true){
		mData.resize(imageWidth,imageHeight);
		bounds=box2i({x,y},{width,height});
	}
	GLImage(const Image<T,C,I>& image);
	static GLImage* read(const std::string& file);
	bool write(const std::string& file);
	bool writeTexture(const std::string& file);
	virtual ~GLImage();
};

}

#endif /* IMAGE_H_ */
