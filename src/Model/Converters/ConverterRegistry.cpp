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

#include "ConverterRegistry.h"

#include "PalToImgConverter.h"
#include "ColormapToImgConverter.h"

namespace warstudio {
	namespace model {

bool operator< (const ConverterId &left, const ConverterId &right)
{
		if (left.space != right.space)	return (left.space < right.space);
		return (left.id < right.id);
}

void ConverterRegistry::doInit(Registry& registry) const
{
	add(registry, ConverterId(SPACE_WARSTUDIO, CONVERTER_TYPE::PALETTE_TO_IMAGE), new PalToImgConverter());
	add(registry, ConverterId(SPACE_WARSTUDIO, CONVERTER_TYPE::COLORMAP_TO_IMAGE), new ColormapToImgConverter());
}

	}
}