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

#include "BaseNode.h"
#include "DirectoryNode.h"

#include <typeinfo>

namespace warstudio {
	namespace model {
	
bool BaseNode::parentfilter() const throw() {
	return typeid(getParent()) == typeid(this);
};
void BaseNode::setName(std::string newname) {
	if (!directory->checkName(newname))	return;	//todo: error message
	name_ = directory->validateName(newname);
}

	}
}
