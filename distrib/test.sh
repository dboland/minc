if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/../minc-test"; then
        exit 1
fi

copy_file /etc "MAKEDEV"
copy_file /sbin "$SBIN"

echo -n "Inpakken test61.tgz... "
tar -zcf $DISTROOT/test61.tgz *
echo gereed.
