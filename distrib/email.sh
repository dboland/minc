if ! cd "$PKGROOT/eMail-release"; then
        exit 1
fi

echo -n "Inpakken email32.tgz... "
tar -zcf $DISTROOT/email32.tgz *
echo gereed.

