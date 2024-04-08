SRCDIR    = src
INCLUDEDIR= include
OBJDIR    = objs
PROG      = fractal2D
DEPENDENCY_FILE=${OBJDIR}/makefile.dep

UNAME := $(shell uname)

CC        = gcc
LD        = gcc

FLOAT_PRECISION = 1
CFLAGS    = -std=c99 -pedantic -Wall -Wextra
CFLAGS   += -DFLOAT_PRECISION=$(FLOAT_PRECISION)
#DEBUG     = true
ifdef DEBUG
	CFLAGS += -Werror -O0
else
	CFLAGS += -O2 -ffast-math
endif
CFLAGS    += -I${INCLUDEDIR}

LDFLAGS   = -lm -lpthread

OBJECTS = $(OBJDIR)/main.o \
	$(OBJDIR)/fractal_anti_aliasing.o \
	$(OBJDIR)/fractal_addend_function.o \
	$(OBJDIR)/fractal_coloring.o \
	$(OBJDIR)/fractal_iteration_count.o \
	$(OBJDIR)/fractal_loop.o \
	$(OBJDIR)/fractal_orbit.o \
	$(OBJDIR)/fractal_rendering_parameters.o \
	$(OBJDIR)/fractal_transfer_function.o \
	$(OBJDIR)/color.o \
	$(OBJDIR)/command_line.o \
	$(OBJDIR)/file_parsing.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/fractal.o \
	$(OBJDIR)/gradient.o \
	$(OBJDIR)/help.o \
	$(OBJDIR)/image.o \
	$(OBJDIR)/misc.o \
	$(OBJDIR)/ppm.o \
	$(OBJDIR)/rectangle.o \
	$(OBJDIR)/thread.o

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all : $(OBJDIR) ${DEPENDENCY_FILE} $(PROG)
	
${DEPENDENCY_FILE}: $(OBJDIR) ${SRCDIR}/*.c ${INCLUDEDIR}/*.h
	$(call quiet-command,for i in ${SRCDIR}/*.c; do ${CC} ${CFLAGS} -MM "$${i}"; done | sed "s/\(^.*:\)/${OBJDIR}\/\1/" > $@,"  BUILDING DEPENDENCY DATABASE");
-include ${DEPENDENCY_FILE}

fractal2D: $(OBJECTS)
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

