.POSIX:

CC=c99
CFLAGS=-c -s -O
LDFLAGS=-s
LDLIBS=-lglfw -lGLESv2 -lopenal -lvorbisfile -llz4

OBJ=src/main.o
DST=sss

$(DST): $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LDLIBS)

.c.o:
	$(CC) -o $@ $(CFLAGS) $<

clean:
	rm -f $(OBJ)

distclean:
	rm -f $(OBJ) $(DST)
