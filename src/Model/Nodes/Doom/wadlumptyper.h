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

#include "../../lumptypes.h"

#include <string>
#include <vector>

namespace warstudio {
	namespace model {
		namespace doom {

class wadlump;

struct wadlumptypedata {
	std::string parentpattern; //is "" if no parent is needed
	std::string namepattern;
	LUMP_TYPE type;

	wadlumptypedata(std::string parent, std::string name, LUMP_TYPE type) : parentpattern(parent), namepattern(name), type(type) {}
};

class wadlumptyper/*:public lumptyper*/ {
protected:
	struct checkstruct {
		std::string lumpname;
		bool required;
	};

	typedef std::vector<wadlumptypedata> wltdvctr;
	static wltdvctr typelist;
	static void add(std::string, std::string, LUMP_TYPE type);
	static void init();
public:
	//todo: add null characters to the constants or implement this less hackish
	static const std::string _MAP;	// special constants for parents that have no fixed names
	static const std::string _ZMAP;	// (the constants have length greater than 8, so they guaranteed no to be present in a wad
	static LUMP_TYPE typeof(const wadlump &) /*override*/;	//beware, it calls baselump->parent()->type() often
	static bool checkMapSublumpNum(const wadlump &lump, int &i, bool textmap);
};

		}
	}
}
