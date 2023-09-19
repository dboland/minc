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

#include <stdlib.h>
#include <stdio.h>

#include "win/windows.h"
#include "win_types.h"
#include "msvc_posix.h"
#include "arch_posix.h"

#include <libgen.h>
#include <nlist.h>

//char *__progname = NULL;		// gas: NULL solves "can't be equated to common symbol"
char *__progname;
char **__environ;

extern __import char *__PROGNAME;

void 
_init(char *cmdbuf, int *_argc, char ***_argv, char ***_env, void *frame_address)
{
	msvc_init(_argc, _argv, _env);
	task_init(cmdbuf, *_argv, frame_address);
	__environ = *_env;
	__progname = __PROGNAME = basename(*_argv[0]);
}
int 
__fdnlist(int fd, struct nlist *list)
{
	int result = 0;

	while (*list->n_name){
printf("__fdnlist(%d): name(%s) type(0x%x) other(%d) desc(%d) value(%d)\n", 
		fd, list->n_name, list->n_type, list->n_other, list->n_desc, list->n_value);
		result++;
		list++;
	}
	return(result);
}
