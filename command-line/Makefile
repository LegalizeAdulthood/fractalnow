SRCDIR        = src
INCLUDEDIR    = include
FRACTALLIBDIR = ../lib
OBJDIR        = objs
PROG          = fractal2D
DEPENDENCY_FILE=${OBJDIR}/makefile.dep

UNAME := $(shell uname)

CC            = gcc
LD            = gcc

FLOAT_PRECISION = 1
CFLAGS        = -std=c99 -pedantic -Wall -Wextra
CFLAGS       += -DFLOAT_PRECISION=$(FLOAT_PRECISION)
#DEBUG     = true
ifdef DEBUG
	CFLAGS += -Werror -O0
else
	CFLAGS += -O2
endif
CFLAGS       += -I${INCLUDEDIR} -I${FRACTALLIBDIR}/include

LDFLAGS   = -lm -lpthread -L${FRACTALLIBDIR} -lfractal2D 

OBJECTS = \
	$(OBJDIR)/anti_aliasing.o \
	$(OBJDIR)/command_line.o \
	$(OBJDIR)/help.o \
	$(OBJDIR)/main.o

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all : $(OBJDIR) ${DEPENDENCY_FILE} $(PROG)
	
${DEPENDENCY_FILE}: $(OBJDIR) ${SRCDIR}/*.c ${INCLUDEDIR}/*.h
	$(call quiet-command,for i in ${SRCDIR}/*.c; do ${CC} ${CFLAGS} -MM "$${i}"; done | sed "s/\(^.*:\)/${OBJDIR}\/\1/" > $@,"  BUILDING DEPENDENCY DATABASE");
-include ${DEPENDENCY_FILE}


$(PROG): $(OBJECTS)
	$(call quiet-command, $(LD) -o $@ $^ $(LDFLAGS), "  LD     $@")

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

