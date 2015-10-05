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
#include "../../include/example/CompositeEx.h"
using namespace aly;
CompositeEx::CompositeEx() :
		Application(800, 600, "Composite Example") {
}
bool CompositeEx::init(Composite& rootNode) {
	BorderCompositePtr bcomp = std::shared_ptr<BorderComposite>(
		new BorderComposite("Border Layout", CoordPX(0, 0),
			CoordPercent(0.7f, 1.0f)));
	TextLabelPtr label;
	CompositePtr sidebar = MakeComposite("Side", CoordPercent(0.7f, 0.0f), CoordPercent(0.3f, 1.0f), Color(64, 128, 255), COLOR_NONE, UnitPX(0));
	sidebar->add(label = MakeTextLabel("Side", CoordPercent(0.5f, 0.5f), CoordPX(100, 30), FontType::Bold, UnitPT(16.0f), COLOR_WHITE, HorizontalAlignment::Center, VerticalAlignment::Middle));
	label->setOrigin(Origin::MiddleCenter);
	CompositePtr north = MakeComposite("North", CoordPX(0, 0), CoordPercent(1.0, 1.0),Color(128, 64, 32), COLOR_NONE, UnitPX(0));
	north->add(label=MakeTextLabel("North", CoordPercent(0.5f, 0.5f), CoordPX(100, 30), FontType::Bold, UnitPT(16.0f),COLOR_WHITE,HorizontalAlignment::Center,VerticalAlignment::Middle));
	label->setOrigin(Origin::MiddleCenter);
	CompositePtr south = MakeComposite("South", CoordPX(0, 0),CoordPercent(1.0, 1.0), Color(64, 128, 32));
	south->add(label = MakeTextLabel("South", CoordPercent(0.5f, 0.5f), CoordPX(100, 30), FontType::Bold, UnitPT(16.0f), COLOR_WHITE, HorizontalAlignment::Center, VerticalAlignment::Middle));
	label->setOrigin(Origin::MiddleCenter);
	CompositePtr east = MakeComposite("East", CoordPX(0, 0), CoordPercent(1.0, 1.0),Color(128, 128, 64), COLOR_NONE, UnitPX(0));
	east->add(label = MakeTextLabel("East", CoordPercent(0.5f, 0.5f), CoordPX(100, 30), FontType::Bold, UnitPT(16.0f), COLOR_WHITE, HorizontalAlignment::Center, VerticalAlignment::Middle));
	label->setOrigin(Origin::MiddleCenter);
	CompositePtr west = MakeComposite("West", CoordPX(0, 0), CoordPercent(1.0, 1.0),Color(128, 64, 128), COLOR_NONE, UnitPX(0));
	west->add(label = MakeTextLabel("West", CoordPercent(0.5f, 0.5f), CoordPX(100, 30), FontType::Bold, UnitPT(16.0f), COLOR_WHITE, HorizontalAlignment::Center, VerticalAlignment::Middle));
	label->setOrigin(Origin::MiddleCenter);
	CompositePtr center = MakeComposite("Center", CoordPX(0, 0),CoordPercent(1.0, 1.0), Color(64, 128, 128));
	center->add(label = MakeTextLabel("Center", CoordPercent(0.5f, 0.5f), CoordPX(100, 30), FontType::Bold, UnitPT(16.0f), COLOR_WHITE, HorizontalAlignment::Center, VerticalAlignment::Middle));
	label->setOrigin(Origin::MiddleCenter);

	bcomp->setNorth(north,0.3f);
	bcomp->setEast(east,0.25f);
	bcomp->setWest(west,0.3f);
	bcomp->setSouth(south,0.1f);
	bcomp->setCenter(center);
	rootNode.add(sidebar);

	rootNode.add(bcomp);
	return true;
}

