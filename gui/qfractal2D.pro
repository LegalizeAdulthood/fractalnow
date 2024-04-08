TEMPLATE = app
TARGET = qfractal2D
OBJECTS_DIR = objs/
MOC_DIR = mocs/
FRACTALLIBPATH = ../lib
DEPENDPATH += . include src
INCLUDEPATH += . include $$FRACTALLIBPATH/include
CONFIG += staticlib
LIBS += -L$$FRACTALLIBPATH -lfractal2D
QMAKE_CXXFLAGS += -D__STDC_FORMAT_MACROS

# Input
HEADERS += include/help.h include/command_line.h include/image_label.h include/main.h include/main_window.h
SOURCES += src/help.cpp src/command_line.cpp src/image_label.cpp src/main.cpp src/main_window.cpp
