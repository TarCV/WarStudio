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

#include "../../stdafx.h"

#include "Image.h"

#include "../../utility.h"
#include "../../filesystem.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <assert.h>
#include <algorithm>
#include <climits>

using namespace std;

namespace warstudio {
	namespace model {

Image::FormatMap Image::format_info_;
const size_t Image::MARKER_WIDTH = 5;

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
	is_bgcolor_appended_(false)
{
    static_assert(Image::MARKER_WIDTH % 2 == 1, "MARKER_WIDTH must be odd");

    image_.backgroundColor(bgcolor.getNativeColor_());
	if (nullptr == palette)
	{
		image_.classType(MagickLib::ClassType::DirectClass);
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
	is_bgcolor_appended_(false)
{
    static_assert(Image::MARKER_WIDTH % 2 == 1, "MARKER_WIDTH must be odd");
    open(file);
}

Image::Image(const Image& image) :
	image_(image.image_),
	anchor_x_(image.anchor_x_),
	anchor_y_(image.anchor_y_),
	is_bgcolor_appended_(image.is_bgcolor_appended_)
{
    static_assert(Image::MARKER_WIDTH % 2 == 1, "MARKER_WIDTH must be odd");
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

        palette.pop_back();

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

bool Image::removeBgColor(Image& copy)
{
	assert(copy.isPaletted());

	const Color bgcolor = copy.getBackgroundColor();
	const Palette palette = copy.getPalette(PALETTE_TYPE::WITHOUT_EXTRA_BGCOLOR);
	const size_t bgcolor_index(palette.size());
	size_t found_index(bgcolor_index);
	unsigned int found_difference(UINT_MAX);

	auto palette_it(palette.cbegin());
	size_t index(0);
	for (; palette_it != palette.cend(); ++palette_it, ++index)
	{
		if (bgcolor.getA() < 0xff && palette_it->getA() == 0xff)	continue;
		if (bgcolor.getA() == 0xff && palette_it->getA() < 0xff)	continue;

		unsigned int cur_difference = difference(bgcolor, *palette_it);
		if (cur_difference < found_difference)
		{
			found_index = index;
			found_difference = cur_difference;
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
			return false;
		}
	}
	
	copy.image_.colorMapSize(copy.image_.colorMapSize() - 1);
	//copy.doSetPalette(palette, PALETTE_TYPE::WITH_EXTRA_BGCOLOR);
	return true;
}

Rect Image::getRectForAnchor() const
{
	assert(anchor_x_ != 0 || anchor_y_ != 0);

	Rect ret(0, 0, getWidth(), getHeight());

	bool is_x_inside_picture = (0 <= anchor_x_ && anchor_x_ < ret.width);
	bool is_y_inside_picture = (0 <= anchor_y_ && anchor_y_ < ret.height);

/*

	*(2)-+ - -+-(2)*		  * - anchor
	i    i         i--output image dimensions		
	      *(3)i				(1)	- if clause
	i - -+----+- - i	+---+--input image dimensions
	 (3)*|(1)*|(3)  
	i    |    |*   i
	i - -+----+- - i
	     i *       i
	i     (3) i     
	*(2)-+- - +-(2)*


*/

	if (is_x_inside_picture || is_y_inside_picture)	//(1), (3)
	{
		auto helper = [](signed int anchor, bool other_inside, size_t &out_offset, size_t &out_size) {
			bool inside_picture = (0 <= anchor && anchor < out_size);
			if (anchor < 0)
			{
                out_offset = max<size_t>(Image::MARKER_WIDTH, -anchor);	//size will be automatically changed later

			} else if (other_inside)
			{
                out_size = max<size_t>(out_size + Image::MARKER_WIDTH, anchor + 1);
			}
		};
		helper(anchor_x_, is_y_inside_picture, ret.offset_X, ret.width);
		helper(anchor_y_, is_x_inside_picture, ret.offset_Y, ret.height);
	}
    else	//(2): is exactly in the corner and is always outside of the image, so we never need to add Image::MARKER_WIDTH!
	{
		auto helper = [](signed int anchor, size_t &out_offset, size_t &out_size) {
			if (anchor < 0)
			{
				out_offset = -anchor;	//size will be automatically changed later
			}
			else
			{
				out_size = anchor + 1;
			}
		};
		helper(anchor_x_, ret.offset_X, ret.width);
		helper(anchor_y_, ret.offset_Y, ret.height);
	}

	if (ret.offset_X > 0)	ret.width += ret.offset_X;
	if (ret.offset_Y > 0)	ret.height += ret.offset_Y;

    ret.width = max(ret.width, Image::MARKER_WIDTH);
    ret.height = max(ret.height, Image::MARKER_WIDTH);

	return ret;
}

void Image::fillRectangle(const Rect& rectangle, size_t colorindex)
{
	assert(isPaletted());	//todo: implement for full color

	auto window = getIndexWindow(rectangle);
	auto& indexes = window.pixels();
	fill(indexes.begin(), indexes.end(), colorindex);
}

void Image::composite(const Image& source, size_t at_x, size_t at_y)
{
	assert(isPaletted());	//todo: implement for full color

	Rect destination(at_x, at_y, source.getWidth(), source.getHeight());
	const auto source_window = source.getConstIndexWindow(Rect(0, 0, destination.width, destination.height));
	const auto& source_pixels = source_window.pixels();
	auto dest_window = getIndexWindow(destination);
	auto& dest_pixels = dest_window.pixels();
	assert(copy(source_pixels.cbegin(), source_pixels.cend(), dest_pixels.begin()) == dest_pixels.end());
}

void Image::save(std::string file) const
{
	//todo: support anchor offsets
	
	initFormatData();
    const path filepath(file);
    const string extension = boost::to_lower_copy(filepath.extension());
	if (format_info_.count(extension) == 0)
	{
		error("Writing in the specified image format is not supported");
	}
	FormatInfo	&format = format_info_.at(extension);

	unique_ptr<Image> copy;
	bool has_anchor = anchor_x_ != 0 || anchor_y_ != 0;
	Rect copy_dimensions = Rect();	//offset_X/Y are the offsets of the input image in the output image
	if (has_anchor)
	{
		copy_dimensions = getRectForAnchor();
		const auto palette = getPalette(PALETTE_TYPE::WITHOUT_EXTRA_BGCOLOR);
		copy.reset(new Image(copy_dimensions.width, copy_dimensions.height, getBackgroundColor(), &palette));
		copy->composite(*this, copy_dimensions.offset_X, copy_dimensions.offset_Y);

		if (anchor_x_ <= 0 && anchor_y_ <= 0)
		{
			//marker would be at (0, 0) of the output image, so don't draw it
			has_anchor = false;
		}
	}
	else
	{
		copy.reset(new Image(*this));
	}

	bool remove_last_color(false);
	if (isPaletted())
	{
		bool removed = removeBgColor(*copy);

		if (copy->image_.colorMapSize() > format.palette_max)
		{
			assert(copy->image_.colorMapSize() == format.palette_max + 1);		
			assert(copy->getBackgroundColor().getA() == 0);
			assert(!removed);
			remove_last_color = true;
		}
	}

	if (remove_last_color || format.transparency_support != TRANSPARENCY_TYPE::HAS_ALPHA)
	{
		const TRANSPARENCY_TYPE transparency_needed(checkTransparency(*copy));

		if (remove_last_color || static_cast<int>(transparency_needed) > static_cast<int>(format.transparency_support))
		{
			Image mask(copy->getWidth(), copy->getHeight(), Color(0, 0, 0, 0xff));

			emulateTransparency(*copy, mask);

            path mask_path(filepath.branch_path());
            mask_path /= filepath.stem() + "__mask" + extension;
			mask.image_.write(mask_path.file_string());
		}
	}

	if (has_anchor)
	{
		enum class MarkerSection{BEFORE, IN, AFTER};
		auto getMarkerSection = [] (signed int anchor_position, size_t image_size) -> MarkerSection {
			if (anchor_position < 0)
			{
				return MarkerSection::BEFORE;

			} else if (anchor_position < image_size)
			{
				return MarkerSection::IN;

			} else
			{
				return MarkerSection::AFTER;
			}
		};
		MarkerSection x_section = getMarkerSection(anchor_x_, getWidth());
		MarkerSection y_section = getMarkerSection(anchor_y_, getHeight());

		auto getMarkerOffset = [] (MarkerSection section, size_t size, MarkerSection other_section, size_t other_offset) -> size_t {
			if (MarkerSection::BEFORE == section)
			{
				return 0;
			} else if (MarkerSection::AFTER == section || MarkerSection::IN == other_section)
			{
                return (size - Image::MARKER_WIDTH);
			} else
			{
                int ret = other_offset - Image::MARKER_WIDTH/2;
				if (ret < 0)	ret = 0;
				return ret;
			}
		};
        Rect h_marker(0, copy_dimensions.offset_Y + anchor_y_, Image::MARKER_WIDTH, 1);
        Rect v_marker(copy_dimensions.offset_X + anchor_x_, 0, 1, Image::MARKER_WIDTH);
		h_marker.offset_X = getMarkerOffset(x_section, copy_dimensions.width,  y_section, v_marker.offset_X);
		v_marker.offset_Y = getMarkerOffset(y_section, copy_dimensions.height, x_section, h_marker.offset_Y);

		const Color bg_color = copy->getBackgroundColor();
		const auto palette = copy->getPalette(PALETTE_TYPE::WITHOUT_EXTRA_BGCOLOR);
		auto ignore_error = [] (unsigned int val) { return ((val != UINT_MAX) ? val : 0); };
		auto difference_cmp = [bg_color, &ignore_error] (const Color& a, const Color& b) { return (ignore_error(difference(a, bg_color)) < ignore_error(difference(b, bg_color))); };
		size_t marker_color = std::distance(palette.begin(), std::max_element(palette.begin(), palette.end(), difference_cmp));

		copy->fillRectangle(h_marker, marker_color);
		copy->fillRectangle(v_marker, marker_color);
	}

	copy->image_.write(file);
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

/*void Image::trim()
{
	image_.trim();
}

Color Image::getColor(const ImagePixel& pixel) const
{
	assert(!isPaletted());
	return Color(image_.pixelColor(pixel.x_, pixel.y_));
}

void Image::setColor(ImagePixel& pixel, const Color& c)
{
	assert(!isPaletted());
	image_.pixelColor(pixel.x_, pixel.y_, c.getNativeColor_());
}*/

bool Image::doContainsPoint(size_t x, size_t y) const
{
	bool ret = 0 <= x && x < getWidth();
	ret = ret && 0 <= y && y < getHeight();
	return ret;
}
bool Image::doContainsRect(const Rect& rect) const
{
	bool ret = doContainsPoint(rect.offset_X, rect.offset_Y);
	ret = ret && doContainsPoint(rect.offset_X + rect.width - 1, rect.offset_Y + rect.height - 1);
	return ret;
}

Image::IndexWindow::Pixels Image::createIndexBufferFromRect(const Rect& window) const
{
	assert(isPaletted());
	assert(doContainsRect(window));
		
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
	assert(doContainsRect(window));

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

template <class T> void ImageWindowTemplate<T>::flush()
{
    image_.setWindow(*this);
}

/*template <class T> ImageWindowTemplate<T>::ImageWindowTemplate(const ImageWindowTemplate<T> &&r)
    : ConstImageWindowTemplate<T>(r.window_, r.buffer_), image_(r.image_)
{}*/

template <class T> ImageWindowTemplate<T>::ImageWindowTemplate(const Rect& window, typename ConstImageWindowTemplate<T>::Pixels&& buffer, Image& image)
    : ConstImageWindowTemplate<T>(window, std::move(buffer)), image_(image)
{}


	}
}
