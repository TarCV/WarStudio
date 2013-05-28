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

#include "../BaseArchiver.h"

namespace warstudio {
	namespace model {
		namespace doom {

class PlaypalArchiver : public BaseArchiver
{
public:
	PlaypalArchiver(size_t colors_per_palette, size_t palette_num);
protected:
	const BaseContainer *doExtract(const InputLumpData& in) const override;
	void doArchive(const BaseContainer& in, const Context& context, OutputLumpData& out) const override;
	int doEstimateSize(const BaseContainer& in) const override;
	ContainerType doGetContainerType() const override {return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::PALETTES);}
private:
	PlaypalArchiver();

private:
	const size_t colors_per_palette_;
	const size_t palette_num_;
};

		}
	}
}
