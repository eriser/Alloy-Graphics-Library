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
#include "AlloyUnits.h"
#include "AlloyImage.h"
int printOglError(const char *file, int line);
#define CHECK_GL_ERROR() printOglError(__FILE__, __LINE__)


namespace aly{
	struct ImageVAO{
		GLuint vao=0;
		GLuint positionBuffer=0;
		GLuint uvBuffer=0;
	};
	const RGBA COLOR_NONE(0,0,0,0);
	const RGBA COLOR_BLACK(0,0,0,255);
	const RGBA COLOR_WHITE(255,255,255,255);

	enum class GlyphType{Image, Awesome};
    enum class InputType {Unspecified,Cursor,MouseButton,Key,Character,Scroll};
    enum class HorizontalAlignment { Left=NVG_ALIGN_LEFT, Center=NVG_ALIGN_CENTER, Right=NVG_ALIGN_RIGHT };
	enum class VerticalAlignment { Top=NVG_ALIGN_TOP, Middle=NVG_ALIGN_MIDDLE, Bottom=NVG_ALIGN_BOTTOM, Baseline=NVG_ALIGN_BASELINE };
	enum class AspectRatio { Unspecified, FixedWidth, FixedHeight };
	enum class Shape { Rectangle, Ellipse};
    enum class Orientation {Unspecified=0, Horizontal=1, Vertical=2 };
	enum class FontType {Normal=0,Bold=1,Italic=2,Icon=3};
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const GlyphType& type) {
		switch(type){
			case GlyphType::Image: return ss<<"Image";
			case GlyphType::Awesome: return ss<<"Awesome";
		}
		return ss;
    }

	template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const FontType& type) {
		switch(type){
			case FontType::Normal: return ss<<"Normal";
			case FontType::Bold: return ss<<"Bold";
			case FontType::Italic: return ss<<"Italic";
			case FontType::Icon: return ss<<"Icon";
		}
		return ss;
    }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const InputType& type) {
		switch(type){
			case InputType::Unspecified: return ss<<"Unspecified";
			case InputType::Cursor: return ss<<"Cursor";
			case InputType::MouseButton: return ss<<"Mouse";
			case InputType::Key: return ss<<"Key";
			case InputType::Character: return ss<<"Character";
			case InputType::Scroll: return ss<<"Scroll";
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
	struct Glyph{
		const std::string name;
		pixel width;
		pixel height;
		const GlyphType type;
		Glyph(const std::string& name,GlyphType type,pixel w,pixel h):name(name),type(type),width(w),height(h){

		}
		virtual inline ~Glyph(){};
		virtual void draw(const box2px& bounds,const Color& color,AlloyContext* context)=0;
	};
	struct ImageGlyph: public Glyph{
		int handle;
		const std::string file;
		ImageGlyph(const std::string& file,AlloyContext* context,bool mipmap=false);
		ImageGlyph(const ImageRGBA& rgba,AlloyContext* context,bool mipmap=false);
		void draw(const box2px& bounds,const Color& color,AlloyContext* context) override;
	};
	struct AwesomeGlyph: public Glyph{
		const int codePoint;
		AwesomeGlyph(int codePoint,AlloyContext* context,pixel fontHeight=32);
		void draw(const box2px& bounds,const Color& color,AlloyContext* context) override;
	};
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Font & v) { return ss <<"Font: "<<v.name<<"["<<v.handle<<"]: \""<<v.file<<"\""; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const ImageGlyph & v) { return ss <<"ImageGlyph: "<<v.name<<"["<<v.handle<<"]: dimensions= ("<<v.width<<", "<<v.height<<")"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const AwesomeGlyph & v) { return ss <<"Awesome Glyph: "<<v.name<<"["<<v.codePoint<<"]: dimensions= ("<<v.width<<", "<<v.height<<")"; }
    template<class C, class R> std::basic_ostream<C,R> & operator << (std::basic_ostream<C,R> & ss, const Glyph & v) { return ss <<"Glyph: "<<v.name<<": dimensions= ("<<v.width<<", "<<v.height<<")"; }

    struct InputEvent
    {
        InputType type=InputType::Unspecified;
        pixel2 cursor=pixel2(-1,-1);
        pixel2 scroll=pixel2(0,0);
        uint32_t codepoint=0;
        int action=0;
        int mods=0;
        int scancode=0;
        int key=0;
        int button=0;
        bool isMouseDown() const { return (action != GLFW_RELEASE); }
        bool isMouseUp() const { return (action == GLFW_RELEASE); }
        bool isShiftDown() const { return ((mods & GLFW_MOD_SHIFT)!=0); }
	    bool isControlDown() const { return ((mods & GLFW_MOD_CONTROL)!=0); }
	    bool isAltDown() const { return ((mods & GLFW_MOD_ALT)!=0); }
    };

    /*
    static InputEvent MakeInputEvent(GLFWwindow * window, InputEvent::Type type, const float2 & cursor, int action)
    {
        InputEvent e;
        e.window = window;
        e.type = type;
        e.cursor = cursor;
        e.action = action;
        e.mods = 0;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) | glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) e.mods |= GLFW_MOD_SHIFT;
        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) | glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL)) e.mods |= GLFW_MOD_CONTROL;
        if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) | glfwGetKey(window, GLFW_KEY_RIGHT_ALT)) e.mods |= GLFW_MOD_ALT;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SUPER) | glfwGetKey(window, GLFW_KEY_RIGHT_SUPER)) e.mods |= GLFW_MOD_SUPER;
        return e;
    }
    */

	struct AlloyContext {
		private:
			std::list<std::string> assetDirectories;
			std::shared_ptr<Font> fonts[4];
			static std::mutex contextLock;
			GLFWwindow* current;
			bool enableDebugInterface=false;
		public:
			ImageVAO vaoImage;
			NVGcontext* nvgContext;
			GLFWwindow* window;
			box2i viewport;
			pixel2 cursor=pixel2(-1,-1);
			double2 dpmm;
			double pixelRatio;
			inline void setDebug(bool enabled){
				enableDebugInterface=enabled;
			}
			inline bool toggleDebug(){
				return enableDebugInterface=!enableDebugInterface;
			}
			inline bool isDebugEnabled(){
				return enableDebugInterface;
			}
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
				return std::shared_ptr<ImageGlyph>(new ImageGlyph(fileName,this));
			}
			inline std::shared_ptr<ImageGlyph> createImageGlyph(const ImageRGBA& img,bool mipmap=false){
				return std::shared_ptr<ImageGlyph>(new ImageGlyph(img,this));
			}
			inline std::shared_ptr<AwesomeGlyph> createAwesomeGlyph(int codePoint){
				return std::shared_ptr<AwesomeGlyph>(new AwesomeGlyph(codePoint,this));
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
