/*
 * AlloyCommon.h
 *
 *  Created on: May 16, 2015
 *      Author: blake
 */

#ifndef ALLOYCOMMON_H_
#define ALLOYCOMMON_H_
#include <iostream>
#include <string>
#include <sstream>
namespace aly{
	struct MakeString{
		std::ostringstream ss;
		operator std::string() const { return ss.str(); }
		template<class T> MakeString & operator << (const T & val) { ss << val; return *this; }
	};
}

#endif /* ALLOYCOMMON_H_ */
