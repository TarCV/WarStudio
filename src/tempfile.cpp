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

#include "tempfile.h"
#include "utility.h"
#include "filesystem.h"

using namespace std;

namespace warstudio {

TempFile::TempFile()
{
}
TempFile::TempFile(string path) :
	path_(path)
{
    if (!exists(path) || is_directory(path))
	{
		error("no such file");
	}
}
TempFile::~TempFile()
{
	Delete();
}

void TempFile::Delete()
{
	if (path_.size())
	{
		remove(path_.c_str());
		path_ = "";
	}
}
void TempFile::CreateNew()
{
	Delete();

//TODO: check returns values
#ifdef WINVER
	char *tempfile = _tempnam("temp", "wad");
	path_ = tempfile;
	free(tempfile);
#else
    char tempfile[] = "wadXXXXXXXX";
    int fd = mkstemp(tempfile);
    close(fd);
    path_ = tempfile;
#endif
}
string TempFile::Path() const	
{
	assert(path_.size());
	return path_;
}

}
