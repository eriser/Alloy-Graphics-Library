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

#include "AlloyContext.h"
#include "AlloyMath.h"
#include "AlloyApplication.h"
#include "GLShader.h"

namespace aly {
/*
template<class T,int C,ImageType I> GLShader* GLTexture<T,C,I>::getShader(){
	if(imageShader==NULL){
		if(defaultShader.get()==nullptr){
			std::vector<std::string> attrib={"vp","uv"};
			defaultShader=std::unique_ptr<GLShader>(new GLShader(attrib,
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
})"));
		}
		imageShader=defaultShader.get();
	}
	return imageShader;
}
*/
template<class T,int C,ImageType I> GLTexture<T,C,I>::GLTexture(int x,int y,int width,int height,int imageWidth,int imageHeight):
		GLTexture<T,C,I>(x,y,width,height,imageWidth,imageHeight,Application::getContext()){
}
template<class T,int C,ImageType I> GLTexture<T,C,I>::GLTexture(const Image<T,C,I>& image):
		GLTexture<T,C,I>(image,Application::getContext()){
}
}
