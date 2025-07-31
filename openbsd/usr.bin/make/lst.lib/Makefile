OBJECTS= lstAddNew.o lstAppend.o lstConcat.o lstConcatDestroy.o \
	lstDeQueue.o lstDestroy.o lstDupl.o lstFindFrom.o lstForEachFrom.o \
	lstInsert.o lstMember.o lstRemove.o lstReplace.o lstRequeue.o lstSucc.o

.SUFFIXES:
.DEFAULT:

all: liblst.a

%.o: ${UBINSRC}/make/lst.lib/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${UBINSRC}/make -o $@ $<

liblst.a: ${OBJECTS}
	@echo "done."
	ar cru $@ ${OBJECTS}

clean:
	/bin/rm -f ${OBJECTS} liblst.a
