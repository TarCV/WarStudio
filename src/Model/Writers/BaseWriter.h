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

#include <iosfwd>
#include <assert.h>
#include <memory>

namespace warstudio {
	namespace model {

struct InputFileInfo
{
	std::string		filepath;
	const size_t	size;
};
struct OutputFileInfo
{
	std::string		filepath;
	std::string		format;	//output file extension
};

struct Context;
class BaseWriter {
protected:
	virtual void doWrite(const BaseContainer &in, OutputFileInfo outfile) const = 0;
    virtual std::unique_ptr<const BaseContainer> doRead(InputFileInfo infile, const Context& context) const = 0;

	virtual ContainerType doGetContainerType() const = 0;
	virtual void doCookContext(Context &) const {/* most adapters do not change context */}
public:
	void write(const BaseContainer &in, OutputFileInfo outfile) const;
    std::unique_ptr<const BaseContainer> read(InputFileInfo infile, const Context& context) const;

	ContainerType getContainerType() const {return doGetContainerType();}
	void cookContext(Context &context) const {doCookContext(context);}
};

	}
}
