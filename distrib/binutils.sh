if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/binutils-release"; then
        exit 1
fi

echo -n "Inpakken binutils228.tgz... "
tar -zcf $DISTROOT/binutils228.tgz *
echo gereed.

