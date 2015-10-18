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
#include "../../include/example/PoissonInpaintEx.h"
using namespace aly;
PoissonInpaintEx::PoissonInpaintEx() :
		Application(800, 600, "Poisson Inpaint Example") {
}
bool PoissonInpaintEx::init(Composite& rootNode) {

	ImageRGBA srcImg, tarImg, initImg;
	ReadImageFromFile(getFullPath("images/sfmarket.png"), srcImg);
	ReadImageFromFile(getFullPath("images/sfsunset.png"), tarImg);

	ConvertImage(srcImg, src);
	ConvertImage(tarImg, tar);

	int w = src.width;
	int h = src.height;
	float r = h * 0.4f;
	mask = tar;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			float diff = r
					- length(float2((float) (i - w / 2), (float) (j - h / 2)));
			float alpha = 1.0f - clamp(diff / 128, 0.0f, 1.0f);
			mask(i, j).w = alpha;
		}
	}
	ConvertImage(mask, initImg);
	ImageGlyphPtr srcGlyph = createImageGlyph(srcImg, false);
	ImageGlyphPtr tarGlyph = createImageGlyph(tarImg, false);
	ImageGlyphPtr resultGlyph = createImageGlyph(tarImg, false);

	GlyphRegionPtr srcRegion = MakeGlyphRegion(srcGlyph,
			CoordPercent(0.05f, 0.0f), CoordPercent(0.4f, 0.3f),
			AspectRule::FixedWidth, COLOR_NONE, COLOR_NONE,
			Color(200, 200, 200, 255), UnitPX(1.0f));

	GlyphRegionPtr tarRegion = MakeGlyphRegion(tarGlyph,
			CoordPercent(0.55f, 0.0f), CoordPercent(0.4f, 0.3f),
			AspectRule::FixedWidth, COLOR_NONE, COLOR_NONE,
			Color(200, 200, 200, 255), UnitPX(1.0f));

	TextLabelPtr textLabel = TextLabelPtr(
			new TextLabel("Solving Poisson Inpaint ...",
					CoordPercent(0.1f, 0.35f), CoordPercent(0.8f, 0.5f)));
	textLabel->fontSize = UnitPX(20.0f);
	textLabel->fontType = FontType::Bold;
	textLabel->fontStyle = FontStyle::Outline;

	GlyphRegionPtr resultRegion = MakeGlyphRegion(resultGlyph,
			CoordPercent(0.1f, 0.35f), CoordPercent(0.8f, 0.5f),
			AspectRule::FixedWidth, COLOR_NONE, COLOR_NONE,
			Color(200, 200, 200, 255), UnitPX(1.0f));

	rootNode.add(srcRegion);
	rootNode.add(tarRegion);
	rootNode.add(resultRegion);
	rootNode.add(textLabel);
	workerTask = WorkerTaskPtr(new Worker([=] {
		PoissonInpaint(mask,src, tar, 32, 6);
		ImageRGBA out;
		ConvertImage(tar,out);
		getContext()->addDeferredTask([=]() {
					resultGlyph->set(out,getContext().get());
					textLabel->label="Finished!";
				});
	}));
	workerTask->execute(isForcedClose());
	return true;
}
