/*
 * Copyright(C) 2014, Blake C. Lucas, Ph.D. (img.science@gmail.com)
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

#ifndef GLCOMPONENT_H_
#define GLCOMPONENT_H_
#include <vector>
#include <list>
#include <memory>
#include "AlloyContext.h"
#include "AlloyApplication.h"
#include "AlloyMath.h"
namespace aly {
class GLComponentGroup;
class GLComponent {
protected:
	virtual void draw(AlloyContext* context)=0;
	virtual void update(AlloyContext* context)=0;
public:
	friend class GLComponentGroup;
	box2i bounds;
	virtual ~GLComponent()=0;
	GLComponent(){}
	GLComponent(const box2i& b):bounds(b){}
	void draw(){
		draw(Application::getContext());
	}
	void update(){
		update(Application::getContext());
	}

	inline void setBounds(int x,int y,int w,int h){
		bounds.position.x=x;
		bounds.position.y=y;
		bounds.dimensions.x=w;
		bounds.dimensions.y=h;
	}
};

class GLComponentGroup: protected GLComponent {
protected:
	std::list<std::unique_ptr<GLComponent>> components;
	virtual void draw(AlloyContext* context) {
		for (std::unique_ptr<GLComponent>& comp : components) {
			comp->draw(context);
		}
	}
	virtual void update(AlloyContext* context) {
		for (std::unique_ptr<GLComponent>& comp : components) {
			comp->update(context);
		}
	}
public:
	GLComponentGroup() :
			GLComponent() {

	}
	virtual ~GLComponentGroup()=0;
};

} /* namespace imagesci */

#endif /* GLCOMPONENT_H_ */
