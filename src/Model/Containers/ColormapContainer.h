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

#include "BaseContainer.h"

#include "../Graphic/Palette.h"

#include <vector>

namespace warstudio {
	namespace model {

typedef std::vector<size_t>     Colormap;
typedef std::vector<Colormap>   ColormapSet;


class ColormapContainer : public BaseContainer {
public:
	ColormapContainer() : mapset(), palette() {}
	ColormapContainer(size_t colormaps_number, size_t color_number) : mapset(colormaps_number), palette(color_number)
	{
		for (auto &map : mapset)
		{
			map.resize(color_number);
		}
	}

	ColormapSet mapset;
	Palette     palette;
protected:
	ContainerType type_() const override {return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::COLORMAP);};
};

	}
}