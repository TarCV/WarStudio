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

#include "ChainRegistry.h"
#include "Globals.h"

using namespace std;

namespace warstudio {
    namespace model {

void ChainRegistry::doInit() const
{
    registerPath(LUMP_TYPE::RAW, BUILTIN_ARCHIVER::DUMMY, BUILTIN_WRITER::RAW, "dat");

    {
        const ConverterId	palconv[] = {ConverterId(SPACE_WARSTUDIO, CONVERTER_TYPE::PALETTE_TO_IMAGE)};
        registerPath(LUMP_TYPE::DOOMPLAYPAL, BUILTIN_ARCHIVER::DOOM_PLAYPAL, AdapterChain::ConverteridList(begin(palconv), end(palconv)), BUILTIN_WRITER::IMAGE, "png");
    }

    {
        const ConverterId	mapconv[] = {ConverterId(SPACE_WARSTUDIO, CONVERTER_TYPE::COLORMAP_TO_IMAGE)};
        registerPath(LUMP_TYPE::DOOMCOLORMAP, BUILTIN_ARCHIVER::DOOM_COLORMAP, AdapterChain::ConverteridList(begin(mapconv), end(mapconv)), BUILTIN_WRITER::IMAGE, "png");
    }

    registerPath(LUMP_TYPE::DOOMDMXGUS, BUILTIN_ARCHIVER::DUMMY, BUILTIN_WRITER::RAW, "ini");

    registerPath(LUMP_TYPE::DOOMPATCH, BUILTIN_ARCHIVER::DOOM_PATCH, BUILTIN_WRITER::IMAGE, "png");
    registerPath(LUMP_TYPE::DOOMFLAT, BUILTIN_ARCHIVER::DOOM_FLAT, BUILTIN_WRITER::IMAGE, "png");
}

void ChainRegistry::registerPath(LUMP_TYPE type, BUILTIN_ARCHIVER archiver, BUILTIN_WRITER writer, string file_extension) const
{
    registerPath(type, archiver, AdapterChain::ConverteridList(), writer, file_extension);
}
void ChainRegistry::registerPath(LUMP_TYPE type, BUILTIN_ARCHIVER archiver, const AdapterChain::ConverteridList&& converters, BUILTIN_WRITER writer, string file_extension) const
{
    const ArchiverId	archiver_id(SPACE_WARSTUDIO, archiver);
    assert(global.archivers.isKnown(archiver_id));

#ifndef NDEBUG
    for (const auto converter_id : converters)
    {
        assert(global.converters.isKnown(converter_id));
    }
#endif

    const WriterId	writer_id(SPACE_WARSTUDIO, writer);
    assert(global.writers.isKnown(writer_id));
    add(type, new AdapterChain(archiver, std::move(converters), writer, file_extension));
}


    }
}
