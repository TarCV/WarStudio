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

#include "StreamBuffer.h"
#include "utility.h"
#include "config.h"
#include <fstream>
#include <sstream>
#include <assert.h>

using namespace std;

namespace warstudio {

StreamBuffer::StreamBuffer(std::istream *input_stream, bool own_stream, int size) :
	readable_stream_(input_stream),
	own_stream_(own_stream),
	writable_stream_(nullptr),
	streamtype(input_stream ? STREAM_TYPE::INPUTSTREAM : STREAM_TYPE::NOTHING),
	size_(size)
{
	assert(input_stream ? true : (!own_stream));	//can't own nothing (when input_stream == nullptr)
}

void StreamBuffer::deletereadable() {
	if (own_stream_ && readable_stream_) {
		delete readable_stream_;		//sometimes we don't own this stream, so here is wild pointer machinery
		readable_stream_ = nullptr;
		own_stream_ = false;
		size_ = -1;
		if (streamtype == STREAM_TYPE::INPUTSTREAM)	streamtype = STREAM_TYPE::NOTHING;
	}
}
StreamBuffer::~StreamBuffer(void)
{
	deletereadable();
}

istream& StreamBuffer::ReadableData() const
{
	istream *ret = writable_stream_ ? writable_stream_.get() : readable_stream_;
	ret->seekg(0);
	return *ret;
}
ostream& StreamBuffer::WritableData(int maxsize)
{
	if (!writable_stream_ || (maxsize >= STRING_STREAM_MAX && streamtype != STREAM_TYPE::FILESTREAM)) {
		if (maxsize < STRING_STREAM_MAX) {
			writable_stream_.reset(new stringstream(ios_base::out|ios_base::in|ios_base::binary|ios_base::trunc));
			streamtype = STREAM_TYPE::STRINGSTREAM;
			if (readable_stream_)	{
				StreamCopy(*readable_stream_, *writable_stream_);
				deletereadable();
			}
		} else {
			filebuffer.CreateNew();

			unique_ptr<fstream> newdata(new fstream(filebuffer.Path(), ios_base::out|ios_base::in|ios_base::binary|ios_base::trunc));
			if (readable_stream_)	{
				StreamCopy(*readable_stream_, *newdata);
				deletereadable();
			} else if (writable_stream_) {
				StreamCopy(*writable_stream_, *newdata);
			}
			writable_stream_.reset(newdata.release());	//doesn't support pointers to child classes
			streamtype = STREAM_TYPE::FILESTREAM;
		}
	}
	writable_stream_->seekp(0);
	return *writable_stream_;
}

int StreamBuffer::size() const 
{
	if (streamtype == STREAM_TYPE::INPUTSTREAM) {
		return size_;
	} else if (streamtype == STREAM_TYPE::STRINGSTREAM) {
		stringstream *stream = dynamic_cast<stringstream*>(writable_stream_.get());
		return stream->str().size();
	} else if (streamtype == STREAM_TYPE::FILESTREAM) {
		assert(dynamic_cast<fstream*>(writable_stream_.get()) != 0);
		writable_stream_->clear();
		streampos oldpos = writable_stream_->tellg();
		writable_stream_->seekg(0, ios_base::end);
		int ret = writable_stream_->tellg();
		writable_stream_->seekg(oldpos);
		return ret;
	}
	assert(false);
	return 0;
}

}