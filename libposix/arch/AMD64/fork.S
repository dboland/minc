/*
 * Copyright (c) 2016 Daniel Boland <dboland@xs4all.nl>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its 
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

.global fork, fork_exit, tls_get, tls_free

.def	fork;
	.scl 2
	.type 32;
.endef
.def	posix__exit;
	.scl 2
	.type 32;
.endef
.def	tls_get;
	.scl 2
	.type 32;
.endef
.def	tls_free;
	.scl 2
	.type 32;
.endef

.section .text

return = 0
base   = 8
size   = 16
source = 24
offset = 32
bx     = 40
cx     = 48
dx     = 56
di     = 64
si     = 72
x12    = 80
x13    = 88

fork:
	call win_init_fork
	mov %rbx,bx(%rax)
	mov %rcx,cx(%rax)
	mov %rdx,dx(%rax)
	mov %rdi,di(%rax)
	mov %rsi,si(%rax)
	mov %r12,x12(%rax)
	mov %r13,x13(%rax)
	pop %rbx                # align stack
	mov %rbx,return(%rax)
	mov %gs:8,%rbx          # NT_TIB.StackBase
	mov %rbx,base(%rax)
	mov %rbx,size(%rax)
	sub %rbp,size(%rax)
	mov %rbp,source(%rax)
	mov %rbp,offset(%rax)
	sub %rsp,offset(%rax)
	mov %gs:0xe00,%rbx	# NT_TIB.UserReserved
	mov %rbx,task(%rax)
	push return(%rax)
	enter $0,$0
	push bx(%rax)
	push %r12
	sub $0x40,%esp
	mov %rax,%rbx

	mov %gs:8,%rdx
	mov %rbp,%rcx
	call unwind

	lea token(%rbx),%r8
	mov %rbx,%rdx
	mov $fork_child,%rcx
	call win_fork

	add $0x40,%esp
	pop %r12
	pop %rbx
	leave
	ret

fork_child:
	mov %rsp,%rbp           # fix base pointer
	add $8,%rbp
	mov (%rsp),%rbx
	mov %rcx,%r12
	enter $0,$0

	sub size(%r12),%rsp	# allocate stack
	sub $16,%rsp
	mov %rbp,(%rsp)		# dest
	mov %rsp,%r8
	mov source(%r12),%rdx	# source
	mov base(%r12),%rcx	# base
	sub $0x20,%rsp		# add shadow space
	call copy_process
	add $0x20,%rsp
	mov (%rsp),%rbp		# restore pointers
	mov %rbp,%rsp
	sub offset(%r12),%rsp

	mov %gs:8,%rdx
	mov %rbp,%rcx
	sub $32,%rsp
	call unwind
	add $32,%rsp

	mov source(%r12),%rsi	# add current frame
	sub offset(%r12),%rsi
	mov %rsp,%rdi
	mov offset(%r12),%rcx
	cld
	rep movsb

	push return(%r12)
	enter $0,$0

	cmpl $0,token(%r12)	# assign logon token (if any)
	je 1f
	mov token(%r12),%rdx
	mov $0,%ecx
	sub $32,%rsp
	call SetThreadToken
	mov token(%r12),%rcx
	sub $32,%rsp
	call CloseHandle
1:	mov %r12,%rax		# restore registers
	mov bx(%rax),%rbx
	mov cx(%rax),%rcx
	mov dx(%rax),%rdx
	mov di(%rax),%rdi
	mov si(%rax),%rsi
	mov x12(%rax),%r12
	mov x13(%rax),%r13

	mov %rax,%rcx		# free thread struct
	sub $32,%rsp
	call free
	add $32,%rsp
	mov $0,%rax

	leave
	ret

posix_exit:
	enter $0,$0
	mov %rax,%rcx
	call ExitThread
	leave
	nop

tls_get:
	enter $0x20,$0
	mov %gs:0xe00,%rax
	cmp $0,%rax
	jne 1f
	call get_free_page
	mov %rax,%gs:0xe00
1:	leave
	ret

tls_free:
	enter $0x20,$0
	cmp $0,%gs:0xe00
	je 1f
	mov %gs:0xe00,%rcx
	call free
1:	leave
	ret


