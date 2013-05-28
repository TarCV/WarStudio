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

#include "../../stdafx.h"

#include "ArchiverRegistry.h"

#include "Doom/PLAYPAL.h"
#include "Doom/ColormapArchiver.h"
#include "Doom/PatchArchiver.h"
#include "Doom/FlatArchiver.h"
#include "Generic/DummyArchiver.h"

namespace warstudio {
	namespace model {

bool operator< (const ArchiverId &left, const ArchiverId &right)
{
	if (left.space != right.space)	return (left.space < right.space);
	return (left.id < right.id);
}

void ArchiverRegistry::doInit() const
{
    add(ArchiverId(SPACE_WARSTUDIO, BUILTIN_ARCHIVER::DUMMY), new DummyArchiver());
    add(ArchiverId(SPACE_WARSTUDIO, BUILTIN_ARCHIVER::DOOM_PLAYPAL), new doom::PlaypalArchiver(256, 14));
    add(ArchiverId(SPACE_WARSTUDIO, BUILTIN_ARCHIVER::DOOM_COLORMAP), new doom::ColormapArchiver(256, 34, "PLAYPAL"));
    add(ArchiverId(SPACE_WARSTUDIO, BUILTIN_ARCHIVER::DOOM_PATCH), new doom::PatchArchiver());
    add(ArchiverId(SPACE_WARSTUDIO, BUILTIN_ARCHIVER::DOOM_FLAT), new doom::FlatArchiver());
}

	}
}
