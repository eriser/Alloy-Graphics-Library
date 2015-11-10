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
#include "../../include/example/DistanceFieldEx.h"
#include "AlloyDistanceField.h"
#include "AlloyIsoContour.h"
using namespace aly;
DistanceFieldEx::DistanceFieldEx() :
		Application(800, 600, "Distance Field Example") {
}
bool DistanceFieldEx::init(Composite& rootNode) {
	//SANITY_CHECK_DISTANCE_FIELD();
	w = getContext()->width();
	h = getContext()->height();
	GLFrameBuffer renderBuffer;
	//Render text to image
	NVGcontext* nvg = getContext()->nvgContext;
	renderBuffer.initialize(w, h);
	renderBuffer.begin(RGBAf(1.0f, 1.0f, 1.0f, 1.0f));
		nvgBeginFrame(nvg, w, h, 1.0f);
			nvgFontFaceId(nvg, getContext()->getFont(FontType::Bold)->handle);
			nvgFillColor(nvg, Color(0, 0, 0));
			nvgFontSize(nvg, 300.0f);
			nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			nvgText(nvg, w * 0.5f, h * 0.5f, "Alloy", nullptr);
		nvgEndFrame(nvg);
	renderBuffer.end();
	ImageRGBAf img = renderBuffer.getTexture().read();
	FlipVertical(img);
	Image1f gray, distField;
	ConvertImage(img, gray);
	//Make boundary == 0, outside == 0.5 inside == -0.5
	gray -= float1(0.5f);
	const float maxDistance = 40.0f;
	DistanceField2f df;
	//Solve distance field out to +/- 40 pixels
	df.solve(gray, distField, maxDistance);
	IsoContour isoContour;
	isoContour.solve(distField, 0.0f, TopologyRule2D::Unconstrained);
	curvePoints=isoContour.getPoints();
	curveIndexes = isoContour.getIndexes();
	//Normalize distance field range so it can be rendered as gray scale image.
	distField = (distField + float1(maxDistance)) / float1(2.0f * maxDistance);
	GlyphRegionPtr imageRegion = MakeGlyphRegion(createImageGlyph(distField),
			CoordPX(0.0f, 0.0f), CoordPercent(1.0f,1.0f), AspectRule::FixedHeight,
			COLOR_NONE, COLOR_NONE, Color(200, 200, 200, 255), UnitPX(1.0f));
	rootNode.add(imageRegion);
	DrawPtr draw = DrawPtr(new Draw("Iso-Contour", CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)));
	draw->setAspectRule(AspectRule::FixedHeight);
	draw->onDraw = [this](const AlloyContext* context, const box2px& bounds) {
		NVGcontext* nvg = context->nvgContext;
		nvgStrokeWidth(nvg, 4.0f);
		nvgStrokeColor(nvg, Color( 255,128,64));
		nvgLineCap(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		for (int n = 0;n < (int)curveIndexes.size();n++) {
			uint2 e = curveIndexes[n];
			float2 pt = curvePoints[e.x];
			pt.x = pt.x / (float)w;
			pt.y = pt.y / (float)h;
			pt = pt*bounds.dimensions + bounds.position;
			nvgMoveTo(nvg, pt.x, pt.y);
			pt = curvePoints[e.y];
			pt.x = pt.x / (float)w;
			pt.y = pt.y / (float)h;
			pt = pt*bounds.dimensions + bounds.position;
			nvgLineTo(nvg, pt.x, pt.y);
		}
		nvgStroke(nvg);
	};
	rootNode.add(draw);
	return true;
}

