SRCDIR    = src
INCLUDEDIR= include
OBJDIR    = objs
BINDIR    = bin
ARCHIVE   = libfractal2D.a
TARGET    = ${BINDIR}/${ARCHIVE}
DEPENDENCY_FILE=${OBJDIR}/makefile.dep
VERSION_NUMBER = 0.7.3

UNAME := $(shell uname)

CC        = gcc
AR        = ar

ARFLAGS = -r -c -s

CFLAGS    = -std=c99 -pedantic -Wall -Wextra
ifdef FLOAT_PRECISION
CFLAGS   += -DFLOAT_PRECISION=${FLOAT_PRECISION}
endif
ifdef VERSION_NUMBER
CFLAGS   += -DVERSION_NUMBER=${VERSION_NUMBER}
endif

ifdef DEBUG
	CFLAGS += -O0 -g -DDEBUG
else
	CFLAGS += -O2 -ffast-math
endif
CFLAGS    += -I${INCLUDEDIR}

OBJECTS = \
	$(OBJDIR)/fractal_addend_function.o \
	$(OBJDIR)/fractal_coloring.o \
	$(OBJDIR)/fractal_counting_function.o \
	$(OBJDIR)/fractal_formula.o \
	$(OBJDIR)/error.o \
	$(OBJDIR)/fractal_loop.o \
	$(OBJDIR)/fractal_rendering_parameters.o \
	$(OBJDIR)/fractal_transfer_function.o \
	$(OBJDIR)/color.o \
	$(OBJDIR)/file_parsing.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/fractal2D.o \
	$(OBJDIR)/fractal.o \
	$(OBJDIR)/gradient.o \
	$(OBJDIR)/image.o \
	$(OBJDIR)/misc.o \
	$(OBJDIR)/ppm.o \
	$(OBJDIR)/rectangle.o \
	$(OBJDIR)/thread.o

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all : $(OBJDIR) $(BINDIR) ${DEPENDENCY_FILE} $(TARGET)
	
${DEPENDENCY_FILE}: $(OBJDIR) ${SRCDIR}/*.c ${INCLUDEDIR}/*.h
	$(call quiet-command,for i in ${SRCDIR}/*.c; do ${CC} ${CFLAGS} -MM "$${i}"; done | sed "s/\(^.*:\)/${OBJDIR}\/\1/" > $@,"  BUILDING DEPENDENCY DATABASE");
-include ${DEPENDENCY_FILE}

$(TARGET): $(OBJECTS)
	$(call quiet-command, $(AR) $(ARFLAGS) $@ $^, "  AR     $@")

$(OBJDIR):
	$(call quiet-command, mkdir -p $(OBJDIR),)

$(BINDIR):
	$(call quiet-command, mkdir -p $(BINDIR),)

$(OBJDIR)/%.o:${SRCDIR}/%.c
	${CC} ${CFLAGS} -c $< -o $@

clean:
	$(call quiet-command, rm -f ${DEPENDENCY_FILE}, "  CLEAN    (DEPENDENCY DATABASE)")
	$(call quiet-command, rm -f $(OBJECTS) , "  CLEAN    (OBJECTS)")

distclean: clean
	$(call quiet-command, rm -rf $(OBJDIR), "  CLEAN    (OBJDIR)")
	$(call quiet-command, rm -rf $(BINDIR), "  CLEAN    (BINDIR)")

