TEMPLATE = app
TARGET = qfractal2D
RESOURCES = qfractal2D.qrc
RCC_DIR = rcc/
OBJECTS_DIR = objs/
MOC_DIR = mocs/
DESTDIR = bin/

FRACTALLIBPATH = ../lib
DEPENDPATH += . include src
INCLUDEPATH += . include $$FRACTALLIBPATH/include
LIBS += -L$$FRACTALLIBPATH/bin -lfractal2D
DEFINES += __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS
CONFIG += warn_on qt thread

ENV_FLOAT_PRECISION = $$(FLOAT_PRECISION)
!isEmpty(ENV_FLOAT_PRECISION) {
DEFINES += FLOAT_PRECISION=$$ENV_FLOAT_PRECISION
}

ENV_DEBUG = $$(DEBUG)
!isEmpty(ENV_DEBUG) {
	DEFINES += DEBUG=$$ENV_DEBUG
	CONFIG += debug console
} else {
	CONFIG += release console
}

# Input
HEADERS += include/gradient_editor.h include/shade_widget.h include/gradient_dialog.h include/gradient_label.h include/gradient_box.h include/hoverpoints.h include/action_progress_dialog.h include/export_fractal_image_dialog.h include/color_button.h include/fractal_rendering_widget.h include/fractal_config_widget.h include/help.h include/command_line.h include/fractal_explorer.h include/main.h include/main_window.h
SOURCES += src/gradient_editor.cpp src/shade_widget.cpp src/gradient_dialog.cpp src/gradient_label.cpp src/gradient_box.cpp src/hoverpoints.cpp src/action_progress_dialog.cpp src/export_fractal_image_dialog.cpp src/color_button.cpp src/fractal_rendering_widget.cpp src/fractal_config_widget.cpp src/help.cpp src/command_line.cpp src/fractal_explorer.cpp src/main.cpp src/main_window.cpp
