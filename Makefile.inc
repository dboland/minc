include ${ROOTDIR}/config.inc

.SUFFIXES:
.DEFAULT:

# PIC default: 0x10000000 (library space)
# PIE default: 0x00400000 (program space)

IMGBASE_LIBD=		0x0FF80000 # (500K)
IMGBASE_LIBPOSIX=	0x0FF00000 # (500K)
IMGBASE_LIBC=		0x0FE00000
IMGBASE_LIBSOCKET=	0x0FD00000
IMGBASE_LIBEDIT=	0x0FC00000
#LIBUTIL_IMGBASE=	0x0FB00000
#LIBZ_IMGBASE=		0x0FA00000
IMGBASE_LIBCURL=	0x0F900000
LIBPERL_IMGBASE=	0x0F600000 # (3MB)

BUILD=$(shell date +%s)

GPREFIX= ${PREFIX}/cross
GBINDIR= ${GPREFIX}/bin
GLIBDIR= ${GPREFIX}/lib
GINCDIR= ${GPREFIX}/include
GLIBXDIR= ${GPREFIX}/libexec

XGCC= ${GBINDIR}/gcc
XWINDRES= ${GBINDIR}/windres

# The location of the BSD install program. It will be created
# automatically. Don't try to install it yourself. It is used
# to check if your build was successful.

INSTALL= /mingw/bin/install

# In GCC 6.3.0, -ffixed-ebx crashes compilation

CC= gcc
CFLAGS= -Wimplicit-function-declaration -Werror -ffreestanding -Wl,--disable-auto-import
LDFLAGS= --disable-auto-import
