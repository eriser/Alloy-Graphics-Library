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

#ifndef ALLOYCONTEXT_H_
#define ALLOYCONTEXT_H_
#define GLFW_INCLUDE_GLU

#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GLFW/glfw3.h>
#include <mutex>
#include <memory>
#include "nanovg.h"
#include "AlloyMath.h"
int printOglError(const char *file, int line);
#define CHECK_GL_ERROR() printOglError(__FILE__, __LINE__)


namespace aly{
	struct GLGlobalImage{
		GLuint vao=0;
		GLuint positionBuffer=0;
		GLuint uvBuffer=0;
	};
	struct AlloyContext {
		private:
			static std::mutex contextLock;
			GLFWwindow* current;
		public:
			GLGlobalImage globalImage;
			NVGcontext* nvgContext;
			GLFWwindow* window;
			box2i viewport;
			double2 dpmm;
			inline int width(){return viewport.dimensions.x;}
			inline int height(){return viewport.dimensions.y;}

			AlloyContext(int width,int height,const std::string& title);
			bool begin();
			bool end();
			void makeCurrent();
			~AlloyContext();
	};
}
#endif /* ALLOYCONTEXT_H_ */
