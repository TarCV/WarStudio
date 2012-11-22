WadDirectory/WarStudio

This project goal is to create wad lump manager for classic game Doom. It will
rely on external applications to edit lumps. Therefore the main job of the manager
is to convert lumps to formats suitable for edit in stand-alone applications.

WadDirectory is the name of command-line wad archiver and it will be similar to DeuTEX.

WarStudio is a code name for GUI version of this editor. Probably, in some
distant future, WarStudio will support other games.
The WarStudio core will share code with WadDirectory.

---

Currently WadDirectory/WarStudio have the following dependencies:
- Boost library
- GraphicMagick library (currently I work with Q8 debug build)