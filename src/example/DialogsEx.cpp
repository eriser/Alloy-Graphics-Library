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
#include "../../include/example/DialogsEx.h"
using namespace aly;
DialogsEx::DialogsEx() :
		Application(800, 600, "Dialog Example") {
}
bool DialogsEx::init(Composite& rootNode) {
	FileButtonPtr saveButton=FileButtonPtr(new FileButton("Save File", CoordPerPX(0.2f, 0.0f, 0.0f, 5.0f),CoordPX(40, 40), FileDialogType::SaveFile));
	FileButtonPtr openMultiButton = FileButtonPtr(new FileButton("Open Multi-File", CoordPerPX(0.2f, 0.0f, 45.0f, 5.0f), CoordPX(40, 40), FileDialogType::OpenMultiFile));
	FileSelectorPtr fileSelector=FileSelectorPtr(new FileSelector("Selector", CoordPerPX(0.5f, 0.0f,0.0f,90.0f),CoordPX(300.0f, 30.0f)));
	ColorSelectorPtr colorselect = ColorSelectorPtr(new ColorSelector("Color", CoordPerPX(0.2f, 0.0f, 90.0f, 5.0f), CoordPX(150, 40)));
	ListBoxPtr listBox = ListBoxPtr(new ListBox("List Box", CoordPX( 5.0f, 90.0f),CoordPX(300,300)));
	for (int i = 0;i < 30;i++) {
		listBox->addEntry(ListEntryPtr(new ListEntry(listBox.get(),MakeString()<<"Multi-Selection Entry ("<<i<<")",30.0f)));
	}
	listBox->update();
	listBox->setEnableMultiSelection(true);

	rootNode.add(saveButton);
	rootNode.add(openMultiButton);
	rootNode.add(fileSelector);
	rootNode.add(colorselect);
	rootNode.add(listBox);

	using extensions = std::initializer_list<std::string>;
	std::string exampleFile = getContext()->getFullPath("images" + ALY_PATH_SEPARATOR + "sfsunset.png");
	fileSelector->addFileExtensionRule("Portable Network Graphics", "png");
	fileSelector->addFileExtensionRule("XML", extensions { "raw", "xml" });
	fileSelector->addFileExtensionRule("Text", "txt");
	fileSelector->setValue(exampleFile);

	saveButton->addFileExtensionRule("Portable Network Graphics", "png");
	saveButton->addFileExtensionRule("XML", extensions { "raw", "xml" });
	saveButton->addFileExtensionRule("Text", "txt");
	saveButton->setValue(exampleFile);

	openMultiButton->addFileExtensionRule("Portable Network Graphics", "png");
	openMultiButton->addFileExtensionRule("XML", extensions { "raw", "xml" });
	openMultiButton->addFileExtensionRule("Text", "txt");
	openMultiButton->setValue(exampleFile);
	return true;
}

