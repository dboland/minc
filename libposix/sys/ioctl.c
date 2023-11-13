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

#include <sys/ioctl.h>

/****************************************************/

int 
sys_ioctl(call_t call, int fd, unsigned long request, ...)
{
	int result = 0;
	va_list args;

	va_start(args, request);
	if (fd < 0 || fd >= OPEN_MAX){
		result = -EBADF;
	}else switch (IOCGROUP(request)){
		case 'd':
			result = disk_ioctl(call.Task, fd, request, args);
			break;
		case 't':
			result = tty_ioctl(call.Task, fd, request, args);
			break;
		case 's':
		case 'i':
			result = sock_ioctl(call.Task, fd, request, args);
			break;
		case 'f':
			result = file_ioctl(call.Task, fd, request, args);
			break;
		case 'p':
			result = pci_ioctl(call.Task, fd, request, args);
			break;
		case 'W':
			result = wscons_ioctl(call.Task, fd, request, args);
			break;
		case 'm':		/* sys/mtio.h (tar.exe) */
			result = mtape_ioctl(call.Task, fd, request, args);
			break;
		default:
			result = -EOPNOTSUPP;
	}
	va_end(args);
	return(result);
}
