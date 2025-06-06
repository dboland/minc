if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/eMail-release"; then
        exit 1
fi

echo -n "Inpakken email32.tgz... "
tar -zcf $DISTROOT/email32.tgz *
echo gereed.

