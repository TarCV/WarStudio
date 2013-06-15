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

#include "../lumptypes.h"
#include "../Containers/BaseContainer.h"
#include "../Graphic/Palette.h"

#include <string>
#include <iosfwd>
#include <memory>
#include <vector>
#include <map>

namespace warstudio {
	namespace model {

struct InputLumpData
{
	std::istream&	stream;
	size_t	size;

	std::map<std::string, Palette> palettes;	//adapter code is untrusted, so we will pass copies of needed palettes
};
struct OutputLumpData
{
	std::ostream&	stream;

	std::map<std::string, Palette>	palettes;
};

struct Context;
class BaseArchiver {
public:
	typedef	std::vector<std::string> PaletteDependencyList;
	
    std::unique_ptr<const BaseContainer> extract(const InputLumpData& in) const;
	void archive(const BaseContainer& in, const Context& context, OutputLumpData& out) const;
	int estimateSize(const BaseContainer& in) const {return doEstimateSize(in);}

	ContainerType getContainerType() const {return doGetContainerType();}
    PaletteDependencyList getPaletteDependencies() const {return doGetPaletteDependencies();}
	void cookContext(Context &context) const {doCookContext(context);}

	virtual ~BaseArchiver() {}

protected:
    virtual std::unique_ptr<const BaseContainer> doExtract(const InputLumpData& in) const = 0;
	virtual void doArchive(const BaseContainer& in, const Context& context, OutputLumpData& out) const = 0;
	virtual int doEstimateSize(const BaseContainer& in) const = 0;

	virtual ContainerType doGetContainerType() const = 0;
	virtual PaletteDependencyList doGetPaletteDependencies() const {return PaletteDependencyList(); /* most archivers do not depend on anything */}
	virtual void doCookContext(Context &) const {/* most adapters do not change context */}
};

	}
}
