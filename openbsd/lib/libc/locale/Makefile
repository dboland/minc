# for LOCALECHARSETS
include ${SRCDIR}/../../share/locale/ctype/Makefile.inc

OBJECTS= btowc.o _def_messages.o _def_monetary.o _def_numeric.o _def_time.o \
	localeconv.o nl_langinfo.o setlocale.o iswctype.o __mb_cur_max.o \
	mblen.o mbrlen.o mbstowcs.o mbtowc.o multibyte_citrus.o wcscoll.o \
	wcstombs.o wctob.o wctomb.o wcstod.o wcstol.o \
	wcstoul.o wcstoll.o wcstoull.o wcstoimax.o wcstoumax.o \
	setrunelocale.o runeglue.o rune.o runetable.o ___runetype_mb.o \
	_wctrans.o wcsxfrm.o

OBJECTS+= wcstof.o wcstold.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/locale/%.c
	gcc -c ${CFLAGS} -DLOCALE_CHARSETS="\"${LOCALECHARSETS}\"" -I${SRCDIR}/include -I${SRCDIR}/citrus -o $@ $<

# runetable.c does not match gen/ctype_.c
# %.c.o: %.c
#	gcc -c ${CFLAGS} -DLOCALE_CHARSETS="\"${LOCALECHARSETS}\"" -I${SRCDIR}/locale -I${SRCDIR}/citrus -o $@ $<

