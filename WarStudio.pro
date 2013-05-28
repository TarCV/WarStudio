#-------------------------------------------------
#
# Project created by QtCreator 2013-03-19T15:57:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WarStudio
TEMPLATE = app

INCLUDEPATH += /usr/include/ImageMagick
LIBS +=\
    /usr/lib/libboost_system.so.1.48.0 \
    /usr/lib/libboost_filesystem.so.1.48.0 \
    /usr/lib/libMagick++.so.4
PRECOMPILED_HEADER = stdafx.h
QMAKE_CXXFLAGS += -std=c++11

SOURCES +=\
    src/windowstream.cpp \
    src/utility.cpp \
    src/tempfile.cpp \
    src/StreamBuffer.cpp \
    src/stdafx.cpp \
    src/Model/Globals.cpp \
    src/Model/defaultconverters.cpp \
    src/Model/AdapterChain.cpp \
    src/Model/Archivers/BaseArchiver.cpp \
    src/Model/Archivers/ArchiverRegistry.cpp \
    src/Model/Archivers/Doom/PLAYPAL.cpp \
    src/Model/Archivers/Doom/PatchArchiver.cpp \
    src/Model/Archivers/Doom/FlatArchiver.cpp \
    src/Model/Archivers/Doom/ColormapArchiver.cpp \
    src/Model/Archivers/Generic/DummyArchiver.cpp \
    src/Model/Containers/BaseContainer.cpp \
    src/Model/Converters/PalToImgConverter.cpp \
    src/Model/Converters/ConverterRegistry.cpp \
    src/Model/Converters/ColormapToImgConverter.cpp \
    src/Model/Converters/BaseConverter.cpp \
    src/Model/Graphic/Image.cpp \
    src/Model/Graphic/Color.cpp \
    src/Model/Nodes/directorynode.cpp \
    src/Model/Nodes/BaseNode.cpp \
    src/Model/Nodes/Doom/wadlumptyper.cpp \
    src/Model/Nodes/Doom/wadlump.cpp \
    src/Model/Nodes/Doom/wad.cpp \
    src/Model/Writers/WriterRegistry.cpp \
    src/Model/Writers/Raw.cpp \
    src/Model/Writers/ImageWriter.cpp \
    src/Model/Writers/BaseWriter.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/Model/ChainRegistry.cpp

HEADERS  += \
    src/windowstream.h \
    src/utility.h \
    src/tempfile.h \
    src/StreamBuffer.h \
    src/stdafx.h \
    src/exceptions.h \
    src/config.h \
    src/Model/RegistryTemplate.h \
    src/Model/lumptypes.h \
    src/Model/lumptyper.h \
    src/Model/Globals.h \
    src/Model/defaultconverters.h \
    src/Model/Context.h \
    src/Model/constants.h \
    src/Model/AdapterChain.h \
    src/Model/Archivers/BaseArchiver.h \
    src/Model/Archivers/ArchiverRegistry.h \
    src/Model/Archivers/Doom/PLAYPAL.h \
    src/Model/Archivers/Doom/PatchArchiver.h \
    src/Model/Archivers/Doom/FlatArchiver.h \
    src/Model/Archivers/Doom/ColormapArchiver.h \
    src/Model/Archivers/Generic/DummyArchiver.h \
    src/Model/Containers/StreamBufferContainer.h \
    src/Model/Containers/PaletteContainer.h \
    src/Model/Containers/ImageContainer.h \
    src/Model/Containers/id.h \
    src/Model/Containers/ColormapContainer.h \
    src/Model/Containers/BaseContainer.h \
    src/Model/Converters/PalToImgConverter.h \
    src/Model/Converters/ConverterRegistry.h \
    src/Model/Converters/ColormapToImgConverter.h \
    src/Model/Converters/BaseConverter.h \
    src/Model/Graphic/Palette.h \
    src/Model/Graphic/Image.h \
    src/Model/Graphic/Color.h \
    src/Model/Nodes/BaseNode.h \
    src/Model/Nodes/Doom/wadlumptyper.h \
    src/Model/Nodes/Doom/wadlump.h \
    src/Model/Nodes/Doom/wad.h \
    src/Model/Writers/WriterRegistry.h \
    src/Model/Writers/Raw.h \
    src/Model/Writers/ImageWriter.h \
    src/Model/Writers/BaseWriter.h \
    src/mainwindow.h \
    src/magick.h \
    src/Model/Nodes/DirectoryNode.h \
    src/filesystem.h \
    src/Model/ChainRegistry.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    src/todo.txt
