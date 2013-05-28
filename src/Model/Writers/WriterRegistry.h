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
#include "BaseWriter.h"
#include "../constants.h"	//for SPACE_WARSTUDIO

namespace warstudio {
	namespace model {

enum class BUILTIN_WRITER : uint32_t {
	RAW,
	IMAGE,
};

struct WriterId {
	uint32_t	space;	//namespace. 0x53524157 ('WARS') for warstudio built-in writers
	uint32_t	id;

	WriterId(uint32_t space_, BUILTIN_WRITER id_) : space(space_), id(uint32_t(id_)) {assert(SPACE_WARSTUDIO == space_);}
	WriterId(uint32_t space_, uint32_t id_) : space(space_), id(id_) {assert(SPACE_WARSTUDIO != space_);}
};
bool operator< (const WriterId &left, const WriterId &right);	//defined in basewriter.cpp

class WriterRegistry : public RegistryTemplate<WriterId, BaseWriter>
{
	friend class Globals;
    friend class ChainRegistry;
    friend class AdapterChain;
protected:
    void doInit() const override;
};

	}
}
