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
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#include <string>
#include <vector>
namespace aly {
#if defined(WIN32) || defined(_WIN32)
#define ALY_PATH_SEPARATOR std::string("\\")
#else
#define ALY_PATH_SEPARATOR std::string("/")
#endif
bool SANITY_CHECK_FILE_IO();
enum class FileType{Unknown,File,Directory,Link};
std::string GetFileExtension(const std::string& fileName);
std::string GetFileWithoutExtension(const std::string& file);
std::string GetFileNameWithoutExtension(const std::string& file);
std::string GetParentDirectory(const std::string& dir);
std::string ReplaceFileExtension(const std::string& file,
		const std::string& ext);
std::string GetFileDirectoryPath(const std::string& file);
std::string GetFileName(const std::string& file);
std::string RemoveTrailingSlash(const std::string& file);
std::string ConcatPath(const std::string& dir, const std::string& file);
std::vector<std::string> GetDirectoryFileListing(const std::string& dirName,
		const std::string& ext = "", const std::string& mask = "");
std::vector<std::pair<std::string,FileType>> GetDirectoryListingAndTypes(const std::string& dirName);
std::vector<std::string> GetDirectoryListing(const std::string& dirName);
std::string ReadTextFile(const std::string& str);
std::vector<char> ReadBinaryFile(const std::string& str);
bool FileExists(const std::string& name);
bool IsDirectory(const std::string& file);
bool IsFile(const std::string& file);
std::string CodePointToUTF8(int cp);
std::vector<std::string> split(const std::string& str,char c);
std::vector<std::string> splitPath(const std::string& file);
std::string concat(const std::vector<std::string>& list);
std::vector<std::string> AutoComplete(const std::string& str,const std::vector<std::string>& list,int maxSuggestions=-1);
}

#endif /* ALLOYFILEUTIL_H_ */
