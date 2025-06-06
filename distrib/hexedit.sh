if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/hexedit-release"; then
        exit 1
fi

echo -n "Inpakken hexedit12.tgz... "
tar -zcf $DISTROOT/hexedit12.tgz *
echo gereed.

