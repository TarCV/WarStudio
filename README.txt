WadDirectory/WarStudio

The project goal is to create a cross-platform wad file editor (wad files are data files used in Doom engine based games and ports) which will allow opening and editing lumps directly in external applications. I believe that is the way a lump manager should be implemented because built-in editors can never be as powerful as existing stand-alone text processors, script editors, graphic editors, etc. 

Within the frameworks of the project there will be developed two programs: WadDirectory and WarStudio.
WadDirectory will be a console program for creating and editing wad files, and to some extent it will be similar to DeuTEX. Its job will be to convert between conventional file formats and formats used in Doom, and, of course, to archive and extract lumps to/from wad files.
Sometime later WadDirectory will be the basis for WarStudio.
WarStudio will be a full-featured GUI lump manager. Its interface will be implemented using Qt library.

---

Technical details

The project is being written in subset of C++11 supported by MS Visual Studio Express 2012.
Currently the project has the following dependencies:
- Boost 1.51
- GraphicMagick 1.3.17 Q16
- Filesystem from TR2 (VS includes it as a part of the default SDK)
All the libraries are built and linked as multi-threaded dlls.
