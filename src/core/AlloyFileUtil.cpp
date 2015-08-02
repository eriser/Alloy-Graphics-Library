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
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <ios>
#include "stdint.h"
#include "AlloyFileUtil.h"
#include "AlloyCommon.h"
#include "AlloyFilesystem.h"
#if defined(WIN32) || defined(_WIN32)

#include <windows.h>
#include <codecvt>
#define WINDOWS
#else
#include <dirent.h>
#endif

#include <stdexcept>
using namespace std;

namespace aly {
std::string CodePointToUTF8(int cp) {
	int n = 0;
	if (cp < 0x80)
		n = 1;
	else if (cp < 0x800)
		n = 2;
	else if (cp < 0x10000)
		n = 3;
	else if (cp < 0x200000)
		n = 4;
	else if (cp < 0x4000000)
		n = 5;
	else if (cp <= 0x7fffffff)
		n = 6;
	char str[7];
	str[n] = '\0';
	switch (n) {
	case 6:
		str[5] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0x4000000;
	case 5:
		str[4] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0x200000;
	case 4:
		str[3] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0x10000;
	case 3:
		str[2] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0x800;
	case 2:
		str[1] = 0x80 | (cp & 0x3f);
		cp = cp >> 6;
		cp |= 0xc0;
	case 1:
		str[0] = cp;
	}
	return std::string(str);
}
std::vector<std::string> split(const std::string &str, char delim) {
	std::stringstream ss;
	std::vector<std::string> elems;
	std::string delimString = "";
	delimString += delim;
	std::string comp;
	for (char c : str) {
		if (c == delim) {
			comp = ss.str();
			if (comp.size() > 0) {
				elems.push_back(comp);
				ss.str("");
			}
			elems.push_back(delimString);
		} else {
			ss << c;
		}
	}
	comp = ss.str();
	if (comp.size() > 0) {
		elems.push_back(comp);
	}
	return elems;

}
std::string FormatSize(size_t size) {
	static const size_t kb = ((size_t)1 << (size_t)10);
	static const size_t mb = ((size_t)1 << (size_t)20);
	static const size_t gb = ((size_t)1 << (size_t)30);
	static const size_t tb = ((size_t)1 << (size_t)40);

	
	if (size < kb) {
		return  MakeString() << std::setprecision(4)<<size << " B";
	} else if (size < mb) {
		if (size%kb == 0) {
			return MakeString() << std::setw(4) << (size>>(size_t)10) << " KB";
		}
		else {
			return MakeString() << std::setw(5) << std::setprecision(2) << size / (double)kb << " KB";
		}
	} else if (size < gb) {
		if (size%mb == 0) {
			return MakeString() << std::setw(4) << (size >> (size_t)20) << " MB";
		}
		else {
			return MakeString() << std::setw(5) << std::setprecision(2) << size / (double)gb << " GB";
		}
	} else if (size < tb) {
		if (size%gb == 0) {
			return MakeString() << std::setw(4) << (size >> (size_t)30) << " GB";
		}
		else {
			return MakeString() << std::setw(5) << std::setprecision(2) << size / (double)gb << " GB";
		}
	}  else {
		if (size%tb == 0) {
			return MakeString() << std::setw(4) << (size >> (size_t)40) << " TB";
		}
		else {
			return MakeString() << std::setw(5) << std::setprecision(2) << size / (double)tb << " TB";
		}
	}
}
std::string FormatTime(const std::time_t& t) {
	struct tm* timed = localtime(&t);
	int hour = timed->tm_hour;

	if (hour == 0) {
		hour = 12;
	}
	else if (hour > 12) {
		hour -= 12;
	}
	return MakeString() << std::setw(2) << std::setfill(' ') << hour << ":" << std::setw(2) << std::setfill(' ') << timed->tm_min << " " << ((timed->tm_hour >= 12) ? "pm" : "am");
}
std::string FormatDate(const std::time_t& t) {
	struct tm* timed = localtime(&t);
	int hour = timed->tm_hour;
	if (hour == 0) {
		hour = 12;
	}
	else if (hour > 12) {
		hour -= 12;
	}
	return MakeString() <<std::setw(2) << std::setfill('0') << timed->tm_mon << "/" << std::setw(2) << std::setfill('0') << timed->tm_mday << "/" << timed->tm_year + 1900;
}
std::string FormatDateAndTime(const std::time_t& t) {
	struct tm* timed = localtime(&t);
	int hour = timed->tm_hour;
	if (hour == 0) {
		hour = 12;
	}
	else if (hour > 12) {
		hour -= 12;
	}
	return MakeString() << std::setw(2) << std::setfill(' ') << hour << ":" << std::setw(2) << std::setfill(' ') << timed->tm_min << " " << ((timed->tm_hour >= 12) ? "pm" : "am") << " " << std::setw(2) << std::setfill('0') << timed->tm_mon << "/" << std::setw(2) << std::setfill('0') << timed->tm_mday << "/" << timed->tm_year + 1900;
}
std::vector<std::string> splitPath(const std::string& file) {
	const char c = ALY_PATH_SEPARATOR.c_str()[0];
	return split(file,c);
}
std::string concat(const std::vector<std::string>& list) {
	std::stringstream ss;
	for (std::string str : list) {
		ss << str;
	}
	return ss.str();
}
std::string ReadTextFile(const std::string& str) {
	std::ifstream myfile;
	myfile.open(str);
	if (myfile.is_open()) {
		std::stringstream buffer;
		std::string line;
		while (getline(myfile, line)) {
			buffer << line << std::endl;
		}
		return buffer.str();
	} else {
		throw runtime_error(MakeString() << "Could not open " << str);
	}
}
std::vector<char> ReadBinaryFile(const std::string& str) {
	streampos size;
	ifstream file(str, ios::in | ios::binary | ios::ate);
	if (file.is_open()) {
		size = file.tellg();
		std::vector<char> memblock((size_t) size);
		file.seekg(0, ios::beg);
		file.read(memblock.data(), size);
		file.close();
		return memblock;
	} else
		throw runtime_error(MakeString() << "Could not open " << str);
}
bool FileExists(const std::string& name) {
	try {
		return (filesystem::internal::exists(name));
	} catch (...) {
		return false;
	}
}
bool IsDirectory(const std::string& file) {
	return FileExists(file) && filesystem::internal::is_directory(file);
}
bool IsFile(const std::string& file) {
	return FileExists(file) && filesystem::internal::is_file(file);
}
std::vector<std::string> AutoComplete(const std::string& str,
		const std::vector<std::string>& list, int maxSuggestions) {
	std::vector<std::string> suggestions = list;
#ifdef WINDOWS

	static const std::locale local;
	//use case insenstive complete on windows.
	std::sort(suggestions.begin(),suggestions.end(),
			[](const std::string& first, const std::string& second) {
				return std::lexicographical_compare(first.begin(),first.end(),second.begin(),second.end(),
						[](char c1, char c2) {return (std::tolower(c1,local)<std::tolower(c2,local));});
			}
	);
#else
	std::sort(suggestions.begin(), suggestions.end(),
			[](const std::string& first, const std::string& second) {
				return std::lexicographical_compare(first.begin(),first.end(),second.begin(),second.end());
			});
#endif

	for (size_t index = 0; index < suggestions.size(); index++) {
		std::string entry = suggestions[index];
#ifdef WINDOWS
		if(std::lexicographical_compare(str.begin(),str.end(),entry.begin(),entry.end(),
						[](char c1, char c2) {return (std::tolower(c1,local)<std::tolower(c2,local));})) {
#else
		if (std::lexicographical_compare(str.begin(), str.end(), entry.begin(),
				entry.end())) {
#endif
			suggestions.erase(suggestions.begin(), suggestions.begin() + index);
			break;
		}
	}

	if (maxSuggestions > 0 && suggestions.size() > 0) {
		suggestions.erase(
				suggestions.begin()
						+ std::min((size_t) maxSuggestions, suggestions.size()),
				suggestions.end());
	}
	return suggestions;
}
std::string GetFileExtension(const std::string& fileName) {
	size_t pos = fileName.find_last_of(".");
	if (pos != string::npos) {
		if (pos == fileName.size() - 1) {
			return "";
		} else {
			return fileName.substr(pos + 1);
		}
	}
	return string("");
}

std::string GetFileName(const std::string& fileName) {
	size_t pos = fileName.find_last_of(ALY_PATH_SEPARATOR);
	if ( pos!= string::npos) {
		if(pos==fileName.size()-1) {
			return "";
		} else {
			return fileName.substr(pos+ 1);
		}
	}
	return fileName;
}
std::string RemoveTrailingSlash(const std::string& file) {
	size_t pos = file.find_last_of(ALY_PATH_SEPARATOR);
	if (pos != string::npos&&pos==file.size()-1) {
		return file.substr(0, pos);
	}
	return file;
}
std::string ConcatPath(const std::string& dir, const std::string& file) {
	return RemoveTrailingSlash(dir) + ALY_PATH_SEPARATOR+file;
}
std::string GetFileNameWithoutExtension(const std::string& fileName) {
	if (fileName.find_last_of(".") != string::npos
			&& fileName.find_last_of(ALY_PATH_SEPARATOR) != string::npos) {
		size_t start = fileName.find_last_of(ALY_PATH_SEPARATOR) + 1;
		size_t end = fileName.find_last_of(".");
		return fileName.substr(start, end - start);
	}
	return fileName;
}

std::string GetFileWithoutExtension(const std::string& fileName) {
	if (fileName.find_last_of(".") != string::npos) {
		size_t end = fileName.find_last_of(".");
		return fileName.substr(0, end);
	}
	return fileName;
}
std::string ReplaceFileExtension(const std::string& file,
		const std::string& ext) {
	return GetFileWithoutExtension(file) + "." + ext;
}
std::string GetFileDirectoryPath(const std::string& fileName) {
	if (fileName.find_last_of(ALY_PATH_SEPARATOR) != string::npos) {
		size_t end = fileName.find_last_of(ALY_PATH_SEPARATOR);
		return fileName.substr(0, end);
	}
	return fileName;
}
std::string GetParentDirectory(const std::string& file) {
	if (file.find_last_of(ALY_PATH_SEPARATOR) != string::npos&&file.find_last_of(ALY_PATH_SEPARATOR) == file.size() - 1) {
		return file;
	}
	else {
		return GetFileDirectoryPath(file) + ALY_PATH_SEPARATOR;
	}
}
#ifndef WINDOWS
std::vector<std::string> GetDirectoryFileListing(const std::string& dirName,
		const std::string& ext, const std::string& mask) {
	std::vector<std::string> files;
	dirent* dp;
	std::string cleanPath = RemoveTrailingSlash(dirName) + ALY_PATH_SEPARATOR;
	DIR* dirp = opendir(cleanPath.c_str());
	while ((dp = readdir(dirp)) != NULL) {
		string fileName(dp->d_name);
		if (dp->d_type == DT_REG) {
			if (ext.length() == 0 || GetFileExtension(fileName) == ext) {
				if (mask.length() == 0
						|| (mask.length() > 0
								&& fileName.find(mask) < fileName.length()))
					files.push_back(cleanPath + fileName);
			}
		}
	}
	closedir(dirp);
	std::sort(files.begin(), files.end());
	return files;
}
std::vector<FileDescription> GetDirectoryDescriptionListing(
		const std::string& dirName) {
	std::vector<std::pair<std::string, FileType>> files;
	dirent* dp;
	std::string cleanPath = RemoveTrailingSlash(dirName) + ALY_PATH_SEPARATOR;
	DIR* dirp = opendir(cleanPath.c_str());
	if (dirp) {
		while ((dp = readdir(dirp)) != NULL) {
			string fileName(dp->d_name);
			FileType type = FileType::Unknown;
			if (dp->d_type == DT_REG) {
				type = FileType::File;
			} else if (dp->d_type == DT_DIR) {
				type = FileType::Directory;
			} else if (dp->d_type == DT_LNK) {
				type = FileType::Link;
			}
			if (type != FileType::Unknown) {
				if (fileName != ".." && fileName != ".") {
					files.push_back(
							std::pair<std::string, FileType>(
									cleanPath + fileName, type));
				}
			}
		}
		closedir(dirp);
		std::sort(files.begin(), files.end());
	}
	return files;
}
std::vector<std::string> GetDirectoryListing(const std::string& dirName) {
	std::vector<std::string> files;
	dirent* dp;
	std::string cleanPath = RemoveTrailingSlash(dirName) + ALY_PATH_SEPARATOR;
	DIR* dirp = opendir(cleanPath.c_str());
	if (dirp) {
		while ((dp = readdir(dirp)) != NULL) {
			string fileName(dp->d_name);
			FileType type = FileType::Unknown;
			if (dp->d_type == DT_REG || dp->d_type == DT_DIR
					|| dp->d_type == DT_LNK) {
				if (fileName != ".." && fileName != ".") {
					files.push_back(cleanPath + fileName);
				}
			}
		}
		closedir(dirp);
		std::sort(files.begin(), files.end());
	}
	return files;
}
#else 

std::wstring ToWString(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(str);
	return wide;
}

std::string ToString(const std::wstring& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string narrow = converter.to_bytes(str);
	return narrow;
}
std::time_t FileTimeToTime(const FILETIME& ft) {
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return std::time_t(ull.QuadPart / 100000000ULL - 1164);
}
std::vector<FileDescription> GetDirectoryDescriptionListing(const std::string& dirName) {
	std::vector<FileDescription> files;
	WIN32_FIND_DATAW fd;
	std::string path = RemoveTrailingSlash(dirName);
	std::wstring query = ToWString(path + ALY_PATH_SEPARATOR + string("*"));
	HANDLE h = FindFirstFileW(query.c_str(), &fd);
	if (h == INVALID_HANDLE_VALUE) {
		return files;
	}
	do {
		std::string fileName = ToString(fd.cFileName);
		if (fileName != "." && fileName != "..")
		{
			std::string fileLocation = path + ALY_PATH_SEPARATOR + fileName;
			FileType fileType = FileType::Unknown;
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				fileType = FileType::Directory;
			} else if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) {
				fileType = FileType::File;
			} 
			if (fileType != FileType::Unknown) {
				std::time_t creationTime = FileTimeToTime(fd.ftCreationTime);
				std::time_t modifiedTime = FileTimeToTime(fd.ftLastWriteTime);
				ULARGE_INTEGER ull;
				ull.LowPart = fd.nFileSizeLow;
				ull.HighPart = fd.nFileSizeHigh;
				size_t fileSize = (size_t)ull.QuadPart;
				files.push_back(FileDescription(fileLocation, fileType, fileSize, fd.dwFileAttributes&&FILE_ATTRIBUTE_READONLY, creationTime, modifiedTime));
			}
		}
	}while (FindNextFile(h, &fd));
	FindClose(h);
	return files;
}
std::vector<std::string> GetDirectoryListing(const std::string& dirName) {
	WIN32_FIND_DATAW fd;
	std::string path = RemoveTrailingSlash(dirName);
	std::wstring query = ToWString(path + ALY_PATH_SEPARATOR + string("*"));
	HANDLE h = FindFirstFileW(query.c_str(), &fd);
	if (h == INVALID_HANDLE_VALUE) {
		return std::vector<std::string>();
	}
	std::vector<std::string> list;
	do {
		std::string fileName = ToString(fd.cFileName);
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) {
			if (fileName != "." && fileName != "..")
			{
				list.push_back(path + ALY_PATH_SEPARATOR + fileName);
			}
		}
	}while (FindNextFile(h, &fd));
	FindClose(h);
	return list;
}
std::vector<std::string> GetDirectoryFileListing(const std::string& dirName,const std::string& ext, const std::string& mask) {
	WIN32_FIND_DATAW fd;
	std::string path = RemoveTrailingSlash(dirName);
	std::wstring query = ToWString(path + ALY_PATH_SEPARATOR+string("*"));
	HANDLE h = FindFirstFileW(query.c_str(), &fd);
	if (h == INVALID_HANDLE_VALUE) {
		return std::vector<std::string>();
	}
	std::vector<std::string> list;
	do {
		std::string fileName = ToString(fd.cFileName);
		if (fileName != "." && fileName != "..")
		{
			if (ext.length() == 0 || GetFileExtension(fileName) == ext) {
				if (mask.length() == 0|| (mask.length() > 0&& fileName.find(mask) < fileName.length())) {
					if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)&& !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) list.push_back(path + ALY_PATH_SEPARATOR + fileName);
				}
			}
		}
	}while (FindNextFile(h, &fd));
	FindClose(h);
	return list;
}
#endif

}

