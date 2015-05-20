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
#include <list>
#include <map>
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
    enum class HorizontalAlignment { Left=NVG_ALIGN_LEFT, Center=NVG_ALIGN_CENTER, Right=NVG_ALIGN_RIGHT };
	enum class VerticalAlignment { Top=NVG_ALIGN_TOP, Middle=NVG_ALIGN_MIDDLE, Bottom=NVG_ALIGN_BOTTOM, Baseline=NVG_ALIGN_BASELINE };
    enum class Shape { Rectangle, Ellipse};
    enum class Orientation { Horizontal, Vertical };
	enum class FontType {Bold,Italic,Normal,Icon};
    class AlloyContext;
	struct Font{
		int handle;
		const std::string name;
		const std::string file;
		Font(const std::string& name,const std::string& file,AlloyContext* context);
	};
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Font & v) { return ss << v.name<<" ["<<v.handle<<"] ["<<v.file<<"]"; }

	struct AlloyContext {
		private:
			std::list<std::string> assetDirectories;
			std::map<FontType,std::shared_ptr<Font>> fonts;
			static std::mutex contextLock;
			GLFWwindow* current;
		public:
			GLGlobalImage globalImage;
			NVGcontext* nvgContext;
			GLFWwindow* window;
			box2i viewport;
			double2 dpmm;
			void addAssetDirectory(const std::string& dir);
			std::shared_ptr<Font>& loadFont(FontType type,const std::string& name,const std::string& file);
			std::string getFullPath(const std::string& partialFile);
			inline int width(){return viewport.dimensions.x;}
			inline int height(){return viewport.dimensions.y;}
			inline const char* getFontName(FontType type){
				return fonts[type]->name.c_str();
			}
			inline int getFontHandle(FontType type){
				return fonts[type]->handle;
			}
			AlloyContext(int width,int height,const std::string& title);
			bool begin();
			bool end();
			void makeCurrent();
			~AlloyContext();
	};
}
#endif /* ALLOYCONTEXT_H_ */
