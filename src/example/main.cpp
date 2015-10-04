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
	try {

		//UI Test case
		//ExampleUI app;

		//Mesh Render Test case
		MeshViewer app;

		//SANITY_CHECK_ANY();
		//SANITY_CHECK_SVD();
		//SANITY_CHECK_ALGO();
		//SANITY_CHECK_IMAGE();
		//SANITY_CHECK_UI();
		//SANITY_CHECK_CEREAL();
		//SANITY_CHECK_KDTREE();
		//SANITY_CHECK_PYRAMID();
		//SANITY_CHECK_SPARSE_SOLVE();
		//SANITY_CHECK_DENSE_SOLVE();
		//SANITY_CHECK_DENSE_MATRIX();
		//SANITY_CHECK_IMAGE_PROCESSING();
		//SANITY_CHECK_IMAGE_IO();
		//SANITY_CHECK_ROBUST_SOLVE();
		//SANITY_CHECK_SUBDIVIDE();
		app.run(1);
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

