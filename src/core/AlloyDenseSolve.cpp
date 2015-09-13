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
				MakeString() << "Cannot solve. Image dimensions do not match "
						<< sourceImg.dimensions() << " "
						<< targetImg.dimensions());
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
				MakeString() << "Cannot solve. Image dimensions do not match "
						<< sourceImg.dimensions() << " "
						<< targetImg.dimensions());
	Image4f divergence(sourceImg.width, sourceImg.height);
	divergence.set(float4(0.0f));
#pragma omp parallel for
	for (int j = 1; j < sourceImg.height - 1; j++) {
		for (int i = 1; i < sourceImg.width - 1; i++) {
			float4 src = sourceImg(i, j);
			float4 tar = targetImg(i, j);
			float alpha = src.w;
			tar.w = 1.0f;
			src.w = 1.0f;
			float4 val1 = sourceImg(i, j);
			float4 val2 = sourceImg(i, j + 1);
			float4 val3 = sourceImg(i, j - 1);
			float4 val4 = sourceImg(i + 1, j);
			float4 val5 = sourceImg(i - 1, j);
			float4 div = val1 - 0.25f * (val2 + val3 + val4 + val5);
			div.w = 0.0f;
			divergence(i, j) = alpha * div;
			targetImg(i, j) = mix(tar, src, alpha);
		}
	}
	const int xShift[] = { 0, 0, 1, 1 };
	const int yShift[] = { 0, 1, 0, 1 };
	for (int iter = 0; iter < iterations; iter++) {
		for (int k = 0; k < 4; k++) {
			//Assumes color at boundary of target image is fixed!
#pragma omp parallel for
			for (int j = yShift[k] + 1; j < sourceImg.height - 1; j += 2) {
				for (int i = xShift[k] + 1; i < sourceImg.width - 1; i += 2) {
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
void PoissonInpaint(const Image4f& sourceImg, const Image4f& targetImg,
		Image4f& outImg, int iterations, int levels, float lambda) {
	//Assumes mask is encoded in the W channel of the source image.
	if (sourceImg.dimensions() != targetImg.dimensions()
			|| sourceImg.dimensions() != outImg.dimensions())
		throw std::runtime_error(
				MakeString() << "Cannot solve. Image dimensions do not match "
						<< sourceImg.dimensions() << " "
						<< targetImg.dimensions());
	if (levels <= 1) {
		PoissonInpaint(sourceImg, targetImg, outImg, iterations, lambda);
	} else {
		std::vector<Image4f> srcPyramid(levels);
		std::vector<Image4f> tarPyramid(levels);
		std::vector<Image4f> outPyramid(levels);
		srcPyramid[0] = sourceImg;
		tarPyramid[0] = targetImg;
		outPyramid[0] = outImg;
		for (int l = 1; l < levels; l++) {
			srcPyramid[l - 1].downsample(srcPyramid[l]);
			tarPyramid[l - 1].downsample(tarPyramid[l]);
			outPyramid[l - 1].downsample(outPyramid[l]);
		}
		for (int l = levels - 1; l >= 1; l--) {
			PoissonInpaint(srcPyramid[l], tarPyramid[l], outPyramid[l],
					iterations, lambda);
			outPyramid[l].upsample(outPyramid[l - 1]);
		}
		outImg = outPyramid[0];
		PoissonInpaint(sourceImg, targetImg, outImg, iterations, lambda);
	}
}
void PoissonInpaint(const Image4f& sourceImg, const Image4f& targetImg,
		Image4f& outImg, int iterations, float lambda) {
	//Assumes mask is encoded in the W channel of the source image.
	if (sourceImg.dimensions() != targetImg.dimensions()
			|| sourceImg.dimensions() != outImg.dimensions())
		throw std::runtime_error(
				MakeString() << "Cannot solve. Image dimensions do not match "
						<< sourceImg.dimensions() << " "
						<< targetImg.dimensions());
	Image4f divergence(sourceImg.width, sourceImg.height);
	divergence.set(float4(0.0f));
#pragma omp parallel for
	for (int j = 1; j < sourceImg.height - 1; j++) {
		for (int i = 1; i < sourceImg.width - 1; i++) {
			float4 src = sourceImg(i, j);
			float4 out = outImg(i, j);
			float alpha = src.w;

			out.w = 1.0f;
			src.w = 1.0f;
			float4 val1 = sourceImg(i, j);
			float4 val2 = sourceImg(i, j + 1);
			float4 val3 = sourceImg(i, j - 1);
			float4 val4 = sourceImg(i + 1, j);
			float4 val5 = sourceImg(i - 1, j);
			float4 divSrc = val1 - 0.25f * (val2 + val3 + val4 + val5);
			divSrc.w = 0.0f;

			val1 = targetImg(i, j);
			val2 = targetImg(i, j + 1);
			val3 = targetImg(i, j - 1);
			val4 = targetImg(i + 1, j);
			val5 = targetImg(i - 1, j);
			float4 divTar = val1 - 0.25f * (val2 + val3 + val4 + val5);
			divTar.w = 0.0f;
			divergence(i, j) = mix(divTar, divSrc, alpha);
		}
	}
	const int xShift[] = { 0, 0, 1, 1 };
	const int yShift[] = { 0, 1, 0, 1 };
	for (int iter = 0; iter < iterations; iter++) {
		for (int k = 0; k < 4; k++) {
			//Assumes color at boundary of target image is fixed!
#pragma omp parallel for
			for (int j = yShift[k] + 1; j < sourceImg.height - 1; j += 2) {
				for (int i = xShift[k] + 1; i < sourceImg.width - 1; i += 2) {
					float4 div = outImg(i, j)
							- 0.25f
									* (outImg(i, j - 1) + outImg(i, j + 1)
											+ outImg(i - 1, j)
											+ outImg(i + 1, j));
					div = (div - divergence(i, j));
					outImg(i, j) -= lambda * div;
				}
			}
		}
	}
}

void PoissonBlend(const Image4f& sourceImg, Image4f& targetImg, int iterations,
		int levels, float lambda) {
	if (sourceImg.dimensions() != targetImg.dimensions())
		throw std::runtime_error(
				MakeString() << "Cannot solve. Image dimensions do not match "
						<< sourceImg.dimensions() << " "
						<< targetImg.dimensions());
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
				MakeString() << "Cannot solve. Image dimensions do not match "
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
			//Assumes color at boundary of target image is fixed!
#pragma omp parallel for
			for (int j = yShift[k] + 1; j < sourceImg.height - 1; j += 2) {
				for (int i = xShift[k] + 1; i < sourceImg.width - 1; i += 2) {
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