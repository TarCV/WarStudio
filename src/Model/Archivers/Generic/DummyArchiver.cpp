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

#include "../../../stdafx.h"

#include "DummyArchiver.h"

#include "../../Containers/StreamBufferContainer.h"
#include "../../../StreamBuffer.h"
#include "../../../utility.h"

#include <assert.h>
#include <memory>

using namespace std;

namespace warstudio {
	namespace model {

const BaseContainer *DummyArchiver::doExtract(const InputLumpData& in) const
{
	return (new StreamBufferContainer(&in.stream, false, in.size));
}
void DummyArchiver::doArchive(const BaseContainer& in, const Context& context, OutputLumpData& out) const
{
	const StreamBufferContainer& inbuf = dynamic_cast<const StreamBufferContainer&>(in);
	StreamCopy(inbuf.buffer.ReadableData(), out.stream);
}
int DummyArchiver::doEstimateSize(const BaseContainer& in) const 
{
	const StreamBufferContainer& inbuf = dynamic_cast<const StreamBufferContainer&>(in);
	return inbuf.buffer.size();
}
ContainerType DummyArchiver::doGetContainerType() const
{
	return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::STREAMBUFFER);
}

	}
}
