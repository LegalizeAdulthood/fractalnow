SRCDIR    = src
INCLUDEDIR= include
OBJDIR    = objs
PROG      = fractal2D
DEPENDENCY_FILE=${OBJDIR}/makefile.dep

UNAME := $(shell uname)

CC        = gcc
LD        = gcc

CFLAGS    = -std=c99 -pedantic -D_GNU_SOURCE -Wall -Wextra
#DEBUG     = true
ifdef DEBUG
	CFLAGS += -Werror -O0 -g
else
	CFLAGS += -O2 -ffast-math
endif
CFLAGS    += -I${INCLUDEDIR}

LDFLAGS   = -lm -lpthread

OBJECTS = $(OBJDIR)/main.o \
	$(OBJDIR)/gradient.o \
	$(OBJDIR)/queue.o \
	$(OBJDIR)/ppm.o \
	$(OBJDIR)/rectangle.o \
	$(OBJDIR)/fractal.o \
	$(OBJDIR)/image.o \
	$(OBJDIR)/config.o \
	$(OBJDIR)/help.o \
	$(OBJDIR)/filter.o \
	$(OBJDIR)/color.o \
	$(OBJDIR)/thread.o \
	$(OBJDIR)/command_line.o

quiet-command = $(if $(VERB),$1,$(if $(2),@echo $2 && $1, @$1))

all : $(OBJDIR) $(OBJDIR) ${DEPENDENCY_FILE} $(PROG)
	

${DEPENDENCY_FILE}: $(OBJDIR) ${SRCDIR}/*.c ${INCLUDEDIR}/*.h
	$(call quiet-command,for i in ${SRCDIR}/*.c; do ${CC} ${CFLAGS} -MM "$${i}"; done | sed "s/\(^.*:\)/${OBJDIR}\/\1/" > $@,"  BUILDING DEPENDENCY DATABASE");
-include ${DEPENDENCY_FILE}

$(PROG): $(OBJECTS)
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

