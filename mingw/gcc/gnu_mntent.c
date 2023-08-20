/* Utilities for reading/writing fstab, mtab, etc.
   Copyright (C) 1995-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

/* Needed for GNU coreutils (stat.exe) */

#include <stdlib.h>
#include <string.h>

#include "gnu_mntent.h"

#define BUFLEN 1024

/*******************************************************/

FILE *setmntent(const char *filename, const char *type)
{
	return(fopen(filename, type));
}

int endmntent(FILE *fp)
{
	if (fp)
		fclose(fp);
	return 1;
}

struct mntent *getmntent_r(FILE *fp, struct mntent *mntbuf, char *buf, int buflen)
{
	char *line = NULL, *saveptr = NULL;
	const char *sep = " \t\n";

	if (!fp || !mntbuf || !buf)
		return NULL;

	while ((line = fgets(buf, buflen, fp)) != NULL) {
		if (buf[0] == '#' || buf[0] == '\n')
			continue;
		break;
	}

	if (!line)
		return NULL;

	mntbuf->mnt_fsname = strtok_r(buf, sep, &saveptr);
	if (!mntbuf->mnt_fsname)
		return NULL;

	mntbuf->mnt_dir = strtok_r(NULL, sep, &saveptr);
	if (!mntbuf->mnt_fsname)
		return NULL;

	mntbuf->mnt_type = strtok_r(NULL, sep, &saveptr);
	if (!mntbuf->mnt_type)
		return NULL;

	mntbuf->mnt_opts = strtok_r(NULL, sep, &saveptr);
	if (!mntbuf->mnt_opts)
		mntbuf->mnt_opts = "";

	line = strtok_r(NULL, sep, &saveptr);
	mntbuf->mnt_freq = !line ? 0 : atoi(line);

	line = strtok_r(NULL, sep, &saveptr);
	mntbuf->mnt_passno = !line ? 0 : atoi(line);

	return mntbuf;
}

struct mntent *getmntent(FILE *fp)
{
	static char *buf = NULL;
	static struct mntent mntbuf;

	buf = malloc(BUFLEN);
	if (!buf)
		perror("malloc");

	return getmntent_r(fp, &mntbuf, buf, BUFLEN);
}
