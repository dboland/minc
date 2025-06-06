if ! [ -d "$DISTROOT" ]; then
	echo "$DISTROOT: No such directory"
	exit 1
elif ! cd "$PKGROOT/bind-release"; then
	exit 1
fi

echo -n "Inpakken bind93.tgz... "
tar -zcf $DISTROOT/bind93.tgz usr/bin usr/share/man/man1
echo gereed.
