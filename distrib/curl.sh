if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/curl-release"; then
        exit 1
fi

ln -s libcurl.so.10.0 ./usr/lib/libcurl.so 2>/dev/null

echo -n "Inpakken curl772.tgz... "
tar -zcf $DISTROOT/curl772.tgz *
echo gereed.
