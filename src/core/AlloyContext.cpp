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

#include "AlloyFileUtil.h"
#include "AlloyContext.h"
#include "AlloyUI.h"
#include "AlloyDrawUtil.h"
#include "AlloyApplication.h"
#include "nanovg.h"

#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include <iostream>
#include <chrono>
int printOglError(const char *file, int line) {

	GLenum glErr;
	int retCode = 0;

	glErr = glGetError();
	if (glErr != GL_NO_ERROR) {
		throw std::runtime_error(
				aly::MakeString() << "GL error occurred in \"" << file
						<< "\" on line " << line << ": "
						<< gluErrorString(glErr));
		return 1;
	}
	return retCode;
}
namespace aly {
std::mutex AlloyContext::contextLock, taskLock;
std::shared_ptr<AlloyContext> AlloyContext::defaultContext;
Font::Font(const std::string& name, const std::string& file,
		AlloyContext* context) :
		nvg(context->nvgContext), handle(0), name(name), file(file) {
	handle = nvgCreateFont(nvg, name.c_str(), file.c_str());
}
int Font::getCursorPosition(const std::string & text, float fontSize,
		int xCoord) const {
	std::vector<NVGglyphPosition> positions(text.size());
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg, handle);
	nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	positions.resize(
			nvgTextGlyphPositions(nvg, 0, 0, text.data(),
					text.data() + text.size(), positions.data(),
					(int) positions.size()));
	for (size_t i = 0; i < positions.size(); ++i) {
		if (xCoord < positions[i].maxx) {
			return static_cast<int>(i);
		}
	}
	return static_cast<int>(positions.size());
}
AwesomeGlyph::AwesomeGlyph(int codePoint, AlloyContext* context,
		const FontStyle& style, pixel height) :
		Glyph(CodePointToUTF8(codePoint), GlyphType::Awesome, 0, height), codePoint(
				codePoint), style(style) {
	NVGcontext* nvg = context->nvgContext;
	nvgFontSize(nvg, height);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	width = nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr);

}
void AwesomeGlyph::draw(const box2px& bounds, const Color& fgColor,
		const Color& bgColor, AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	nvgFontSize(nvg, height);
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	drawText(nvg, bounds.position + HALF_PIX(bounds.dimensions), name, style,
			fgColor, bgColor, nullptr);
}

ImageGlyph::ImageGlyph(const std::string& file, AlloyContext* context,
		bool mipmap) :
		Glyph(GetFileNameWithoutExtension(file), GlyphType::Image, 0, 0), file(
				file) {
	handle = nvgCreateImage(context->nvgContext, file.c_str(),
			(mipmap) ? NVG_IMAGE_GENERATE_MIPMAPS : 0);
	int w, h;
	nvgImageSize(context->nvgContext, handle, &w, &h);
	width = (pixel) w;
	height = (pixel) h;
}
void ImageGlyph::set(const ImageRGBA& rgba, AlloyContext* context) {
	nvgUpdateImage(context->nvgContext, handle, rgba.ptr());
}
ImageGlyph::~ImageGlyph() {
	AlloyContext* context = AlloyApplicationContext().get();
	if (context)
		nvgDeleteImage(context->nvgContext, handle);
}
ImageGlyph::ImageGlyph(const ImageRGBA& rgba, AlloyContext* context,
		bool mipmap) :
		Glyph("image_rgba", GlyphType::Image, 0, 0) {
	handle = nvgCreateImageRGBA(context->nvgContext, rgba.width, rgba.height,
			(mipmap) ? NVG_IMAGE_GENERATE_MIPMAPS : 0, rgba.ptr());
	width = (pixel) rgba.width;
	height = (pixel) rgba.height;
}
void ImageGlyph::draw(const box2px& bounds, const Color& fgColor,
		const Color& bgColor, AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	NVGpaint imgPaint = nvgImagePattern(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y, 0.f,
			handle, 1.0f);
	nvgBeginPath(nvg);
	nvgFillColor(nvg, Color(COLOR_WHITE));
	nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
			bounds.dimensions.y);
	nvgFillPaint(nvg, imgPaint);
	nvgFill(nvg);
	if (fgColor.a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
		nvgFillColor(nvg, Color(fgColor));
		nvgFill(nvg);
	}

}
CheckerboardGlyph::CheckerboardGlyph(int width, int height, int horizTiles,
		int vertTiles, AlloyContext* context, bool mipmap) :
		Glyph("image_rgba", GlyphType::Image, (pixel) width, (pixel) height) {
	ImageRGBA img(width, height);
	int cellWidth = width / horizTiles;
	int cellHeight = height / vertTiles;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			bool vt = (i / cellWidth) % 2 == 0;
			bool ht = (j / cellHeight) % 2 == 0;
			img(i, j) =
					((vt && !ht) || (!vt && ht)) ?
							RGBA(0, 0, 0, 0) : RGBA(255, 255, 255, 255);
		}
	}
	handle = nvgCreateImageRGBA(context->nvgContext, width, height,
			(mipmap) ? NVG_IMAGE_GENERATE_MIPMAPS : 0, img.ptr());
}
void CheckerboardGlyph::draw(const box2px& bounds, const Color& fgColor,
		const Color& bgColor, AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	NVGpaint imgPaint = nvgImagePattern(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y, 0.f,
			handle, 1.0f);
	if (bgColor.a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
		nvgFillColor(nvg, Color(bgColor));
		nvgFill(nvg);
	}
	nvgBeginPath(nvg);
	nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
			bounds.dimensions.y);
	nvgFillPaint(nvg, imgPaint);
	nvgFill(nvg);
	if (fgColor.a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x, bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y);
		nvgFillColor(nvg, Color(fgColor));
		nvgFill(nvg);
	}

}
void AlloyContext::addAssetDirectory(const std::string& dir) {
	std::string dirCopy=dir;
	if (ALY_PATH_SEPARATOR[0] != '/') {
		for (char& c : dirCopy) {
			if (c == '/') {
				c = ALY_PATH_SEPARATOR[0];
			}
		}
	} else if (ALY_PATH_SEPARATOR[0] != '\\') {
		for (char& c : dirCopy) {
			if (c == '\\') {
				c = ALY_PATH_SEPARATOR[0];
			}
		}
	}
	assetDirectories.push_back(dirCopy);
}
std::shared_ptr<Font>& AlloyContext::loadFont(FontType type,
		const std::string& name, const std::string& file) {
	fonts[static_cast<int>(type)] = std::shared_ptr<Font>(
			new Font(name, getFullPath(file), this));
	return fonts[static_cast<int>(type)];
}
std::string AlloyContext::getFullPath(const std::string& partialFile) {
	std::string fileName = partialFile;
	if (ALY_PATH_SEPARATOR[0] != '/') {
		for (char& c : fileName) {
			if (c == '/') {
				c = ALY_PATH_SEPARATOR[0];
			}
		}
	}
	else if (ALY_PATH_SEPARATOR[0] != '\\') {
		for (char& c : fileName) {
			if (c == '\\') {
				c = ALY_PATH_SEPARATOR[0];
			}
		}
	}
	for (std::string& dir : assetDirectories) {
		std::string fullPath = RemoveTrailingSlash(dir) + ALY_PATH_SEPARATOR+ fileName;
		if (FileExists(fullPath)) {
			return fullPath;
		}
	}
	std::string executableDir = GetExecutableDirectory();
	for (std::string& dir : assetDirectories) {
		std::string fullPath = RemoveTrailingSlash(executableDir) + ALY_PATH_SEPARATOR + RemoveTrailingSlash(dir) + ALY_PATH_SEPARATOR + fileName;
		if (FileExists(fullPath)) {
			return fullPath;
		}
	}
	std::cout<<"Could not find \""<< fileName <<"\"\nThis is where I looked:"<<std::endl;
	for (std::string& dir : assetDirectories) {
		std::string fullPath=RemoveTrailingSlash(dir) +ALY_PATH_SEPARATOR + fileName;
		std::cout<<"\""<<fullPath<<"\""<<std::endl;
		fullPath = executableDir + ALY_PATH_SEPARATOR + RemoveTrailingSlash(dir) + ALY_PATH_SEPARATOR + fileName;
		std::cout << "\"" << fullPath << "\"" << std::endl;
	}
	throw std::runtime_error(MakeString()<<"Could not find \""<< fileName <<"\"");
	return std::string("");
}
void AlloyContext::setDragObject(Region* region) {
	mouseDownRegion = region;
	cursorDownPosition = cursorPosition - mouseDownRegion->getBoundsPosition();
}
bool AlloyContext::isOnTop(Region* region) const {
	return (onTopRegion != nullptr
			&& (onTopRegion == region || region->hasParent(onTopRegion)));
}
pixel2 AlloyContext::getAbsoluteCursorDownPosition() const {
	return cursorDownPosition + ((mouseDownRegion != nullptr) ? mouseDownRegion->getBoundsPosition() : pixel2(0.0f));
}
bool AlloyContext::fireListeners(const InputEvent& event) {
	for (auto iter = listeners.rbegin(); iter != listeners.rend(); iter++) {
		EventHandler* handler = *iter;
		if(handler->onEventHandler(this, event))
			return true;
	}
	return false;
}
void AlloyContext::addListener(EventHandler* region) {
	listeners.push_back(region);
}
void AlloyContext::removeListener(EventHandler* region) {
	for (auto iter = listeners.begin(); iter != listeners.end(); iter++) {
		if (region == *iter) {
			listeners.erase(iter);
			break;
		}
	}
}
EventHandler::~EventHandler() {
	Application::removeListener(this);
}
void AlloyContext::setOnTopRegion(Region* region) {
	if (region == nullptr)
		throw std::runtime_error(
				"On top region cannot be null. use removeOnTopRegion() instead.");
	if (onTopRegion != nullptr) {
		if (onTopRegion->onRemoveFromOnTop)
			onTopRegion->onRemoveFromOnTop();
	}
	onTopRegion = region;
	region->setVisible(true);
}
void AlloyContext::removeOnTopRegion(Region* region) {
	if (region == nullptr)
		throw std::runtime_error("Remove on top region cannot be null.");
	if (region == onTopRegion) {
		if (onTopRegion->onRemoveFromOnTop)
			onTopRegion->onRemoveFromOnTop();
		onTopRegion = nullptr;
	}

}

AlloyContext::AlloyContext(int width, int height, const std::string& title,
		const Theme& theme) :
		current(nullptr), nvgContext(nullptr), window(nullptr), theme(theme) {
	std::lock_guard<std::mutex> lock(contextLock);
	if (glfwInit() != GL_TRUE) {
		throw std::runtime_error("Could not initialize GLFW.");
	}
	glfwSetErrorCallback(
			[](int error, const char* desc) {std::cout<<"GLFW Error ["<<error<<"] "<<desc<<std::endl;});
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (!window) {
		glfwTerminate();

		throw std::runtime_error("Could not create window.");
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw std::runtime_error("Could not initialize GLEW.");
	}
	glGetError();
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable( GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	viewSize = int2(width, height);
	nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	const float2 TextureCoords[6] = { float2(1.0f, 0.0f), float2(0.0f, 0.0f),
			float2(0.0f, 1.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f), float2(
					1.0f, 0.0f) };
	const float3 PositionCoords[6] = { float3(1.0f, 1.0f, 0.0f), float3(0.0f,
			1.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 0.0f),
			float3(1.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 0.0f) };
	glGenVertexArrays(1, &vaoImage.vao);
	glBindVertexArray(vaoImage.vao);
	glGenBuffers(1, &vaoImage.positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vaoImage.positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 6, PositionCoords,
	GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vaoImage.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vaoImage.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 6, TextureCoords,
	GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	int widthMM, heightMM;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	if (monitor == nullptr)
		throw std::runtime_error("Could not find monitor.");
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (mode == nullptr)
		throw std::runtime_error("Could not find video monitor.");
	glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);
	dpmm = double2(mode->width / (double) widthMM,
			mode->height / (double) heightMM);
	screenSize = int2(mode->width, mode->height);
	int winWidth, winHeight, fbWidth, fbHeight;
	glfwGetWindowSize(window, &winWidth, &winHeight);
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	// Calculate pixel ration for hi-dpi devices.
	pixelRatio = (float) fbWidth / (float) winWidth;
	lastAnimateTime = std::chrono::steady_clock::now();
	lastCursorTime = std::chrono::steady_clock::now();
	lastUpdateTime = std::chrono::steady_clock::now();
}
void AlloyContext::addDeferredTask(const std::function<void()>& func) {
	std::lock_guard<std::mutex> guard(taskLock);
	deferredTasks.push_back(func);
}
bool AlloyContext::executeDeferredTasks() {
	std::lock_guard<std::mutex> guard(taskLock);
	if (deferredTasks.size() > 0) {
		for (std::function<void()>& func : deferredTasks) {
			func();
		}
		deferredTasks.clear();
		return true;
	}
	return false;
}
bool AlloyContext::isMouseContainedIn(Region* region) const {
	return (region->getBounds().contains(cursorPosition));
}
bool AlloyContext::isMouseContainedIn(const box2px& box) const {
	return (box.contains(cursorPosition));
}
bool AlloyContext::isMouseContainedIn(const pixel2& pos,
		const pixel2& dims) const {
	return ((box2px(pos, dims)).contains(cursorPosition));
}
std::shared_ptr<Composite>& AlloyContext::getGlassPanel() {
	if (glassPanel.get() == nullptr) {
		glassPanel = std::shared_ptr<Composite>(
				new Composite("Glass Pane", CoordPX(0, 0),
						CoordPercent(1.0f, 1.0f)));
		//glassPanel->setIgnoreCursorEvents(true);
		glassPanel->setVisible(false);
		glassPanel->backgroundColor = MakeColor(
				theme.SHADOW.toSemiTransparent(0.5f));
	}
	return glassPanel;
}
void AlloyContext::clearEvents(Region* region) {
	if (mouseOverRegion == region)
		mouseOverRegion = nullptr;
	if (mouseDownRegion == region)
		mouseDownRegion = nullptr;
	if (mouseFocusRegion == region)
		mouseFocusRegion = nullptr;
	if (onTopRegion == region)
		region = nullptr;
}
Region* AlloyContext::locate(const pixel2& cursor) const {
	if (onTopRegion != nullptr) {
		if (onTopRegion->isVisible()) {
			Region* r = onTopRegion->locate(cursor);
			if (r != nullptr)
				return r;
		}
	}
	return cursorLocator.locate(cursor);
}
bool AlloyContext::begin() {
	if (current == nullptr) {
		contextLock.lock();
		current = glfwGetCurrentContext();
		glfwMakeContextCurrent(window);
		return true;
	}
	return false;
}
bool AlloyContext::end() {
	if (current != nullptr) {
		glfwMakeContextCurrent(current);
		current = nullptr;
		contextLock.unlock();
		return true;
	} else
		return false;
}
bool AlloyContext::isFocused(Region* region) {
	if (mouseFocusRegion != nullptr) {
		if (mouseFocusRegion->isVisible()) {
			return (region == mouseFocusRegion);
		} else {
			mouseFocusRegion = nullptr;
			return false;
		}
	}
	return false;
}
void AlloyContext::update(Composite& rootNode) {
	endTime = std::chrono::steady_clock::now();
	double updateElapsed = std::chrono::duration<double>(
			endTime - lastUpdateTime).count();
	double animateElapsed = std::chrono::duration<double>(
			endTime - lastAnimateTime).count();
	double cursorElapsed = std::chrono::duration<double>(
			endTime - lastCursorTime).count();
	if (deferredTasks.size() > 0) {
		executeDeferredTasks();
		cursorLocator.reset(viewSize);
		rootNode.update(&cursorLocator);
		dirtyCursorLocator = false;
		mouseOverRegion = locate(cursorPosition);
		dirtyCursor = false;
		dirtyLayout = true;
	}
	if (updateElapsed > UPDATE_LOCATOR_INTERVAL_SEC) {
		if (dirtyCursorLocator) {
			cursorLocator.reset(viewSize);
			rootNode.update(&cursorLocator);
			dirtyCursorLocator = false;
			mouseOverRegion = locate(cursorPosition);
			dirtyCursor = false;
		}
		lastUpdateTime = endTime;
	}
	if (cursorElapsed >= UPDATE_CURSOR_INTERVAL_SEC) { //Dont try to animate faster than 60 fps.
		if (dirtyCursor && !dirtyCursorLocator) {
			mouseOverRegion = locate(cursorPosition);
			dirtyCursor = false;
		}
		dirtyUI = true;
		lastCursorTime = endTime;
	}
	if (animateElapsed >= ANIMATE_INTERVAL_SEC) { //Dont try to animate faster than 60 fps.
		lastAnimateTime = endTime;
		if (animator.step(animateElapsed)) {
			dirtyLayout = true;
			dirtyUI = true;
		}
	}
	if (dirtyLayout) {
		rootNode.pack(this);
		animator.firePostEvents();
		dirtyCursorLocator = true;
		dirtyLayout = false;
	}

}
void AlloyContext::makeCurrent() {
	std::lock_guard<std::mutex> lock(contextLock);
	glfwMakeContextCurrent(window);
}

AlloyContext::~AlloyContext() {
	std::lock_guard<std::mutex> lock(contextLock);
	glfwMakeContextCurrent(window);
	if (vaoImage.vao) {
		glDeleteVertexArrays(1, &vaoImage.vao);
	}
	if (vaoImage.uvBuffer) {
		glDeleteBuffers(1, &vaoImage.uvBuffer);
	}
	if (vaoImage.positionBuffer) {
		glDeleteBuffers(1, &vaoImage.positionBuffer);
	}
	nvgDeleteGL3(nvgContext);
	glfwDestroyWindow(window);
	glfwTerminate();
}
}

