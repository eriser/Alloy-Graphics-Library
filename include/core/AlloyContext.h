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

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/glew.h>
#else
#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

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
#include "AlloyAnimator.h"
#include "AlloyEnum.h"
#include "AlloyCursorLocator.h"
int printOglError(const char *file, int line);
#define CHECK_GL_ERROR() printOglError(__FILE__, __LINE__)

namespace aly {

struct ImageVAO {
	GLuint vao = 0;
	GLuint positionBuffer = 0;
	GLuint uvBuffer = 0;
};
const RGBA COLOR_NONE(0, 0, 0, 0);
const RGBA COLOR_BLACK(0, 0, 0, 255);
const RGBA COLOR_WHITE(255, 255, 255, 255);

class AlloyContext;
struct Font {
	int handle;
	const std::string name;
	const std::string file;
	Font(const std::string& name, const std::string& file,
			AlloyContext* context);
};
struct Glyph {
	const std::string name;
	pixel width;
	pixel height;
	const GlyphType type;
	Glyph(const std::string& name, GlyphType type, pixel w, pixel h) :
			name(name), type(type), width(w), height(h) {

	}
	virtual inline ~Glyph() {
	}
	;
	virtual void draw(const box2px& bounds, const Color& color,
			AlloyContext* context)=0;
};
struct ImageGlyph: public Glyph {
	int handle;
	const std::string file;
	ImageGlyph(const std::string& file, AlloyContext* context, bool mipmap =
			false);
	ImageGlyph(const ImageRGBA& rgba, AlloyContext* context,
			bool mipmap = false);
	void draw(const box2px& bounds, const Color& color, AlloyContext* context)
			override;
};
struct AwesomeGlyph: public Glyph {
	const int codePoint;
	AwesomeGlyph(int codePoint, AlloyContext* context, pixel fontHeight = 32);
	void draw(const box2px& bounds, const Color& color, AlloyContext* context)
			override;
};
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Font & v) {
	return ss << "Font: " << v.name << "[" << v.handle << "]: \"" << v.file
			<< "\"";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const ImageGlyph & v) {
	return ss << "ImageGlyph: " << v.name << "[" << v.handle
			<< "]: dimensions= (" << v.width << ", " << v.height << ")";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const AwesomeGlyph & v) {
	return ss << "Awesome Glyph: " << v.name << "[" << v.codePoint
			<< "]: dimensions= (" << v.width << ", " << v.height << ")";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Glyph & v) {
	return ss << "Glyph: " << v.name << ": dimensions= (" << v.width << ", "
			<< v.height << ")";
}
struct InputEvent {
	InputType type = InputType::Unspecified;
	pixel2 cursor = pixel2(-1, -1);
	pixel2 scroll = pixel2(0, 0);
	uint32_t codepoint = 0;
	int action = 0;
	int mods = 0;
	int scancode = 0;
	int key = 0;
	int button = 0;
	bool isMouseDown() const {
		return (action != GLFW_RELEASE);
	}
	bool isMouseUp() const {
		return (action == GLFW_RELEASE);
	}
	bool isShiftDown() const {
		return ((mods & GLFW_MOD_SHIFT) != 0);
	}
	bool isControlDown() const {
		return ((mods & GLFW_MOD_CONTROL) != 0);
	}
	bool isAltDown() const {
		return ((mods & GLFW_MOD_ALT) != 0);
	}
};
class Composite;
class AlloyContext {
private:
	std::list<std::string> assetDirectories;
	std::shared_ptr<Font> fonts[4];
	static std::mutex contextLock;
	GLFWwindow* current;
	bool dirtyLayout = false;
	bool dirtyCursorLocator = false;
	bool dirtyCursor = false;
	bool enableDebugInterface = false;
	Animator animator;
	CursorLocator cursorLocator;
	const double ANIMATE_INTERVAL_SEC = 1.0 / 60.0;
	const double UPDATE_LOCATOR_INTERVAL_SEC = 1.0 / 15.0;
	const double UPDATE_CURSOR_INTERVAL_SEC = 1.0 / 30.0;
	pixel2 lastCursorOffset = pixel2(0, 0);
	bool dragEnabled = false;
	std::chrono::high_resolution_clock::time_point endTime;
	std::chrono::high_resolution_clock::time_point lastAnimateTime;
	std::chrono::high_resolution_clock::time_point lastUpdateTime;
	std::chrono::high_resolution_clock::time_point lastCursorTime;
	pixel2 cursorDownPosition = pixel2(-1, -1);
	Region* mouseOverRegion = nullptr;
	Region* mouseDownRegion = nullptr;
public:
	friend class Application;
	NVGcontext* nvgContext;
	GLFWwindow* window;
	ImageVAO vaoImage;
	box2i viewport;
	pixel2 cursorPosition = pixel2(-1, -1);
	double2 dpmm;
	bool hasFocus = false;
	inline bool isMouseOver(Region* region){
		return (mouseOverRegion==region);
	}
	inline bool isMouseDown(Region* region){
		return (mouseDownRegion==region);
	}
	double pixelRatio;

	void update(Composite& rootNode);
	void requestPack() {
		dirtyLayout = true;
	}
	void requestUpdateCursor() {
		dirtyCursor = true;
	}
	void requestUpdateCursorLocator() {
		dirtyCursorLocator = true;
	}
	inline void setDebug(bool enabled) {
		enableDebugInterface = enabled;
	}
	inline bool toggleDebug() {
		return enableDebugInterface = !enableDebugInterface;
	}
	inline bool isDebugEnabled() {
		return enableDebugInterface;
	}
	std::list<std::string>& getAssetDirectories() {
		return assetDirectories;
	}
	void addAssetDirectory(const std::string& dir);
	std::shared_ptr<Font>& loadFont(FontType type, const std::string& name,
			const std::string& partialFile);
	std::string getFullPath(const std::string& partialFile);
	inline int width() {
		return viewport.dimensions.x;
	}
	inline int height() {
		return viewport.dimensions.y;
	}
	inline const char* getFontName(FontType type) {
		if (fonts[static_cast<int>(type)].get() == nullptr)
			throw std::runtime_error("Font type not found.");
		return fonts[static_cast<int>(type)]->name.c_str();
	}
	inline int getFontHandle(FontType type) {
		if (fonts[static_cast<int>(type)].get() == nullptr)
			throw std::runtime_error("Font type not found.");
		return fonts[static_cast<int>(type)]->handle;
	}
	inline std::shared_ptr<ImageGlyph> createImageGlyph(
			const std::string& fileName, bool mipmap = false) {
		return std::shared_ptr<ImageGlyph>(new ImageGlyph(fileName, this));
	}
	inline std::shared_ptr<ImageGlyph> createImageGlyph(const ImageRGBA& img,
			bool mipmap = false) {
		return std::shared_ptr<ImageGlyph>(new ImageGlyph(img, this));
	}
	inline std::shared_ptr<AwesomeGlyph> createAwesomeGlyph(int codePoint) {
		return std::shared_ptr<AwesomeGlyph>(new AwesomeGlyph(codePoint, this));
	}
	inline std::shared_ptr<Font>& getFont(FontType type) {
		return fonts[static_cast<int>(type)];
	}
	template<class A> std::shared_ptr<Tween>& addTween(AColor& out,
			const Color& start, const Color& end, double duration, const A& a =
					Linear()) {
		return animator.add(out, start, end, duration, a);
	}
	template<class A> std::shared_ptr<Tween>& addTween(AUnit2D& out,
			const AUnit2D& start, const AUnit2D& end, double duration,
			const A& a = Linear()) {
		return animator.add(out, start, end, duration, a);
	}
	template<class A> std::shared_ptr<Tween>& addTween(AUnit1D& out,
			const AUnit1D& start, const AUnit1D& end, double duration,
			const A& a = Linear()) {
		return animator.add(out, start, end, duration, a);
	}
	AlloyContext(int width, int height, const std::string& title);
	bool begin();
	bool end();
	void makeCurrent();
	~AlloyContext();
};
}
#endif /* ALLOYCONTEXT_H_ */
