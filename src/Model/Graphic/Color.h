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

#include "../../magick.h"

#include <stdint.h>

namespace warstudio {
	namespace model {

class Color
{
public:
	// in Magick++ alphaQuantum of QuantumMax is fully transparent, so we use (0xff - alphaByte)
	Color() : color_(ConvertByteToQuantum(0), ConvertByteToQuantum(0), ConvertByteToQuantum(0), ConvertByteToQuantum(0xff - 0xff)) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff) : color_(ConvertByteToQuantum(r), ConvertByteToQuantum(g), ConvertByteToQuantum(b), ConvertByteToQuantum(0xff - a)) {}

	void setR(uint8_t r) {color_.redQuantum(ConvertByteToQuantum(r));}
	uint8_t getR() const {return ConvertQuantumToByte(color_.redQuantum());}
	void setG(uint8_t g) {color_.greenQuantum(ConvertByteToQuantum(g));}
	uint8_t getG() const {return ConvertQuantumToByte(color_.greenQuantum());}
	void setB(uint8_t b) {color_.blueQuantum(ConvertByteToQuantum(b));}
	uint8_t getB() const {return ConvertQuantumToByte(color_.blueQuantum());}

	void setA(uint8_t a) {color_.alphaQuantum(ConvertByteToQuantum(0xff - a));}
	uint8_t getA() const {return 0xff - ConvertQuantumToByte(color_.alphaQuantum());}
	 
	bool operator==(const Color &r) const  {return (getNativeColor_() == r.getNativeColor_() && getA() == r.getA());}
	bool operator!=(const Color &r) const  {return !operator==(r);}

private:
	friend class Image;
	Color(const Magick::Color &c) : color_(c) {}
	const Magick::Color& getNativeColor_() const {return color_;};

private:
	Magick::Color			color_;

	static const int                quantum_shift_ = QuantumDepth - 8;
	static const MagickLib::Quantum quantum_fix_ = (QuantumDepth > 8) ? (1 << (QuantumDepth - 9)) : 0;
	static uint8_t ConvertQuantumToByte(MagickLib::Quantum q) {return (q >> quantum_shift_);}
	static MagickLib::Quantum ConvertByteToQuantum(uint8_t b) {
		if (0 == b)
		{
			return 0;
		}
		else if (0xff == b)
		{
			static_assert(MaxRGB > ((0xff << quantum_shift_) + quantum_fix_), "precondition failed");
			return MaxRGB;
		}
		else
		{
			return (b << quantum_shift_) + quantum_fix_;
		}
	}
};

unsigned int difference(const Color& a, const Color& b, uint8_t alpha_tolerance = 5);

	}
}
