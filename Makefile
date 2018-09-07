.POSIX:
.SUFFIXES:.lua .luac .ppm

CC=cc
CFLAGS=-c -O -I/usr/local/include -I/usr/X11R6/include
LDFLAGS=-s -L/usr/local/lib -L/usr/X11R6/lib
LDLIBS=-lglfw -lGLESv2 -lopenal -lvorbisfile -llz4

OBJ=src/main.o  \
    src/sdata.o \
    src/sdraw.o \
    src/stick.o \
    src/sop.o   \
    lib/bio.o   \
    lib/image.o \
    lib/r.o     \
    lib/batch.o \
    lib/bmp.o   \
    lib/coll.o
DST=sss

LVL=lvl/1  lvl/2  lvl/3  lvl/4  lvl/5  lvl/6  lvl/7  lvl/8  lvl/9
LC=lvl/lvl.luac

all: $(DST) $(LVL)

$(DST): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LDLIBS)

src/main.o src/sdata.o src/sdraw.o src/stick.o src/sop.o: src/state.h
lib/bio.o: lib/bio.h
lib/image.o: lib/bio.h lib/image.h
lib/r.o: lib/image.h lib/r.h
lib/batch.o: lib/image.h lib/r.h lib/batch.h
lib/bmp.o: lib/bio.h lib/bmp.h
lib/coll.o: lib/bmp.h lib/coll.h
.c.o:
	$(CC) -o $@ $(CFLAGS) $<

.lua.luac:
	luac53 -o $@ $<

$(LVL): $(LC)
.ppm:
	lua53 -- $(LC) < $< > $@

clean:
	rm -f $(OBJ) $(LC)

distclean:
	rm -f $(OBJ) $(LC) $(DST) $(LVL)
