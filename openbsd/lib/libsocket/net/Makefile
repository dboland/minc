OBJECTS=base64.o freeaddrinfo.o gai_strerror.o \
	getnetbyaddr.o getnetbyname.o getnetent.o \
	getpeereid.o getproto.o getprotoent.o getprotoname.o \
	getservbyname.o getservbyport.o getifaddrs.o \
	herror.c.o if_nameindex.o inet_addr.o \
	inet_lnaof.o inet_makeaddr.o inet_neta.o inet_netof.o inet_network.o \
	inet_net_ntop.o inet_net_pton.o inet_ntoa.o inet_ntop.o inet_pton.o \
	res_comp.o res_data.o rcmd.o rcmdsh.o rresvport.o \
	res_debug_syms.o ruserok.o \
	res_random.o ethers.o recv.o send.o vars6.o

OBJECTS+=getservent.c.o linkaddr.c.o if_indextoname.c.o if_nametoindex.c.o

all: ${OBJECTS}

%.o: ${SRCDIR}/libc/net/%.c
	gcc -c ${CFLAGS} -o $@ $<

%.c.o: %.c
	gcc -c ${CFLAGS} -o $@ $<

