OBJECTS=bcmp.o bcopy.o bzero.o ffs.o index.o memchr.o memcmp.o memset.o \
	rindex.o strcat.o strcmp.o strcpy.o strcspn.c.o strlcpy.c.o strlen.c.o \
	strncat.c.o strncmp.o strncpy.c.o strpbrk.c.o strsep.c.o \
	strspn.c.o strstr.c.o swab.o
OBJECTS+=memcpy.o memmove.o strchr.o strrchr.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/arch/${ARCH}/string/%.S
	gcc -c ${CFLAGS} -o $@ $<

%.c.o: ${SRCDIR}/string/%.c
	gcc -c ${CFLAGS} -o $@ $<

MAN=bcmp.3 bcopy.3 bzero.3 ffs.3 memchr.3 memcmp.3 memset.3 \
	strcat.3 strcmp.3 strcpy.3 strcspn.3 strlcpy.3 strlen.3 \
	strncat.3 strncpy.3 strpbrk.3 strsep.3 \
	strspn.3 strstr.3 swab.3
MAN+=memcpy.3 memmove.3 strchr.3 strrchr.3

install: ${MAN}

