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
#include "../../include/example/ControlsEx.h"
using namespace aly;
ControlsEx::ControlsEx() :
		Application(800, 600, "Controls Example") {
}
bool ControlsEx::init(Composite& rootNode) {

	HSliderPtr hslider1 = HSliderPtr(
		new HorizontalSlider("Integer Slider", CoordPerPX(0.1f, 0.3f, 0, 0),
			CoordPX(200.0f, 40.0f), Integer(1), Integer(12),
			Integer(7)));
	rootNode.add(hslider1);

	HSliderPtr hslider2 = HSliderPtr(
		new HorizontalSlider("Double Slider", CoordPercent(0.5f, 0.7f),
			CoordPX(350.0f, 50.0f), Double(1), Double(12),
			Double(7)));
	rootNode.add(hslider2);

	VSliderPtr vslider1 = VSliderPtr(
		new VerticalSlider("Vert Slider", CoordPerPX(0.85f, 0.1f, 0, 0),
			CoordPX(100.0f, 200.0f), Integer(0), Integer(20),
			Integer(70)));
	rootNode.add(vslider1);

	CheckBoxPtr checkbox = CheckBoxPtr(
		new CheckBox("Check", CoordPX(10.0f, 100.0f), CoordPercent(0.4f, 0.07f),
			false));
	rootNode.add(checkbox);

	ToggleBoxPtr togglebox = ToggleBoxPtr(
		new ToggleBox("Toggle", CoordPX(200.0f, 40.0f),
			CoordPercent(0.4f, 0.07f), false));
	rootNode.add(togglebox);

	TextButtonPtr textButton = TextButtonPtr(
		new TextButton("Button", CoordPX(10.0f, 300.0f),
			CoordPX(100,30)));
	rootNode.add(textButton);
	TextLabelPtr textLabel = TextLabelPtr(
		new TextLabel("Label", CoordPX(120.0f, 300.0f),
			CoordPX(100,30)));
	textLabel->backgroundColor = MakeColor(128, 0, 0);
	textLabel->borderColor = MakeColor(200, 200, 200);
	textLabel->borderWidth = UnitPX(1.0f);
	textLabel->setRoundCorners(true);
	textLabel->horizontalAlignment = HorizontalAlignment::Center;
	rootNode.add(textLabel);

	ColorSelectorPtr colorselect = ColorSelectorPtr(
		new ColorSelector("Color", CoordPercent(0.5f, 0.4f),
			CoordPX(200, 50)));
	colorselect->setOrigin(Origin::MiddleCenter);
	colorselect->setColor(Color(200, 128, 32));
	rootNode.add(colorselect);

	ProgressBarPtr pbar = ProgressBarPtr(
		new ProgressBar("Progress", CoordPercent(0.05f, 0.7f),
			CoordPercent(0.4f, 0.05f)));

	rootNode.add(pbar);

	TextIconButtonPtr textIcon = std::shared_ptr<TextIconButton>(
		new TextIconButton(
			"Text Icon",
			0xf115, CoordPerPX(1.0f, 1.0f, -110.0f, -40.0f),
			CoordPX(100, 30)));
	rootNode.add(textIcon);
	IconButtonPtr iconButton = std::shared_ptr<IconButton>(
		new IconButton(
			0xf062, CoordPerPX(1.0f, 0.0f, -35.0f, 5.0f),
			CoordPX(30, 30)));
	rootNode.add(iconButton);
	TextFieldPtr tfield = MakeTextField("Text Field", CoordPercent(0.1f, 0.8f),
		CoordPX(200.0f, 50.0f), Theme::Default.LIGHT,
		Theme::Default.DARK_TEXT);
	rootNode.add(tfield);
	progressTask = std::unique_ptr<aly::RecurrentWorker>(
		new RecurrentWorker([pbar](uint64_t iter) {
		//std::cout << "Iteration " << iter << std::endl;
		pbar->setValue("Task Executing ...", (iter++) / 20.0f);
		return (iter<20);
	}, [pbar]() {
		pbar->setValue("Task Complete.", 1.0f);
	}, 100));
	progressTask->execute();


	//getContext()->setDebug(true);
	return true;
}

