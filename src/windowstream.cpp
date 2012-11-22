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

namespace warstudio {

void windowbuf::INITOPERATIONS() {
	assert(!initedops_);
	source_oldpos = source_->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
	if (source_oldpos != windowcursor_) {
		std::streamoff result = source_->pubseekoff(windowcursor_, std::ios_base::beg, std::ios_base::in);
		if (result != windowcursor_)	error("windowbuf::initoperations failed");
	}
	initedops_ = true;
}
void windowbuf::FINISHOPERATIONS() {
	assert(initedops_);

	windowcursor_ = source_->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
	assert(windowbegin_ <= windowcursor_ && windowcursor_ <= windowend_);

	source_->pubseekoff(source_oldpos, std::ios_base::beg, std::ios_base::in);
	initedops_ = false;
}

windowbuf::windowbuf() : 
#ifndef NDEBUG
	initedops_(false),
#endif
	source_(0), 
	readpos_base_(0) 
{}


void windowbuf::init(std::streambuf& source, int offset, size_t size, size_t buff_size, size_t put_back)
{
	assert(!initedops_);	//initops wasn't called yet, should be false

	if (source_)	error("windowbuf is already inited");
	source_ = &source;
	windowbegin_ = offset;
	windowend_ = offset+size;
	windowcursor_ = offset;
	readpos_base_ = offset;
    put_back_ = std::max(put_back, size_t(1));
    buffer_.resize(std::max(buff_size, put_back_) + put_back_);
    char *end = &buffer_.front() + buffer_.size();
    setg(end, end, end);
}
size_t windowbuf::updatebuffer(bool onunderflow) 
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
	readpos_base_ = windowcursor_;
	if (toread) {
		n = source_->sgetn(start, toread);
	} else {
		n = 0;
	} 
	setg(onunderflow ? base : start, start, start + n);
	return n;
}

std::streampos windowbuf::seekoff (std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)  {
	assert((which & ~std::ios_base::in) == 0);
	assert(!initedops_);	//initops wasn't called yet, should be false

	if (way == std::ios_base::beg)
		;
	else if (way == std::ios_base::end)
		off = windowend_ - off - windowbegin_;
	else if (way == std::ios_base::cur)
		off += readpos() - windowbegin_;
	else
		assert(false);

	return seekpos(off, which);
}

std::streampos windowbuf::seekpos(std::streampos sp, std::ios_base::openmode which)  {
	assert(!initedops_);	//initops wasn't called yet, should be false

	INITOPERATIONS();
		assert((which & ~std::ios_base::in) == 0);

		std::streamoff oldpos = source_->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
		std::streamoff result = source_->pubseekpos(windowbegin_ + sp, std::ios_base::in);
		if (-1 == result || result < windowbegin_ || result > windowend_) {
			source_->pubseekoff(oldpos, std::ios_base::cur, std::ios_base::in);
			return -1;
		}

		if (oldpos != result)
		{
			windowcursor_ = source_->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
			assert(windowbegin_ <= windowcursor_ && windowcursor_ <= windowend_);
			updatebuffer();
		}
	FINISHOPERATIONS();

	return result - windowbegin_;
}
std::streambuf::int_type windowbuf::underflow() {
	assert(!initedops_);	//initops wasn't called yet, should be false

    if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());

	INITOPERATIONS();
		size_t n = updatebuffer(true);
	FINISHOPERATIONS();

	if (n == 0)	return traits_type::eof();
    return traits_type::to_int_type(*gptr());
}


iwindowstream::iwindowstream(std::streambuf& source, int offset, size_t size) :
	std::istream(&buffer)
{
	buffer.init(source, offset, size);
}

}