.POSIX:

CC=cc
CFLAGS=-c -O -g -I/usr/local/include -I/usr/X11R6/include
LDFLAGS=-L/usr/local/lib -L/usr/X11R6/lib
LDLIBS=-lglfw -lGLESv2 -lopenal -lvorbisfile -lm

OBJ=src/main.o   \
    src/data.o   \
    src/draw.o   \
    src/tick.o   \
    src/op.o     \
    lib/audio.o  \
    lib/matrix.o \
    lib/cam.o    \
    lib/r.o      \
    lib/batch.o  \
    lib/bmp.o    \
    lib/coll.o
DST=sss

$(DST): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LDLIBS)

$(OBJ): lib/dragon.h
src/main.o src/data.o src/draw.o src/tick.o src/op.o: src/s.h

.c.o:
	$(CC) -o $@ $(CFLAGS) $<

clean:
	rm -f $(OBJ)

distclean:
	rm -f $(OBJ) $(DST)
