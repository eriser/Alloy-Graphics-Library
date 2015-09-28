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

#ifndef __gl_h_
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/glew.h>
#else
#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#endif
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
private:
	NVGcontext* nvg;
public:
	int handle;
	const std::string name;
	const std::string file;
	int getCursorPosition(const std::string & text, float fontSize,
			int xCoord) const;
	Font(const std::string& name, const std::string& file,
			AlloyContext* context);
};
struct Glyph {
	const std::string name;
	const GlyphType type;
	pixel width;
	pixel height;
	Glyph(const std::string& name, GlyphType type, pixel w, pixel h) :
			name(name), type(type), width(w), height(h) {

	}
	virtual inline ~Glyph() {
	}
	;
	virtual void draw(const box2px& bounds, const Color& fgColor,
			const Color& bgColor, AlloyContext* context)=0;
};
struct ImageGlyph: public Glyph {
	int handle;
	const std::string file;
	ImageGlyph(const std::string& file, AlloyContext* context, bool mipmap =
			false);
	ImageGlyph(const ImageRGBA& rgba, AlloyContext* context,
			bool mipmap = false);
	void draw(const box2px& bounds, const Color& fgColor, const Color& bgColor,
			AlloyContext* context) override;
	void set(const ImageRGBA& rgba,AlloyContext* context);
	~ImageGlyph();
};
struct CheckerboardGlyph: public Glyph {
	int handle;
	CheckerboardGlyph(int width, int height, int horizTiles, int vertTiles,
			AlloyContext* context, bool mipmap = false);
	void draw(const box2px& bounds, const Color& fgColor, const Color& bgColor,
			AlloyContext* context) override;
};
struct AwesomeGlyph: public Glyph {
	const int codePoint;
	const FontStyle style;
	AwesomeGlyph(int codePoint, AlloyContext* context, const FontStyle& style =
			FontStyle::Normal, pixel fontHeight = 32);
	void draw(const box2px& bounds, const Color& fgColor, const Color& bgColor,
			AlloyContext* context) override;
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
	int button = -1;
	bool isDown() const {
		return (action != GLFW_RELEASE);
	}
	bool isUp() const {
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
struct EventHandler {
	virtual bool onEventHandler(AlloyContext* context,
			const InputEvent& event)=0;
	virtual ~EventHandler();
};
struct Composite;
struct Region;
class AlloyContext {
private:
	std::list<std::string> assetDirectories;
	std::shared_ptr<Font> fonts[8];
	static std::mutex contextLock;
	std::list<GLFWwindow*> windowHistory;
	bool dirtyLayout = false;
	bool dirtyUI = true;
	bool dirtyCursorLocator = false;
	bool dirtyCursor = false;
	bool enableDebugInterface = false;
	Animator animator;
	CursorLocator cursorLocator;
	const double ANIMATE_INTERVAL_SEC = 1.0 / 30.0;
	const double UPDATE_LOCATOR_INTERVAL_SEC = 1.0 / 15.0;
	const double UPDATE_CURSOR_INTERVAL_SEC = 1.0 / 30.0;
	bool leftMouseButton = false;
	bool rightMouseButton = false;
	std::chrono::steady_clock::time_point endTime;
	std::chrono::steady_clock::time_point lastAnimateTime;
	std::chrono::steady_clock::time_point lastUpdateTime;
	std::chrono::steady_clock::time_point lastCursorTime;
	pixel2 cursorDownPosition = pixel2(-1, -1);
	Region* mouseOverRegion = nullptr;
	Region* mouseDownRegion = nullptr;
	Region* mouseFocusRegion = nullptr;
	Region* onTopRegion = nullptr;
	std::list<EventHandler*> listeners;
	std::shared_ptr<Composite> glassPanel;
	std::list<std::function<void()>> deferredTasks;
	static std::shared_ptr<AlloyContext> defaultContext;
	int2 viewSize;
	int2 screenSize;
	void addListener(EventHandler* region);
	void removeListener(EventHandler* region);
public:
	friend class Application;
	NVGcontext* nvgContext;
	GLFWwindow* window;
	GLFWwindow* offscreenWindow;
	const Theme theme;
	ImageVAO vaoImageOnScreen;
	ImageVAO vaoImageOffScreen;
	pixel2 cursorPosition = pixel2(-1, -1);
	double2 dpmm;
	bool hasFocus = false;
	bool isOnScreenRender() const;
	bool isOffScreenRender() const;
	void clearEvents(Region* region);
	void setOffScreenVisible(bool vis);
	static inline std::shared_ptr<AlloyContext>& getDefaultContext() {
		return defaultContext;
	}
	box2px getViewport() const {
		return box2px(pixel2(0.0f, 0.0f), pixel2(viewSize));
	}
	pixel2 getScreenSize() const {
		return pixel2(screenSize);
	}
	int getScreenWidth() {
		return screenSize.x;
	}
	int getScreenHeight() {
		return screenSize.y;
	}
	void addDeferredTask(const std::function<void()>& func);
	bool hasDeferredTasks() const {
		return (deferredTasks.size() > 0);
	}
	bool executeDeferredTasks();
	std::shared_ptr<Composite>& getGlassPanel();

	inline pixel2 getRelativeCursorDownPosition() const {
		return cursorDownPosition;
	}
	pixel2 getCursorDownPosition() const;
	bool fireListeners(const InputEvent& event);
	void setDragObject(Region* region);
	inline bool isMouseOver(Region* region) {
		return (mouseOverRegion == region);
	}
	bool isFocused(Region* region);
	inline void setMouseDownObject(Region* region) {
		mouseDownRegion = region;
	}
	inline void setMouseFocusObject(Region* region) {
		mouseFocusRegion = region;
	}
	void setOnTopRegion(Region* region);
	void removeOnTopRegion(Region* region);

	inline Region* getOnTopRegion() const {
		return onTopRegion;
	}
	bool isMouseContainedIn(const box2px& box) const;
	bool isMouseContainedIn(const pixel2& pos, const pixel2& dims) const;
	bool isMouseContainedIn(Region* region) const;
	bool isLeftMouseButtonDown() const {
		return leftMouseButton;
	}
	bool isRightMouseButtonDown() const {
		return rightMouseButton;
	}
	inline bool isMouseDown(Region* region) {
		return (mouseDownRegion == region);
	}
	inline bool isMouseDown() {
		return (mouseDownRegion != nullptr);
	}
	bool isOnTop(Region* region) const;
	inline bool isMouseDrag(Region* region) {
		return (mouseDownRegion == region && leftMouseButton);
	}
	inline bool isMouseDrag() {
		return (mouseDownRegion != nullptr && leftMouseButton);
	}
	double pixelRatio;

	void update(Composite& rootNode);
	void requestPack() {
		dirtyLayout = true;
	}
	Region* locate(const pixel2& cursor) const;
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
	inline int width() const {
		return viewSize.x;
	}
	inline int height() const {
		return viewSize.y;
	}
	inline int2 dimensions() const {
		return viewSize;
	}
	inline int2 screenDimensions() const {
		return screenSize;
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
	inline std::shared_ptr<AwesomeGlyph> createAwesomeGlyph(int codePoint,
			const FontStyle& style = FontStyle::Normal, pixel height = 32) {
		std::shared_ptr<AwesomeGlyph> g = std::shared_ptr<AwesomeGlyph>(
				new AwesomeGlyph(codePoint, this, style, height));
		return g;
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
	AlloyContext(int width, int height, const std::string& title,
			const Theme& theme = Theme::Default);
	bool begin(bool onScreen);
	void initOffScreenDraw();
	bool beginOffScreen() {
		return begin(false);
	}
	bool beginOnScreen() {
		return begin(true);
	}
	bool end();
	void repaintUI() {
		dirtyUI = true;
	}
	void makeCurrent();
	~AlloyContext();
};
inline std::shared_ptr<AlloyContext>& AlloyDefaultContext() {
	return AlloyContext::getDefaultContext();
}
typedef std::shared_ptr<Font> FontPtr;
typedef std::shared_ptr<Glyph> GlyphPtr;
typedef std::shared_ptr<AwesomeGlyph> AwesomeGlyphPtr;
typedef std::shared_ptr<ImageGlyph> ImageGlyphPtr;

}
#endif /* ALLOYCONTEXT_H_ */
