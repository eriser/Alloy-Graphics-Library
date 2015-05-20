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

#include "AlloyContext.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "../../include/core/nanovg_gl.h"
#include <iostream>
int printOglError(const char *file, int line)
{

    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n",
			     file, line, gluErrorString(glErr));
        retCode = 1;
    }
    return retCode;
}
namespace aly{
	std::mutex AlloyContext::contextLock;
	Font::Font(const std::string& name,const std::string& file,AlloyContext* context):name(name),file(file){
		handle=nvgCreateFont(context->nvgContext,name.c_str(),file.c_str());
	}
	AlloyContext::AlloyContext(int width,int height,const std::string& title):window(nullptr),nvgContext(nullptr),current(nullptr){
			std::lock_guard<std::mutex> lock(contextLock);
			if (glfwInit() != GL_TRUE) {
				throw std::runtime_error("Could not initialize GLFW.");
			}
			glfwSetErrorCallback([](int error, const char* desc){std::cout<<"GLFW Error ["<<error<<"] "<<desc<<std::endl;});
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
			window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
			if (!window) {
				glfwTerminate();

				throw std::runtime_error("Could not create window.");
			}
			glfwMakeContextCurrent(window);
			glewExperimental = GL_TRUE;
			if(glewInit() != GLEW_OK) {
				throw std::runtime_error("Could not initialize GLEW.");
			}
			glGetError();
			glViewport(0,0,width,height);
			viewport=box2i(int2(0,0),int2(width,height));
			nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
			float2 TextureCoords[4]={float2(1.0f,1.0f),float2(0.0f,1.0f),float2(0.0f,0.0f),float2(1.0f,0.0f)};
			float3 PositionCoords[4]={float3(1.0f,1.0f,0.0f),float3(0.0f,1.0f,0.0f),float3(0.0f,0.0f,0.0f),float3(1.0f,0.0f,0.0f)};

			glGenVertexArrays (1, &globalImage.vao);
			glBindVertexArray (globalImage.vao);
				glGenBuffers(1, &globalImage.positionBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, globalImage.positionBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) *3* 4,PositionCoords, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				glGenBuffers(1, &globalImage.uvBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, globalImage.uvBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2* 4,TextureCoords, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			int widthMM, heightMM;

			GLFWmonitor* monitor=glfwGetWindowMonitor(window);
			const GLFWvidmode* mode =glfwGetVideoMode(monitor);
			glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
			dpmm = double2(mode->width / (double)widthMM ,mode->height / (double)heightMM );
		}
		bool AlloyContext::begin(){
			if(current==nullptr){
				contextLock.lock();
				current=glfwGetCurrentContext();
				glfwMakeContextCurrent(window);
				return true;
			} return false;
		}
		bool AlloyContext::end(){
			if(current!=nullptr){
				glfwMakeContextCurrent(current);
				current=nullptr;
				contextLock.unlock();
				return true;
			} else return false;
		}
		void AlloyContext::makeCurrent(){
			std::lock_guard<std::mutex> lock(contextLock);
			glfwMakeContextCurrent(window);
		}
		AlloyContext::~AlloyContext(){
			std::lock_guard<std::mutex> lock(contextLock);
			GLFWwindow* current=glfwGetCurrentContext();
			glfwMakeContextCurrent(window);
			if(globalImage.vao){
				glDeleteVertexArrays(1,&globalImage.vao);
			}
			if(globalImage.uvBuffer){
				glDeleteBuffers(1,&globalImage.uvBuffer);
			}
			if(globalImage.positionBuffer){
				glDeleteBuffers(1,&globalImage.positionBuffer);
			}
			nvgDeleteGL3(nvgContext);
			glfwDestroyWindow(window);
			glfwTerminate();
		}
}

