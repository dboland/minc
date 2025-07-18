if ! cd "$PKGROOT/wget-release"; then
        exit 1
fi

echo -n "Inpakken wget118.tgz... "
tar -zcf $DISTROOT/wget118.tgz *
echo gereed.

