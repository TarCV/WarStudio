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

#include "BaseContainer.h"

#include "../Graphic/Image.h"

namespace warstudio {
	namespace model {

class ImageContainer : public BaseContainer {
public:
	ImageContainer(size_t width, size_t height, Color bgcolor = Color(0, 0, 0, 0), const Palette* palette = nullptr) : image(width, height, bgcolor, palette) {}
	ImageContainer(std::string file) : image(file) {}

	Image	image;
protected:
	ContainerType type_() const override {return ContainerType(SPACE_WARSTUDIO, CONTAINER_TYPE::IMAGE);};
};

	}
}