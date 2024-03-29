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

#ifndef ALLOYCOMMON_H_
#define ALLOYCOMMON_H_
#include "tinyformat.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ios>
#include <locale>
namespace aly {
	struct MakeString {
		std::ostringstream ss;
		operator std::string() const {
			return ss.str();
		}
		template<class T> MakeString & operator <<(const T & val) {
			ss.setf(std::ios::fixed, std::ios::floatfield);
			ss << val;
			return *this;
		}
	};
	inline bool Contains(const std::string& str, const std::string& pattern) {
		return (str.find(pattern) != std::string::npos);
	}
	inline std::string ToLower(const std::string& str) {
		std::stringstream ss;
		static const std::locale local;
		for (char c : str) {
			ss << std::tolower(c,local);
		}
		return ss.str();
	}
	inline std::string ToUpper(const std::string& str) {
		std::stringstream ss;
		static const std::locale local;
		for (char c : str) {
			ss << std::toupper(c, local);
		}
		return ss.str();
	}
	inline bool ContainsIgnoreCase(const std::string& str, const std::string& pattern) {
		std::string strl = ToLower(str);
		std::string patternl = ToLower(pattern);
		return (strl.find(patternl) != std::string::npos);
	}
}

#endif /* ALLOYCOMMON_H_ */
