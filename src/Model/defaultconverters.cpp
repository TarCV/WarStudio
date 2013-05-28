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

#include "../stdafx.h"

#include "defaultconverters.h"

#include "Containers/BaseContainer.h"

#include "Archivers/Generic/DummyArchiver.h"
#include "Archivers/Doom/PLAYPAL.h"

#include "Converters/PalToImgConverter.h"

#include "Writers/Raw.h"
#include "Writers/ImageWriter.h"

#include "Nodes/BaseNode.h"
#include "Nodes/DirectoryNode.h"

#include "../utility.h"
#include "Globals.h"

#include <fstream>
#include <memory>
#include <array>

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem3;

namespace warstudio {
	namespace model {

/*const BaseContainer* DefaultConverters::Read(string& infile)
{
    if (!sys::exists<sys::path>(infile) || sys::is_directory<sys::path>(infile))	error("File not found");
    const InputFileInfo ininfo = {infile, sys::file_size<sys::path>(infile)};
    const RawWriter	writer;
    return writer.read(ininfo);	//now caller is responsible for freeing the container
}
void DefaultConverters::Archive(const BaseContainer& in, ostream& out)
{
    //todo:
    const DummyArchiver arc;
    OutputLumpData outex = {out};
    arc.archive(in, outex);
}
void DefaultConverters::Archive(const BaseContainer& in, BaseNode& out)
{
    //todo:
    const DummyArchiver arc;
    Archive(in, out.WritableData(arc.estimateSize(in)));
}*/
/*string DefaultConverters::getFileExtension(const BaseNode& node)
{
	init();

	LUMP_TYPE type = node.type();
	if (convertpaths.count(type) == 0)	//todo: remove when everything is implemented
		type = LUMP_TYPE::RAW;
    const AdapterChain& path = convertpaths.at(type);
    return path.getExtension();
}*/
/*string DefaultConverters::extractWrite(const BaseNode& node, const string& outfile)
{
	init();

	LUMP_TYPE type = node.type();

	if (convertpaths.count(type) == 0)	//todo: remove when everything is implemented
	{
		type = LUMP_TYPE::RAW;
	}

#ifndef NDEBUG
	if (LUMP_TYPE::RAW == type)
	{
		warning("DEBUG: skipped");
		return "";
	}
#endif

    const AdapterChain& path = convertpaths.at(type);

	InputLumpData inex = {node.ReadableData(), node.size()};
    const BaseArchiver::PaletteDependencyList palette_names = path.getPaletteDependencies();
	for (auto palette_name : palette_names)
	{
		inex.palettes.insert(make_pair(palette_name, node.getDirectory()->getPalette(palette_name)));
	}

    path.extract(node, outfile);

	unique_ptr<const BaseContainer> data(archiver.extract(inex));
	ContainerType	prevtype = archiver.getContainerType();
	
	for (auto it = path.converters.begin(); it != path.converters.end(); ++it) {
		if (!global.converters.isKnown(*it))	error("Converter is not registered");
		const BaseConverter &converter = global.converters.get(*it);
		if (converter.getArchiveContainerType() != prevtype)	error("Adapters are incompatible");
		prevtype = converter.getFileContainerType();
		data.reset(converter.Extract(*data));
	}

	if (!global.writers.isKnown(path.writer))	error("Writer is not registered");
	const BaseWriter	&writer = global.writers.get(path.writer);
	if (writer.getContainerType() != prevtype)	error("Adapters are incompatible");
	const OutputFileInfo outinfo = {outfile, path.file_extension};
	writer.write(*data, outinfo);

	ostringstream	pathinfo;
	pathinfo.exceptions(ofstream::failbit | ofstream::badbit);
	pathinfo << path;
	return pathinfo.str();
}*/

	}
}
