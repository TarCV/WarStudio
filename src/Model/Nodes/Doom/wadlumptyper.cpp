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

#include "../../../stdafx.h"

#include "wadlumptyper.h"
#include "wad.h"
#include "../../lumptypes.h"
#include "../../../utility.h"

#include <assert.h>

namespace warstudio {
    namespace model {
        namespace doom {

wadlumptyper::wltdvctr wadlumptyper::typelist;
const std::string wadlumptyper::_MAP = "______MAP";
const std::string wadlumptyper::_ZMAP = "______ZMAP";

void wadlumptyper::add(std::string parent, std::string name, LUMP_TYPE type) {
    typelist.push_back(wadlumptypedata(parent, name, type));
}
bool wadlumptyper::checkMapSublumpNum(const wadlump &lump, int &i, bool textmap) {
    if (typelist.size() == 0)  init();

    //todo: support mangled and nonstandard names of the lumps
    /*  todo:
    if (ishexen ? !HexenThings::fastsanecheck(*++it) : !DoomThings::fastsanecheck(*++it)) return;
    if (ishexen ? !HexenLinedefs::fastsanecheck(*++it) : !DoomLinedefs::fastsanecheck(*++it)) return;
    if (!DoomSidedefs::fastsanecheck(*++it)) return;
    if (ishexen ? !HexenSectors::fastsanecheck(*++it) : !DoomSectors::fastsanecheck(*++it)) return;*/
    std::string name = lump.getName();
    if (!textmap) {
        static const checkstruct check[] = { /* directly (with one small change) from zdoom source */
            {"THINGS",	 true},
            {"LINEDEFS", true},
            {"SIDEDEFS", true},
            {"VERTEXES", true},
            {"SEGS",	 false},
            {"SSECTORS", false},
            {"NODES",	 false},
            {"SECTORS",	 true},
            {"REJECT",	 false},
            {"BLOCKMAP", false},
            {"BEHAVIOR", false},
            {"SCRIPTS",	 false},
        };
        static const int szcheck = sizeof(check) / sizeof(check[0]);
        int j = 0;
        for (; j < szcheck; ++j) {
            if (name == check[j].lumpname)	break;
            if (j >= i && check[j].required)	warning("Lump \"" + check[j].lumpname + "\" is required but not present");
        }
        if (j == szcheck)	return false;
        if (j < i)
            warning("\"" + name + "\" is probably placed out of order");
        else if (j == i - 1)
            warning("\"" + name + "\" is probably a duplicate");
        else
            i = j;
        return true;
    } else {
        static const std::string checkudmf[] = {
            "ZNODES"
            "BLOCKMAP"
            "REJECT"
            "DIALOGUE"
            "BEHAVIOR"
            "ENDMAP"
        };
        static const int szcheckudmf = sizeof(checkudmf) / sizeof(checkudmf[0]);
        for (i = 0; i < szcheckudmf; ++i) {
            if (checkudmf[i] == name)	return true;
        }
        return false;
    }
    assert(false);	//should be never executed
}
void wadlumptyper::init() {
    assert(typelist.size() == 0);

    add("", "PLAYPAL", LUMP_TYPE::DOOMPLAYPAL);
    add("", "COLORMAP", LUMP_TYPE::DOOMCOLORMAP);
    add("", "ENDOOM", LUMP_TYPE::DOOMENDOOM);       //todo: implement adapters for these formats
    add("", "DEMO#", LUMP_TYPE::DOOMDEMO);          //todo
    add(_MAP, "THINGS", LUMP_TYPE::DOOMTHINGS);     //todo
    add(_MAP, "LINEDEFS", LUMP_TYPE::DOOMLINEDEFS); //todo
    add(_MAP, "SIDEDEFS", LUMP_TYPE::DOOMSIDEDEFS); //todo
    add(_MAP, "VERTEXES", LUMP_TYPE::DOOMVERTEXES); //todo
    add(_MAP, "SEGS", LUMP_TYPE::DOOMMAPRAW);	    //todo
    add(_MAP, "SSECTORS", LUMP_TYPE::DOOMMAPRAW);   //todo
    add(_MAP, "NODES", LUMP_TYPE::DOOMMAPRAW);      //todo
    add(_MAP, "SECTORS", LUMP_TYPE::DOOMSECTORS);   //todo
    add(_MAP, "REJECT", LUMP_TYPE::DOOMREJECT);     //todo
    add(_MAP, "BLOCKMAP", LUMP_TYPE::DOOMMAPRAW);   //todo
    add("", "TEXTURE#", LUMP_TYPE::DOOMTEXTURE);    //todo
    add("", "PNAMES", LUMP_TYPE::DOOMPNAMES);       //todo
    add("", "GENMIDI", LUMP_TYPE::DOOMGENMIDI);     //todo
    add("", "DMXGUS", LUMP_TYPE::DOOMDMXGUS);
    add("", "DP*", LUMP_TYPE::DOOMSPEAKERFX);       //todo
    add("", "DS*", LUMP_TYPE::DOOMSFX);             //todo
    add("", "D_*", LUMP_TYPE::DOOMMIDI);            //todo
    add("S_START","*", LUMP_TYPE::DOOMPATCH);
    add("P#_START","*", LUMP_TYPE::DOOMPATCH);
    add("F#_START","*", LUMP_TYPE::DOOMFLAT);

    add(_ZMAP, "THINGS", LUMP_TYPE::ZDOOMTHINGS);     //todo
    add(_ZMAP, "LINEDEFS", LUMP_TYPE::ZDOOMLINEDEFS); //todo
    add(_ZMAP, "SIDEDEFS", LUMP_TYPE::DOOMSIDEDEFS);  //todo
    add(_ZMAP, "VERTEXES", LUMP_TYPE::DOOMVERTEXES);  //todo
    add(_ZMAP, "SEGS", LUMP_TYPE::DOOMMAPRAW);        //todo
    add(_ZMAP, "SSECTORS", LUMP_TYPE::DOOMMAPRAW);    //todo
    add(_ZMAP, "NODES", LUMP_TYPE::DOOMMAPRAW);       //todo
    add(_ZMAP, "SECTORS", LUMP_TYPE::ZDOOMSECTORS);   //todo
    add(_ZMAP, "REJECT", LUMP_TYPE::DOOMREJECT);      //todo
    add(_ZMAP, "BLOCKMAP", LUMP_TYPE::DOOMMAPRAW);    //todo
    add(_ZMAP, "BEHAVIOR", LUMP_TYPE::ZDOOMBEHAVIOR); //todo
    add(_ZMAP, "SCRIPTS", LUMP_TYPE::ZDOOMACS);       //todo

    add("","*", LUMP_TYPE::DOOMPATCH);	//unknown lumps in the main namespace are usually patches

    add("*","*", LUMP_TYPE::RAW);	//everything else should be read as raw data
}
LUMP_TYPE wadlumptyper::typeof(const wadlump &lump) {
    if (typelist.size() == 0)  init();
    for(const auto &it : typelist) {
        if (lump.hasInarchiveParent()) {
            std::string parent;
            if (lump.getParent()->type() == LUMP_TYPE::DOOMMAP)	//it's ok, we call type() for the parent of the lump, not for the lump itself
                parent = _MAP;
            else if (lump.getParent()->type() == LUMP_TYPE::ZDOOMMAP)
                parent = _ZMAP;
            else
                parent = lump.getParent()->getName();
            if (!strlike(parent, it.parentpattern)) continue;
        } else {
            if (it.parentpattern != "") continue;
        }
        if (!strlike(lump.getName(), it.namepattern)) continue;
        return it.type;
    }
    return LUMP_TYPE::UNKNOWN;
}

        }
    }
}
