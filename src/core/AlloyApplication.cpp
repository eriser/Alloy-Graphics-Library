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
namespace aly {

std::shared_ptr<AlloyContext> Application::context;
void Application::initInternal() {
	rootNode.position=Percent(0.0f,0.0f);
	rootNode.dimensions=Percent(1.0f,1.0f);
	context->addAssetDirectory("assets/");
	context->addAssetDirectory("../assets/");
	context->addAssetDirectory("../../assets/");
	context->loadFont(FontType::Normal,"sans", "fonts/Roboto-Regular.ttf");
	context->loadFont(FontType::Bold,"sans-bold", "fonts/Roboto-Bold.ttf");
	context->loadFont(FontType::Italic,"sans-italic", "fonts/Roboto-Italic.ttf");
	context->loadFont(FontType::Icon,"icons", "fonts/fontawesome.ttf");
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
std::shared_ptr<GLTextureRGBA> Application::loadTextureRGBA(const std::string& partialFile){
	ImageRGBA image;
	ReadImageFromFile(context->getFullPath(partialFile),image);
	return std::shared_ptr<GLTextureRGBA>(new GLTextureRGBA(image,context));
}
std::shared_ptr<GLTextureRGB> Application::loadTextureRGB(const std::string& partialFile){
	ImageRGB image;
	ReadImageFromFile(context->getFullPath(partialFile),image);
	return std::shared_ptr<GLTextureRGB>(new GLTextureRGB(image,context));
}
std::shared_ptr<Font> Application::loadFont(const std::string& name,const std::string& file){
	return std::shared_ptr<Font>(new Font(name,context->getFullPath(file),context.get()));
}
Application::Application(int w, int h, const std::string& title):rootNode("Root") {
	if (context.get() == nullptr) {
		context = std::shared_ptr<AlloyContext>(new AlloyContext(w, h, title));
	} else {
		throw std::runtime_error(
				"Cannot instantiate more than one application.");
	}
	initInternal();
}
void Application::draw(){
	DrawEvent3D e3d;
	DrawEvent2D e2d;
	draw(e3d);
	draw(e2d);
	drawUI();

}
void Application::drawUI(){
	box2i& view=context->viewport;
	glViewport(view.position.x,view.position.y,view.dimensions.x,view.dimensions.y);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	NVGcontext* nvg=context->nvgContext;
	nvgBeginFrame(nvg,context->width(),context->height(),context->pixelRatio);
	nvgBeginPath(nvg);
	nvgRect(nvg,100,100,300,300);
	nvgFillColor(nvg,Color(64,64,64));
	nvgFill(nvg);
	rootNode.draw(context.get());
	nvgEndFrame(nvg);
}
void Application::run(int swapInterval) {
	context->makeCurrent();
	rootNode.pack(context.get());
	if (!init(rootNode)) {
		throw std::runtime_error("Error occurred in application init()");
	}
	rootNode.pack(context.get());
	glfwSwapInterval(swapInterval);
	double prevt = 0, cpuTime = 0;
	glfwSetTime(0);
	double lastCpuTime;
	uint64_t frameCounter=0;
	lastCpuTime = glfwGetTime();
	do {
		draw();
		double t = glfwGetTime();
		double dt=(t - lastCpuTime);
		frameCounter++;
		if(dt>0.5f){//Poll every 0.5 seconds
			frameRate=(float)(frameCounter/(t - lastCpuTime));
			lastCpuTime = t;
			//std::cout<<"FRAME RATE "<<frameRate<<stxd::endl;
			frameCounter=0;
		}
		glfwSwapBuffers(context->window);
		glfwPollEvents();
		for(std::exception_ptr e:caughtExceptions){
			throw e;
		}
	} while (!glfwWindowShouldClose(context->window));
}
}

