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
		Application(1280, 720, "ExampleUI") {
}
bool ExampleUI::init(Composite& rootNode) {

	/*
	 TextLabelPtr label = MakeTextLabel("Hello Blake", CoordPercent(0.1f, 0.1f),
	 CoordPercent(0.6f, 0.5f), FontType::Normal, UnitPT(16.0f),
	 RGBA(255, 255, 255, 255), HorizontalAlignment::Center,
	 VerticalAlignment::Baseline);
	 TextFieldPtr field1 = MakeTextField("Field 1", CoordPercent(0.6f, 0.8f),
	 CoordPX(200.0f, 50.0f), Theme::Default.DARK,
	 Theme::Default.LIGHT_TEXT);
	 TextFieldPtr field2 = MakeTextField("Field 2", CoordPercent(0.1f, 0.8f),
	 CoordPX(200.0f, 50.0f), Theme::Default.LIGHT,
	 Theme::Default.DARK_TEXT);
	 ImageRGBA robotImg;
	 std::shared_ptr<ImageGlyph> img = createImageGlyph(
	 getFullPath("images/sfmarket.png"));
	 GlyphRegionPtr imgr = MakeGlyphRegion(img, CoordPX(160, 160),
	 CoordPX(100, 300), AspectRule::FixedHeight, Color(32, 64, 128, 255),
	 Color(128, 128, 128, 128), Color(200, 200, 200, 255), UnitPX(1.0f));

	 GlyphRegionPtr iconr = MakeGlyphRegion(createAwesomeGlyph(0xf188),
	 CoordPX(20, 20), CoordPX(50, 100), Color(32, 64, 128, 255),
	 Color(255, 255, 255, 255));
	 imgr->setOrigin(Origin::MiddleCenter);

	 ProgressBarPtr pbar = ProgressBarPtr(
	 new ProgressBar("Progress", CoordPercent(0.05f, 0.7f),
	 CoordPercent(0.4f, 0.05f)));

	 progressTask = std::unique_ptr<aly::RecurrentWorker>(
	 new RecurrentWorker([pbar](uint64_t iter) {
	 //std::cout << "Iteration " << iter << std::endl;
	 pbar->setValue("Task Executing ...",(iter++)/20.0f);
	 return (iter<20);
	 }, [pbar]() {
	 pbar->setValue("Task Complete.",1.0f);
	 }, 100));
	 progressTask->execute();


	 addTween(imgr->foregroundColor, Color(128, 128, 128, 255),
	 Color(128, 128, 128, 0), 3.0, SineOut());

	 addTween(imgr->getDimensions(), CoordPX(50, 50), CoordPX(300, 300), 1.0,SineOut());//->addCompleteEvent([](Tweenable* object) {std::cout<<"Finished Dimension Change! "<<std::endl;});

	 addTween(iconr->backgroundColor, Color(255, 64, 32, 255),
	 Color(32, 64, 255, 255), 3.0, SineIn());
	 addTween(iconr->foregroundColor, Color(0, 0, 0, 255),
	 Color(255, 255, 255, 255), 3.0, SineIn());
	 addTween(iconr->getPosition(), CoordPX(100, 100), CoordPX(300, 100), 3.0,ExponentialOut());//->addCompleteEvent([](Tweenable* object) {std::cout<<"Finished Position Change! "<<std::endl;});
	 addTween(label->fontSize, UnitPT(10.0f), UnitPT(30.0f), 1.0, Linear());


	 CompositePtr comp = MakeComposite("Rect 1", CoordPercent(0.5, 0.0),
	 CoordPercent(0.5f, 1.0f), Theme::Default.DARK);
	 comp->setOrientation(Orientation::Vertical);

	 RegionPtr scrollHandle = MakeRegion("Scroll Handle", CoordPercent(0.0, 0.0),
	 CoordPX(30, 30), RGBA(255, 128, 64, 255), RGBA(255, 255, 255, 255),
	 UnitPX(2.0f));

	 scrollHandle->setDragEnabled(true);
	 CompositePtr scrollTrack = MakeComposite("Scroll Track",
	 CoordPercent(0.2f, 0.7f), CoordPX(300, 30), RGBA(128, 128, 128, 255));
	 scrollTrack->add(scrollHandle);
	 //iconr->setDragEnabled(true);

	 ButtonPtr button1 = std::shared_ptr<TextButton>(
	 new TextButton("Click Me", CoordPX(10, 110), CoordPX(140, 50)));

	 ButtonPtr button2 = std::shared_ptr<TextButton>(
	 new TextButton("Drag Me", CoordPX(10, 10), CoordPX(140, 50)));


	 HSliderPtr hslider1 = HSliderPtr(
	 new HorizontalSlider("Label A", CoordPerPX(0.1f, 0.3f, 0, 0),
	 CoordPX(200.0f, 40.0f), Integer(1), Integer(12),
	 Integer(7)));
	 HSliderPtr hslider2 = HSliderPtr(
	 new HorizontalSlider("Label B", CoordPercent(0.7f, 0.7f),
	 CoordPX(200.0f, 50.0f), Double(1), Double(12),
	 Double(7)));

	 VSliderPtr vslider1 = VSliderPtr(
	 new VerticalSlider("Slider", CoordPerPX(0.85f, 0.1f, 0, 0),
	 CoordPX(100.0f, 200.0f), Integer(0), Integer(20),
	 Integer(70)));


	 CheckBoxPtr checkbox = CheckBoxPtr(
	 new CheckBox("Check", CoordPX(200.0f, 40.0f), CoordPercent(0.4f, 0.07f),
	 false));

	 ToggleBoxPtr togglebox = ToggleBoxPtr(
	 new ToggleBox("Toggle", CoordPX(200.0f, 40.0f),
	 CoordPercent(0.4f, 0.07f), false));

	 ColorSelectorPtr colorselect = ColorSelectorPtr(
	 new ColorSelector("Color", CoordPercent(0.6f, 0.5f),
	 CoordPX(200, 50)));
	 colorselect->setColor(Color(200, 128, 32));

	 SelectionPtr dropdown =
	 SelectionPtr(
	 new Selection("Selection", CoordPercent(0.6f, 0.1f),
	 CoordPX(200, 30), std::vector<std::string> {
	 "Mission", "Bernal Heights", "Noe Valley",
	 "Telegraph Hill", "North Beach",
	 "South Beach", "Richmond", "Sunset",
	 "Daly City", "Dogpatch", "Potrero Hill",
	 "Ocean Beach", "SoMa", "Pacific Heights",
	 "Cow Hollow", "Russian Hill", "Tenderloin",
	 "Hayes Valley", "Financial District" }));

	 CompositePtr scrollPane = MakeComposite("Scroll  Pane",
	 CoordPercent(0.1f, 0.1f), CoordPercent(0.35f, 0.5f),
	 RGBA(64, 128, 64, 255));
	 for (int n = 0; n < 12; n++) {
	 HSliderPtr hslider = HSliderPtr(
	 new HorizontalSlider(MakeString() << "Item " << (n + 1),
	 CoordPX(0, 0), CoordPX(400.0f, 50.0f), Integer(0),
	 Integer(10), Integer(7)));
	 scrollPane->add(hslider);
	 }
	 scrollPane->setOrientation(Orientation::Vertical);
	 scrollPane->setScrollEnabled(true);

	 comp->add(hslider1);
	 comp->add(button1);
	 comp->add(dropdown);
	 comp->add(checkbox);
	 comp->add(togglebox);

	 button2->setDragEnabled(true);

	 //button1->setDragEnabled(true);
	 //rootNode.add(scrollPane);

	 rootNode.add(comp);
	 rootNode.add(colorselect);
	 rootNode.add(imgr);
	 //rootNode.add(iconr);
	 //rootNode.add(scrollTrack);
	 //rootNode.backgroundColor = MakeColor(Theme::Default.LIGHT);
	 rootNode.add(button2);
	 rootNode.add(hslider2);
	 rootNode.add(vslider1);
	 //rootNode.add(field1);
	 //rootNode.add(field2);
	 //rootNode.add(vslider1);
	 //rootNode.add(pbar);
	 //rootNode.add(colorselect);
	 //getContext()->toggleDebug();
	 //std::cout<<rootNode<<std::endl;
	 */

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
	expandBar->add(geomRegion, true);

	FileSelector* selector;
	geomRegion->add(
			selector = new FileSelector("Mesh", CoordPX(2, 5),
					CoordPerPX(1.0, 0.0f, -4.0f, 30.0f)));

	selector->addFileExtensionRule("Portable Network Graphics", "png");
	selector->addFileExtensionRule("XML", { "raw", "xml" });
	selector->addFileExtensionRule("Text", "txt");
	selector->setValue("mesh.obj");



	geomRegion->setOrientation(Orientation::Vertical);
	geomRegion->add(scrollRegion);
	RegionPtr apprRegion = RegionPtr(
			new aly::Region("Appearance", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 400.0f)));
	expandBar->add(apprRegion, true);
	CompositePtr lightRegion = CompositePtr(
			new aly::Composite("Lighting", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 400.0f)));
	lightRegion->setOrientation(Orientation::Vertical);
	SelectionPtr dropdown2 = SelectionPtr(
			new Selection("Selection", CoordPX(0, 0), CoordPX(200, 30),
					std::vector<std::string> { "Mission", "Bernal Heights",
							"Noe Valley", "Telegraph Hill", "North Beach" }));
	lightRegion->add(dropdown2);
	expandBar->add(lightRegion, true);
	RegionPtr renderingRegion = RegionPtr(
			new aly::Region("Rendering", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 400.0f)));
	expandBar->add(renderingRegion, false);
	RegionPtr filterRegion = RegionPtr(
			new aly::Region("Filtering", CoordPX(0, 0),
					CoordPerPX(1.0f, 0.0f, 0.0f, 400.0f)));
	expandBar->add(filterRegion, false);

	BorderCompositePtr bcomp = std::shared_ptr<BorderComposite>(
			new BorderComposite("Border Layout", CoordPX(0, 0),
					CoordPercent(0.7f, 1.0f)));
	RegionPtr north = MakeRegion("North", CoordPX(0, 0), CoordPercent(1.0, 1.0),
			Color(255, 0, 0), COLOR_NONE, UnitPX(0));
	CompositePtr south = MakeComposite("South", CoordPX(0, 0),
			CoordPercent(1.0, 1.0), Color(0, 255, 0));

	ToggleBoxPtr togglebox1 = ToggleBoxPtr(
			new ToggleBox("Toggle 1", CoordPX(0.0f, 0.0f), CoordPX(200, 40),
					false));
	ToggleBoxPtr togglebox2 = ToggleBoxPtr(
			new ToggleBox("Toggle 2", CoordPX(0.0f, 0.0f), CoordPX(200, 30),
					false));
	ToggleBoxPtr togglebox3 = ToggleBoxPtr(
			new ToggleBox("Toggle 3", CoordPX(0.0f, 0.0f), CoordPX(400, 30),
					false));
	CheckBoxPtr checkbox1 = CheckBoxPtr(
			new CheckBox("CheckBox 3", CoordPX(0.0f, 0.0f), CoordPX(400, 30),
					false));
	SelectionPtr dropdown = SelectionPtr(
			new Selection("Selection", CoordPX(0, 0), CoordPX(200, 30),
					std::vector<std::string> { "Mission", "Bernal Heights",
							"Noe Valley", "Telegraph Hill", "North Beach" }));
	south->setOrientation(Orientation::Vertical);

	south->add(dropdown);
	south->add(togglebox1);
	south->add(togglebox2);
	south->add(togglebox3);
	south->add(checkbox1);
	RegionPtr east = MakeRegion("East", CoordPX(0, 0), CoordPercent(1.0, 1.0),
			Color(255, 255, 0), COLOR_NONE, UnitPX(0));
	RegionPtr west = MakeRegion("West", CoordPX(0, 0), CoordPercent(1.0, 1.0),
			Color(255, 0, 255), COLOR_NONE, UnitPX(0));
	CompositePtr center = MakeComposite("Center", CoordPX(0, 0),
			CoordPercent(1.0, 1.0), Color(0, 255, 255));

	HSliderPtr hslider1 = HSliderPtr(
			new HorizontalSlider("Label A", CoordPerPX(0.1f, 0.3f, 0, 0),
					CoordPX(200.0f, 40.0f), Integer(1), Integer(12),
					Integer(7)));
	HSliderPtr hslider2 = HSliderPtr(
			new HorizontalSlider("Label B", CoordPercent(0.7f, 0.7f),
					CoordPX(200.0f, 50.0f), Double(1), Double(12), Double(7)));

	VSliderPtr vslider1 = VSliderPtr(
			new VerticalSlider("Slider", CoordPerPX(0.85f, 0.1f, 0, 0),
					CoordPX(100.0f, 200.0f), Integer(0), Integer(20),
					Integer(70)));
	TextFieldPtr field1 = MakeTextField("Field 1", CoordPercent(0.6f, 0.8f),
			CoordPX(200.0f, 50.0f), Theme::Default.DARK,
			Theme::Default.LIGHT_TEXT);
	FileButton* fbutton;
	center->add(
		fbutton = new FileButton("Mesh", CoordPerPX(0.2f, 0.0f, 0.0f, 0.0f), CoordPX(40, 40),
			FileDialogType::SaveFile));

	fbutton->addFileExtensionRule("Portable Network Graphics", "png");
	fbutton->addFileExtensionRule("XML", { "raw", "xml" });
	fbutton->addFileExtensionRule("Text", "txt");
	fbutton->setValue("mesh.obj");

	center->add(
		fbutton = new FileButton("Mesh", CoordPerPX(0.2f, 0.0f, 45.0f,0.0f), CoordPX(40, 40),
			FileDialogType::OpenMultiFile));

	ColorSelectorPtr colorselect = ColorSelectorPtr(
		new ColorSelector("Color", CoordPercent(0.5f, 0.0f),
			CoordPX(200, 50)));
	colorselect->setColor(Color(200, 128, 32));
	center->add(colorselect);
	fbutton->addFileExtensionRule("Portable Network Graphics", "png");
	fbutton->addFileExtensionRule("XML", { "raw", "xml" });
	fbutton->addFileExtensionRule("Text", "txt");
	fbutton->setValue("mesh.obj");
	center->add(hslider1);

	center->add(hslider2);

	center->add(vslider1);
	center->add(field1);
	bcomp->setNorth(north, UnitPercent(0.1f));
	bcomp->setSouth(south, UnitPercent(0.3f));
	bcomp->setEast(east, UnitPercent(0.2f));
	//bcomp->setWest(west,0.4);
	bcomp->setCenter(center);

	rootNode.backgroundColor = MakeColor(Color(128, 64, 92));
	rootNode.add(bcomp);
	rootNode.add(expandBar);

	return true;
}

