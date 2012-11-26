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


#include "Palette.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <Magick++.h>

namespace warstudio {
	namespace model {

class Image;

struct Rect
{
	size_t offset_X, offset_Y;
	size_t width, height;
	Rect(size_t x, size_t y, size_t width_, size_t height_) : offset_X(x), offset_Y(y), width(width_), height(height_) {}
};

template<class T>
	class ImageWindowTemplate
{
public:
	typedef std::vector<T> Pixels;

	Pixels& pixels() {return *buffer_;}
	const Pixels& pixels() const {return *buffer_;}

	void flush() {image_->setWindow(*this);}

	~ImageWindowTemplate()	{flush();}

private:
	friend class Image;
	ImageWindowTemplate(const Rect& window, Pixels* buffer, Image* image = nullptr) : window_(window), buffer_(buffer), image_(image) {}
private:
	Image*	image_;
	Rect	window_;
	std::unique_ptr<Pixels>	buffer_;
//	bool	fast_;
};

//class ImagePixel;
class Image
{
public:
	typedef ImageWindowTemplate<size_t>	IndexWindow;
	typedef ImageWindowTemplate<Color>	Window;

	Image(size_t width, size_t height, Color bgcolor = Color(0, 0, 0, 0), const Palette* palette = nullptr);
	Image(std::string file);
	Image(const Image& image);
	void open(std::string file);
	void save(std::string file) const;
/*	ImagePixel getPixel(size_t x, size_t y);
    const ImagePixel getPixel(size_t x, size_t y) const;*/
	
	IndexWindow getIndexWindow(const Rect& window);
	const IndexWindow getIndexWindow(const Rect& window) const;
	Window getWindow(const Rect& window);
	const Window getWindow(const Rect& window) const;

	void setBackgroundColor(Color newcolor);
	Color getBackgroundColor() const;
	void setWidth(size_t newwidth);
	size_t getWidth() const;
	void setHeight(size_t newheight);
	size_t getHeight() const;

	void setAnchor(size_t x, size_t y);

//	void setPalette(const Palette&);
	enum class PALETTE_TYPE {WITHOUT_EXTRA_BGCOLOR, WITH_EXTRA_BGCOLOR};
	Palette getPalette(PALETTE_TYPE include_background = PALETTE_TYPE::WITHOUT_EXTRA_BGCOLOR) const;
	bool isPaletted() const;

    void resize(size_t newwidth, size_t newheight);
	void trim();

private:
/*	friend class ImagePixel;
	Color getColor(const ImagePixel&) const;
	void setColor(ImagePixel&, const Color& );*/
	friend class IndexWindow;
	friend class Window;
	void setWindow(const IndexWindow& buffer);
	void setWindow(const Window& buffer);
	IndexWindow::Pixels* createIndexBufferFromRect(const Rect& window) const;
	Window::Pixels* createBufferFromRect(const Rect& window) const;


private:
	void doSetPalette(const Palette& newpalette);

	static void initFormatData();

	enum class TRANSPARENCY_TYPE {NONE, HAS_TRANSPARENCY, HAS_ALPHA};
	static TRANSPARENCY_TYPE checkTransparency(Image& copy);

	static void emulateTransparency(Image& copy, Image& mask, bool remove_last_color);

	struct FormatInfo
	{
		size_t	palette_max;
		TRANSPARENCY_TYPE transparency_support;

		FormatInfo(size_t palette_max_, TRANSPARENCY_TYPE transparency_support_) : palette_max(palette_max_), transparency_support(transparency_support_) {}
	};
	typedef std::map<std::string, FormatInfo> FormatMap;
	static FormatMap format_info_;

	Magick::Image image_;

	bool    is_bgcolor_appended_;	//true if bgcolor was automatically appended to the palette

	size_t	anchor_x_, anchor_y_;
	bool    is_anchor_set_;

};

/*class ImagePixel
{
public:
	Color getColor() const {return imageread_.getColor(*this);}
	void setColor(const Color& newcolor) {imagewrite_->setColor(*this, newcolor);}

	void operator= (const Color& newcolor) {setColor(newcolor);}
	operator Color() const {return getColor();}

	unsigned char getR() const {return getColor().getR();}
	unsigned char getG() const {return getColor().getG();}
	unsigned char getB() const {return getColor().getB();}
	void setR(unsigned char r) 
	{
		Color c = getColor();
		c.setR(r);
		setColor(c);
	}
	void setG(unsigned char g) 
	{
		Color c = getColor();
		c.setG(g);
		setColor(c);
	}
	void setB(unsigned char b) 
	{
		Color c = getColor();
		c.setB(b);
		setColor(c);
	}

private:	//delete
	ImagePixel();


private:
	friend class Image;
	friend class Color;
    ImagePixel::ImagePixel(const Image& owner, size_t x, size_t y) : imageread_(owner), imagewrite_(nullptr), x_(x), y_(y) {}
	ImagePixel::ImagePixel(Image& owner, size_t x, size_t y) : imageread_(owner), imagewrite_(&owner), x_(x), y_(y) {}
private:
	const Image&	imageread_;
    Image*	imagewrite_;    //const_cast is dangerous, so I use this workaround
	const ssize_t	x_;
	const ssize_t	y_;
};*/

	}
}