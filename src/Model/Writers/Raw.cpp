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

#include "../../stdafx.h"

#include "Raw.h"
#include "../Containers/StreamBufferContainer.h"
#include "../../utility.h"

#include <assert.h>
#include <memory>
#include <fstream>

using namespace std;

namespace warstudio {
	namespace model {

void RawWriter::doWrite(const BaseContainer &in, OutputFileInfo out) const
{
	ofstream outdata(out.filepath, ios_base::out | ios_base::binary | ios_base::trunc);
	const StreamBuffer&	buffer = dynamic_cast<const StreamBufferContainer &>(in).buffer;
	StreamCopy(buffer.ReadableData(), outdata);
}
const BaseContainer* RawWriter::doRead(InputFileInfo in, const Context& context) const
{
	ifstream	indata(in.filepath);
	unique_ptr<StreamBufferContainer> ret(new StreamBufferContainer(&indata, false, in.size));
	StreamCopy(indata, ret->buffer.WritableData(in.size));
	return ret.release();
}
ContainerType RawWriter::doGetContainerType() const
{
	return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::STREAMBUFFER);
}

	}
}
