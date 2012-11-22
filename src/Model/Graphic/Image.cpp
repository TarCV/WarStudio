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

#include <assert.h>

using namespace std;

namespace warstudio {
	namespace model {

Image::Image(size_t width, size_t height, Color bgcolor, const Palette* palette) :
	image_(Magick::Geometry(width, height), bgcolor.getNativeColor_())
{
    image_.backgroundColor(bgcolor.getNativeColor_());
	if (nullptr == palette)
	{
		image_.classType(MagickLib::ClassType::DirectClass);
		image_.depth(8);
	}
	else
	{
		doSetPalette(*palette);
	}
}

Image::Image(string file) :
	image_()
{
    image_.backgroundColor(Color(0, 0, 0, 0).getNativeColor_());
	image_.depth(8);
	open(file);
}

void Image::open(string file)
{
	image_.read(file);
}

void Image::save(std::string file) const
{
	Magick::Image	copy(image_);
	//todo: support file format abstraction (e.g. emulate transparency using layers etc.)
	copy.write(file);
}

void Image::setBackgroundColor(Color newcolor)
{
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

void Image::setPalette(const Palette& newpalette)
{
	//todo: use remap for converting to this palette
	image_.quantizeColors(newpalette.size());
	image_.quantize();
	doSetPalette(newpalette);
}
void Image::doSetPalette(const Palette& newpalette)
{
	image_.classType(MagickLib::ClassType::PseudoClass);
	
	if (getBackgroundColor().getA() != 0xff)
	{
		image_.type(MagickLib::ImageType::PaletteMatteType);
	}
	else
	{
		image_.type(MagickLib::ImageType::PaletteType);
	}

	image_.colorMapSize(newpalette.size());
	size_t i(0);
	auto it = newpalette.cbegin();
	for (; it != newpalette.cend(); ++it, ++i)
	{
		image_.colorMap(i, it->getNativeColor_());
	}
	assert(it - newpalette.cbegin() == i);
}

Palette Image::getPalette() const
{
	Magick::Image	copy(image_);
	Palette ret(copy.colorMapSize());
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

Image::IndexWindow::Pixels* Image::createIndexBufferFromRect(const Rect& window) const
{
	assert(isPaletted());

	unique_ptr<IndexWindow::Pixels> buffer(new IndexWindow::Pixels(window.width * window.height));
	image_.getConstPixels(window.offset_X, window.offset_Y, window.width, window.height);
	const MagickLib::IndexPacket *indexes = image_.getConstIndexes();
	auto buffer_it = buffer->begin();
	auto index_it = &indexes[0];
	for(; buffer_it != buffer->end(); ++buffer_it, ++index_it)
	{
		*buffer_it = static_cast<IndexWindow::Pixels::value_type>(*index_it);
	}
	assert(index_it == indexes + window.width * window.height);

	return buffer.release();
}
Image::Window::Pixels* Image::createBufferFromRect(const Rect& window) const
{
	assert(!isPaletted());

	unique_ptr<Window::Pixels> buffer(new Window::Pixels(window.width * window.height));

	const MagickLib::PixelPacket *colors = image_.getConstPixels(window.offset_X, window.offset_Y, window.width, window.height);

	auto buffer_it = buffer->begin();
	auto colors_it = &colors[0];
	for(; buffer_it != buffer->end(); ++buffer_it, ++colors_it)
	{
		*buffer_it = Color(*colors_it);
	}
	assert(colors_it == colors + window.width * window.height);

	return buffer.release();
}

Image::IndexWindow Image::getIndexWindow(const Rect& window)
{
	return IndexWindow(window, createIndexBufferFromRect(window), this);
}

const Image::IndexWindow Image::getIndexWindow(const Rect& window) const
{
	return IndexWindow(window, createIndexBufferFromRect(window), nullptr);
}

Image::Window Image::getWindow(const Rect& window)
{
	return Window(window, createBufferFromRect(window), this);
}

const Image::Window Image::getWindow(const Rect& window) const
{
	return Window(window, createBufferFromRect(window), nullptr);
}

void Image::setWindow(const IndexWindow& buffer)
{
	image_.modifyImage();

	const Rect& window = buffer.window_;
	image_.getPixels(window.offset_X, window.offset_Y, window.width, window.height);
	MagickLib::IndexPacket *indexes = image_.getIndexes();

	auto buffer_it = buffer.buffer_->begin();
	auto index_it = &indexes[0];
	for(; buffer_it != buffer.buffer_->end(); ++buffer_it, ++index_it)
	{
		*index_it = static_cast<MagickLib::IndexPacket>(*buffer_it);
	}
	assert(index_it == indexes + window.width * window.height);
	image_.syncPixels();

	createIndexBufferFromRect(window);
}

void Image::setWindow(const Window& buffer)
{
	image_.modifyImage();

	const Rect& window = buffer.window_;
	MagickLib::PixelPacket *colors = image_.getPixels(window.offset_X, window.offset_Y, window.width, window.height);

	auto buffer_it = buffer.buffer_->begin();
	auto colors_it = &colors[0];
	for(; buffer_it != buffer.buffer_->end(); ++buffer_it, ++colors_it)
	{
		*colors_it = buffer_it->getNativeColor_();
	}
	assert(colors_it == colors + window.width * window.height);

	image_.syncPixels();
}

	}
}