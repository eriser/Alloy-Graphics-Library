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
#include "../../include/example/ExpandEx.h"
using namespace aly;
ExpandEx::ExpandEx() :
		Application(800, 600, "Expand Bar Example") {
}
bool ExpandEx::init(Composite& rootNode) {
	ExpandBarPtr expandBar = ExpandBarPtr(
			new ExpandBar("exapander", CoordPercent(0.7f, 0.0f),
					CoordPercent(0.3f, 1.0f)));

	CompositePtr geomRegion = CompositePtr(
			new aly::Composite("Geometry", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 400.0f)));

	CompositePtr scrollRegion = CompositePtr(
			new aly::Composite("Scroll", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 200.0f)));
	scrollRegion->setScrollEnabled(true);
	scrollRegion->setOrientation(Orientation::Vertical);
	scrollRegion->add(
			MakeRegion("Region 1", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 300.0f), Color(255, 0, 0)));
	scrollRegion->add(
			MakeRegion("Region 2", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 300.0f), Color(0, 255, 0)));
	expandBar->add(geomRegion, 400, false);
	std::string exampleFile = getContext()->getFullPath(
			"models" + ALY_PATH_SEPARATOR+"monkey.ply");
	geomRegion->setOrientation(Orientation::Vertical);
	geomRegion->add(scrollRegion);
	RegionPtr apprRegion = RegionPtr(
			new aly::Region("Appearance", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 3000.0f)));
	expandBar->add(apprRegion, 400, false);
	apprRegion->backgroundColor = MakeColor(128, 64, 255);
	CompositePtr lightRegion = CompositePtr(
			new aly::Composite("Lighting", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 300.0f)));
	expandBar->add(lightRegion, 300, false);
	RegionPtr renderingRegion = RegionPtr(
			new aly::Region("Rendering", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 300.0f)));
	expandBar->add(renderingRegion, 300, true);
	RegionPtr filterRegion = RegionPtr(
			new aly::Region("Filtering", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 300.0f)));
	expandBar->add(filterRegion, 300, false);
	rootNode.backgroundColor = MakeColor(getContext()->theme.LIGHT);
	rootNode.add(expandBar);
	return true;
}

