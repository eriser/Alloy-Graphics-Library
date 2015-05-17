/*
 * AlloyFileUtil.h
 *
 *  Created on: May 16, 2015
 *      Author: blake
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
	std::string GetFileWithoutExtension(const std::string& file);
	std::string GetFileNameWithoutExtension(const std::string& file);
	std::string GetFileDirectoryPath(const std::string& file);
	std::string GetFileName(const std::string& file);
	std::string RemoveTrailingSlash(const std::string& file);
	std::string ConcatPath(const std::string& dir,const std::string& file);
	std::vector<std::string> GetDirectoryListing(const std::string& dirName,const std::string& mask,const std::string& ext);
	bool WriteImageToFile(const std::string& file,const ImageRGBA& img);
	bool WriteImageToFile(const std::string& file,const ImageRGB& img);
	bool ReadImageFromFile(const std::string& file,ImageRGBA& img);
	bool ReadImageFromFile(const std::string& file,ImageRGB& img);
}





#endif /* ALLOYFILEUTIL_H_ */
