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
#include "../../include/example/AlloyExampleUI.h"
using namespace aly;
ExampleUI::ExampleUI() :
		Application(640, 480, "ExampleUI") {
}
bool ExampleUI::init(Composite& rootNode) {
	TextLabelPtr label = MakeTextLabel("Hello Blake", CoordPercent(0.5f, 0.5f),
			CoordPercent(0.5f, 0.5f), FontType::Normal, UnitPT(16.0f),
			RGBA(255, 255, 255, 255), HorizontalAlignment::Center,
			VerticalAlignment::Middle);
	ImageRGBA robotImg;
	std::shared_ptr<ImageGlyph> img = createImageGlyph(
			getFullPath("images/robot.png"));
	GlyphRegionPtr imgr = MakeGlyphRegion(img, CoordPX(160, 160),
			CoordPX(100, 300), AspectRatio::FixedHeight, RGBA(32, 64, 128, 255),
			RGBA(128, 128, 128, 128), RGBA(200,200,200, 255), UnitPX(1.0f));

	GlyphRegionPtr iconr = MakeGlyphRegion(createAwesomeGlyph(0xf188),
			CoordPX(20, 20), CoordPX(50, 100), RGBA(32, 64, 128, 255),
			RGBA(255, 255, 255, 255));
	imgr->origin = Origin::Center;
	addTween(imgr->foregroundColor, Color(128, 128, 128, 255), Color(128, 128, 128, 0),
			3.0, SineOut());
	std::cout << "Add Tween" << std::endl;
	addTween(imgr->getDimensions(), CoordPX(50, 50), CoordPX(300, 300), 1.0,
			SineOut())->addCompleteEvent(
			[](Tweenable* object) {std::cout<<"Finished Dimension Change! "<<std::endl;});
	addTween(iconr->backgroundColor, Color(255, 64, 32, 255), Color(32, 64, 255, 255),
			3.0, SineIn());
	addTween(iconr->foregroundColor, Color(0, 0, 0, 255), Color(255, 255, 255, 255),
			3.0, SineIn());
	addTween(iconr->getPosition(), CoordPX(100, 100), CoordPX(300, 100), 3.0,
			ExponentialOut())->addCompleteEvent(
			[](Tweenable* object) {std::cout<<"Finished Position Change! "<<std::endl;});
	addTween(label->fontSize, UnitPT(10.0f), UnitPT(30.0f), 1.0, Linear());
	CompositePtr comp = MakeComposite("Rect 1", CoordPercent(0.2, 0.3),
			CoordPX(400, 300), RGBA(128, 32, 32, 255));

	RegionPtr scrollHandle = MakeRegionLabel("Scroll Handle",CoordPercent(0.0,0.0),
			CoordPX(30, 30), RGBA(255, 128, 64, 255), RGBA(255, 255, 255, 255),UnitPX(2.0f));
	scrollHandle->setEnableDrag(true);
	CompositePtr scrollTrack = MakeComposite("Scroll Track", CoordPercent(0.2, 0.7),
			CoordPX(300, 30), RGBA(128, 128, 128, 255));
	scrollTrack->add(scrollHandle);
	iconr->setEnableDrag(true);
	comp->add(label);

	rootNode.add(comp);
	rootNode.add(imgr);
	rootNode.add(iconr);
	rootNode.add(scrollTrack);
	getContext()->toggleDebug();
	std::cout << *imgr << std::endl;
	std::cout << *label << std::endl;
	std::cout << *comp << std::endl;
	std::cout << *iconr << std::endl;

	//std::cout<<rootNode<<std::endl;
	//std::cout<<"Label "<<label->bounds<<std::endl;
	return true;
}

int main() {
	try {
		//SANITY_CHECK_IMAGE();
		//SANITY_CHECK_UI();
		ExampleUI app;
		//std::cout<<*Application::getContext()->getFont(FontType::Normal)<<std::endl;
		app.run();
		return 0;
	} catch (std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		std::flush(std::cout);
		std::cout << "Exiting ..." << std::endl;
		//std::cout<<"Hit any key ..."<<std::endl;
		//getchar();
		return 1;
	}
}
