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

#include "../Containers/BaseContainer.h"

#include <memory>

namespace warstudio {
	namespace model {

struct Context;
class BaseConverter {
protected:
    virtual std::unique_ptr<const BaseContainer> doExtract(const BaseContainer& in) const = 0;
    virtual std::unique_ptr<const BaseContainer> doArchive(const BaseContainer& in, const Context& context) const = 0;

	virtual ContainerType doGetArchiveContainerType() const = 0;
	virtual ContainerType doGetFileContainerType() const = 0;
	virtual void doCookContext(Context &) const {/* most adapters do not change context */}
public:
    std::unique_ptr<const BaseContainer> extract(const BaseContainer& in) const;
    std::unique_ptr<const BaseContainer> archive(const BaseContainer& in, const Context& context) const;

	ContainerType getArchiveContainerType() const {return doGetArchiveContainerType();}
	ContainerType getFileContainerType() const {return doGetFileContainerType();}
	void cookContext(Context &context) const {doCookContext(context);}

	virtual ~BaseConverter() {}
};

	}
}
