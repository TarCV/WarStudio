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

#include <Magick++.h>

namespace warstudio {
	namespace model {

class Color
{
public:
	Color() : color_(0, 0, 0, 0xff) {}
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xff) : color_(ConvertByteToQuantum(r), ConvertByteToQuantum(g), ConvertByteToQuantum(b), ConvertByteToQuantum(a)) {}

	void setR(unsigned char r) {color_.redQuantum(ConvertByteToQuantum(r));}
	unsigned char getR() const {return ConvertQuantumToByte(color_.redQuantum());}
	void setG(unsigned char g) {color_.greenQuantum(ConvertByteToQuantum(g));}
	unsigned char getG() const {return ConvertQuantumToByte(color_.greenQuantum());}
	void setB(unsigned char b) {color_.blueQuantum(ConvertByteToQuantum(b));}
	unsigned char getB() const {return ConvertQuantumToByte(color_.blueQuantum());}
	void setA(unsigned char a) {color_.alphaQuantum(ConvertByteToQuantum(a));}
	unsigned char getA() const {return ConvertQuantumToByte(color_.alphaQuantum());}

private:
	friend class Image;
	Color(Magick::Color c) : color_(c) {}
	Magick::Color getNativeColor_() const {return color_;};

private:
	Magick::Color			color_;

	static const int		quantum_shift_ = QuantumDepth - 8;
	static const int		quantum_fix_ = (QuantumDepth > 8) ? (1 << (QuantumDepth - 9)) : 0;
	static unsigned char ConvertQuantumToByte(MagickLib::Quantum q) {return (q >> quantum_shift_);}
	static MagickLib::Quantum ConvertByteToQuantum(unsigned char b) {return ((b << quantum_shift_) + quantum_fix_);}
};

	}
}