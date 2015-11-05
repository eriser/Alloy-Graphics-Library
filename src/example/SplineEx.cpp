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
#include "../../include/example/SplineEx.h"

using namespace aly;
SplineEx::SplineEx() :
	Application(600, 600, "Spline Example") {
}
bool SplineEx::init(Composite& rootNode) {
	//SANITY_CHECK_BSPLINE();
	Vector2f controlPt;
	controlPt.push_back(float2(-1.75, -1.0f));
	controlPt.push_back(float2(-1.5, -0.5f));
	controlPt.push_back(float2(-1.5, 0.0f));
	controlPt.push_back(float2(-1.25, 0.5f));
	controlPt.push_back(float2(-0.75, 0.75f));
	controlPt.push_back(float2(0.0f, 0.5f));
	controlPt.push_back(float2(0.5f, 0.0f));
	BSpline2f bspline(controlPt, SplineType::Clamp, 3);
	int N = 100;
	Vector2f pts(N+1);
	for (int n = 0;n <= N;n++) {
		pts[n] = bspline.evaluate(n / (float)N);
	}
	std::pair<float2, float2> range = pts.range();
	DrawPtr draw = DrawPtr(new Draw("B-Spline", CoordPX(30.0f, 30.0f), CoordPerPX(1.0f, 1.0f,-60.0f,-60.0f)));
	draw->setAspectRule(AspectRule::FixedHeight);
	float2 dims = range.second - range.first;
	dims =float2(std::max(dims.x, dims.y));
	range.second = range.first + dims;
	draw->setAspectRatio(1.0f);
	draw->onDraw = [=](const AlloyContext* context, const box2px& bounds) {

		NVGcontext* nvg = context->nvgContext;
		nvgStrokeWidth(nvg, 3.0f);
		nvgStrokeColor(nvg, Color(64, 128, 255));
		
		float2 pt = pts[0];
		pt = (pt - range.first) / (range.second - range.first);
		pt.y = 1.0f - pt.y;
		pt = pt*bounds.dimensions + bounds.position;
		nvgLineCap(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, pt.x, pt.y);
		for (int n = 1;n < pts.size();n++) {
			float2 pt = pts[n];
			pt = (pt - range.first) / (range.second - range.first);
			pt.y = 1.0f - pt.y;
			pt = pt*bounds.dimensions + bounds.position;
			nvgLineTo(nvg, pt.x, pt.y);
			
		}
		nvgStroke(nvg);
		nvgFillColor(nvg,Color(255, 255, 255));
		for (int n = 0;n <controlPt.size();n++) {

			nvgBeginPath(nvg);
			float2 pt = controlPt[n];
			pt = (pt - range.first) / (range.second - range.first);
			pt.y = 1.0f - pt.y;
			pt = pt*bounds.dimensions + bounds.position;
			nvgCircle(nvg, pt.x, pt.y,6);
			nvgFill(nvg);
		}
	};
	rootNode.add(draw);
	return true;
}

