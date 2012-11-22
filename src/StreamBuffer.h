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

#include <iosfwd>
#include <string>
#include <memory>

#include "tempfile.h"

namespace warstudio {

class StreamBuffer
{
private:
	std::istream	*readable_stream_;		//sometimes we don't own this stream, so here is wild pointer (todo: fix)
	bool	own_stream_;

	std::unique_ptr<std::iostream>	writable_stream_;
	enum class STREAM_TYPE {NOTHING, INPUTSTREAM, STRINGSTREAM, FILESTREAM}	streamtype;

	TempFile	filebuffer;

	int size_;

private:
	void deletereadable();
public:
	StreamBuffer(std::istream *input_stream = nullptr, bool own_stream = false, int size = -1);
	~StreamBuffer(void);

	std::istream& ReadableData() const;
	std::ostream& WritableData(int maxsize);
	int size() const;
};

}