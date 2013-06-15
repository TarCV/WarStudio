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

#include "PLAYPAL.h"

#include "../../Containers/PaletteContainer.h"
#include "../../../utility.h"

#include <assert.h>
#include <memory>

using namespace std;
namespace warstudio {
	namespace model {
		namespace doom {

PlaypalArchiver::PlaypalArchiver(size_t colors_per_palette, size_t palette_num) :
	colors_per_palette_(colors_per_palette),
	palette_num_(palette_num)
{
}
unique_ptr<const BaseContainer> PlaypalArchiver::doExtract(const InputLumpData& in) const
{
	assert(in.size % (colors_per_palette_*3) == 0);
	unique_ptr<PaletteContainer>	out(new PaletteContainer(in.size / 768));

	assert(out->palset.size() == in.size / 768);
	for (auto &pal : out->palset)
	{
		pal.resize(colors_per_palette_);
		for (auto &c : pal)
		{
			unsigned char pixel[3];
			readFromStream<unsigned char, 3>(in.stream, pixel);
			c = Color(pixel[0], pixel[1], pixel[2]);
		}
	}
    return std::move(out);
}
void PlaypalArchiver::doArchive(const BaseContainer& in, const Context& context, OutputLumpData& out) const
{
	const PaletteSet	&palset = dynamic_cast<const PaletteContainer &>(in).palset;
	for (const auto &pal : palset)
	{
		assert(pal.size() == colors_per_palette_);
		for (const auto &c : pal)
		{
            assert(c.getA() == 0xff);
			unsigned char pixel[3] = {c.getR(), c.getG(), c.getB()};
			writeToStream(out.stream, pixel);
		}
	}
}

int PlaypalArchiver::doEstimateSize(const BaseContainer& in) const 
{
	const PaletteSet	&palset = dynamic_cast<const PaletteContainer &>(in).palset;
	return palset.size()*768;
}
/*bool PlaypalArchiver::fastsanecheck(const baselump &lump) {	// todo: move to the 'doom::PlaypalType' class
	bool check = (lump.getSize() % 768 == 0);
	if (lump.getSize() == 0) {
		warning(lump.getName() + " is empty");
		return false;
	}
	if (!check)	{
		warning(lump.getName() + " is probably a corrupted PLAYPAL: has size that is not multiple of 256*3 bytes");
		return false;
	}
	int numpalettes = lump.getSize() / 768;
	if (numpalettes != 14) {
		char buf[sizeof(int)*4]; sprintf(buf, "%s", numpalettes);
		warning(lump.getName() + " is probably a corrupted PLAYPAL: has " + buf + "palettes instead of 14 palettes");
		return true;
	}
	return true;
}*/

		}
	}
}
