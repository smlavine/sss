# POSIX make is awesome!
# nullprogram.com/blog/2016/04/30/
# nullprogram.com/blog/2017/08/20/
# opengroup.org/onlinepubs/9699919799/utilities/make.html
.POSIX:

# Flag macros
CC=cc
CFLAGS=-c -O -g -I/usr/local/include -I/usr/X11R6/include -Wall -Wextra
LDFLAGS=-L/usr/local/lib -L/usr/X11R6/lib
LDLIBS=-lglfw -lGLESv2 -lopenal -lvorbisfile -lm

# File set macros
SRC_OBJ=src/main.o src/data.o src/draw.o src/tick.o src/op.o
LIB_OBJ=lib/audio.o lib/r.o lib/batch.o lib/coll.o
OBJ=$(SRC_OBJ) $(LIB_OBJ)
DST=sss

# The first and default target
$(DST): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LDLIBS)

# Dependencies on headers
$(OBJ): lib/lib.h
$(SRC_OBJ): src/s.h

# Inference rules
.c.o:
	$(CC) -o $@ $(CFLAGS) $<

# Phony targets
clean:
	rm -f $(OBJ)
distclean:
	rm -f $(OBJ) $(DST)
