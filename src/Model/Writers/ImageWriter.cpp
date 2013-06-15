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

#include "ImageWriter.h"

#include "../Containers/ImageContainer.h"

#include <assert.h>
#include <memory>

using namespace std;

namespace warstudio {
	namespace model {

void ImageWriter::doWrite(const BaseContainer &in, OutputFileInfo out) const
{
	const Image& image = dynamic_cast<const ImageContainer &>(in).image;
	image.save(out.filepath);
}
std::unique_ptr<const BaseContainer> ImageWriter::doRead(InputFileInfo infile, const Context&) const
{
    unique_ptr<const ImageContainer> ret(new ImageContainer(infile.filepath));
    return std::move(ret);
}
ContainerType ImageWriter::doGetContainerType() const
{
	return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::IMAGE);
}

	}
}
