CFLAGS+=-D__DBINTERFACE_PRIVATE

.SUFFIXES:
.DEFAULT:

all: btree.d db.d hash.d mpool.d recno.d

%.d: %
	@CFLAGS="${CFLAGS}" ${MAKE} -C $< SRCDIR=${SRCDIR} ARCH=${ARCH}
