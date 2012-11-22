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

#include "ColormapArchiver.h"

#include "../../Containers/ColormapContainer.h"
#include "../../../utility.h"

#include <assert.h>

using namespace std;

namespace warstudio {
	namespace model {
		namespace doom {

ColormapArchiver::ColormapArchiver(size_t colors_per_map, size_t colormaps_number, string palette_name) :
	colormaps_number_(colormaps_number), colors_per_map_(colors_per_map), palette_name_(palette_name)
{
}

const BaseContainer *ColormapArchiver::doExtract(const InputLumpData& in) const
{
    if (in.size % colors_per_map_) warning("Incomplete map present or wrong number of colors per map line");
    const size_t map_number = in.size / colors_per_map_;
    if (colormaps_number_ != map_number) warning("Wrong number of map lines");
    unique_ptr<ColormapContainer> ret(new ColormapContainer(map_number, colors_per_map_));
    ret->palette = in.palettes.at(palette_name_);

	assert(ret->mapset.size() == map_number);
    for (auto &map_it : ret->mapset)
	{
		assert(map_it.size() == colors_per_map_);
        for (auto &color_it : map_it)
		{
			color_it = readFromStream<unsigned char>(in.stream);
		}
	}
	return ret.release();
}
void ColormapArchiver::doArchive(const BaseContainer& in, const Context& context, OutputLumpData& out) const
{

	const ColormapContainer	&data = dynamic_cast<const ColormapContainer &>(in);
	if (data.mapset.size() != colormaps_number_)	warning("Wrong number of map lines");
	for (const auto &map_it : data.mapset)
	{
		if (map_it.size() != colors_per_map_) error("Wrong number of colors in a map line. Probably conversion settings are incorrect");	//todo: specify line
	
		assert(doEstimateSize(in) > 0);	//we shouldn't be here if input is invalid
		
		for (const auto &color_it : map_it)
		{
			assert(0 <= color_it && color_it <= 0xff);
			writeToStream(out.stream, color_it & 0xff);
		}
	}
	out.palettes[palette_name_] = data.palette;
}
int ColormapArchiver::doEstimateSize(const BaseContainer& in) const 
{
	const ColormapSet	&mapset = dynamic_cast<const ColormapContainer &>(in).mapset;
	
	if (mapset.front().size() != colors_per_map_)
	{
		//do not allocate anything for incorrect colormap
		return 0;
	}
	
	return mapset.size()*colors_per_map_;
}


		}
	}
}