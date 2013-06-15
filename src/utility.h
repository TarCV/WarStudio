/*

	Copyright 2012 Constantin Tarc <whitetrakon@gmail.com>

	This file is part of WadDirectory.

    WadDirectory is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    WadDirectory is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with WadDirectory.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <string>
#include <iostream>
#include <assert.h>
#include <memory>

namespace warstudio {

//todo: use compiler which support 'export' keyword and move the function to the cpp file
template <class T> /*export*/
	int numDigits(T number)
{
	if (0 == number)	return 1;
    int digits = 0;
    
	if (number < 0)	digits = 1;
    while (number) {
		assert(number <= -1 || number >= 1);	//catch floating point problems
        number /= 10;
        digits++;
    }
    return digits;
}

template <class T>
	T readFromStream(std::istream& in)
{
	//todo: implement appropiate static assertion
	T ret;
	in.read(reinterpret_cast<char *>(&ret), sizeof(ret));
	assert(in.gcount() == sizeof(ret));
	return ret;
}
template <class T, size_t N>
	void readFromStream(std::istream& in, T (&ret)[N])
{
	//todo: implement appropiate static assertion
	in.read(reinterpret_cast<char *>(&ret), sizeof(ret));
	assert(in.gcount() == sizeof(ret));
}

template <class T>
	void writeToStream(std::ostream& out, T data)
{
	//todo: implement appropiate static assertion
	out.write(reinterpret_cast<const char *>(&data), sizeof(data));
}

template <typename T_SRC, typename T_DEST>
    void unique_ptr_castmove(std::unique_ptr<T_SRC> && src,
                             std::unique_ptr<T_DEST> & dest
                            )
{
    if (!src) {
        dest.reset();
        return;
    }

    T_DEST * dest_ptr = dynamic_cast<T_DEST *>(src.get());

    if (!dest_ptr)
    {
        throw std::bad_cast();
    }

    src.release();
    dest.reset(dest_ptr);
}

/*

void IsAggregate()	//failes at compile-time if T is not an aggregate
{
	T x = {};
}

void IsPOD()	//fails at compile-time if T is not a POD
{
  goto label;
  T x;
label:
}

*/

bool strlike(std::string testee, std::string pattern);
bool endsWith(std::string testee, std::string tester);


void StreamCopy(std::istream& in, std::ostream& out, bool dontresetout = false);

void warning(std::string);
void error(std::string);
void updateCurrentOperation(std::string);

/*#ifndef  NDEBUG

class wad;
void g_debug(wad* base = nullptr);

#endif//*/

}
