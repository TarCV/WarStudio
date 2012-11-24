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

#include "PatchArchiver.h"

#include "../../Containers/ImageContainer.h"
#include "../../../utility.h"

#include <assert.h>
#include <stdint.h>

using namespace std;

namespace warstudio {
	namespace model {
		namespace doom {

const BaseContainer *PatchArchiver::doExtract(const InputLumpData& in) const
{
	const size_t width = readFromStream<uint16_t>(in.stream);
	size_t height = readFromStream<uint16_t>(in.stream);
	int offset_x = readFromStream<int16_t>(in.stream);
	int offset_y = readFromStream<int16_t>(in.stream);

	vector<uint32_t> column_pointers(width);

	int columns_begin_p = in.stream.tellg();

	{
		auto it(column_pointers.begin());
		size_t x(0);
		for (; x < width; ++x, ++it) {
			*it = readFromStream<uint32_t>(in.stream);
		}
		assert(it == column_pointers.end());
	}

	for (const auto column_pointer : column_pointers)
	{
		size_t last_post_start(0);
		bool tall_patch_mode(false);
		size_t post_start(0xff);

		if (static_cast<uint32_t>(in.stream.tellg()) != column_pointer)
		{
			in.stream.seekg(column_pointer);
		}

		while ((post_start = readFromStream<uint8_t>(in.stream)) != 0xff)
		{
			if (tall_patch_mode || post_start < last_post_start)
			{
				if (!tall_patch_mode)	warning("The patch is corrupted or is a tall patch. Assuming it is a tall patch.");
				post_start += last_post_start;
				tall_patch_mode = true;
			}
			last_post_start = post_start;

			size_t post_height = readFromStream<uint8_t>(in.stream);
			if (post_start + post_height > height)
			{
				warning("Actual height of the patch is greater than the one stated in the patch header.");
				height = post_start + post_height;
			}
			in.stream.seekg(post_height + 2, SEEK_CUR);
		}
	}

	auto ret = unique_ptr<ImageContainer>(new ImageContainer(width, height, Color(0, 0, 0, 0), &in.palettes.at("PLAYPAL")));
	Image& image = ret->image;

	image.setAnchor(offset_x, offset_y);

	Image::IndexWindow window(image.getIndexWindow(Rect(0, 0, image.getWidth(), image.getHeight())));	//window is to be destructed exactly in the end of scope, so I use an explicit var
    auto& pixels = window.pixels();
	assert(pixels.size() == width*height);

	{
		auto it(column_pointers.cbegin());
		size_t x(0);
		for (; x < width; ++x, ++it) {
			if (static_cast<uint32_t>(in.stream.tellg()) != *it)
			{
				in.stream.seekg(*it);
			}

			uint8_t last_post_start(0);
			bool tall_patch_mode(false);
			uint8_t post_start(0xff);
			while ((post_start = readFromStream<uint8_t>(in.stream)) != 0xff)
			{
				if (tall_patch_mode || post_start < last_post_start)
				{
					post_start += last_post_start;
					tall_patch_mode = true;
				}
				last_post_start = post_start;
				auto post_height = readFromStream<uint8_t>(in.stream);

				readFromStream<uint8_t>(in.stream);	//extract unused byte

				uint8_t i(0);
				size_t vector_index(x + post_start*width);
				for (; i < post_height; ++i, vector_index += width)
				{
					pixels[vector_index] = readFromStream<uint8_t>(in.stream);
				}

				readFromStream<uint8_t>(in.stream);	//extract unused byte
			}
		}
		assert(it == column_pointers.cend());
	}

	return ret.release();
}
void PatchArchiver::doArchive(const BaseContainer& in, const Context& context, OutputLumpData& out) const
{
	assert(false);	//not implemented

	const Image	&image = dynamic_cast<const ImageContainer &>(in).image;
	
	Image::IndexWindow window(image.getIndexWindow(Rect(0, 0, image.getWidth(), image.getHeight())));	//window is to be destructed exactly in the end of scope, so I use an explicit var
   
}
int PatchArchiver::doEstimateSize(const BaseContainer& in) const 
{
	assert(false);	//not implemented

	return 0;
}


		}
	}
}