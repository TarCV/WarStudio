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

#include "../BaseNode.h"
#include "../../../StreamBuffer.h"

#include <iosfwd>
#include <memory>

namespace warstudio {

	namespace model {
		namespace doom {

class wad;
class wadlump : public BaseNode {
	friend wad;
/*	friend iwindowstream* wad::CreateStreamFor(const wadlump &lump) const;
	friend wadlump* wad::CreateLump(BaseNode* parent, std::string name, int offset, int size);*/
private:
	mutable model::LUMP_TYPE type_;
	mutable std::unique_ptr<StreamBuffer>	buffer_;

protected:
	uint32_t offset;
	uint32_t size_;
	void validatebuffer() const;
	wadlump(wad& root, BaseNode* parent, std::string name, int offset = 0, int size = 0);
public:
	model::LUMP_TYPE type() const override;
    size_t size() const override;
	void type(model::LUMP_TYPE newtype) {
		//todo: fastsanecheck
		type_ = newtype;
	}
	std::istream& ReadableData() const override;	//returns _mutable_ istream
	std::ostream& WritableData(int maxsize) override;
	void Stored() override;
};

		}
	}
}
