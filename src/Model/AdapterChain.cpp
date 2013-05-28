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

#include "../stdafx.h"

#include "AdapterChain.h"

#include "Nodes/BaseNode.h"
#include "Archivers/BaseArchiver.h"
#include "Writers/BaseWriter.h"
#include "../exceptions.h"
#include "../utility.h"
#include "Globals.h"

#include <algorithm>

namespace warstudio {
	namespace model {

    using namespace std;

AdapterChain::AdapterChain(BUILTIN_ARCHIVER archiver, const ConverteridList&& converters, BUILTIN_WRITER writer, const string extension)
    : AdapterChain(
          &global.archivers.get(ArchiverId(SPACE_WARSTUDIO, archiver)),
          translateConverterids(std::move(converters)),
          &global.writers.get(WriterId(SPACE_WARSTUDIO, writer)),
          extension)
{}

AdapterChain::AdapterChain(const BaseArchiver* archiver, const ConverterList&& converters, const BaseWriter* writer, const string extension) :
	archiver_(archiver),
    converters_(std::move(converters)),
	writer_(writer),
	file_extension_(extension)
{}

void AdapterChain::extract(const BaseNode& node, std::string to_file) const
{
	unique_ptr<const BaseContainer> data(doExtractFromNode(node));
	data.reset(doExtract(*data));
	doExtractToFile(*data, to_file);
}

void AdapterChain::extract(const BaseContainer& from_data, std::string to_file) const
{
	if (archiver_)	throw MethodNotApplicableException("an archiver is specified but not used");
	
	unique_ptr<const BaseContainer> data(doExtract(from_data));
	doExtractToFile(*data, to_file);
}

const BaseContainer* AdapterChain::extract(const BaseNode& node) const
{
	if (writer_)	throw MethodNotApplicableException("a writer is specified but not used");

	return doExtractFromNode(node);
}

const BaseContainer* AdapterChain::extract(const BaseContainer& data) const
{
	if (writer_)	throw MethodNotApplicableException("a writer is specified but not used");

    return doExtract(data);
}

string AdapterChain::getExtension() const
{
    return file_extension_;
}

const BaseArchiver::PaletteDependencyList AdapterChain::getPaletteDependencies() const
{
    return archiver_->getPaletteDependencies();
}

const BaseContainer* AdapterChain::doExtractFromNode(const BaseNode& from_node) const
{
	if (!archiver_)	throw MethodNotApplicableException("an archiver is needed but is not specified");

	InputLumpData inex = {from_node.ReadableData(), from_node.size()};
	return archiver_->extract(inex);
}

const BaseContainer* AdapterChain::doExtract(const BaseContainer& from_data) const
{
	if (converters_.empty())	throw MethodNotApplicableException("converters are needed but are not specified");

	unique_ptr<const BaseContainer> data(&from_data);
	
	for (const auto& converter : converters_)
	{
        if (converter->getArchiveContainerType() != data->getType())	error("Adapters are incompatible");
        data.reset(converter->extract(*data));
	}

	return data.release();
}

void AdapterChain::doExtractToFile(const BaseContainer& data, std::string to_file) const
{
	if (!writer_)	throw MethodNotApplicableException("a writer is needed but is not specified");

	if (writer_->getContainerType() != data.getType())	error("Adapters are incompatible");
	const OutputFileInfo outinfo = {to_file, file_extension_};
    writer_->write(data, outinfo);
}

const AdapterChain::ConverterList AdapterChain::translateConverterids(const AdapterChain::ConverteridList &&ids)
{
    ConverterList   ret;
    ret.reserve(ids.size());
    for (const auto id : ids)
    {
        ret.push_back(&global.converters.get(id));
    }
    return ret;
}

/*void AdapterChain::archive(std::string from_file, const BaseNode& to_node) const;
void AdapterChain::archive(const BaseContainer& from, const BaseNode& to_node) const;
BaseContainer* AdapterChain::archive(std::string from_file) const;
BaseContainer* AdapterChain::archive(const BaseContainer& from) const;*/

/*std::ostream& operator << (std::ostream& stream, const AdapterChain& chain)
{
	auto oldflags = stream.setf(ostream::hex, ostream::basefield);
	stream << "{";
	stream << chain.archiver.space << ":" << path.archiver.id << " ";
	for (auto it = path.converters.begin(); it != path.converters.end(); it++)
	{
		stream << it->space << ":" << it->id << " ";
	}
	stream << path.writer.space << ":" << path.writer.id;
	stream << "}";
	stream.setf(oldflags);
	return stream;
}

ostream& operator << (ostream& stream, const DefaultConverters::ConvertPath& path)
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
}*/

	}
}
