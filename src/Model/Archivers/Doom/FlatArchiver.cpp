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

#include "../../../stdafx.h"

#include "FlatArchiver.h"

#include "../../Containers/ImageContainer.h"
#include "../../../utility.h"

#include <assert.h>

using namespace std;

namespace warstudio {
	namespace model {
		namespace doom {

const BaseContainer *FlatArchiver::doExtract(const InputLumpData& in) const
{
	size_t width(0), height(0);
	unsigned char kresolution(1);
	if (in.size % 64 != 0)	error("Flat width is not multiple of 64");
	if (64*64 == in.size)
	{
		//classic doom flat
		width = height = 64;
	}
	else if (128*128 == in.size || 256*256 == in.size)
	{
		//hi-res flat
		warning("High resolution flat");

		size_t squares = in.size / (64*64);
		kresolution = 0;
		while(squares > 0)
		{
			squares >>= 1;
			++kresolution;
		}
		assert(kresolution > 0);
		kresolution -= 1;

		kresolution = in.size / (64*64);
		width = height = kresolution*64;
	}
	else
	{
		warning("Non-square flat. Probably it is heretic/hexen hack for scrolling flats");
		width = 64;
		height = in.size / 64;
		if (65 != height && 128 != height)	warning("Flat height is nor 65 nor 128");
	}
	//todo: apply kresolution to the output picture

	auto ret = unique_ptr<ImageContainer>(new ImageContainer(width, height, Color(0, 0, 0, 0xff), &in.palettes.at("PLAYPAL")));
	Image& image = ret->image;
	Image::IndexWindow window(image.getIndexWindow(Rect(0, 0, image.getWidth(), image.getHeight())));	//window is to be destructed exactly in the end of scope, so I use an explicit var
    
	assert(window.pixels().size() == in.size);
	for (auto &index : window.pixels())
	{
		index = readFromStream<unsigned char>(in.stream);
	}
	return ret.release();
}
void FlatArchiver::doArchive(const BaseContainer& in, const Context& context, OutputLumpData& out) const
{
	const Image	&image = dynamic_cast<const ImageContainer &>(in).image;
	
	if (image.getWidth() != 64)
	{
		if (image.getWidth() != image.getHeight())	
		{
			error("Flat must be square or have width of 64");
		}
		else
		{
			if (image.getWidth() != 128 && image.getWidth() != 256)	error("Hi-res flat must have dimensions of 128x128 or 256x256");
		}
		if (image.getWidth() % 64 != 0)	error("Flat width should be multiple of 64");
	}
	else
	{
		if (image.getHeight() != 65 && image.getHeight() != 128)	warning("Flat height is nor 65 nor 128");
	}

	Image::ConstIndexWindow window(image.getConstIndexWindow(Rect(0, 0, image.getWidth(), image.getHeight())));
    
	assert(false);
	//implement pallete change check

	assert(window.pixels().size() == doEstimateSize(in));
	for (auto &index : window.pixels())
	{
		writeToStream<unsigned char>(out.stream, index);
	}
}
int FlatArchiver::doEstimateSize(const BaseContainer& in) const 
{
	const Image	&image = dynamic_cast<const ImageContainer &>(in).image;
	
	if (image.getWidth() != 64)
	{
		if (image.getWidth() != image.getHeight()
			|| (image.getWidth() != 128 && image.getWidth() != 256))
		{
			return 0;
		}
		if (image.getWidth() % 64 != 0)	return 0;
	}

	return image.getWidth() * image.getHeight();
}


		}
	}
}
