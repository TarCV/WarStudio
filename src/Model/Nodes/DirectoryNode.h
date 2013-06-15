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

#include "BaseNode.h"

#include "../Graphic/Palette.h"

#include <string>

namespace warstudio {
	namespace model {

/*
	represent a node that physically holds subnodes (e.g. wad-file)
	therefore its derived classes:
	- loads/saves subnodes
	- validates directory structure and subnode names
*/
class DirectoryNode : public BaseNode{
public:
	virtual bool checkName(std::string newname) const = 0;
	virtual std::string validateName(std::string newname) const throw() = 0;

	DirectoryNode(std::string named, DirectoryNode* withindirectory, BaseNode* withparent);

	const Palette& getPalette(std::string name) const;
protected:
	virtual void updatePaletteLump(std::string name, const Palette& pal, const BaseNode& becauseof) = 0;

	/*
		becauseof == (node)		the update is needed because palette part was changed in (node)
		becauseof == nullptr	the palette lump itself was changed, not some arbitrary lump
	*/
	void loadPalette(std::string name, const Palette& pal, const BaseNode* becauseof);

private:
    std::map<std::string, Palette> palettes_;
};

	}
}
