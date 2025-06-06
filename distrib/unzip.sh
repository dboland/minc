if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/unzip-release"; then
        exit 1
fi

echo -n "Inpakken unzip552.tgz... "
tar -zcf $DISTROOT/unzip552.tgz *
echo gereed.

