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

#include "wadlump.h"

#include "../DirectoryNode.h"
#include "../../../utility.h"

#include <string>
#include <iosfwd>
#include <fstream>
#include <boost/ptr_container/ptr_list.hpp>

namespace warstudio {

	class iwindowstream;

	namespace model {
		namespace doom {

class wad : public DirectoryNode {
public:
	explicit wad(std::string file, bool truncate = false, DirectoryNode* withindirectory = nullptr, BaseNode* withparent = nullptr);

	iwindowstream* CreateStreamFor(const wadlump &lump) const;	//caller is the owner of new iwindowstream object
	void exportAll(std::string todir);
	void importAll(std::string listfile);
	LUMP_TYPE type() const override;
	bool checkName(std::string newname) const override;
    std::string validateName(std::string newname) const throw() override;

	std::istream& ReadableData() const override {error("Wads are not convertible yet"); throw false; /* workaround to not write 'return' */};	//todo:
	std::ostream& WritableData(int) override {error("Wads are not convertible yet"); throw false;};	//todo:
    size_t size() const override {error("Wads are not convertible yet"); throw false;}	//todo:
	void Stored() override {error("Wads are not convertible yet"); throw false;}	//todo:

private:
	std::fstream waddata;
	std::string wadtype;
	typedef boost::ptr_list<wadlump> LumpList;
	LumpList lumps;

	void checkmap(LumpList::iterator);
	void checkudmf(LumpList::iterator);
	void loadPalette(const wadlump&, const BaseNode* becauseof);
	wadlump& AppendLump(BaseNode* parent, std::string name, int offset = -1, int size = 0);

	void updatePaletteLump(std::string name, const Palette& pal, const BaseNode& becauseof) override;

	wadlump& getLump(std::string parent, std::string name);	// todo: protected until proper lump dependency auto update mechanics will be implemented
};

		}
	}
}
