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

#ifndef ALLOYFILEUTIL_H_
#define ALLOYFILEUTIL_H_
#include "AlloyImage.h"
#include <string>
#include <vector>
#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#define PATH_SEPARATOR std::string("\\")
#else
#include <dirent.h>
#define PATH_SEPARATOR std::string("/")
#endif
namespace aly{
	bool SANITY_CHECK_FILE_IO();
	std::string GetFileExtension(const std::string& fileName);
	std::string GetFileWithoutExtension(const std::string& file);
	std::string GetFileNameWithoutExtension(const std::string& file);
	std::string ReplaceFileExtension(const std::string& file,const std::string& ext);
	std::string GetFileDirectoryPath(const std::string& file);
	std::string GetFileName(const std::string& file);
	std::string RemoveTrailingSlash(const std::string& file);
	std::string ConcatPath(const std::string& dir,const std::string& file);
	std::vector<std::string> GetDirectoryListing(const std::string& dirName,const std::string& ext="",const std::string& mask="");

	void WriteImageToFile(const std::string& file,const ImageRGBA& img);
	void WriteImageToFile(const std::string& file,const ImageRGB& img);
	void ReadImageFromFile(const std::string& file,ImageRGBA& img);
	void ReadImageFromFile(const std::string& file,ImageRGB& img);
}





#endif /* ALLOYFILEUTIL_H_ */
