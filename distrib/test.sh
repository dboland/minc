if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/../minc-test"; then
        exit 1
fi

copy_file /sbin "$SBIN"
copy_file /etc "$ETC"

echo -n "Inpakken test61.tgz... "
tar -zcf $DISTROOT/test61.tgz *
echo gereed.
