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
 * 3. Neither the name of the copyright holders nor the names of its 
 *    contributors may be used to endorse or promote products derived 
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

.file "context.S"
.global _ucontext_posix

_ucontext_posix:
	movl $0,4(%esp)	# fpstate
	movl $0,8(%esp)	# err
	movl $0,12(%esp)	# trapno
	movl $0,16(%esp)	# mask
	movl $0,20(%esp)	# unused
	mov %ss,24(%esp)
	mov %esp,28(%esp)
	movl $0,32(%esp)	# eflags
	mov %cs,36(%esp)
#	movl %eip,40(%esp)
	movl $0,40(%esp)
	mov %eax,44(%esp)
	mov %ecx,48(%esp)
	mov %edx,52(%esp)
	mov %ebx,56(%esp)
	mov %ebp,60(%esp)
	mov %esi,64(%esp)
	mov %edi,68(%esp)
	mov %ds,72(%esp)
	mov %es,76(%esp)
	mov %fs,80(%esp)
	mov %gs,84(%esp)
	mov 4(%esp),%eax
	ret
