DIRS = lib command-line gui

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all: 
	$(call quiet-command, for i in ${DIRS}; do make -C "$${i}" $@; done,"")
	ln -s -f command-line/fractal2D fractal2D
	ln -s -f gui/qfractal2D qfractal2D

%:
	$(call quiet-command, for i in ${DIRS}; do make -C "$${i}" $@; done,"")
	rm fractal2D
	rm qfractal2D

