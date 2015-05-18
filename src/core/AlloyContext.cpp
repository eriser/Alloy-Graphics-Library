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
namespace aly{
	std::mutex AlloyContext::contextLock;
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
			nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
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
			nvgDeleteGL3(nvgContext);
			glfwDestroyWindow(window);
			glfwTerminate();
		}
}

