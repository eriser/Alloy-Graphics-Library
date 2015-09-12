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
#include "AlloyFileUtil.h"
namespace aly {
void LaplaceFill(const Image4f& sourceImg, Image4f& targetImg, int iterations,
		int levels, float lambda) {
	if (sourceImg.dimensions() != targetImg.dimensions())
		throw std::runtime_error(
				"Cannot solve. Image dimensions do not match ");
	if (levels <= 1) {
		PoissonBlend(sourceImg, targetImg, iterations, lambda);
	} else {
		std::vector<Image4f> srcPyramid(levels);
		std::vector<Image4f> tarPyramid(levels);
		srcPyramid[0] = sourceImg;
		tarPyramid[0] = targetImg;
		for (int l = 1; l < levels; l++) {
			srcPyramid[l - 1].downsample(srcPyramid[l]);
			tarPyramid[l - 1].downsample(tarPyramid[l]);
		}
		for (int l = levels - 1; l >= 1; l--) {
			LaplaceFill(srcPyramid[l], tarPyramid[l], iterations, lambda);
			tarPyramid[l].upsample(tarPyramid[l - 1]);
		}
		targetImg = tarPyramid[0];
		LaplaceFill(sourceImg, targetImg, iterations, lambda);
	}
}

void LaplaceFill(const Image4f& sourceImg, Image4f& targetImg, int iterations,
		float lambda) {
	if (sourceImg.dimensions() != targetImg.dimensions())
		throw std::runtime_error(
				MakeString()
						<< "Cannot solve Laplace. Image dimensions do not match "
						<< sourceImg.dimensions() << " "
						<< targetImg.dimensions());
	Image4f divergence(sourceImg.width, sourceImg.height);
	divergence.set(float4(0.0f));
#pragma omp parallel for
	for (int j = 1; j < sourceImg.height - 1; j++) {
		for (int i = 1; i < sourceImg.width - 1; i++) {
			if (sourceImg(i, j).w > 0) {
				float4 val1 = sourceImg(i, j);
				float4 val2 = sourceImg(i, j + 1);
				float4 val3 = sourceImg(i, j - 1);
				float4 val4 = sourceImg(i + 1, j);
				float4 val5 = sourceImg(i - 1, j);
				float4 div = val1 - 0.25f * (val2 + val3 + val4 + val5);
				div.w = 0.0f;
				divergence(i, j) = div;
				targetImg(i, j) = float4(sourceImg(i, j).xyz(), 1.0f);
			} else {
				divergence(i, j) = float4(0.0f);
				targetImg(i, j).w = 1.0;
			}
		}
	}
	const int xShift[] = { 0, 0, 1, 1 };
	const int yShift[] = { 0, 1, 0, 1 };
	for (int iter = 0; iter < iterations; iter++) {
		for (int k = 0; k < 4; k++) {
#pragma omp parallel for
			for (int j = yShift[k]+1; j < sourceImg.height-1; j += 2) {
				for (int i = xShift[k]+1; i < sourceImg.width-1; i += 2) {
					float4 div = targetImg(i, j)
							- 0.25f
									* (targetImg(i, j - 1) + targetImg(i, j + 1)
											+ targetImg(i - 1, j)
											+ targetImg(i + 1, j));
					div = (div - divergence(i, j));
					targetImg(i, j) -= lambda * div;
				}
			}
		}
	}
}
void PoissonBlend(const Image4f& sourceImg, Image4f& targetImg, int iterations,
		int levels, float lambda) {
	if (sourceImg.dimensions() != targetImg.dimensions())
		throw std::runtime_error(
				"Cannot solve. Image dimensions do not match ");
	if (levels <= 1) {
		PoissonBlend(sourceImg, targetImg, iterations, lambda);
	} else {
		std::vector<Image4f> srcPyramid(levels);
		std::vector<Image4f> tarPyramid(levels);
		srcPyramid[0] = sourceImg;
		tarPyramid[0] = targetImg;
		for (int l = 1; l < levels; l++) {
			srcPyramid[l - 1].downsample(srcPyramid[l]);
			tarPyramid[l - 1].downsample(tarPyramid[l]);
		}
		for (int l = levels - 1; l >= 1; l--) {
			PoissonBlend(srcPyramid[l], tarPyramid[l], iterations, lambda);
			tarPyramid[l].upsample(tarPyramid[l - 1]);
		}
		targetImg = tarPyramid[0];
		PoissonBlend(sourceImg, targetImg, iterations, lambda);
	}
}
void PoissonBlend(const Image4f& sourceImg, Image4f& targetImg, int iterations,
		float lambda) {
	if (sourceImg.dimensions() != targetImg.dimensions())
		throw std::runtime_error(
				MakeString()
						<< "Cannot solve Laplace. Image dimensions do not match "
						<< sourceImg.dimensions() << " "
						<< targetImg.dimensions());
	Image4f divergence(sourceImg.width, sourceImg.height);
#pragma omp parallel for
	for (int j = 1; j < sourceImg.height - 1; j++) {
		for (int i = 1; i < sourceImg.width - 1; i++) {
			float4 val1 = sourceImg(i, j);
			float4 val2 = sourceImg(i, j + 1);
			float4 val3 = sourceImg(i, j - 1);
			float4 val4 = sourceImg(i + 1, j);
			float4 val5 = sourceImg(i - 1, j);
			float4 div = val1 - 0.25f * (val2 + val3 + val4 + val5);
			div.w = 0.0f;
			divergence(i, j) = div;

		}
	}
	const int xShift[] = { 0, 0, 1, 1 };
	const int yShift[] = { 0, 1, 0, 1 };
	for (int iter = 0; iter < iterations; iter++) {
		for (int k = 0; k < 4; k++) {
#pragma omp parallel for
			for (int j = yShift[k]+1; j < sourceImg.height-1; j += 2) {
				for (int i = xShift[k]+1; i < sourceImg.width-1; i += 2) {
					float4 div = targetImg(i, j)
							- 0.25f
									* (targetImg(i, j - 1) + targetImg(i, j + 1)
											+ targetImg(i - 1, j)
											+ targetImg(i + 1, j));
					div = (div - divergence(i, j));
					targetImg(i, j) -= lambda * div;
				}
			}
		}
	}
}

}
