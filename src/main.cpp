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

#include "mainwindow.h"
#include "utility.h"
#include "Model/Nodes/Doom/wad.h"
#include "magick.h"

#include <QApplication>
#include <string>
//#include <direct.h>
#include <iostream>


int main(int /*argc*/, char *argv[]) {
/*
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
*/

    using namespace warstudio::model::doom;
    using namespace warstudio;

    Magick::InitializeMagick(*argv);

    std::string wadpath(argv[1]);

/*	wad lumps(wadpath + "1", true);
    std::string listfile = wadpath + ".d\\lumps.lst";
    try {
        lumps.importAll(listfile);
    } catch (std::string e) {
        warning(e);
    }//*/
    wad lumps(wadpath);
    std::string todir = wadpath + ".d";
//	_mkdir(todir.c_str());
    try {
        lumps.exportAll(todir);
    } catch (std::string e) {
        warning(e);
    }//*/
    /*using namespace std;
    using namespace Magick;
    Image image;
    try {
    /*	image.read( "Alenushka.psd" );
        size_t nImages = image.subImage();
        for (size_t i = 0; i < nImages; ++i) {
            image.subImage(i);
        }//* /
        std::list<Image> imageList;
        readImages( &imageList, "Test.psd" );
        //imageList.begin()	is always the composed imaged but one should not rely on this
        for (std::list<Image>::iterator it = imageList.begin(); it != imageList.end(); ++it)
        {
            cout << it->label() << endl;
            it->write("Test_" + it->label() + ".png");
        }
    } catch(Exception error_)
    {
      std::cout << "Caught exception: " << error_.what() << std::endl;
    }//*/
}
