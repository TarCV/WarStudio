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

#include "ColormapToImgConverter.h"
#include "../Containers/ColormapContainer.h"
#include "../Containers/ImageContainer.h"
#include "../../utility.h"

using namespace std;

namespace warstudio {
	namespace model {

const BaseContainer* ColormapToImgConverter::doExtract(const BaseContainer& in) const
{
	const ColormapContainer &container = dynamic_cast<const ColormapContainer &>(in);
	const ColormapSet& mapset = container.mapset;
	const size_t colors(mapset.front().size());
	auto ret = unique_ptr<ImageContainer>(new ImageContainer(colors, mapset.size(), Color(0, 0, 0, 0xff), &container.palette));
	Image& image = ret->image;
	Image::IndexWindow window(image.getIndexWindow(Rect(0, 0, image.getWidth(), image.getHeight())));	//window is to be destructed exactly in the end of scope, so I use an explicit var
    
	size_t y(0);
	auto colormap = mapset.cbegin();
	auto it = window.pixels().begin();
	//todo: rewrite using <algorithm>
	for (;colormap != mapset.cend(); ++colormap, ++y)
	{
		if (colormap->size() != colors)	error("Numbers of colors varies between lines of the colormap. Probably conversion settings for the type are incorrect.");
        
		for (auto colormap_it = colormap->cbegin(); colormap_it != colormap->cend(); ++colormap_it, ++it)
		{
			*it = *colormap_it;
		}
	}
    return ret.release();
}
const BaseContainer* ColormapToImgConverter::doArchive(const BaseContainer& in, const Context& context) const
{
	const Image& image = dynamic_cast<const ImageContainer &>(in).image;

    auto ret = unique_ptr<ColormapContainer>(new ColormapContainer(image.getHeight(), image.getWidth()));
	
	error("not implemented");
/*    PaletteSet &palset = ret->palset;
    const size_t colornum = image.width();

    size_t y(0);
	auto pal(palset.begin());
	for (; pal != palset.end(); ++pal, ++y)
	{
        pal->resize(colornum);
        size_t x(0);
		auto color(pal->begin());
        for (; color != pal->end(); ++color, ++x)
        {
            *color = image.pixel(x, y).color();
        }
	}*/
    return ret.release();
}

	}
}