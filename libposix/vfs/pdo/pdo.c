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

#include "pdo_random.c"
#include "pdo_route.c"
#include "pdo_namei.c"
#include "pdo_device.c"
#include "pdo_termio.c"
#include "pdo_unistd.c"
#include "pdo_tty.c"
#include "pdo_fcntl.c"
#include "pdo_stat.c"
#include "pdo_poll.c"
#include "pdo_statvfs.c"
#include "pdo_dkio.c"
#include "pdo_wsconsio.c"

/****************************************************/

VOID 
pdo_init(WIN_DEV_CLASS Devices[])
{
	__Devices = Devices;

	config_init("mem", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_MEM);	/* netstat.exe */
	config_init("kmem", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_KMEM);
	config_init("null", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_NULL);
	config_init("random", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_RANDOM);
	config_init("urandom", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_URANDOM);
	config_init("route", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_ROUTE);
	config_init("stdin", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_STDIN);
	config_init("stdout", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_STDOUT);
	config_init("stderr", FS_TYPE_PDO, WIN_VCHR, DEV_TYPE_STDERR);
	config_init("swap", FS_TYPE_PDO, WIN_VBLK, DEV_TYPE_SWAP);

	config_init("console", FS_TYPE_CHAR, WIN_VCHR, DEV_TYPE_CONSOLE);
	config_init("tty", FS_TYPE_CHAR, WIN_VCHR, DEV_CLASS_TTY);
	config_init("ptm", FS_TYPE_CHAR, WIN_VCHR, DEV_TYPE_PTM);
	config_init("input", FS_TYPE_CHAR, WIN_VCHR, DEV_TYPE_INPUT);
	config_init("screen", FS_TYPE_CHAR, WIN_VCHR, DEV_TYPE_SCREEN);

//	config_init("root", FS_TYPE_DISK, WIN_VBLK, DEV_TYPE_ROOT);

	con_init(DEVICE(DEV_TYPE_CONSOLE));
}
