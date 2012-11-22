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

#include <stdint.h>
#include <assert.h>

#include "../RegistryTemplate.h"

#include "BaseConverter.h"
#include "../constants.h"	//for SPACE_WARSTUDIO

namespace warstudio {
	namespace model {

enum class CONVERTER_TYPE : uint32_t {
	PALETTE_TO_IMAGE,
	COLORMAP_TO_IMAGE,
};
struct ConverterId {
	uint32_t	space;	//namespace. 0x53524157 ('WARS') for warstudio built-in archivers
	uint32_t	id;

	ConverterId(uint32_t space_, CONVERTER_TYPE id_) : space(space_), id(uint32_t(id_)) {assert(SPACE_WARSTUDIO == space_);}
	ConverterId(uint32_t space_, uint32_t id_) : space(space_), id(id_) {assert(SPACE_WARSTUDIO != space_);}
};
bool operator< (const ConverterId &left, const ConverterId &right);	//defined in BaseConverter.cpp

class ConverterRegistry : public RegistryTemplate<ConverterId, BaseConverter>
{
	friend class Globals;
	friend class DefaultConverters;
protected:
	void doInit(Registry& registry) const override;
};

	}
}