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

#include "id.h"
#include "../constants.h"

namespace warstudio {
	namespace model {

struct ContainerType
{
	uint32_t	space;	//namespace. 0x53524157 ('WARS') for warstudio built-in types
	uint32_t	type;

	ContainerType(uint32_t idspace, CONTAINER_TYPE idtype) : space(idspace), type(uint32_t(idtype)) {assert(SPACE_WARSTUDIO == idspace);}
	ContainerType(uint32_t idspace, uint32_t idtype) : space(idspace), type(idtype) {assert(SPACE_WARSTUDIO != idspace);}
};
bool operator==(const ContainerType& left, const ContainerType& right);
bool operator!=(const ContainerType& left, const ContainerType& right);
bool operator< (const ContainerType& left, const ContainerType& right);

class BaseContainer {
public:
	ContainerType Type() const {return type_();}

	virtual ~BaseContainer() {};

protected:
	BaseContainer() {}
	virtual ContainerType type_() const = 0;

private:	//delete: derived classes may be heavy, so do not require them to support copying
	BaseContainer(BaseContainer &) {}
	void operator= (BaseContainer &) {}
};

	}
}