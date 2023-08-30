OBJECTS= cfgetispeed.o cfgetospeed.o cfmakeraw.o cfsetispeed.o cfsetospeed.o \
	cfsetspeed.o tcdrain.o tcflow.o tcflush.o tcgetattr.o tcgetpgrp.o \
	tcsendbreak.o tcsetattr.o tcsetpgrp.o tcgetsid.o

#OBJECTS+=

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/termios/%.c
	gcc -c ${CFLAGS} -o $@ $<

testlib: ${OBJECTS}
	gcc -shared -e "" -Wl,--disable-auto-import -o testlib.dll ${OBJECTS}

libtermios.a: ${OBJECTS}
	ar cru libtermios.a ${OBJECTS}

clean:
	rm -f *.o *.dll *.a
