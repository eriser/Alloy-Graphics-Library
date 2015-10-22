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

#include "Alloy.h"
#include "../../include/example/MenuEx.h"
using namespace aly;
MenuEx::MenuEx() :
		Application(800, 600, "Menu Example") {
}
bool MenuEx::init(Composite& rootNode) {
	MenuBarPtr menuBar = MenuBarPtr(new MenuBar("Menu Bar",CoordPX(0,0),CoordPercent(1.0f,1.0f)));
	MenuPtr fileMenu = MenuPtr(new Menu("File"));
	MenuPtr editMenu = MenuPtr(new Menu("Edit"));
	MenuPtr viewMenu = MenuPtr(new Menu("View"));
	MenuPtr projectMenu = MenuPtr(new Menu("Project"));
	menuBar->add(fileMenu);
	menuBar->add(editMenu);
	menuBar->add(viewMenu);
	menuBar->add(projectMenu);

	CompositePtr north = MakeComposite("North Composite", CoordPX(0, 0), CoordPercent(1.0, 1.0), Color(128, 16, 32), COLOR_NONE, UnitPX(0));
	north->add(menuBar);
	TextLabelPtr label;
	CompositePtr center = MakeComposite("Center Composite", CoordPX(0, 0),CoordPercent(1.0, 1.0), Color(16, 128, 128));
	center->add(label = MakeTextLabel("Center", CoordPercent(0.5f, 0.5f), CoordPX(100, 30), FontType::Bold, UnitPT(16.0f), COLOR_WHITE, HorizontalAlignment::Center, VerticalAlignment::Middle));
	label->setOrigin(Origin::MiddleCenter);
	BorderCompositePtr bcomp = std::shared_ptr<BorderComposite>(new BorderComposite("Border Layout", CoordPX(0, 0),CoordPercent(1.0f, 1.0f)));
	bcomp->setNorth(north,UnitPX(35.0f));
	bcomp->setCenter(center);
	rootNode.add(bcomp);
	return true;
}

