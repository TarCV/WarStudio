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

#include "BaseWriter.h"

#include "../Containers/BaseContainer.h"

#include <assert.h>

using namespace std;

namespace warstudio {
	namespace model {

void BaseWriter::write(const BaseContainer &in, OutputFileInfo outfile) const
{
    assert(in.getType() == getContainerType());
	doWrite(in, outfile);
}
const BaseContainer* BaseWriter::read(InputFileInfo infile, const Context& context) const
{
	auto ret = unique_ptr<const BaseContainer>(doRead(infile, context));
    assert(ret->getType() == getContainerType());
	return ret.release();
}

	}
}
