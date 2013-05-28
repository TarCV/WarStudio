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

#include "windowstream.h"

#include "utility.h"

#include <algorithm>
#include <assert.h>
#include <cstring>

namespace warstudio {

iwindowbuf_sentry::iwindowbuf_sentry(iwindowbuf& buf)
	: buf_(buf)
{
	buf_.begin_operations();
}

iwindowbuf_sentry::~iwindowbuf_sentry()
{
	buf_.finish_operations();
}

void iwindowbuf::begin_operations() {
	assert(!initedops_);

	source_oldpos_ = source_->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
	assert(source_oldpos_ >= 0);

	if (source_oldpos_ != windowcursor_) {
		std::streamoff result = source_->pubseekoff(windowcursor_, std::ios_base::beg, std::ios_base::in);
		if (result != windowcursor_)	error("iwindowbuf::initoperations failed");
	}
	initedops_ = true;
}
void iwindowbuf::finish_operations() {
	assert(initedops_);

	windowcursor_ = source_->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
	assert(windowbegin_ <= windowcursor_ && windowcursor_ <= windowend_);

	source_->pubseekoff(source_oldpos_, std::ios_base::beg, std::ios_base::in);
	initedops_ = false;
}

iwindowbuf::iwindowbuf() : 
	source_(0), 
    readpos_base_(0),
    initedops_(false)
{}



void iwindowbuf::init(std::streambuf& source, int offset, size_t size, size_t buff_size, size_t put_back)
{
	assert(!initedops_);	//initops wasn't called yet, should be false

	if (source_)	error("iwindowbuf is already inited");
	source_ = &source;
	windowbegin_ = offset;
	windowend_ = offset+size;
	windowcursor_ = offset;
	readpos_base_ = 0;
    put_back_ = std::max(put_back, size_t(1));
    buffer_.resize(std::max(buff_size, put_back_) + put_back_);
    char *end = &buffer_.front() + buffer_.size();
    setg(end, end, end);
}
size_t iwindowbuf::updatebuffer(bool onunderflow) 
{
	assert(initedops_);

    char *base = &buffer_.front();
    char *start = base;

    if (onunderflow && eback() == base) // true when this isn't the first fill
    {
        std::memmove(base, egptr() - put_back_, put_back_);
        start += put_back_;
    }
	size_t toread =  buffer_.size() - (start - base);
	size_t n;
	if (windowcursor_ + toread > windowend_)	toread = windowend_ - windowcursor_;
	readpos_base_ = windowcursor_ - windowbegin_ - (start - base);
	if (toread) {
		n = source_->sgetn(start, toread);
	} else {
		n = 0;
	} 
	setg(onunderflow ? base : start, start, start + n);
	return n;
}

std::streampos iwindowbuf::seekoff (std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)  {
	assert((which & ~std::ios_base::in) == 0);
	assert(!initedops_);	//initops wasn't called yet, should be false

	if (way == std::ios_base::beg)
		;
	else if (way == std::ios_base::end)
		off = windowend_ - off - windowbegin_;
	else if (way == std::ios_base::cur)
		off += readpos();
	else
		assert(false);

	return seekpos(off, which);
}

std::streampos iwindowbuf::seekpos(std::streampos sp, std::ios_base::openmode which)  {
	assert(!initedops_);	//initops wasn't called yet, should be false
	assert((which & ~std::ios_base::in) == 0);

	if (readpos_base_ <= sp && sp <= readpos_base_ + (egptr() - eback()))
	{
		//we have already buffered data at the requested position so just move the internal pointer
		//yes, position exactly after the last buffered character is valid as we don't want to cause buffer update before it is really needed
		this->setg(eback(), eback() + sp - readpos_base_, egptr());
	}
	else
	{
		iwindowbuf_sentry sentry(*this);

		std::streamoff result = source_->pubseekpos(windowbegin_ + sp, std::ios_base::in);
		if (-1 == result || result < windowbegin_ || result > windowend_) // > (not >=) because position exactly after the last character is valid, this allows checking length of a stream
		{
			return -1;
		}

		windowcursor_ = result;
		assert(windowbegin_ <= windowcursor_ && windowcursor_ <= windowend_);
		updatebuffer();
	}

	assert(readpos() == sp);	//by now, after all error checks, position visible outside must be equal to the requested position
	return readpos();
}
std::streambuf::int_type iwindowbuf::underflow() {
	assert(!initedops_);	//initops wasn't called yet, should be false

	if (gptr() >= egptr())
	{
		iwindowbuf_sentry sentry(*this);
		
		size_t n = updatebuffer(true);
		if (n == 0)	return traits_type::eof();
	}
    return traits_type::to_int_type(*gptr());
}


iwindowstream::iwindowstream(std::streambuf& source, int offset, size_t size) :
	std::istream(&buffer)
{
	buffer.init(source, offset, size);
}

}
