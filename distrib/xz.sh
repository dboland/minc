if ! cd "$PKGROOT/xz-release"; then
        exit 1
fi

echo -n "Inpakken xz52.tgz... "
tar -zcf $DISTROOT/xz52.tgz usr/bin usr/share/man
echo gereed.

