LIB_DIR = lib
BIN_DIR = bin
COMMAND_LINE_DIR = command-line
COMMAND_LINE_BINARY = fractal2D
GUI_DIR = gui
GUI_BINARY = qfractal2D
QMAKEFLAG = 
ifdef FLOAT_PRECISION
QMAKEFLAG += "FLOAT_PRECISION=$(FLOAT_PRECISION)"
endif
ifdef DEBUG
QMAKEFLAG += "DEBUG=$(DEBUG)"
endif

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all: $(BIN_DIR)
	$(call quiet-command,make -C ${LIB_DIR} all,"")
	$(call quiet-command,make -C ${COMMAND_LINE_DIR} all,"")
	$(call quiet-command,cd ${GUI_DIR} && qmake ${QMAKEFLAG},"  GENERATING GUI MAKEFILE")
	$(call quiet-command,make -C ${GUI_DIR} all,"")
	cp ${COMMAND_LINE_DIR}/${BIN_DIR}/${COMMAND_LINE_BINARY} ${BIN_DIR}/${COMMAND_LINE_BINARY}
	cp ${GUI_DIR}/${BIN_DIR}/${GUI_BINARY} ${BIN_DIR}/${GUI_BINARY}

$(BIN_DIR):
	$(call quiet-command, mkdir -p $(BIN_DIR),)

clean:
	$(call quiet-command,make -C ${LIB_DIR} clean,"")
	$(call quiet-command,make -C ${COMMAND_LINE_DIR} clean,"")
	$(call quiet-command,cd ${GUI_DIR} && qmake ${QMAKEFLAG},"  GENERATING GUI MAKEFILE")
	$(call quiet-command,make -C ${GUI_DIR} clean,"")

distclean:
	$(call quiet-command,make -C ${LIB_DIR} distclean,"")
	$(call quiet-command,make -C ${COMMAND_LINE_DIR} distclean,"")
	$(call quiet-command,cd ${GUI_DIR} && qmake ${QMAKEFLAG},"  GENERATING GUI MAKEFILE")
	$(call quiet-command,make -C ${GUI_DIR} distclean,"")
	rm -rf ${BIN_DIR}

%:
	$(call quiet-command,make -C ${LIB_DIR} $@,"")
	$(call quiet-command,make -C ${COMMAND_LINE_DIR} $@,"")
	$(call quiet-command,cd ${GUI_DIR} && qmake ${QMAKEFLAG},"  GENERATING GUI MAKEFILE")
	$(call quiet-command,make -C ${GUI_DIR} $@,"")
