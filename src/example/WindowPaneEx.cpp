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

#include "../../include/example/WindowPaneEx.h"

#include "Alloy.h"
using namespace aly;
WindowPaneEx::WindowPaneEx() :
		Application(800, 600, "Window Pane Example") {
}
bool WindowPaneEx::init(Composite& rootNode) {
	GlyphRegionPtr imageRegion1 = MakeGlyphRegion(createImageGlyph(getFullPath("images/sfmarket.png")), "Market St.",CoordPX(5.0f, 5.0f), CoordPX(400,265), AspectRule::FixedWidth, COLOR_NONE, COLOR_NONE, Color(200, 200, 200, 255), UnitPX(1.0f));
	WindowPanePtr window1 = WindowPanePtr(new WindowPane(imageRegion1));
	GlyphRegionPtr imageRegion2 = MakeGlyphRegion(createImageGlyph(getFullPath("images/sfsunset.png")), "Presidio Heights", CoordPX(50.0f, 50.0f), CoordPX(400, 265), AspectRule::FixedWidth, COLOR_NONE, COLOR_NONE, Color(200, 200, 200, 255), UnitPX(1.0f));
	WindowPanePtr window2 = WindowPanePtr(new WindowPane(imageRegion2));
	rootNode.add(window1);
	rootNode.add(window2);
	//Good idea to enable this. What happens if window gets resized? Will things disappear?
	rootNode.setScrollEnabled(true);
	return true;
}

