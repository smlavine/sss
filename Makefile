.POSIX:
.SUFFIXES:.lua .luac .ppm

CC=cc
CFLAGS=-c -O -I/usr/local/include -I/usr/X11R6/include
LDFLAGS=-s -L/usr/local/lib -L/usr/X11R6/lib
LDLIBS=-lglfw -lGLESv2 -lopenal -lvorbisfile -llz4

OBJ=src/main.o     \
    src/sdata.o    \
    src/sdraw.o    \
    src/supdate.o  \
    src/bio.o      \
    src/image.o    \
    src/renderer.o \
    src/batch.o    \
    src/bitmap.o   \
    src/coll.o
DST=sss

LVL=lvl/1  lvl/2  lvl/3
LC=lvl/lvl.luac

all: $(DST) $(LVL)

$(DST): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LDLIBS)

src/main.o: src/matrix.h   \
            src/camera.h   \
            src/image.h    \
            src/renderer.h \
            src/batch.h    \
            src/bitmap.h   \
            src/coll.h     \
            src/state.h
src/sdata.o: src/image.h    \
             src/renderer.h \
             src/batch.h    \
             src/bitmap.h   \
             src/coll.h     \
             src/state.h
src/sdraw.o: src/matrix.h   \
             src/camera.h   \
             src/image.h    \
             src/renderer.h \
             src/batch.h    \
             src/bitmap.h   \
             src/coll.h     \
             src/state.h
src/supdate.o: src/image.h    \
               src/renderer.h \
               src/batch.h    \
               src/bitmap.h   \
               src/coll.h     \
               src/state.h
src/bio.o: src/bio.h
src/image.o: src/bio.h src/image.h
src/renderer.o: src/image.h src/renderer.h
src/batch.o: src/image.h src/renderer.h src/batch.h
src/bitmap.o: src/bio.h src/bitmap.h
src/coll.o: src/bitmap.h src/coll.h
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
