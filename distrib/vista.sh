if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/../minc-vista"; then
        exit 1
fi

echo -n "Inpakken vista.tgz... "
tar -zcf $DISTROOT/vista.tgz *
echo gereed.

