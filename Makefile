LIB_DIR = lib
COMMAND_LINE_DIR = command-line
GUI_DIR = gui
ifdef FLOAT_PRECISION
QMAKEFLAG = "FLOAT_PRECISION=$(FLOAT_PRECISION)"
endif

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all:
	$(call quiet-command,make -C ${LIB_DIR} all,"")
	$(call quiet-command,make -C ${COMMAND_LINE_DIR} all,"")
	$(call quiet-command,cd ${GUI_DIR} && qmake ${QMAKEFLAG},"  GENERATING GUI MAKEFILE")
	$(call quiet-command,make -C ${GUI_DIR} all,"")
	ln -s -f ${COMMAND_LINE_DIR}/fractal2D fractal2D
	ln -s -f ${GUI_DIR}/qfractal2D qfractal2D

clean:
	$(call quiet-command,make -C ${LIB_DIR} clean,"")
	$(call quiet-command,make -C ${COMMAND_LINE_DIR} clean,"")
	$(call quiet-command,cd ${GUI_DIR} && qmake ${QMAKEFLAG},"  GENERATING GUI MAKEFILE")
	$(call quiet-command,make -C ${GUI_DIR} clean,"")
	rm -f fractal2D qfractal2D

%:
	$(call quiet-command,make -C ${LIB_DIR} $@,"")
	$(call quiet-command,make -C ${COMMAND_LINE_DIR} $@,"")
	$(call quiet-command,cd ${GUI_DIR} && qmake ${QMAKEFLAG},"  GENERATING GUI MAKEFILE")
	$(call quiet-command,make -C ${GUI_DIR} $@,"")
