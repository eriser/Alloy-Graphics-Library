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
#include "AlloyDenseSolve.h"
namespace aly {
	void PoissonFill(const Image4f& in, Image4f& out, int iterations , float lambda) {
		Image4f divergence(in.width, in.height);
		divergence.set(float4(0.0f));
#pragma omp parallel for
		for (int j = 1; j < in.height - 1; j++) {
			for (int i = 1; i < in.width - 1; i++) {
				float4 val1 = in(i, j);
				float4 val2 = in(i, j + 1);
				float4 val3 = in(i, j - 1);
				float4 val4 = in(i + 1, j);
				float4 val5 = in(i - 1, j);
				float4 div = val1 - 0.25f*(val2 + val3 + val4 + val5);
				if (val1.w > 0.0f&&val2.w>0.0f&&val3.w > 0.0f&&val4.w > 0.0f&&val5.w > 0.0f) {
					div.w = 0.0f;
				}
				else {
					div = float4(0.0f);
				}
				out(i, j) = val1;
				divergence(i, j) = div;
			}
		}
		for (int iter = 0; iter < iterations; iter++) {
			for (int k = 0; k < 4; k++) {
#pragma omp parallel for
				for (int j = 0; j < in.height; j += 2) {
					for (int i = 0; i < in.width; i += 2) {
						if (in(i,j).w>0) {
							float4 div = out(i, j) - 0.25f*(out(i, j - 1) + out(i, j + 1) + out(i - 1, j) + out(i + 1, j));
							div = (div - divergence(i, j));
							div.w = 0.0f;
							out(i, j) -= lambda*div;
						}
					}
				}
			}
		}
		for (float4& c : out.data) {
			if (c.w > 0.0f) {
				c /= c.w;
			}
		}
	}
}
