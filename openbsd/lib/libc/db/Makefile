.SUFFIXES:
.DEFAULT:

all: btree.d db.d hash.d mpool.d recno.d

%.d: %
	@${MAKE} -C $< SRCDIR=${SRCDIR} ARCH=${ARCH} CFLAGS="${CFLAGS} -D__DBINTERFACE_PRIVATE"
