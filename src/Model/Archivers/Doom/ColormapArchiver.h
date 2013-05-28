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

#include "../BaseArchiver.h"

namespace warstudio {
	namespace model {
		namespace doom {

class ColormapArchiver : public BaseArchiver
{
public:
	ColormapArchiver(size_t colors_per_map, size_t colormaps_number, std::string palette_name);
protected:
	const BaseContainer *doExtract(const InputLumpData& in) const override;
	void doArchive(const BaseContainer& in, const Context& context, OutputLumpData& out) const override;
	int doEstimateSize(const BaseContainer& in) const override;
	PaletteDependencyList doGetPaletteDependencies() const override {return PaletteDependencyList(1, palette_name_); /* depends on assumption that PaletteDependencyList is a vector */}
	ContainerType doGetContainerType() const override {return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::COLORMAP);}

private:
	ColormapArchiver();

	const size_t colors_per_map_;
	const size_t colormaps_number_;
	std::string  palette_name_;
};

		}
	}
}
