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

#include "WriterRegistry.h"

#include "Raw.h"
#include "ImageWriter.h"

namespace warstudio {
	namespace model {

bool operator< (const WriterId &left, const WriterId &right)
{
	if (left.space != right.space)	return (left.space < right.space);
	return (left.id < right.id);
}

void WriterRegistry::doInit(Registry& registry) const
{
	add(registry, WriterId(SPACE_WARSTUDIO, BUILTIN_WRITER::RAW), new RawWriter());
	add(registry, WriterId(SPACE_WARSTUDIO, BUILTIN_WRITER::IMAGE), new ImageWriter());
}

	}
}