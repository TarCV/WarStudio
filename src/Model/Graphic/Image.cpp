/*

	WadDirectory
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

#include "Image.h"

#include "../../utility.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <assert.h>
#include <algorithm>
#include <filesystem>

using namespace std;
using namespace std::tr2;

namespace warstudio {
	namespace model {

Image::FormatMap Image::format_info_;

void Image::initFormatData()
{
	if (format_info_.size() > 0)	return;

	format_info_.insert(make_pair(".bmp", FormatInfo(256, TRANSPARENCY_TYPE::NONE)));
	format_info_.insert(make_pair(".png", FormatInfo(256, TRANSPARENCY_TYPE::HAS_ALPHA)));
}

Image::Image(size_t width, size_t height, Color bgcolor, const Palette* palette) :
	image_(Magick::Geometry(width, height),bgcolor.getNativeColor_()),
	anchor_x_(0),
	anchor_y_(0),
	is_anchor_set_(false),
	is_bgcolor_appended_(false)
{
    image_.backgroundColor(bgcolor.getNativeColor_());
	if (nullptr == palette)
	{
		image_.classType(MagickLib::ClassType::DirectClass);
		image_.depth(8);
	}
	else
	{
		doSetPalette(*palette, PALETTE_TYPE::WITHOUT_EXTRA_BGCOLOR);

		auto window = getIndexWindow(Rect(0, 0, getWidth(), getHeight()));
		auto &window_pixels = window.pixels();

		fill(window_pixels.begin(), window_pixels.end(), palette->size());	//fill window with bgcolor (which index is palette->size())
	}
}

Image::Image(string file) :
	image_(),
	anchor_x_(0),
	anchor_y_(0),
	is_anchor_set_(false),
	is_bgcolor_appended_(false)
{
	open(file);
}

Image::Image(const Image& image) :
	image_(image.image_),
	anchor_x_(image.anchor_x_),
	anchor_y_(image.anchor_y_),
	is_anchor_set_(image.is_anchor_set_),
	is_bgcolor_appended_(image.is_bgcolor_appended_)
{
}

void Image::open(string file)
{
	image_.read(file);
	//todo: support file format abstraction
	//todo: support anchor offsets
}

Image::TRANSPARENCY_TYPE Image::checkTransparency(Image& copy)
{
	TRANSPARENCY_TYPE ret(TRANSPARENCY_TYPE::NONE);


	if (copy.isPaletted())
	{
		for (size_t i = 0; i < copy.image_.colorMapSize(); ++i)
		{
			if (Color(copy.image_.colorMap(i)).getA() < 0xff)
			{
				ret = TRANSPARENCY_TYPE::HAS_TRANSPARENCY;
				if (Color(copy.image_.colorMap(i)).getA() > 0)
				{
					return TRANSPARENCY_TYPE::HAS_ALPHA;
				}
			}
		}
	}
	else
	{
		const auto window = copy.getConstWindow(Rect(0, 0, copy.getWidth(), copy.getHeight()));
		for (const auto &pixel : window.pixels())
		{
			if (pixel.getA() < 0xff)
			{
				ret = TRANSPARENCY_TYPE::HAS_TRANSPARENCY;
				if (pixel.getA() > 0)
				{
					return TRANSPARENCY_TYPE::HAS_ALPHA;
				}
			}
		}
	}

	return ret;
}

void Image::emulateTransparency(Image& copy, Image& mask)
{
	assert(mask.getWidth() == copy.getWidth());
	assert(mask.getHeight() == copy.getHeight());
	assert(mask.getBackgroundColor() == Color(0, 0, 0, 0xff));
//	mask.image_.type(MagickLib::ImageType::GrayscaleType);

	auto mask_window = mask.getWindow(Rect(0, 0, copy.getWidth(), copy.getHeight()));
	auto &mask_pixels = mask_window.pixels();
	auto mask_it(mask_pixels.begin());

	if (copy.isPaletted())
	{
		assert(copy.is_bgcolor_appended_);

		auto palette = copy.getPalette(PALETTE_TYPE::WITH_EXTRA_BGCOLOR);
		auto window = copy.getIndexWindow(Rect(0, 0, copy.getWidth(), copy.getHeight()));
		auto &window_pixels = window.pixels();
		auto copy_it(window_pixels.begin());
		for (; mask_it != mask_pixels.end(); ++mask_it, ++copy_it)
		{
			*mask_it = Color(palette[*copy_it].getA(), palette[*copy_it].getA(), palette[*copy_it].getA(), 0xff);

			if (*copy_it == palette.size() - 1)
			{
				*copy_it = 0;	//todo: use color nearest to the old bgcolor
			}
		}
		assert(copy_it == window_pixels.end());

		palette.erase(--palette.cend());	//todo: consider if it really should be done always as there are removeBgColor method

		for_each(palette.begin(), palette.end(), [](Color &color) { color.setA(0xff); });

		copy.setBackgroundColor(palette.front());	//todo: use color nearest to the old bgcolor
		copy.doSetPalette(palette, PALETTE_TYPE::WITH_EXTRA_BGCOLOR);
	}
	else
	{
		auto window = copy.getWindow(Rect(0, 0, copy.getWidth(), copy.getHeight()));
		auto &window_pixels = window.pixels();
		auto copy_it(window_pixels.begin());
		for (; mask_it != mask_pixels.end(); ++mask_it, ++copy_it)
		{
			*mask_it = Color(copy_it->getA(), copy_it->getA(), copy_it->getA(), 0xff);
			copy_it->setA(0xff);
		}
		assert(copy_it == window_pixels.end());
	}
}

void Image::removeBgColor(Image& copy)
{
	assert(copy.isPaletted());

	const Color bgcolor = copy.getBackgroundColor();
	const Palette palette = copy.getPalette(PALETTE_TYPE::WITHOUT_EXTRA_BGCOLOR);
	const size_t bgcolor_index(palette.size());
	size_t found_index(bgcolor_index);
	size_t found_difference(0x100);

	auto palette_it(palette.cbegin());
	size_t index(0);
	for (; palette_it != palette.cend(); ++palette_it, ++index)
	{
		if (bgcolor.getA() < 0xff && palette_it->getA() == 0xff)	continue;
		if (bgcolor.getA() == 0xff && palette_it->getA() < 0xff)	continue;

		size_t difference = max(max(abs(bgcolor.getR() - palette_it->getR()), abs(bgcolor.getG() - palette_it->getG())), max(abs(bgcolor.getB() - palette_it->getB()), abs(bgcolor.getA() - palette_it->getA())));
		if (difference < found_difference)
		{
			found_index = index;
			found_difference = difference;
			if (0 == found_difference)	break;
		}
	}
	assert(index == palette.size() || 0 == found_difference);

	bool should_remove(false);
	if (found_difference < 0x100)
	{
		auto window = copy.getIndexWindow(Rect(0, 0, copy.getWidth(), copy.getHeight()));
		auto &window_pixels = window.pixels();

		replace(window_pixels.begin(), window_pixels.end(), bgcolor_index, found_index);
	}
	else
	{
		bool is_bgcolor_used(false);
		const auto window = copy.getConstIndexWindow(Rect(0, 0, copy.getWidth(), copy.getHeight()));
		const auto &window_pixels = window.pixels();

		if (any_of(window_pixels.cbegin(), window_pixels.cend(), [bgcolor_index] (size_t index) {return bgcolor_index == index; }))
		{
			/*
				can't substitute the bgcolor and found that it is actually used
				so we can't do anything
			*/
			return;
		}
	}
	
	copy.doSetPalette(palette, PALETTE_TYPE::WITH_EXTRA_BGCOLOR);
}

void Image::save(std::string file) const
{
	Image	copy(*this);
	//todo: support file format abstraction (e.g. emulate transparency using layers etc.)
	//todo: support anchor offsets
	
	initFormatData();
	const sys::path path(file);
	const string extension = boost::to_lower_copy(path.extension());
	if (format_info_.count(extension) == 0)
	{
		error("Writing in the image format is not supported");
	}
	FormatInfo	&format = format_info_.at(extension);

	bool remove_last_color(false);
	if (isPaletted())
	{
		removeBgColor(copy);

		if (copy.image_.colorMapSize() > format.palette_max)
		{
			assert(copy.image_.colorMapSize() == format.palette_max + 1);		
			assert(copy.getBackgroundColor().getA() == 0);
			remove_last_color = true;
		}
	}

	if (remove_last_color || format.transparency_support != TRANSPARENCY_TYPE::HAS_ALPHA)
	{
		const TRANSPARENCY_TYPE transparency_needed(checkTransparency(copy));

		if (remove_last_color || static_cast<int>(transparency_needed) > static_cast<int>(format.transparency_support))
		{
			Image mask(copy.getWidth(), copy.getHeight(), Color(0, 0, 0, 0xff));

			emulateTransparency(copy, mask);

			sys::path mask_path(path.branch_path());
			mask_path /= path.basename() + "__mask" + extension;
			mask.image_.write(mask_path.file_string());
		}
	}

	copy.image_.write(file);
}

void Image::setBackgroundColor(Color newcolor)
{
	if (isPaletted())
	{
		assert(is_bgcolor_appended_);
		image_.colorMap(image_.colorMapSize() - 1, newcolor.getNativeColor_());
	}
	
	image_.backgroundColor(newcolor.getNativeColor_());
}

Color Image::getBackgroundColor() const
{
	return Color(image_.backgroundColor());
}

/*ImagePixel Image::getPixel(size_t x, size_t y)
{
	return ImagePixel(*this, x, y);
}

const ImagePixel Image::getPixel(size_t x, size_t y) const
{
    return ImagePixel(*this, x, y);
}*/

void Image::setWidth(size_t newwidth)
{
    Magick::Geometry newsize = image_.size();
    newsize.width(newwidth);
    image_.size(newsize);
}

size_t Image::getWidth() const
{
    return image_.size().width();
}

void Image::setHeight(size_t newheight)
{
    Magick::Geometry newsize = image_.size();
    newsize.height(newheight);
    image_.size(newsize);
}

size_t Image::getHeight() const
{
    return image_.size().height();
}

void Image::setAnchor(size_t x, size_t y)
{
	anchor_x_ = x;
	anchor_y_ = y;
	is_anchor_set_ = true;
}

/*void Image::setPalette(const Palette& newpalette)
{
	//todo: removed until I implement converting to this palette
	image_.quantizeColors(newpalette.size());
	image_.quantize();
	doSetPalette(newpalette);
}*/
void Image::doSetPalette(const Palette& newpalette, PALETTE_TYPE palette_has_bgcolor)
{
	image_.classType(MagickLib::ClassType::PseudoClass);
	
	const Color bgcolor = getBackgroundColor();
	if (bgcolor.getA() != 0xff)	//todo: if any color in the palette has an alpha value
	{
		image_.type(MagickLib::ImageType::PaletteMatteType);
	}
	else
	{
		image_.type(MagickLib::ImageType::PaletteType);
	}

	if (PALETTE_TYPE::WITHOUT_EXTRA_BGCOLOR == palette_has_bgcolor)
	{
		image_.colorMapSize(newpalette.size() + 1);
		image_.colorMap(newpalette.size(), bgcolor.getNativeColor_());
		is_bgcolor_appended_ = true;
	}
	else
	{
		image_.colorMapSize(newpalette.size());
		is_bgcolor_appended_ = false;
	}

	size_t i(0);
	auto it = newpalette.cbegin();
	for (; it != newpalette.cend(); ++it, ++i)
	{
		image_.colorMap(i, it->getNativeColor_());
	}
	assert(it - newpalette.cbegin() == i);

}

Palette Image::getPalette(PALETTE_TYPE include_background) const
{
	Magick::Image	copy(image_);
	Palette ret(is_bgcolor_appended_ && include_background == PALETTE_TYPE::WITHOUT_EXTRA_BGCOLOR ? copy.colorMapSize() - 1 : copy.colorMapSize());
	size_t i(0);
	auto it = ret.begin();
	for (; it != ret.end(); ++it, ++i)
	{
		*it = copy.colorMap(i);
	}
	assert(it - ret.begin() == i);
	return ret;
}

bool Image::isPaletted() const
{
	return (MagickLib::ClassType::PseudoClass == image_.classType());
}

void Image::resize(size_t newwidth, size_t newheight)
{
    Magick::Geometry newsize = image_.size();
    newsize.width(newwidth);
    newsize.height(newheight);
    image_.size(newsize);
}

void Image::trim()
{
	image_.trim();
}


/*Color Image::getColor(const ImagePixel& pixel) const
{
	assert(!isPaletted());
	return Color(image_.pixelColor(pixel.x_, pixel.y_));
}

void Image::setColor(ImagePixel& pixel, const Color& c)
{
	assert(!isPaletted());
	image_.pixelColor(pixel.x_, pixel.y_, c.getNativeColor_());
}*/

Image::IndexWindow::Pixels Image::createIndexBufferFromRect(const Rect& window) const
{
	assert(isPaletted());

	IndexWindow::Pixels buffer(window.width * window.height);
	image_.getConstPixels(window.offset_X, window.offset_Y, window.width, window.height);
	const MagickLib::IndexPacket *indexes = image_.getConstIndexes();

	assert(transform(indexes, indexes + window.width * window.height, buffer.begin(),
		             [] (const MagickLib::IndexPacket native_index) { return static_cast<IndexWindow::Pixels::value_type>(native_index); })
		   == buffer.end());

	return buffer;
}
Image::Window::Pixels Image::createBufferFromRect(const Rect& window) const
{
	assert(!isPaletted());

	Window::Pixels buffer(window.width * window.height);
	const MagickLib::PixelPacket *colors = image_.getConstPixels(window.offset_X, window.offset_Y, window.width, window.height);

	assert(transform(colors, colors + window.width * window.height, buffer.begin(), 
		             [] (const MagickLib::PixelPacket& native_color) { return static_cast<Window::Pixels::value_type>(native_color); })
		   == buffer.end());

	return buffer;
}

Image::IndexWindow Image::getIndexWindow(const Rect& window)
{
	return IndexWindow(window, createIndexBufferFromRect(window), *this);
}

const Image::ConstIndexWindow Image::getConstIndexWindow(const Rect& window) const
{
	return ConstIndexWindow(window, createIndexBufferFromRect(window));
}

Image::Window Image::getWindow(const Rect& window)
{
	return Window(window, createBufferFromRect(window), *this);
}

const Image::ConstWindow Image::getConstWindow(const Rect& window) const
{
	return ConstWindow(window, createBufferFromRect(window));
}

void Image::setWindow(const IndexWindow& buffer)
{
	image_.modifyImage();

	const Rect& window = buffer.window_;
	image_.getPixels(window.offset_X, window.offset_Y, window.width, window.height);
	MagickLib::IndexPacket *indexes = image_.getIndexes();

	assert(transform(buffer.buffer_.begin(), buffer.buffer_.end(), indexes,
		   [] (size_t index) { return static_cast<MagickLib::IndexPacket>(index); })
		   == indexes + window.width * window.height);

	image_.syncPixels();
}

void Image::setWindow(const Window& buffer)
{
	image_.modifyImage();

	const Rect& window = buffer.window_;
	MagickLib::PixelPacket *colors = image_.getPixels(window.offset_X, window.offset_Y, window.width, window.height);

	assert(transform(buffer.buffer_.begin(), buffer.buffer_.end(), colors,
		   [] (const Color& color) { return color.getNativeColor_(); })
		   == colors + window.width * window.height);

	image_.syncPixels();
}

	}
}