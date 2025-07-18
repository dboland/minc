if ! cd "$PKGROOT/../minc-vista"; then
        exit 1
fi

echo -n "Inpakken vista.tgz... "
tar -zcf $DISTROOT/vista.tgz *
echo gereed.

