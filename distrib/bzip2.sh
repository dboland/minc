if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/bzip-release"; then
        exit 1
fi

echo -n "Inpakken bzip210.tgz... "
tar -zcf $DISTROOT/bzip210.tgz *
echo gereed.

