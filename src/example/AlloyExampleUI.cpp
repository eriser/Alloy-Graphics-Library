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


//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include "Alloy.h"
#include "../../include/example/AlloyExampleUI.h"
using namespace aly;
ExampleUI::ExampleUI():Application(640,480,"ExampleUI"){
}
bool ExampleUI::init(Composite& rootNode) {
	LabelPtr label=MakeLabel(
			"label1",
			Percent(0.5f,0.5f),
			Percent(0.5f,0.5f),
			FontType::Normal,
			UnitPT(16.0f),
			RGBA(255,255,255,255),
			HorizontalAlignment::Center,
			VerticalAlignment::Middle);
	CompositePtr comp=MakeComposite("rect1",CoordPX(100,100),CoordPX(400,300),RGBA(128,32,32,255));
	comp->add(label);
	rootNode.add(comp);
	rootNode.pack();
	std::cout<<*label<<std::endl;
	std::cout<<*comp<<std::endl;
	std::cout<<rootNode<<std::endl;
	//std::cout<<"Label "<<label->bounds<<std::endl;
	return true;
}

int main()
{
	try{
		SANITY_CHECK_UI();
		ExampleUI app;
		std::cout<<*Application::getContext()->getFont(FontType::Normal)<<std::endl;
		app.run();
		return 0;
	} catch(std::exception& e){
		std::cout<<"Error: "<<e.what()<<std::endl;
		std::flush(std::cout);
		std::cout<<"Exiting ..."<<std::endl;
		//std::cout<<"Hit any key ..."<<std::endl;
		//getchar();
		return 1;
	}
}
