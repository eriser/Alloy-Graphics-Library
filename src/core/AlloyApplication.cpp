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

#include "AlloyApplication.h"
#include "AlloyFileUtil.h"
#include "AlloyDrawUtil.h"
#include <thread>
#include <chrono>
namespace aly {

std::shared_ptr<AlloyContext> Application::context;
void Application::initInternal() {
	rootNode.setBounds(CoordPercent(0.0f, 0.0f), CoordPercent(1.0f, 1.0f));
	context->addAssetDirectory("assets/");
	context->addAssetDirectory("../assets/");
	context->addAssetDirectory("../../assets/");
	context->loadFont(FontType::Normal, "sans", "fonts/Roboto-Regular.ttf");
	context->loadFont(FontType::Bold, "sans-bold", "fonts/Roboto-Bold.ttf");
	context->loadFont(FontType::Italic, "sans-italic",
			"fonts/Roboto-Italic.ttf");
	context->loadFont(FontType::Icon, "icons", "fonts/fontawesome.ttf");
	glfwSetWindowUserPointer(context->window, this);
	glfwSetWindowRefreshCallback(context->window,
			[](GLFWwindow * window ) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onWindowRefresh();} catch(...) {app->throwException(std::current_exception());}});
	glfwSetWindowFocusCallback(context->window,
			[](GLFWwindow * window, int focused ) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onWindowFocus(focused);} catch(...) {app->throwException(std::current_exception());}});
	glfwSetWindowSizeCallback(context->window,
			[](GLFWwindow * window, int width, int height ) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onWindowSize(width, height);} catch(...) {app->throwException(std::current_exception());}});
	glfwSetCharCallback(context->window,
			[](GLFWwindow * window, unsigned int codepoint ) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onChar(codepoint);} catch(...) {app->throwException(std::current_exception());}});
	glfwSetKeyCallback(context->window,
			[](GLFWwindow * window, int key, int scancode, int action, int mods) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onKey(key, scancode,action,mods);} catch(...) {app->throwException(std::current_exception());}});
	glfwSetMouseButtonCallback(context->window,
			[](GLFWwindow * window, int button, int action,int mods) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onMouseButton(button, action,mods);} catch(...) {app->throwException(std::current_exception());}});
	glfwSetCursorPosCallback(context->window,
			[](GLFWwindow * window, double xpos, double ypos ) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onCursorPos(xpos, ypos);} catch(...) {app->throwException(std::current_exception());}});
	glfwSetCursorEnterCallback(context->window,
			[](GLFWwindow * window, int enter) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onCursorEnter(enter);} catch(...) {app->throwException(std::current_exception());}});
	glfwSetScrollCallback(context->window,
			[](GLFWwindow * window, double xoffset, double yoffset ) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onScroll(xoffset, yoffset);} catch(...) {app->throwException(std::current_exception());}});

}
std::shared_ptr<GLTextureRGBA> Application::loadTextureRGBA(
		const std::string& partialFile) {
	ImageRGBA image;
	ReadImageFromFile(context->getFullPath(partialFile), image);
	return std::shared_ptr<GLTextureRGBA>(new GLTextureRGBA(image, context));
}
std::shared_ptr<GLTextureRGB> Application::loadTextureRGB(
		const std::string& partialFile) {
	ImageRGB image;
	ReadImageFromFile(context->getFullPath(partialFile), image);
	return std::shared_ptr<GLTextureRGB>(new GLTextureRGB(image, context));
}
std::shared_ptr<Font> Application::loadFont(const std::string& name,
		const std::string& file) {
	return std::shared_ptr<Font>(
			new Font(name, context->getFullPath(file), context.get()));
}
Application::Application(int w, int h, const std::string& title,
		bool showDebugIcon) :
		rootNode("Root"), showDebugIcon(showDebugIcon) {
	if (context.get() == nullptr) {
		context = std::shared_ptr<AlloyContext>(new AlloyContext(w, h, title));
	} else {
		throw std::runtime_error(
				"Cannot instantiate more than one application.");
	}
	initInternal();
}
void Application::draw() {
	DrawEvent3D e3d;
	DrawEvent2D e2d;
	draw(e3d);
	draw(e2d);
	drawUI();
	if (context->isDebugEnabled()) {
		glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		drawDebugUI();
	} else {
		glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}
void Application::drawUI() {
	box2i& view = context->viewport;
	glViewport(view.position.x, view.position.y, view.dimensions.x,
			view.dimensions.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	NVGcontext* nvg = context->nvgContext;
	nvgBeginFrame(nvg, context->width(), context->height(),
			context->pixelRatio);
	nvgScissor(nvg, view.position.x, view.position.y, view.dimensions.x,
			view.dimensions.y);
	rootNode.draw(context.get());
	nvgScissor(nvg, view.position.x, view.position.y, view.dimensions.x,
			view.dimensions.y);

	Region* onTop=context->getOnTopRegion();
	if(onTop!=nullptr){
		if(onTop->isVisible())onTop->drawOnTop(context.get());
	}
	nvgEndFrame(nvg);
}
void Application::drawDebugUI() {
	box2i& view = context->viewport;
	NVGcontext* nvg = context->nvgContext;
	nvgBeginFrame(nvg, context->width(), context->height(),
			context->pixelRatio);
	nvgResetScissor(nvg);
	rootNode.drawDebug(context.get());
	Region* onTop=context->getOnTopRegion();
	if(onTop!=nullptr){
		onTop->drawDebug(context.get());
	}
	int cr = context->theme.CORNER_RADIUS;
	if (context->viewport.contains(
			int2((int) context->cursorPosition.x,
					(int) context->cursorPosition.y))) {
		nvgFontSize(nvg, 16);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		int alignment = 0;
		if (context->cursorPosition.x < context->viewport.dimensions.x * 0.5f) {
			alignment = NVG_ALIGN_LEFT;
		} else {
			alignment = NVG_ALIGN_RIGHT;
		}
		if (context->cursorPosition.y < context->viewport.dimensions.y * 0.5f) {
			alignment |= NVG_ALIGN_TOP;
		} else {
			alignment |= NVG_ALIGN_BOTTOM;
		}
		std::string txt = MakeString() << std::setprecision(4) << " "
				<< context->cursorPosition;
		nvgTextAlign(nvg, alignment);
		nvgFillColor(nvg, Color(0, 0, 0, 128));
		const float shift = 1.0f;
		if (context->hasFocus) {
			drawText(nvg, context->cursorPosition, txt, FontStyle::Outline,
					Color(255), Color(64, 64, 64));
		}
		nvgTextAlign(nvg, NVG_ALIGN_TOP);
		float yoffset = 5;
		txt = context->hasFocus ? "Window Has Focus" : "Window Lost Focus";
		drawText(nvg, 5, yoffset, txt.c_str(), FontStyle::Outline, Color(255),
				Color(64, 64, 64));
		yoffset += 16;
		if (context->mouseOverRegion != nullptr) {
			txt = MakeString() << "Mouse Over "
					<< context->mouseOverRegion->name << " "
					<< context->cursorPosition;
			drawText(nvg, 5, yoffset, txt.c_str(), FontStyle::Outline,
					Color(255), Color(64, 64, 64));
			yoffset += 16;
		}
		if (context->mouseDownRegion != nullptr) {
			txt = MakeString() << "Mouse Down "
					<< context->mouseDownRegion->name << " "
					<< context->cursorDownPosition;
			drawText(nvg, 5, yoffset, txt.c_str(), FontStyle::Outline,
					Color(255), Color(64, 64, 64));
			yoffset += 16;
		}
		if (context->mouseFocusRegion != nullptr) {
			txt = MakeString() << "Mouse Focus "
					<< context->mouseFocusRegion->name;
			drawText(nvg, 5, yoffset, txt.c_str(), FontStyle::Outline,
					Color(255), Color(64, 64, 64));
			yoffset += 16;
		}
		if (context->leftMouseButton) {
			txt = "Left Mouse Button";
			drawText(nvg, 5, yoffset, txt.c_str(), FontStyle::Outline,
					Color(255), Color(64, 64, 64));
			yoffset += 16;
		}
		if (context->rightMouseButton) {
			txt = "Right Mouse Button";
			drawText(nvg, 5, yoffset, txt.c_str(), FontStyle::Outline,
					Color(255), Color(64, 64, 64));
			yoffset += 16;
		}
		if (context->hasFocus) {
			nvgBeginPath(nvg);
			nvgLineCap(nvg, NVG_ROUND);
			nvgStrokeWidth(nvg, 2.0f);
			nvgStrokeColor(nvg, Color(255, 255, 255, 255));
			nvgMoveTo(nvg, context->cursorPosition.x - cr,
					context->cursorPosition.y);
			nvgLineTo(nvg, context->cursorPosition.x + cr,
					context->cursorPosition.y);
			nvgMoveTo(nvg, context->cursorPosition.x,
					context->cursorPosition.y - cr);
			nvgLineTo(nvg, context->cursorPosition.x,
					context->cursorPosition.y + cr);

			nvgStroke(nvg);
			nvgBeginPath(nvg);
			nvgFillColor(nvg, Color(255, 255, 255, 255));
			nvgCircle(nvg, context->cursorPosition.x, context->cursorPosition.y,
					3.0f);
			nvgFill(nvg);

			nvgBeginPath(nvg);
			nvgFillColor(nvg, Color(255, 64, 32, 255));
			nvgCircle(nvg, context->cursorPosition.x, context->cursorPosition.y,
					1.5f);
			nvgFill(nvg);
		}
	}
	nvgEndFrame(nvg);
}
void Application::fireEvent(const InputEvent& event) {
	if (event.type == InputType::Cursor
			|| event.type == InputType::MouseButton) {
		context->requestUpdateCursor();
	}
	Region* onTop=context->getOnTopRegion();
	if(onTop!=nullptr){
		if(!onTop->isVisible()){
			context->setOnTopRegion(nullptr);
		}
	}
	if (event.type == InputType::MouseButton) {
		if (event.isDown()) {
			if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
				context->leftMouseButton = true;
			}
			if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
				context->rightMouseButton = true;
			}
			context->mouseFocusRegion = context->mouseDownRegion =
					context->locate(context->cursorPosition);
			if (context->mouseDownRegion != nullptr) {
				context->cursorDownPosition = context->cursorPosition
						- context->mouseDownRegion->getBoundsPosition();
			}
		} else if (event.isUp()) {
			if (context->mouseDownRegion != nullptr
					&& context->mouseDownRegion->onMouseUp){
				context->mouseDownRegion->onMouseUp(context.get(), event);
			}
			context->leftMouseButton = false;
			context->rightMouseButton = false;
			context->mouseDownRegion = nullptr;
			context->cursorDownPosition = pixel2(0, 0);
		}
	}
	if (context->mouseDownRegion != nullptr && context->leftMouseButton
			&& context->mouseDownRegion->isDragEnabled()) {
			if (context->mouseDownRegion->onMouseDrag) {
				context->mouseDownRegion->onMouseDrag(context.get(), event,
						context->cursorDownPosition);
			} else {
				context->mouseDownRegion->setDragOffset(context->cursorPosition,
						context->cursorDownPosition);
			}
			context->requestPack();
	} else if (context->mouseOverRegion != nullptr) {
			if (event.type == InputType::MouseButton) {
				if (context->mouseOverRegion->onMouseDown && event.isDown())
					context->mouseOverRegion->onMouseDown(context.get(), event);
				if (context->mouseOverRegion->onMouseUp && event.isUp())
					context->mouseOverRegion->onMouseUp(context.get(), event);
				context->requestPack();
			}
			if (event.type == InputType::Cursor) {
				if (context->mouseOverRegion->onMouseOver)
					context->mouseOverRegion->onMouseOver(context.get(), event);
		}
	}
	context->fireListeners(event);
}

void Application::onWindowSize(int width, int height) {
	glViewport(0, 0, width, height);
	if (context->viewport.dimensions.x != width
			|| context->viewport.dimensions.y != height) {
		context->viewport = box2i(int2(0, 0), int2(width, height));
		context->requestPack();
	}
}
void Application::onCursorPos(double xpos, double ypos) {
	context->hasFocus = true;
	context->cursorPosition = pixel2((pixel) xpos, (pixel) ypos);
	InputEvent e;
	e.type = InputType::Cursor;
	e.cursor = pixel2((pixel) xpos, (pixel) ypos);
	fireEvent(e);
}

void Application::onWindowFocus(int focused) {
	if (focused) {
		context->hasFocus = true;

		InputEvent e;
		e.type = InputType::Cursor;
		e.cursor = context->cursorPosition;
		fireEvent(e);
	} else {
		context->mouseOverRegion = nullptr;
		context->mouseDownRegion = nullptr;
		context->cursorPosition = pixel2(-1, -1);
		context->cursorDownPosition = pixel2(-1, -1);
		context->hasFocus = false;
	}
}
void Application::onCursorEnter(int enter) {
	if (!enter) {
		context->hasFocus = false;
		context->mouseOverRegion = nullptr;
		InputEvent e;
		e.type = InputType::Cursor;
		e.cursor = pixel2(-1, -1);
		fireEvent(e);
	} else {
		context->hasFocus = true;
	}
}
void Application::onScroll(double xoffset, double yoffset) {
	InputEvent e;
	e.cursor = context->cursorPosition;
	e.type = InputType::Scroll;
	e.scroll = pixel2((pixel) xoffset, (pixel) yoffset);
	if (context->mouseOverRegion != nullptr
			&& context->mouseOverRegion->onScroll) {
		context->mouseOverRegion->onScroll(context.get(), e);
	}
	fireEvent(e);
}
void Application::onMouseButton(int button, int action, int mods) {
	InputEvent e;
	e.type = InputType::MouseButton;
	e.cursor = context->cursorPosition;
	e.button = button;
	e.action = action;
	e.mods = mods;
	fireEvent(e);
}
void Application::onKey(int key, int scancode, int action, int mods) {
	InputEvent e;
	e.type = InputType::Key;
	e.action = action;
	e.key = key;
	e.scancode = scancode;
	e.mods = mods;
	e.cursor = context->cursorPosition;
	fireEvent(e);
}
void Application::onChar(unsigned int codepoint) {
	InputEvent e;
	e.type = InputType::Character;
	e.codepoint = codepoint;
	e.cursor = context->cursorPosition;
	GLFWwindow* window = context->window;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)
			| glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
		e.mods |= GLFW_MOD_SHIFT;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)
			| glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
		e.mods |= GLFW_MOD_CONTROL;
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT)
			| glfwGetKey(window, GLFW_KEY_RIGHT_ALT))
		e.mods |= GLFW_MOD_ALT;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SUPER)
			| glfwGetKey(window, GLFW_KEY_RIGHT_SUPER))
		e.mods |= GLFW_MOD_SUPER;
	fireEvent(e);
}

void Application::run(int swapInterval) {
	const double POLL_INTERVAL_SEC = 0.5f;

	context->makeCurrent();
	if (!init(rootNode)) {
		throw std::runtime_error("Error occurred in application init()");
	}
	if (showDebugIcon) {
		GlyphRegionPtr debug = MakeGlyphRegion(createAwesomeGlyph(0xf188),
				CoordPercent(1.0f, 1.0f), CoordPX(20, 20),
				RGBA(64, 64, 64, 128), RGBA(255, 255, 255, 128));
		debug->setOrigin(Origin::BottomRight);
		debug->onMouseDown =
				[this,debug](AlloyContext* context,const InputEvent& e) {
					if(e.button==GLFW_MOUSE_BUTTON_LEFT) {
						context->toggleDebug();
						debug->foregroundColor=context->isDebugEnabled()?MakeColor(255,64,64,255):MakeColor(255,255,255,128);
						context->setMouseFocusObject(nullptr);
					}
				};
		rootNode.add(debug);
	}

	//First pack triggers computation of aspect ratios  for components.
	rootNode.pack(context.get());

	context->requestPack();
	glfwSwapInterval(swapInterval);
	double prevt = 0, cpuTime = 0;
	glfwSetTime(0);
	uint64_t frameCounter = 0;
	std::chrono::steady_clock::time_point endTime;
	std::chrono::steady_clock::time_point lastFpsTime =
			std::chrono::steady_clock::now();
	do {
		draw();
		context->update(rootNode);
		double elapsed =
				std::chrono::duration<double>(endTime - lastFpsTime).count();
		frameCounter++;
		if (elapsed > POLL_INTERVAL_SEC) {
			frameRate = (float) (frameCounter / elapsed);
			lastFpsTime = endTime;
			frameCounter = 0;
		}
		glfwSwapBuffers(context->window);
		glfwPollEvents();
		for (std::exception_ptr e : caughtExceptions) {
			std::rethrow_exception(e);
		}
	} while (!glfwWindowShouldClose(context->window));
}
}

