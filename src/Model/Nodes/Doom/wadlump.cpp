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

#include "wadlump.h"
#include "wad.h"
#include "wadlumptyper.h"
#include "../../../StreamBuffer.h"
#include "../../../windowstream.h"

#include <assert.h>

using namespace std;

namespace warstudio {
	namespace model {
		namespace doom {

wadlump::wadlump(wad& root, BaseNode* parent, string name, int offset, int size) :
	BaseNode(name, &root, parent), 
	offset(offset), 
	size_(size), 
	type_(LUMP_TYPE::UNKNOWN), 
	buffer_(nullptr)
{
	assert(sizeof(int) >= sizeof(uint32_t));	//size() returns int, but underlying size_ is uint32_t
	assert(offset == 0 || offset >= 12);
	assert(offset == 0 ? size == 0 : true);
}
void wadlump::validatebuffer() const {
	if (!buffer_) {
		const wad* file = dynamic_cast<const wad *>(directory);
		if (offset != 0)
		{
			buffer_.reset(new StreamBuffer(file->CreateStreamFor(*this), true));
		} else {
			buffer_.reset(new StreamBuffer());
		}
	}
}
istream& wadlump::ReadableData() const
{
	validatebuffer();
	return buffer_->ReadableData();
}
std::ostream& wadlump::WritableData(int maxsize)
{
	validatebuffer();
	return buffer_->WritableData(maxsize);
}
void wadlump::Stored()
{
	buffer_.reset();	//data from the buffer is now stored in the directory, we can now free memory
}
int wadlump::size() const
{
	if (buffer_) {
		int ret = buffer_->size();
		if (-1 == ret)	return size_;
		return ret;
	}
	return size_;
}
model::LUMP_TYPE wadlump::type() const
{	//due the way wadlumptyper::getType implemented, type() is required not to call wadlumptyper::getType for children of the lump
	if (LUMP_TYPE::UNKNOWN == type_) type_ = wadlumptyper::typeof(*this);
	return type_;
}

		}
	}
}