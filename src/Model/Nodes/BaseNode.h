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

#include "../lumptypes.h"
#include <string>

namespace warstudio {
	namespace model {

class DirectoryNode;

class BaseNode {
	BaseNode* parent_;
	std::string name_;
protected:
	virtual bool parentfilter() const throw();
	DirectoryNode* directory;		//TODO: used to represent the file (e.g. a wad file) this lump contained in (this may be the same or not as package field)
public:
    BaseNode(std::string named, DirectoryNode* withindir, BaseNode* withparent) : parent_(withparent), name_(named), directory(withindir) {}
	virtual LUMP_TYPE type() const = 0;
	virtual int size() const = 0;

	virtual std::istream& ReadableData() const = 0;
	virtual std::ostream& WritableData(int maxsize) = 0;
	virtual void Stored() = 0;

	const DirectoryNode* getDirectory() const {return directory;}

	const BaseNode* getParent() const {return parent_;}
	void setParent(BaseNode *newparent) {
		//TODO: call some child method
		parent_ = newparent;
	}

	//parent is another lump, not the archive itself
	bool hasInarchiveParent() const {return parent_ != 0 && parentfilter();}

	/*
	TODO:
	RealNodePath getRealPath() const;
	VirtualNodePath getVirtualPath() const;
	RealNodeName getRealName() const;	//replaces std::string name() const
	void setRealName(RealNodeName newname);		//replaces name(std::string)
	VirtualNodeName getVirtualName() const;
	*/

	std::string getName() const {return name_;}
	void setName(std::string newname);

	virtual ~BaseNode() {}
};

	}
}
