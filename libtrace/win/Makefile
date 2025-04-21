include ../../Makefile.inc

OBJECTS = WinFlagName.o WinAceFlags.o WinFileAccess.o WinSecurityFlags.o \
	WinGroupAttribs.o WinPrivilegeAttribs.o
OBJECTS += win_ACL.o win_SID.o win_TOKEN.o win_SECURITY_DESCRIPTOR.o \
	win_TOKEN_PRIVILEGES.o win_TOKEN_GROUPS.o

CFLAGS += -DWIN_ERR=printf

all: ${OBJECTS}

%.o: %.c
	gcc -c ${CFLAGS} -I${MINGW}/include -I../../include -I.. -o $@ $<

clean:
	rm -f *.o
