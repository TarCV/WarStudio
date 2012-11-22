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

#include "BaseConverter.h"

namespace warstudio {
	namespace model {

class PalToImgConverter : public BaseConverter {
protected:
	const BaseContainer* doExtract(const BaseContainer& in) const override;
	const BaseContainer* doArchive(const BaseContainer& in, const Context& context) const override;

	ContainerType doGetArchiveContainerType() const override {return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::PALETTES);};
	ContainerType doGetFileContainerType() const override {return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::IMAGE);};

};

	}
}