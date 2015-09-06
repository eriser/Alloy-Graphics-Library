/*
 * Copyright(C) 2014, Blake C. Lucas, Ph.D. (img.science@gmail.com)
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
#include "AlloyMath.h"
#include "svd3.h"
namespace aly {
void SVD(const matrix<float, 3, 3>& M, matrix<float, 3, 3>& U,
		matrix<float, 3, 3>& D, matrix<float, 3, 3>& Vt) {
	svd(M(0, 0), M(0, 1), M(0, 2), M(1, 0), M(1, 1), M(1, 2), M(2, 0), M(2, 1),
			M(2, 2), U(0, 0), U(0, 1), U(0, 2), U(1, 0), U(1, 1), U(1, 2),
			U(2, 0), U(2, 1), U(2, 2), D(0, 0), D(0, 1), D(0, 2), D(1, 0),
			D(1, 1), D(1, 2), D(2, 0), D(2, 1), D(2, 2), Vt(0, 0), Vt(0, 1),
			Vt(0, 2), Vt(1, 0), Vt(1, 1), Vt(1, 2), Vt(2, 0), Vt(2, 1),
			Vt(2, 2));

}
}
