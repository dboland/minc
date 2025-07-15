/*	$OpenBSD: src/bin/ls/main.c,v 1.4 2012/12/04 02:27:00 deraadt Exp $	*/

/*
 * Public domain - no warranty.
 */

#include <locale.h>
#include <stdlib.h>
#include <stdio.h>

int ls_main(int argc, char **argv);

int
main(int argc, char *argv[])
{
	int result;

	setlocale(LC_CTYPE, "en_US.CP1251");
	result = ls_main(argc, argv);
	setlocale(LC_CTYPE, "C");
	return result;
}
