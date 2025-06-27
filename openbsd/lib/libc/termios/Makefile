OBJECTS= cfgetispeed.o cfgetospeed.o cfmakeraw.o cfsetispeed.o cfsetospeed.o \
	cfsetspeed.o tcdrain.o tcflow.o tcflush.o tcgetattr.o tcgetpgrp.o \
	tcsendbreak.o tcsetattr.o tcsetpgrp.o tcgetsid.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/termios/%.c
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -o $@ $<
