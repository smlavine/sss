.POSIX:

CC=c99
CFLAGS=-c -s -O
LDFLAGS=-s
LDLIBS=-lglfw -lGLESv2 -lopenal -lvorbisfile -llz4

OBJ=src/main.o src/bio.o src/image.o src/renderer.o
DST=sss

$(DST): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LDLIBS)

bio.o: bio.h
image.o: bio.h image.h
renderer.o: image.h renderer.h
.c.o:
	$(CC) -o $@ $(CFLAGS) $<

clean:
	rm -f $(OBJ)

distclean:
	rm -f $(OBJ) $(DST)
