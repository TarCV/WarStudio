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

#include "wad.h"

#include "wadlumptyper.h"
#include "../../Context.h"
#include "../../defaultconverters.h"
#include "../../lumptypes.h"
#include "../../Containers/BaseContainer.h"
#include "../../Containers/PaletteContainer.h"
#include "../../Archivers/Doom/PLAYPAL.h"
#include "../../Globals.h"
#include "../../../utility.h"
#include "../../../windowstream.h"
#include "../../../filesystem.h"

#include <stdint.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <assert.h>
//#include <direct.h>   //not crossplatform
#include <memory>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

namespace warstudio {
	namespace model {
		namespace doom {

iwindowstream* wad::CreateStreamFor(const wadlump &lump) const
{
	assert(lump.directory == this);
	assert(lump.offset >= 12);	//12 bytes is the header length
	return new iwindowstream(*waddata.rdbuf(), lump.offset, lump.size());
	//caller required to be owner
}
void wad::checkmap(LumpList::iterator it)
{
	//todo: check if strife maps are supported
	bool ishexen = false;

	//todo: implement fragglescript support
	int k = 0;
	auto j(it); ++j;
	for (;
		j != lumps.end();
		++j, ++k) {
			if (!wadlumptyper::checkMapSublumpNum(*j, k, false))	break;
			if (j->getName() == "BEHAVIOR")	ishexen = true;
			j->setParent(&*it);
	}
	it->type(ishexen ? LUMP_TYPE::ZDOOMMAP : LUMP_TYPE::DOOMMAP);
}
void wad::checkudmf(LumpList::iterator it)
{
	{
		auto j(it);
		++j;
		if (j->getName() != "TEXTMAP")	return;
		//todo: if(!DoomUDMF::fastsanecheck(*j)) warning;
	}
	it->type(LUMP_TYPE::DOOMUDMFHEAD);
	int k = 0;
	auto j(it); ++j;
	for (; j->getName() != "ENDMAP" && j != lumps.end(); ++j, ++k) {
		if (!wadlumptyper::checkMapSublumpNum(*j, k, true))	break;
		j->setParent(&*it);
	}
}
wadlump& wad::getLump(std::string parent, std::string name)
{
	auto it = lumps.rbegin();
	for (; it != lumps.rend(); ++it) {
		if (parent != "") {
			if (!it->hasInarchiveParent())	continue;
			if (it->getParent()->getName() != parent)	continue;
		} else {
			if (it->hasInarchiveParent())	continue;
		}
		if (it->getName() != name)	continue;
		return *it;
	}
	error("lump not found");
	throw false;	//workaround to silence warning
}
void wad::loadPalette(const wadlump& lump, const BaseNode* becauseof)
{
	const PlaypalArchiver& decoder = dynamic_cast<const PlaypalArchiver &>(global.archivers.get(ArchiverId(SPACE_WARSTUDIO, BUILTIN_ARCHIVER::DOOM_PLAYPAL)));
	InputLumpData in = {lump.ReadableData(), lump.size()};
	unique_ptr<const BaseContainer> data(decoder.extract(in));
	DirectoryNode::loadPalette(lump.getName(), dynamic_cast<const PaletteContainer*>(data.get())->palset[0], becauseof);
}
void wad::updatePaletteLump(std::string name, const Palette& pal, const BaseNode& becauseof)
{
	wadlump& lump = getLump("", name);

    const PlaypalArchiver& archiver = dynamic_cast<const PlaypalArchiver &>(global.archivers.get(ArchiverId(SPACE_WARSTUDIO, BUILTIN_ARCHIVER::DOOM_PLAYPAL)));

    InputLumpData in = {lump.ReadableData(), lump.size()};

    unique_ptr<const PaletteContainer> data;
    unique_ptr_castmove(archiver.extract(in), data);
    assert(data);

    PaletteContainer newdata(data->palset.size());

    newdata.palset = data->palset;
	newdata.palset[0] = pal;
	OutputLumpData out = {lump.WritableData(archiver.estimateSize(newdata))};
	archiver.archive(newdata, Context(), out);
	warning("Only the first palette in the set was changed. You need to update another palettes in the set manually");
	//todo: update dependent lumps (excluding becauseof lump)
}

wad::wad(string file, bool truncate, DirectoryNode* withindirectory, BaseNode* withparent) :
	DirectoryNode(file, withindirectory, withparent)
{
	if (truncate)
	{
		waddata.open(file, ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc);
		wadtype = "PWAD";
		return;
	}

    assert(exists<path>(file) && !is_directory<path>(file));
	waddata.open(file, ios::in | ios::out | ios::binary);

	uint32_t diroffset, numlumps;
	waddata.exceptions(ios::eofbit | ios::failbit | ios::badbit );
	char typec[4];	waddata.read(typec, 4);	wadtype.assign(typec, 4);
	waddata.read(reinterpret_cast<char *>(&numlumps), 4);
	waddata.read(reinterpret_cast<char *>(&diroffset), 4);
	if (!strlike(wadtype, "?WAD")) error("Incorrect or damaged WAD file");
	vector<string> blocknames; blocknames.reserve(5);
	vector<wadlump*> blocklumps; blocklumps.reserve(5);
	wadlump* parent = 0;
	waddata.seekg(diroffset);
	for (uint32_t i = 0; i < numlumps; ++i)
	{
		uint32_t offset, size;
		waddata.read(reinterpret_cast<char *>(&offset), 4);
		waddata.read(reinterpret_cast<char *>(&size), 4);

		char namec[9] = "NAMENAME";	// this buf has \0 character in the end 
		string name; 
		waddata.read(namec, 8);
		name.assign(namec);

		wadlump& lump = AppendLump(parent, name, offset, size);
		if (size != 0)
		{
			if ("PLAYPAL" == name) //todo: move to virtual method to support other id tech 1 based games
			{
				loadPalette(lump, nullptr);
			}
		}
		else //is a marker
		{

			lumps.back().type(LUMP_TYPE::MARKER);
			if (endsWith(name, "_START"))
			{
				parent = &lumps.back();	//we MUST use pointer to the object IN THE VECTOR
				blocknames.push_back(name.substr(0, name.length()-6));
				blocklumps.push_back(parent);
			}
			else if (endsWith(name, "_END"))
			{
				string block = name.substr(0, name.length() - 4);
				if (*blocknames.rbegin() != block)
				{
					warning("Unexpected " + name + " end marker. Probably wad namespace structure is corrupted.");
					vector<string>::const_iterator it;
					for (it = blocknames.begin(); it != blocknames.end(); ++it)
					{
						if (*it == block)	break;
					}
					if (it != blocknames.end())
					{
						while (blocknames.back() != block)
						{
							warning("Recovering: " + blocknames.back() + "_START/" + blocknames.back() + "_END namespace will be considered closed");
							blocknames.pop_back();
							blocklumps.pop_back();
						}
					}
					else
					{
						warning("Matching " + block + "_START wasn't found. Ignoring " + name);
						continue;
					}
				}
				assert(*blocknames.rbegin() == block);
				blocknames.pop_back();
				blocklumps.pop_back();
				parent = blocklumps.size() ? *blocklumps.rbegin() : 0;
				lump.setParent(parent);
			}
		}
	}

	//map lumps cant be found by simple pattern matching, so here is the finder
	auto it(lumps.begin());
	auto next(lumps.cbegin());	++next;
	for (; next != lumps.cend(); ++it, ++next)
	{
		if (strlike(it->getName(), "E#M#") || strlike(it->getName(), "MAP##"))
		{
			checkmap(it);
		}
		else
		{
			if ("THINGS" == next->getName())
			{
				checkmap(it);
			}
			else if ("TEXTMAP" == next->getName())
			{
				checkudmf(it);
			}
		}
	}
}

model::LUMP_TYPE wad::type() const
{
	return LUMP_TYPE::PACKAGE;
}


wadlump& wad::AppendLump(model::BaseNode* parent, std::string name, int offset, int size)
{
	assert(parent == 0 || dynamic_cast<wadlump *>(parent)->directory == this);
	lumps.push_back(new wadlump(*this, parent, name, offset, size));
	return lumps.back();
}
//todo: This method probably will be used only by waddir. Therefore consider moving this out of this class.
void wad::exportAll(string todir)
{
	assert(lumps.size());

	//todo: make this as close as possible to deutex lumplist file
    const path	todirpath(todir);
    ofstream lumplist((todirpath / path("lumps.lst")).file_string(), ofstream::out | ofstream::trunc);	//file_string() returns native form
	lumplist.exceptions(ofstream::failbit);
	const int indexlen = numDigits(lumps.size() - 1);	//lumps.size() should be at least 1
	int i = 0;
	auto it = lumps.cbegin();
	for (; it != lumps.end(); ++it, ++i) {
		updateCurrentOperation("Extracting " + it->getName());
/*		//todo:		string filename = DefaultConverters::converter(it->type())->exporter(todir, it->data, it->offset, it->size);
		lumplist << (it->hasInarchiveParent() ? it->parent()->name() : "\t") << "\t";
		lumplist << it->name() << "\t" << it->type();
		if (DefaultConverters::knowsType(it->type()))	lumplist << "\t" << DefaultConverters::converter(it->type()).fastsanecheck(*it);
		lumplist << endl;*/
		lumplist << it->getName();
		if (it->type() != LUMP_TYPE::MARKER) {
			const string file_name(dynamic_cast<ostringstream &>(ostringstream() << setfill('0') << setw(indexlen) << i << "_" << it->getName()).str());

            //TODO:
            const AdapterChain& chain =
                    global.adapterchains.get(
                        global.adapterchains.isKnown(it->type())
                        ? it->type()
                        : LUMP_TYPE::RAW);

            const string file_ext = chain.getExtension();
            const path	file_path = todirpath / path(file_name).replace_extension(file_ext);

            chain.extract(*it, file_path.file_string());	//file_string() returns native form

            lumplist << "\t" << file_path.file_string() << "\t" << path(file_path);	//TODO: write platform-independent path to the list
		}
		lumplist << endl;
	}
	lumplist.close();
}
void wad::importAll(string listfile)
{
	assert(false);
/*	ifstream lumplist(listfile);

	struct lumpentry {
		string name;
		string fromfile;
	};
	vector<lumpentry>	lumpentries;
	string lumpline;
	while (getline(lumplist, lumpline)) {
		lumpentry	newlumpentry;
		istringstream entrystream(lumpline);
		entrystream >> newlumpentry.name;
		if (!entrystream)	break;
		entrystream >> newlumpentry.fromfile;
		lumpentries.push_back(newlumpentry);
	}
	if (!lumplist.eof())	error("Error reading lumplist");
	
	waddata.seekp(0);
	waddata.write(wadtype.c_str(), 4);
	
	int32_t diroffset(0), numlumps(lumpentries.size());
	waddata.write(reinterpret_cast<char *>(&numlumps), 4);
	waddata.write(reinterpret_cast<char *>(&diroffset), 4);

	for (vector<lumpentry>::iterator it = lumpentries.begin(); it != lumpentries.end(); ++it) {
		wadlump& lump = AppendLump(0, it->name);
		if (it->fromfile.size()) {
			const unique_ptr<const BaseContainer>	dataptr(model::DefaultConverters::Read(it->fromfile));
			model::DefaultConverters::Archive(*dataptr, lump);

			lump.offset = waddata.tellp();
			StreamCopy(lump.ReadableData(), waddata, true);
		}
		lump.Stored();
		if ("PLAYPAL" == it->name) { // we don't move this to Stored() method as sometimes palette is already updated when loaded from adapters chain
			loadPalette(lump, nullptr);
		}
	}
	
	diroffset = waddata.tellp();
	waddata.seekp(8);
	waddata.write(reinterpret_cast<char *>(&diroffset), 4);
	waddata.seekp(diroffset);

	for (lumpvctr::iterator it = lumps.begin(); it != lumps.end(); ++it) {
		uint32_t offset(it->offset), size(it->size());

		char name[8] = {0, 0, 0, 0, 0, 0, 0, 0};	//ensure that name will be filled with trailing nulls
		memcpy(name, it->name().data(), it->name().size());

		waddata.write(reinterpret_cast<char *>(&offset), 4);
		waddata.write(reinterpret_cast<char *>(&size), 4);
		waddata.write(name, 8);
		//it->Saved();
	}*/
}
bool wad::checkName(string newname) const
{
	bool isvalid(true);
	if (newname.size() > 8) {
		warning("Name length should not be greater than 8 characters");
		isvalid = false;
	}
	//todo: check for invalid characters
	return isvalid;
}
string wad::validateName(string newname) const throw()
{
	return boost::to_upper_copy(newname);
}

		}
	}
}
