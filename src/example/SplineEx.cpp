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
		Application(800, 600, "Spline Example") {
}
bool SplineEx::init(Composite& rootNode) {
	spline.push_back(float2(-1.75, -1.0f));
	spline.push_back(float2(-1.5, -0.5f));
	spline.push_back(float2(-1.5, 0.0f));
	spline.push_back(float2(-1.25, 0.5f));
	spline.push_back(float2(-0.75, 0.75f));
	spline.push_back(float2(0.0f, 0.5f));
	spline.push_back(float2(0.5f, 0.0f));
	spline.build(SplineType::Clamp, 3);
	std::pair<float2, float2> range = std::pair<float2, float2>(float2(-2,-2),float2(2,2));
	DrawPtr draw = DrawPtr(new Draw("B-Spline", CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)));
	draw->onDraw = [=](const AlloyContext* context, const box2px& bounds) {
		
		NVGcontext* nvg = context->nvgContext;
		int N = 10;
		nvgStrokeWidth(nvg, 2.0f);
		nvgStrokeColor(nvg, Color(128, 128, 255));
		for (int n = 0;n <= N;n++) {
			float u = n /(float)N;
			TsDeBoorNet net = spline.evaluate(u);
			float* pts = net.points();
			if (net.nPoints() > 0) {
				float2 pt = float2(pts[0], pts[1]);
				pt = (pt - range.first) / (range.second - range.first);
				pt.y = 1.0f - pt.y;
				pt = pt*bounds.dimensions + bounds.position;
				nvgBeginPath(nvg);
				nvgMoveTo(nvg, pt.x, pt.y);
				for (int k = 1;k < net.nPoints();k++) {
					pt = float2(pts[2 * k], pts[2 * k + 1]);
					pt = (pt - range.first) / (range.second - range.first);
					pt.y = 1.0f - pt.y;
					pt = pt*bounds.dimensions + bounds.position;
					nvgLineTo(nvg, pt.x, pt.y);
				}
				nvgStroke(nvg);
			}
		}
	};
	rootNode.add(draw);
	return true;
}

