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

#include "../../magick.h"

namespace warstudio {
	namespace model {

class Image;

struct Rect
{
	size_t offset_X, offset_Y;
	size_t width, height;
	Rect() : offset_X(0), offset_Y(0), width(0), height(0) {}
	
	Rect(size_t x, size_t y, size_t width_, size_t height_) : offset_X(x), offset_Y(y), width(width_), height(height_) {}
};

template<class T>
	class ConstImageWindowTemplate
{
public:
	typedef typename std::vector<T> Pixels;

	const Pixels& pixels() const {return buffer_;}

	ConstImageWindowTemplate(const ConstImageWindowTemplate<T> &&r) : window_(std::move(r.window_)), buffer_(std::move(r.buffer_)) {}

	virtual ~ConstImageWindowTemplate() {}

protected:
	friend class Image;
	ConstImageWindowTemplate(const Rect& window, Pixels&& buffer) : window_(window), buffer_(std::move(buffer)) {}
	
protected:
	const Rect	window_;
	Pixels	buffer_;

private:
	//delete
    ConstImageWindowTemplate(const ConstImageWindowTemplate<T> &);
	const ConstImageWindowTemplate<T>& operator=(const ConstImageWindowTemplate<T> &) const;
};

template<class T>
	class ImageWindowTemplate : ConstImageWindowTemplate<T>
{
public:
    typename ConstImageWindowTemplate<T>::Pixels& pixels() {return this->buffer_;}

    void flush();

    ImageWindowTemplate(const ImageWindowTemplate<T> &&r);

	~ImageWindowTemplate() override {flush();}

private:
	friend class Image;
    ImageWindowTemplate(const Rect& window, typename ConstImageWindowTemplate<T>::Pixels&& buffer, Image& image);

private:
    Image&	image_;
};

//class ImagePixel;
class Image
{
public:
    typedef ConstImageWindowTemplate<size_t> ConstIndexWindow;

    typedef ConstImageWindowTemplate<Color>	ConstWindow;

    //typedefs can't be used in friend class in GNU C++
    typedef ImageWindowTemplate<size_t> IndexWindow;
    friend class ImageWindowTemplate<size_t>;

    typedef ImageWindowTemplate<Color> Window;
    friend class ImageWindowTemplate<Color>;

	Image(size_t width, size_t height, Color bgcolor = Color(0, 0, 0, 0), const Palette* palette = nullptr);
	Image(std::string file);
	Image(const Image& image);
	void open(std::string file);
	void save(std::string file) const;
/*	ImagePixel getPixel(size_t x, size_t y);
    const ImagePixel getPixel(size_t x, size_t y) const;*/
	
	IndexWindow getIndexWindow(const Rect& window);
	const ConstIndexWindow getConstIndexWindow(const Rect& window) const;
	Window getWindow(const Rect& window);
	const ConstWindow getConstWindow(const Rect& window) const;

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
//	void trim();

	void fillRectangle(const Rect&, size_t colorindex);

	void composite(const Image&, size_t at_x, size_t at_y);

private:
/*	friend class ImagePixel;
	Color getColor(const ImagePixel&) const;
	void setColor(ImagePixel&, const Color& );*/
    void setWindow(const IndexWindow& buffer);
	void setWindow(const Window& buffer);
	IndexWindow::Pixels createIndexBufferFromRect(const Rect& window) const;
	Window::Pixels createBufferFromRect(const Rect& window) const;


private:
	void doSetPalette(const Palette& newpalette, PALETTE_TYPE palette_has_bgcolor);

	bool doContainsPoint(size_t x, size_t y) const;
	bool doContainsRect(const Rect&) const;

	static void initFormatData();

	enum class TRANSPARENCY_TYPE {NONE, HAS_TRANSPARENCY, HAS_ALPHA};
	static TRANSPARENCY_TYPE checkTransparency(Image& copy);

	static bool removeBgColor(Image& copy);
	static void emulateTransparency(Image& copy, Image& mask);

	Rect getRectForAnchor() const;

	struct FormatInfo
	{
		size_t	palette_max;
		TRANSPARENCY_TYPE transparency_support;

		FormatInfo(size_t palette_max_, TRANSPARENCY_TYPE transparency_support_) : palette_max(palette_max_), transparency_support(transparency_support_) {}
	};
	typedef std::map<std::string, FormatInfo> FormatMap;

	Magick::Image image_;
	bool    is_bgcolor_appended_;	//true if bgcolor was automatically appended to the palette
    signed int	anchor_x_, anchor_y_;

    static FormatMap format_info_;
    static const size_t MARKER_WIDTH;
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
