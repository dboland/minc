OBJECTS=explicit_bzero.o memccpy.o memmem.o memrchr.o stpcpy.o stpncpy.o \
	strcasecmp.o strcasestr.o strcoll.o strdup.o \
	strerror.o strerror_r.o strlcat.o strmode.o strndup.o strnlen.o \
	strsignal.o strtok.o strxfrm.o \
	wcscat.o wcschr.o wcscmp.o wcscpy.o wcscspn.o wcslcat.o wcslcpy.o \
	wcslen.o wcsncat.o wcsncmp.o wcsncpy.o wcspbrk.o wcsrchr.o wcsspn.o \
	wcsstr.o wcstok.o wcswcs.o wcswidth.o wmemchr.o wmemcmp.o wmemcpy.o \
	wmemmove.o wmemset.o wcsdup.o \
	timingsafe_bcmp.o wcscasecmp.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/string/%.c
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -I${SRCDIR}/lib/libc -I${SRCDIR}/lib/libc/include -o $@ $<

