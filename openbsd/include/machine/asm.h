/*	$OpenBSD: src/sys/arch/i386/include/asm.h,v 1.13 2013/03/28 17:41:04 martynas Exp $	*/
/*	$NetBSD: asm.h,v 1.7 1994/10/27 04:15:56 cgd Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)asm.h	5.5 (Berkeley) 5/7/91
 */

#ifndef _MACHINE_ASM_H_
#define _MACHINE_ASM_H_

#ifdef __PIC__
#define PIC_PROLOGUE	\
	pushl	%ebx;	\
	call	666f;	\
666:			\
	popl	%ebx;	\
	addl	$_C_LABEL(_GLOBAL_OFFSET_TABLE_)+[.-666b], %ebx
#define PIC_EPILOGUE	\
	popl	%ebx
#define PIC_PLT(x)	x@PLT
#define PIC_GOT(x)	x@GOT(%ebx)
#define PIC_GOTOFF(x)	x@GOTOFF(%ebx)
#else
#define PIC_PROLOGUE
#define PIC_EPILOGUE
#define PIC_PLT(x)	x
#define PIC_GOT(x)	x
#define PIC_GOTOFF(x)	x
#endif

#define _C_LABEL(name)	__CONCAT(_,name)
#define	_ASM_LABEL(x)	x

#define CVAROFF(x, y)	_C_LABEL(x) + y

#ifdef __STDC__
# define __CONCAT(x,y)	x ## y
# define __STRING(x)	#x
#else
# define __CONCAT(x,y)	x/**/y
# define __STRING(x)	"x"
#endif

/*
 * STRONG_ALIAS, WEAK_ALIAS
 *	Create a strong or weak alias.
 */
#define STRONG_ALIAS(alias,sym) \
	.global alias; \
	alias = sym
#define WEAK_ALIAS(alias,sym) \
	.weak alias; \
	alias = sym

/*
 * WARN_REFERENCES: create a warning if the specified symbol is referenced
 */
#define WARN_REFERENCES(_sym,_msg)	\
	.section .gnu.warning. ## _sym ; .ascii _msg ; .text

/* let kernels and others override entrypoint alignment */
#ifndef _ALIGN_TEXT
# define _ALIGN_TEXT .align 2, 0x90
#endif

#define _ENTRY(x) \
	.text; _ALIGN_TEXT; .global x; .def x; .scl 2; .type 32; .endef; x:

#if defined(PROF) || defined(GPROF)
# define _PROF_PROLOGUE	\
	pushl %ebp; movl %esp,%ebp; call PIC_PLT(mcount); popl %ebp
#else
# define _PROF_PROLOGUE
#endif

#define	ENTRY(y)	_ENTRY(_C_LABEL(y)); _PROF_PROLOGUE
#define	NENTRY(y)	_ENTRY(_C_LABEL(y))
#define	ASENTRY(y)	_ENTRY(_ASM_LABEL(y)); _PROF_PROLOGUE
#define	NASENTRY(y)	_ENTRY(_ASM_LABEL(y))

#define	ALTENTRY(name)	.globl _C_LABEL(name); _C_LABEL(name):

#define	ASMSTR		.asciz

#define RCSID(x)	.text; .asciz x

#ifdef _KERNEL

#ifdef MULTIPROCESSOR
#define CPUVAR(var)	%fs:__CONCAT(CPU_INFO_,var)
#else
#define CPUVAR(var)	_C_LABEL(cpu_info_primary)+__CONCAT(CPU_INFO_,var)
#endif

#endif /* _KERNEL */

#endif /* !_MACHINE_ASM_H_ */
