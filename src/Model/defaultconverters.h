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

#include <boost/ptr_container/ptr_map.hpp>
#include <map>
#include <iosfwd>

#include "Archivers/ArchiverRegistry.h"
#include "Converters/ConverterRegistry.h"
#include "Writers/WriterRegistry.h"
#include "lumptypes.h"
#include "AdapterChain.h"


namespace warstudio {
	namespace model {

class BaseContainer;
class BaseNode;
class DefaultConverters {	//TODO: implement this class using Global guidelines
public:
/*	static BaseContainer* Extract(LUMP_TYPE, std::istream&, int size);
	static void Write(const BaseContainer& in, std::ostream& out);

	static BaseContainer* Extract(const BaseNode&);
	static void Write(const BaseContainer& in, std::string& outfile);
*/
/*	static const BaseContainer* Read(std::istream& in, int size);
	static void Archive(const BaseContainer& in, std::ostream& out);
	static const BaseContainer* Read(std::string& infile);
	static void Archive(const BaseContainer& in, BaseNode& out);*/

	static std::string getFileExtension(const BaseNode& node);
	static std::string extractWrite(const BaseNode& node, const std::string& outfile);

private:
	DefaultConverters();
	static void init();

private:
    typedef std::map<LUMP_TYPE, AdapterChain>			ConvMap;
	static ConvMap		convertpaths;
};

	}
}
