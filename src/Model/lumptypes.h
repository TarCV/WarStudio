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

#include <stdint.h>

namespace warstudio {
	namespace model {

enum class LUMP_TYPE : uint32_t {	//will be available to external modules in the future, so must have some fixed size
	UNKNOWN,
	MARKER,
	RAW,
//todo:	TEXT,

	PACKAGE,	//types of the edited package

	DOOMPLAYPAL,
	DOOMCOLORMAP,
	DOOMENDOOM,
	DOOMDEMO,
	DOOMTEXTURE,
	DOOMPNAMES,
	DOOMGENMIDI,
	DOOMDMXGUS,
	DOOMSPEAKERFX,
	DOOMSFX,
	DOOMMIDI,
	DOOMPATCH,
	DOOMFLAT,

	DOOMMAP,
	DOOMTHINGS,
	DOOMLINEDEFS,
	DOOMSIDEDEFS,
	DOOMVERTEXES,
//todo:	DOOMSEGS,
//todo:	DOOMSSECTORS,
//todo: DOOMNODES,
	DOOMSECTORS,
//todo: DOOMBLOCKMAP,
	DOOMREJECT,
	DOOMMAPRAW,

	ZDOOMMAP,
	ZDOOMLINEDEFS,
	ZDOOMSECTORS,
	ZDOOMTHINGS,
	ZDOOMBEHAVIOR,
	ZDOOMACS,

	DOOMUDMFHEAD,
	DOOMENDMAP,

/*
todo:
	DOOMANIMATED,
	DOOMDEHACKED,
	DOOMGENMIDI,
	DOOMSWITCHES,
	
	DOOMDMXGUS,
	DLEGACYFSGLOBAL,

	GZDOOMGLDEFS,
	GZDOOMMODELDEF,

	HERETICLOADING,
	HEXENSTARTUP,
	HEXENNOTCH,	//NOTCH or NETNOTCH
	STRIFESTARTUP0,
	STRIFESTARTUPPATCH,	//STRTBOT, STRTPA1, STRTPB1, STRTPC1, STRTPD1, STRTLZ1, or STRTLZ2

	ZDOOMALTHUD,
	ZDOOMANIMDEFS,
	ZDOOMDECALDEF,
	ZDOOMDECORATE,
	ZDOOMDEHSUPP,	//deprecated in ZDoom, actual in ZDaemon
	ZDOOMDIALOGUE,
	ZDOOMFONTDEFS,
	ZDOOMGAMEINFO,
	ZDOOMKEYCONF,
	ZDOOMLANGUAGE,
	ZDOOMLOADACS,
	ZDOOMLOCKDEFS,
	ZDOOMMAPINFO
	ZDOOMMENUDEF,
	ZDOOMMUSINFO,
	ZDOOMPALVERS,
	ZDOOMREVERBS,
	ZDOOMSKIN,
	ZDOOMSBARINFO,
	ZDOOMSECRETS,
	ZDOOMSNDCURVE,
	ZDOOMSNDINFO,
	ZDOOMSNDSEQ,
	ZDOOMTEAMINFO,
	ZDOOMTERRAIN,
	ZDOOMTEXTCOLO,
	ZDOOMTEXTURES,
	ZDOOMVOXELDEF,
	ZDOOMXHAIRS,
	ZDOOMX11R6RGB,
	ZDOOMINTERSCRIPT,
	ZDOOMMAPARROW,
	ZDOOMXLAT,
*/
	};

	}
}