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
#include "../../include/example/AlloyExampleUI.h"
#include "../../include/example/MeshViewer.h"
#include "AlloyFileUtil.h"
using namespace aly;
int main() {

	std::vector<std::string> fileTest={"/","/var/tmp/","/var/tmp","/usr/lib/up"};
	for(std::string str:fileTest){
		std::vector<std::string> listing;
		std::string rootDir=GetParentDirectory(str);
		std::cout<<str<<" ROOT "<<rootDir<<std::endl;
		std::vector<std::string> files=GetDirectoryListing(rootDir);
		std::vector<std::string> suggestions=AutoComplete(str,files,5);
		int i=0;
		for(std::string suggestion:suggestions){
			std::cout<<i++<<") "<<suggestion<<std::endl;
		}
	}

	try {
		//SANITY_CHECK_IMAGE();
		//SANITY_CHECK_UI();
		//ExampleUI app;
		//std::cout<<*AlloyApplicationContext()->getFont(FontType::Normal)<<std::endl;
		MeshViewer app;
		app.run(0);
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

