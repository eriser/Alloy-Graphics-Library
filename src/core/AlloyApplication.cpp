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

#include "../../include/core/AlloyApplication.h"
namespace aly {
std::unique_ptr<AlloyContext> Application::context;
void Application::initInternal() {
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
	glfwSetScrollCallback(context->window,
			[](GLFWwindow * window, double xoffset, double yoffset ) {Application* app = (Application *)(glfwGetWindowUserPointer(window)); try {app->onScroll(xoffset, yoffset);} catch(...) {app->throwException(std::current_exception());}});
}
Application::Application(int w, int h, const std::string& title) {
	if (context.get() == nullptr) {
		context = std::unique_ptr<AlloyContext>(new AlloyContext(w, h, title));
	} else {
		throw std::runtime_error(
				"Cannot instantiate more than one application.");
	}
	initInternal();
	if (!init()) {
		throw std::runtime_error("Error occurred in application init()");
	}
}
void Application::draw(){
	DrawEvent3D e3d;
	DrawEvent2D e2d;
	draw(e3d);
	draw(e2d);
	drawUI();

}
void Application::drawUI(){

}
void Application::run(int swapInterval) {
	context->MakeCurrent();
	glfwSwapInterval(swapInterval);
	do {
		draw();
		glfwSwapBuffers(context->window);
		glfwPollEvents();
	} while (!glfwWindowShouldClose(context->window));
}
}

