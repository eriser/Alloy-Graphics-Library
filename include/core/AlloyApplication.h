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

#ifndef ALLOYAPPLICATION_H_
#define ALLOYAPPLICATION_H_

#include "AlloyContext.h"
#include "AlloyCommon.h"
#include "GLShader.h"
#include "GLFrameBuffer.h"
#include "AlloyUI.h"

#include <memory>
#include <list>
namespace aly{
	class Application{
	private:
		float frameRate=0.0f;
		static std::shared_ptr<AlloyContext> context;
		void drawUI();
		void drawDebugUI();
		void draw();
		Composite rootNode;
		std::list<std::exception_ptr> caughtExceptions;
		void initInternal();
	public:
		static inline std::shared_ptr<AlloyContext>& getContext(){
			if(context.get()==nullptr)throw std::runtime_error("Cannot get GLFW / NanoVG context.");
			return context;
		}
		std::string getFullPath(const std::string& partialFile){
			return context->getFullPath(partialFile);
		}
		inline std::shared_ptr<ImageGlyph> createImageGlyph(const std::string& fileName,bool mipmap=false){
			return context->createImageGlyph(fileName);
		}
		inline std::shared_ptr<ImageGlyph> createImageGlyph(const ImageRGBA& img,bool mipmap=false){
			return context->createImageGlyph(img);
		}
		inline std::shared_ptr<AwesomeGlyph> createAwesomeGlyph(int codePoint){
			return context->createAwesomeGlyph(codePoint);
		}
		virtual void onWindowRefresh(){};
		virtual void onWindowFocus(int focused){};
		void onWindowSize(int width,int height);
		void onChar(unsigned int codepoint);
		void onKey(int key, int scancode, int action, int mods);
		void onMouseButton(int button, int action, int    mods);
		void onCursorPos(double xpos, double ypos);
		void onScroll(double xoffset, double yoffset);
		void onCursorEnter(int enter);
		inline void throwException(std::exception_ptr e){
			caughtExceptions.push_back(e);
		}
		Application(int w,int h,const std::string& title="");
		float getFrameRate()const {return frameRate;}
		std::shared_ptr<GLTextureRGB> loadTextureRGB(const std::string& partialFile);
		std::shared_ptr<GLTextureRGBA> loadTextureRGBA(const std::string& partialFile);
		std::shared_ptr<Font> loadFont(const std::string& name,const std::string& partialFile);

		virtual void draw(const DrawEvent3D& event){}
		virtual void draw(const DrawEvent2D& event){}
		virtual bool init(Composite& rootNode)=0;
		void run(int swapInterval=0);//no vsync by default
		virtual inline ~Application(){
			context.reset();
		}
	};
}


#endif /* ALLOYAPPLICATION_H_ */
