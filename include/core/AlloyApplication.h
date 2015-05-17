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

#include <memory>
#include <list>
namespace aly{
	class Application{
	private:
		static std::unique_ptr<AlloyContext> context;
		void drawUI();
		void draw();
		std::list<std::exception_ptr> caughtExceptions;
		void initInternal();
	public:
		virtual void onWindowRefresh(){};
		virtual void onWindowFocus(int focused){};
		virtual void onWindowSize(int width,int height){};
		virtual void onChar(unsigned int codepoint){};
		virtual void onKey(int key, int scancode, int action, int mods){};
		virtual void onMouseButton(int button, int action, int    mods){};
		virtual void onCursorPos( double xpos, double ypos     ){};
		virtual void onScroll(double xoffset, double yoffset ){};

		inline void throwException(std::exception_ptr e){
			caughtExceptions.push_back(e);
		}
		Application(int w,int h,const std::string& title="");
		virtual void draw(const DrawEvent3D& event){}
		virtual void draw(const DrawEvent2D& event){}
		virtual bool init(){return true;};
		void run(int swapInterval=0);//no vsync by default
		virtual inline ~Application(){
			context.reset();
		}
	};
}


#endif /* ALLOYAPPLICATION_H_ */
