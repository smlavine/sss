.POSIX:
.SUFFIXES:.lua .luac .ppm

CC=cc
CFLAGS=-c -O -g -I/usr/local/include -I/usr/X11R6/include
LDFLAGS=-L/usr/local/lib -L/usr/X11R6/lib
LDLIBS=-lglfw -lGLESv2 -lopenal -lvorbisfile

OBJ=src/main.o   \
    src/sdata.o  \
    src/sdraw.o  \
    src/stick.o  \
    src/sop.o    \
    src/saudio.o \
    lib/audio.o  \
    lib/matrix.o \
    lib/cam.o    \
    lib/r.o      \
    lib/batch.o  \
    lib/bmp.o    \
    lib/coll.o
DST=sss

LVL=lvl/1  lvl/2  lvl/3  lvl/4  lvl/5  lvl/6  lvl/7  lvl/8  lvl/9 lvl/10 \
    lvl/11 lvl/12 lvl/13 lvl/14 lvl/15 lvl/16 lvl/17
LC=lvl/lvl.luac

all: $(DST) $(LVL)

$(DST): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LDLIBS)

$(OBJ): lib/dragon.h
src/main.o src/sdata.o src/sdraw.o src/stick.o src/sop.o src/saudio.o: src/state.h
.c.o:
	$(CC) -o $@ $(CFLAGS) $<

.lua.luac:
	luac53 -o $@ $<

$(LVL): $(LC)
.ppm:
	lua53 -- $(LC) < $< > $@
	cp $@ rsc

clean:
	rm -f $(OBJ) $(LC) $(LVL)

distclean:
	rm -f $(OBJ) $(LC) $(DST) $(LVL)
	printf "%s" "rm -f $(LVL)" | sed -e 's/lvl/rsc/g' | sh
