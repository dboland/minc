OBJECTS=e_acoshl.o e_atanhl.o e_coshl.o e_expl.o e_fmodl.o e_hypotl.o e_lgammal.o \
	e_log10l.o e_log2l.o e_logl.o e_powl.o e_sinhl.o e_tgammal.o invtrig.o \
	k_cosl.o k_sinl.o k_tanl.o s_asinhl.o s_cbrtl.o s_ceill.o s_erfl.o s_exp2l.o \
	s_expm1l.o s_floorl.o s_log1pl.o s_modfl.o s_nanl.o s_nextafterl.o \
	s_nexttoward.o s_remquol.o s_tanhl.o s_truncl.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/libm/src/ld80/%.c
	${XGCC} -c ${CFLAGS} -I${SRCDIR}/libm/src -o $@ $<

