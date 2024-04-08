SRCDIR    = src
INCLUDEDIR= include
OBJDIR    = objs
PROG      = fractal2D fractal2D-computer fractal2D-renderer
DEPENDENCY_FILE=${OBJDIR}/makefile.dep

UNAME := $(shell uname)

CC        = gcc
LD        = gcc

FLOAT_PRECISION = 1
CFLAGS    = -std=c99 -pedantic -Wall -Wextra
CFLAGS   += -DFLOAT_PRECISION=$(FLOAT_PRECISION)
#DEBUG     = true
ifdef DEBUG
	CFLAGS += -Werror -O0 -g
else
	CFLAGS += -O2 -ffast-math
endif
CFLAGS    += -I${INCLUDEDIR}

LDFLAGS   = -lm -lpthread

OBJECTS = $(OBJDIR)/fractal2D.o \
	$(OBJDIR)/fractal2D_computer.o \
	$(OBJDIR)/fractal2D_renderer.o \
	$(OBJDIR)/gradient.o \
	$(OBJDIR)/queue.o \
	$(OBJDIR)/ppm.o \
	$(OBJDIR)/rectangle.o \
	$(OBJDIR)/fractal.o \
	$(OBJDIR)/image.o \
	$(OBJDIR)/rendering_parameters.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/color.o \
	$(OBJDIR)/thread.o \
	$(OBJDIR)/float_table.o

FRACTAL2D_OBJECTS = $(OBJDIR)/fractal2D.o \
	$(OBJDIR)/gradient.o \
	$(OBJDIR)/queue.o \
	$(OBJDIR)/ppm.o \
	$(OBJDIR)/rectangle.o \
	$(OBJDIR)/fractal.o \
	$(OBJDIR)/image.o \
	$(OBJDIR)/rendering_parameters.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/color.o \
	$(OBJDIR)/thread.o \
	$(OBJDIR)/float_table.o

FRACTAL2DCOMPUTER_OBJECTS = $(OBJDIR)/fractal2D_computer.o \
	$(OBJDIR)/gradient.o \
	$(OBJDIR)/queue.o \
	$(OBJDIR)/rectangle.o \
	$(OBJDIR)/fractal.o \
	$(OBJDIR)/image.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/color.o \
	$(OBJDIR)/thread.o \
	$(OBJDIR)/float_table.o

FRACTAL2DRENDERER_OBJECTS = $(OBJDIR)/fractal2D_renderer.o \
	$(OBJDIR)/gradient.o \
	$(OBJDIR)/queue.o \
	$(OBJDIR)/ppm.o \
	$(OBJDIR)/rectangle.o \
	$(OBJDIR)/fractal.o \
	$(OBJDIR)/image.o \
	$(OBJDIR)/rendering_parameters.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/color.o \
	$(OBJDIR)/thread.o \
	$(OBJDIR)/float_table.o

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all : $(OBJDIR) ${DEPENDENCY_FILE} $(PROG)
	
${DEPENDENCY_FILE}: $(OBJDIR) ${SRCDIR}/*.c ${INCLUDEDIR}/*.h
	$(call quiet-command,for i in ${SRCDIR}/*.c; do ${CC} ${CFLAGS} -MM "$${i}"; done | sed "s/\(^.*:\)/${OBJDIR}\/\1/" > $@,"  BUILDING DEPENDENCY DATABASE");
-include ${DEPENDENCY_FILE}

fractal2D: $(FRACTAL2D_OBJECTS)
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD     $@")

fractal2D-computer: $(FRACTAL2DCOMPUTER_OBJECTS)
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD     $@")

fractal2D-renderer: $(FRACTAL2DRENDERER_OBJECTS)
	$(call quiet-command, $(LD) $^ $(LDFLAGS) -o $@, "  LD     $@")

$(OBJDIR):
	$(call quiet-command, mkdir -p $(OBJDIR),)

$(OBJDIR)/%.o:${SRCDIR}/%.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

clean:
	$(call quiet-command, rm -f $(PROG) *~, "  CLEAN    (PROG)")
	$(call quiet-command, rm -f ${DEPENDENCY_FILE}, "  CLEAN    (DEPENDENCY DATABASE)")
	$(call quiet-command, rm -f $(OBJECTS) , "  CLEAN    (OBJECTS)")

distclean: clean
	$(call quiet-command, rm -fr $(OBJDIR), "  DISTCLEAN    ")

