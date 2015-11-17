CC=clang
OBJDIR=bin
SRCDIR=src
INCDIR=$(SRCDIR)/lib
CFLAGS+=-I$(INCDIR)
platform=$(shell uname -o)

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

CFLAGS+=-O2 -Wall
ifeq ($(platform), GNU/Linux)
CFLAGS+=-DPA_USE_ALSA
else
CFLAGS+=-DPA_USE_COREAUDIO
endif
CFLAGS+=`pkg-config --cflags opencv`
CFLAGS_DEBUG+=-O0 -g3 -Werror -DDEBUG
LDFLAGS+=-lpthread -lncurses -lportaudio -lm
LDFLAGS+=`pkg-config --libs opencv`

all: chatX

.PHONY: all clean debug

chatX: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJS): | $(OBJDIR)
$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(wildcard $(INCDIR)/*.h) Makefile
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -rf $(OBJDIR) chatX
