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

#include "config.h"

#include <streambuf>
#include <istream>
#include <vector>
#include <assert.h>

namespace warstudio {

// These classes implement std interface, so for now I try to be close to std naming conventions here

class iwindowbuf : public std::streambuf
{
public:
	iwindowbuf();
	void init(std::streambuf& source, int offset, size_t size, std::size_t buff_size = WINDOWSTREAM_BUFFER_SIZE, std::size_t put_back = 8);

protected:
    int_type underflow() override;
	std::streampos seekoff (std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) override;
	std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override;

private:
	friend class iwindowbuf_sentry;

	size_t updatebuffer(bool onunderflow = false);
	int readpos() {return readpos_base_ + (gptr() - eback());}	//identical to stream.tellg, returns streampos withOUT windowbegin_ being added

	void begin_operations();
	void finish_operations();

private:
	std::streambuf*	source_;
	std::streamoff	source_oldpos_;

    size_t windowbegin_;
    size_t windowend_;
    size_t windowcursor_;	//streampos in the source_ (NOT suitable for stream.tellg)

	int readpos_base_;	//(windowbegin_ + readpos_base_) is the position of currently buffered data in the source
	std::size_t put_back_;

    std::vector<char> buffer_;

	bool initedops_;
};

class iwindowbuf_sentry
{
public:
	iwindowbuf_sentry(iwindowbuf& buf);
	~iwindowbuf_sentry();

private:
	iwindowbuf& buf_;
};

class iwindowstream : public std::istream {
private:
	iwindowbuf buffer;
public:
	iwindowstream(std::streambuf& source, int offset, size_t size);
};


}
