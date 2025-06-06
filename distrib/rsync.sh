if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/rsync-release"; then
        exit 1
fi

echo -n "Inpakken rsync322.tgz... "
tar -zcf $DISTROOT/rsync322.tgz *
echo gereed.
