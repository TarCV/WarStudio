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

#include "PalToImgConverter.h"
#include "../Containers/PaletteContainer.h"
#include "../Containers/ImageContainer.h"
#include "../../utility.h"

#include <assert.h>

using namespace std;

namespace warstudio {
	namespace model {

std::unique_ptr<const BaseContainer> PalToImgConverter::doExtract(const BaseContainer& in) const
{
	const PaletteSet& palset = dynamic_cast<const PaletteContainer &>(in).palset;
	const size_t colors(palset.front().size());
	
	auto ret = unique_ptr<ImageContainer>(new ImageContainer(colors, palset.size()));
	Image& image = ret->image;
	Image::Window window(image.getWindow(Rect(0, 0, image.getWidth(), image.getHeight())));	//window is to be destructed exactly in the end of scope, so I use an explicit var

    auto it = window.pixels().begin();
	//todo: rewrite using <algorithm>
	for (const auto &pal : palset)
	{
		if (pal.size() != colors)	error("Numbers of colors varies between lines of the palette. Probably conversion settings for the type are incorrect.");

        for (auto color = pal.cbegin(); color != pal.cend(); ++color, ++it)
        {
			assert(it != window.pixels().end());
			*it = *color;
        }
	}
	assert(it == window.pixels().end());
    return std::move(ret);
}
std::unique_ptr<const BaseContainer> PalToImgConverter::doArchive(const BaseContainer& in, const Context&) const
{
	const Image& image = dynamic_cast<const ImageContainer &>(in).image;
	Image::ConstWindow window(image.getConstWindow(Rect(0, 0, image.getWidth(), image.getHeight())));

    auto ret = unique_ptr<PaletteContainer>(new PaletteContainer(image.getHeight()));
    PaletteSet &palset = ret->palset;
    const size_t colornum = image.getWidth();

    auto it = window.pixels().begin();
	//todo: rewrite using <algorithm>
	for (auto &pal : palset)
	{
        pal.resize(colornum);
        for (auto color = pal.begin(); color != pal.end(); ++color, ++it)
        {
			assert(it != window.pixels().end());
			*color = *it;
        }
	}
	assert(it == window.pixels().end());
    return std::move(ret);
}

	}
}
