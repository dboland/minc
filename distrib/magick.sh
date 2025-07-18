if ! cd "$PKGROOT/ImageMagick-release"; then
        exit 1
fi

echo -n "Inpakken magick710.tgz... "
tar -zcf $DISTROOT/magick710.tgz etc usr/bin usr/share/man
echo gereed.

