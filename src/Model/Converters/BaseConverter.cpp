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

#include "BaseConverter.h"

#include "../Containers/BaseContainer.h"

#include <assert.h>

using namespace std;

namespace warstudio {
	namespace model {

unique_ptr<const BaseContainer> BaseConverter::extract(const BaseContainer& in) const
{
    assert(in.getType() == getArchiveContainerType());
    unique_ptr<const BaseContainer> ret(doExtract(in));
    assert(ret->getType() == getFileContainerType());
    return ret;
}
unique_ptr<const BaseContainer> BaseConverter::archive(const BaseContainer& in, const Context& context) const
{
    assert(in.getType() == getFileContainerType());
    unique_ptr<const BaseContainer> ret(doArchive(in, context));
    assert(ret->getType() == getArchiveContainerType());
    return ret;
}

	}
}
