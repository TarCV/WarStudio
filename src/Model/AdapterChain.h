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

#include "Archivers/ArchiverRegistry.h"
#include "Converters/ConverterRegistry.h"
#include "Writers/WriterRegistry.h"

#include <vector>
#include <memory>
#include <string>

namespace warstudio {

	namespace model {

		class BaseNode;
		class BaseContainer;
		class BaseArchiver;
		class BaseWriter;

class AdapterChain
{
public:
    typedef std::vector<const BaseConverter*> ConverterList;
    typedef std::vector<ConverterId>  ConverteridList;

    AdapterChain(BUILTIN_ARCHIVER, const ConverteridList&&, BUILTIN_WRITER, const std::string extension);
	AdapterChain(const BaseArchiver*, const ConverterList&&, const BaseWriter*, const std::string extension);

	void extract(const BaseNode&, std::string to_file) const;
	void extract(const BaseContainer&, std::string to_file) const;
	const BaseContainer* extract(const BaseNode&) const;
	const BaseContainer* extract(const BaseContainer&) const;

    std::string getExtension() const;

//TODO: find more elegant way than using this method
    const BaseArchiver::PaletteDependencyList getPaletteDependencies() const;

/*	void archive(std::string from_file, const BaseNode& to_node) const;
	void archive(const BaseContainer& from, const BaseNode& to_node) const;
	const BaseContainer* archive(std::string from_file) const;
	BaseContainer* archive(const BaseContainer& from) const;*/

	friend std::ostream& operator << (std::ostream&, const AdapterChain&);
	friend std::istream& operator >> (std::istream&, AdapterChain&);
private:
	const BaseContainer* doExtractFromNode(const BaseNode& from_node) const;
	const BaseContainer* doExtract(const BaseContainer&) const;
    void doExtractToFile(const BaseContainer&, std::string to_file) const;

    static const ConverterList translateConverterids(const ConverteridList&&);

private:
    //objects are owned by coressponding registries
    const BaseArchiver* archiver_;
	ConverterList converters_;
    const BaseWriter* writer_;
	std::string file_extension_;
};

	}
}
