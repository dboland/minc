/*	$OpenBSD: src/sys/arch/i386/include/cdefs.h,v 1.10 2013/03/28 17:30:45 martynas Exp $	*/

/*
 * Written by J.T. Conklin <jtc@wimsey.com> 01/17/95.
 * Public domain.
 */

#ifndef	_MACHINE_CDEFS_H_
#define	_MACHINE_CDEFS_H_

#ifndef __bounded__
#define __bounded__(...)	
#endif
#ifndef visibility
#define visibility(a)	
#endif

#undef __MINGW32__
#undef __WIN32__	// <GL/gl.h>
#undef __WINNT__
#undef __WINNT
#undef WIN32		// cc.exe
#undef WINNT
#undef _WIN32		// cc.exe
//#undef _MSC_VER		// GNU libintl.h

#define __thread	__thread __attribute__((section(".tls")))
#define __import	__attribute__((dllimport))

#define __MinC__
#define __OpenBSD__
//#define __THROW						// GLIBC mntent.h
//#define uint_t		u_int				// GLIBC mnttab.h
#define restrict	__restrict
#define environ		__environ
#define alloca		__builtin_alloca
//#define sys_siglist	_sys_siglist

/* pthread_win32 */
#define PTW32_CDECL
#define __struct_timespec_defined
#define __have_typedef_pid_t

#ifdef USEVFORK
#define fork		vfork
#endif

#ifdef NOPIE
#define LIBC_IMPORT		
#define GETOPT_IMPORT	
#define LIBSOCK_IMPORT	
#endif

#ifndef LIBPOSIX_IMPORT
#define LIBPOSIX_IMPORT	__import
#endif
#ifndef LIBC_IMPORT
#define LIBC_IMPORT	__import
#endif
#ifndef GETOPT_IMPORT
#define GETOPT_IMPORT	__import
#endif
#ifndef LIBSOCK_IMPORT
#define LIBSOCK_IMPORT	__import
#endif

/* weak aliases are kind of broken in MinGW32 ld:
 * http://mingw.5.n7.nabble.com/weak-functions-td6379.html
 * https://sourceware.org/bugzilla/show_bug.cgi?id=9687
 * https://sourceware.org/bugzilla/show_bug.cgi?id=2729
 */

#define __strong_alias(name,symbol)			\
	extern __typeof(symbol) name __attribute__((alias(__STRING(symbol))))
//#define __weak_alias(name,symbol)			\
//	extern __typeof(symbol) name __attribute__((weak,alias(__STRING(symbol))))
#define __weak_alias(name,symbol)			\
	__strong_alias(name,symbol)
#define __warn_references(symbol,msg)			\
	extern __typeof(symbol) symbol __attribute__((deprecated(msg)))

#endif /* !_MACHINE_CDEFS_H_ */
