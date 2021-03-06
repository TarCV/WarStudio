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

#include "stdafx.h"

#include "utility.h"
#include "config.h"

#include <cctype>
#include <iostream>
#include <istream>
#include <ostream>
#include <vector>
#include <assert.h>

/*#ifndef  NDEBUG

#include "Model\wad.h"
namespace warstudio {

void g_debug(wad* base_) {
    static	wad *base;
    if (base_)	base = base_;
    int debug = base->waddata.tellp();
}

}
#endif//*/

using namespace std;

namespace warstudio {

/*
wildcard matcher
supported wildcards:
? - any character (incl. digits)
# - any digit
\ - escape char, dont treat next char as wildcard (\? matches ?, \\ matches \)
*/
bool strlike(std::string testee, std::string pattern) {
    size_t i = 0; //testee characters iterator
    bool ignorewildcardnow = false;
    for (size_t j = 0; j < pattern.length(); ++i, ++j) {
        if (i >= testee.length())  return false;
        char p = pattern[j];
        if (!ignorewildcardnow) {
            bool fallthru = false;
            if ('#' == p)
            {
                if (!isdigit(testee[i]))  return false;
            }
            else if ('?' == p)
            {} //any char goes
            else if ('*' == p)
            {
                assert(pattern.length() - 1 == j);	//todo: * for now is supported only as the last character of a pattern
                return true;
            }
            else if ('\\' == p)
            {
                ignorewildcardnow = true;
                --i; //negate ++i in the "for" statement
            }
            else
            {
                fallthru = true; //small hack to not write "continue" every time
            }
            if (!fallthru)  continue;
        }
        if (testee[i] != p)  return false;
        ignorewildcardnow = false;
    }
    if (i < testee.length()) return false;
    return true;
}
bool endsWith(std::string testee, std::string tester) {
    if (tester.length() > testee.length())	return false;
    if (tester.length() == 0)	return true;
    return (testee.substr(testee.length()-tester.length(), tester.length()) == tester);
}

void StreamCopy(istream& in, ostream& out, bool dontresetout)
{
    assert(in.good());
    assert(out.good());

    in.seekg(0, ios_base::end);	//as side effect makes more characters available to read in internal buffer
    streamsize streamlength(in.tellg());
    assert(in.good());
    if (0 == streamlength)	return;

    in.seekg(0);

    //empty stringstream becames invalid from seekp(0), so only reset when needed (if tellp != 0):
    if (!dontresetout && (int)out.tellp() != 0)	out.seekp(0);

#ifndef  NDEBUG
    //check if out is in binary mode
    int oldppos = out.tellp();
    assert(oldppos >= 0);
    out.write("\n", 1);
    assert((int)out.tellp() - oldppos == 1);
    out.seekp(oldppos);
    out.write("\r\n", 2);
    assert((int)out.tellp() - oldppos == 2);
    out.seekp(oldppos);
#endif

    vector<char> buf(WINDOWSTREAM_BUFFER_SIZE);
    while (true) {
        streamsize toread = in.rdbuf()->in_avail();
        if (toread > WINDOWSTREAM_BUFFER_SIZE)	toread = WINDOWSTREAM_BUFFER_SIZE;
        if (toread <= 0) {
            toread = 10;
            if (static_cast<int>(in.tellg()) + toread > streamlength)	toread = streamlength - in.tellg() + 1;	//always try to read at least one char
        }

#ifndef  NDEBUG
        int oldgpos;
        int newgpos, newppos;
        oldgpos = in.tellg();
        oldppos = out.tellp();
#endif

        in.read(&buf.front(), toread);
        out.write(&buf.front(), in.gcount());	//less than toread bytes can be readed if we reached eof, so use gcount()

#ifndef  NDEBUG
        if (in.gcount() > 0) {
            int written = in.gcount();

            newgpos = in.tellg();
            if (newgpos < 0)	newgpos = streamlength;
            newppos = out.tellp();

            //assert that both in and out streams are in the same mode (hopefully it's the binary mode)
            assert(written == newgpos - oldgpos);
            assert(written == newppos - oldppos);
        }
#endif

        if (!in.good())	break;
    }
    assert(in.eof());
}

void warning(string msg)
{
    //todo: change to use some message holder
    cerr << msg << endl;
}

void error(string msg)
{
    throw string(msg);
}

//TODO: really use it
void internal_error(string msg)
{
    error(msg);
}

void updateCurrentOperation(std::string operation)
{
    cout << operation << endl;
}

}
