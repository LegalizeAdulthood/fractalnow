TEMPLATE = app
TARGET = qfractal2D
OBJECTS_DIR = objs/
MOC_DIR = mocs/
FRACTALLIBPATH = ../lib
DEPENDPATH += . include src
INCLUDEPATH += . include $$FRACTALLIBPATH/include
LIBS += -L$$FRACTALLIBPATH -lfractal2D
DEFINES += __STDC_FORMAT_MACROS
!isEmpty(FLOAT_PRECISION) {
DEFINES += FLOAT_PRECISION=$$FLOAT_PRECISION
}
CONFIG += qt warn_on 
!isEmpty(DEBUG) {
	CONFIG += debug
}

# Input
HEADERS += include/color_button.h include/fractal_rendering_widget.h include/fractal_config_widget.h include/help.h include/command_line.h include/fractal_image_label.h include/main.h include/main_window.h
SOURCES += src/color_button.cpp src/fractal_rendering_widget.cpp src/fractal_config_widget.cpp src/help.cpp src/command_line.cpp src/fractal_image_label.cpp src/main.cpp src/main_window.cpp
