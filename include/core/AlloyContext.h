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
#include <string>
#include "nanovg.h"
#include "AlloyMath.h"
#include "AlloyImage.h"
int printOglError(const char *file, int line);
#define CHECK_GL_ERROR() printOglError(__FILE__, __LINE__)


namespace aly{
	struct ImageVAO{
		GLuint vao=0;
		GLuint positionBuffer=0;
		GLuint uvBuffer=0;
	};
    enum class HorizontalAlignment { Left=NVG_ALIGN_LEFT, Center=NVG_ALIGN_CENTER, Right=NVG_ALIGN_RIGHT };
	enum class VerticalAlignment { Top=NVG_ALIGN_TOP, Middle=NVG_ALIGN_MIDDLE, Bottom=NVG_ALIGN_BOTTOM, Baseline=NVG_ALIGN_BASELINE };
	enum class AspectRatio { Unspecified, FixedWidth, FixedHeight };
	enum class Shape { Rectangle, Ellipse};
    enum class Orientation {Unspecified=0, Horizontal=1, Vertical=2 };
	enum class FontType {Normal=0,Bold=1,Italic=2,Icon=3};
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const FontType& type) {
		switch(type){
			case FontType::Normal: return ss<<"Normal";
			case FontType::Bold: return ss<<"Bold";
			case FontType::Italic: return ss<<"Italic";
			case FontType::Icon: return ss<<"Icon";
		}
		return ss;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const HorizontalAlignment& type) {
		switch(type){
			case HorizontalAlignment::Left: return ss<<"Left";
			case HorizontalAlignment::Center: return ss<<"Center";
			case HorizontalAlignment::Right: return ss<<"Right";
		}
		return ss;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const VerticalAlignment& type) {
		switch(type){
			case VerticalAlignment::Top: return ss<<"Top";
			case VerticalAlignment::Middle: return ss<<"Middle";
			case VerticalAlignment::Bottom: return ss<<"Bottom";
			case VerticalAlignment::Baseline: return ss<<"Baseline";
		}
		return ss;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Orientation& type) {
		switch(type){
			case Orientation::Unspecified: return ss<<"Unspecified";
			case Orientation::Horizontal: return ss<<"Horizontal";
			case Orientation::Vertical: return ss<<"Vertical";
		}
		return ss;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const AspectRatio& type) {
		switch(type){
			case AspectRatio::Unspecified: return ss<<"Unspecified";
			case AspectRatio::FixedWidth: return ss<<"Fixed Width";
			case AspectRatio::FixedHeight: return ss<<"Fixed Height";
		}
		return ss;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Shape& type) {
		switch(type){
			case Shape::Rectangle: return ss<<"Rectangle";
			case Shape::Ellipse: return ss<<"Ellipse";
		}
		return ss;
    }
    class AlloyContext;
	struct Font{
		int handle;
		const std::string name;
		const std::string file;
		Font(const std::string& name,const std::string& file,AlloyContext* context);
	};
	struct ImageGlyph{
		int handle;
		const std::string file;
		const std::string name;
		int width;
		int height;
		ImageGlyph(const std::string& file,AlloyContext* context,bool mipmap=false);
		ImageGlyph(const ImageRGBA& rgba,AlloyContext* context,bool mipmap=false);

	};
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Font & v) { return ss <<"Font "<<v.name<<"["<<v.handle<<"]: \""<<v.file<<"\""; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const ImageGlyph & v) { return ss <<"Image "<<v.name<<"["<<v.handle<<"]: dimensions={"<<v.width<<","<<v.height<<"}"; }

	struct AlloyContext {
		private:
			std::list<std::string> assetDirectories;
			std::shared_ptr<Font> fonts[4];
			static std::mutex contextLock;
			GLFWwindow* current;
		public:
			ImageVAO vaoImage;
			NVGcontext* nvgContext;
			GLFWwindow* window;
			box2i viewport;
			double2 dpmm;
			double pixelRatio;
			std::list<std::string>& getAssetDirectories(){return assetDirectories;}
			void addAssetDirectory(const std::string& dir);
			std::shared_ptr<Font>& loadFont(FontType type,const std::string& name,const std::string& partialFile);
			std::string getFullPath(const std::string& partialFile);
			inline int width(){return viewport.dimensions.x;}
			inline int height(){return viewport.dimensions.y;}
			inline const char* getFontName(FontType type){
				if(fonts[static_cast<int>(type)].get()==nullptr)throw std::runtime_error("Font type not found.");
				return fonts[static_cast<int>(type)]->name.c_str();
			}
			inline int getFontHandle(FontType type){
				if(fonts[static_cast<int>(type)].get()==nullptr)throw std::runtime_error("Font type not found.");
				return fonts[static_cast<int>(type)]->handle;
			}
			inline std::shared_ptr<ImageGlyph> createImageGlyph(const std::string& fileName,bool mipmap=false){
				return std::shared_ptr<ImageGlyph>(new ImageGlyph(getFullPath("images/robot.png"),this));
			}
			inline std::shared_ptr<ImageGlyph> createImageGlyph(const ImageRGBA& img,bool mipmap=false){
				return std::shared_ptr<ImageGlyph>(new ImageGlyph(img,this));
			}
			inline std::shared_ptr<Font>& getFont(FontType type){
				return fonts[static_cast<int>(type)];
			}
			AlloyContext(int width,int height,const std::string& title);
			bool begin();
			bool end();
			void makeCurrent();
			~AlloyContext();
	};
}
#endif /* ALLOYCONTEXT_H_ */
