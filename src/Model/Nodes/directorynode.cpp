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

#include "../../stdafx.h"

#include "DirectoryNode.h"

namespace warstudio {
	namespace model {

DirectoryNode::DirectoryNode(std::string named, DirectoryNode* withindirectory, BaseNode* withparent) :
	BaseNode(named, withindirectory, withparent),
	palettes_()	
{
}
void DirectoryNode::loadPalette(std::string name, const Palette &pal, const BaseNode* becauseof)
{
	if (becauseof)
	{ //some arbitrary lump changed the palette, so we need to update the palette lump
		updatePaletteLump(name, pal, *becauseof);
	}
	palettes_[name] = pal;
}
const Palette& DirectoryNode::getPalette(std::string name) const
{
	//todo: also load from IWADs etc.
    return palettes_.at(name);
}

	}
}
