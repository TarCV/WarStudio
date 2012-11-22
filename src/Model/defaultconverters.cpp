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

#include "stdafx.h"

#include "defaultconverters.h"

#include "Containers/basecontainer.h"

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

#include <filesystem>

using namespace std;
using namespace std::tr2;

namespace warstudio {
	namespace model {

DefaultConverters::ConvMap		DefaultConverters::convertpaths;

ostream& operator << (ostream& stream, const DefaultConverters::ConvertPath& path)
{
	auto oldflags = stream.setf(ostream::hex, ostream::basefield);
	stream << "{";
	stream << path.archiver.space << ":" << path.archiver.id << " ";
	for (auto it = path.converters.begin(); it != path.converters.end(); it++)
	{
		stream << it->space << ":" << it->id << " ";
	}
	stream << path.writer.space << ":" << path.writer.id;
	stream << "}";
	stream.setf(oldflags);
	return stream;
}
istream& operator >> (istream& stream, DefaultConverters::ConvertPath& path)
{
	auto oldflags = stream.setf(ostream::hex, ostream::basefield);	//todo: reset path using RAII
	if ('{' != stream.get())	{stream.unget(); path.reset(); return stream;}
	stream >> path.archiver.space;
	if (':' != stream.get())	{stream.setstate(istream::failbit); path.reset(); return stream;}
	stream >> path.archiver.id;

	while (stream.good()) {
		uint32_t	space, id;
		stream >> space;
		if (':' != stream.get())	{stream.setstate(istream::failbit); path.reset(); return stream;}
		stream >> id;
		
		if ('}' != stream.get())
		{
			path.converters.push_back(ConverterId(space, id));
		} else {
			path.writer.space = space;
			path.writer.id = id;
			break;
		}
	}
	stream.setf(oldflags);
	return stream;
}

void DefaultConverters::init() 
{
	if (!convertpaths.empty())	return;

	registerPath(LUMP_TYPE::RAW, BUILTIN_ARCHIVER::DUMMY, BUILTIN_WRITER::RAW, "dat");

	{
		const ConverterId	palconv[] = {ConverterId(SPACE_WARSTUDIO, CONVERTER_TYPE::PALETTE_TO_IMAGE)};
		registerPath(LUMP_TYPE::DOOMPLAYPAL, BUILTIN_ARCHIVER::DOOM_PLAYPAL, ConverterSet(begin(palconv), end(palconv)), BUILTIN_WRITER::IMAGE, "png");
	}

	{
		const ConverterId	mapconv[] = {ConverterId(SPACE_WARSTUDIO, CONVERTER_TYPE::COLORMAP_TO_IMAGE)};
		registerPath(LUMP_TYPE::DOOMCOLORMAP, BUILTIN_ARCHIVER::DOOM_COLORMAP, ConverterSet(begin(mapconv), end(mapconv)), BUILTIN_WRITER::IMAGE, "png");
	}

	registerPath(LUMP_TYPE::DOOMDMXGUS, BUILTIN_ARCHIVER::DUMMY, BUILTIN_WRITER::RAW, "ini");
	
	registerPath(LUMP_TYPE::DOOMPATCH, BUILTIN_ARCHIVER::DOOM_PATCH, BUILTIN_WRITER::IMAGE, "png");
	registerPath(LUMP_TYPE::DOOMFLAT, BUILTIN_ARCHIVER::DOOM_FLAT, BUILTIN_WRITER::IMAGE, "png");
}
void DefaultConverters::registerPath(LUMP_TYPE type, BUILTIN_ARCHIVER archiver, BUILTIN_WRITER writer, string file_extension)
{
	registerPath(type, archiver, ConverterSet(), writer, file_extension);
}
void DefaultConverters::registerPath(LUMP_TYPE type, BUILTIN_ARCHIVER archiver, ConverterSet& converters, BUILTIN_WRITER writer, string file_extension)
{
	const ArchiverId	archiver_id(SPACE_WARSTUDIO, archiver);
	assert(global.archivers.isKnown(archiver_id));

#ifndef  NDEBUG
	for (const auto converter_id : converters)
	{
		assert(global.converters.isKnown(converter_id));
	}
#endif

	const WriterId	writer_id(SPACE_WARSTUDIO, writer);
	assert(global.writers.isKnown(writer_id));
	const ConvertPath	path = {archiver_id, converters, writer_id, file_extension};
	convertpaths.insert(make_pair(type, path));
}
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
string DefaultConverters::getFileExtension(const BaseNode& node)
{
	init();

	LUMP_TYPE type = node.type();
	if (convertpaths.count(type) == 0)	//todo: remove when everything is implemented
		type = LUMP_TYPE::RAW;
	const ConvertPath path = convertpaths.at(type);
	return path.file_extension;
}
string DefaultConverters::extractWrite(const BaseNode& node, const string& outfile)
{
	init();

	LUMP_TYPE type = node.type();

	if (convertpaths.count(type) == 0)	//todo: remove when everything is implemented
		type = LUMP_TYPE::RAW;
	const ConvertPath path = convertpaths.at(type);

	if (!global.archivers.isKnown(path.archiver))	error("Archiver is not registered");
	InputLumpData inex = {node.ReadableData(), node.size()};
	const BaseArchiver &archiver = global.archivers.get(path.archiver);
	const BaseArchiver::PaletteDependencyList palette_names = archiver.getPaletteDependencies();
	for (auto palette_name : palette_names)
	{
		inex.palettes.insert(make_pair(palette_name, node.getDirectory()->getPalette(palette_name)));
	}

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
}

	}
}