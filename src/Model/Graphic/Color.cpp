/*

	WadDirectory
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

#include "stdafx.h"

#include "Color.h"

#include <algorithm>
#include <assert.h>

namespace warstudio {
	namespace model {

using namespace std;

unsigned int difference(const Color& a, const Color& b, uint8_t alpha_tolerance)
{
	if (abs(a.getA() - b.getA()) > alpha_tolerance)	return UINT_MAX;

	uint8_t differences[] = {
		abs(a.getR() - b.getR()), 
		abs(a.getG() - b.getG()),
		abs(a.getB() - b.getB()),
		abs(a.getA() - b.getA())
		};
	unsigned int ret = *std::max_element(std::begin(differences), std::end(differences));
	assert(ret < UINT_MAX - 1 - std::distance(std::begin(differences), std::end(differences)));
	ret += std::count_if(std::begin(differences), std::end(differences), [](const uint8_t &val) {return val > 0;} );
	return ret;
}

	}
}