if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/BitchX-release"; then
        exit 1
fi

bunzip2 ./usr/share/man/man1/BitchX.1.bz2 2>/dev/null

#rm -f ./usr/bin/BitchX
#cp -p /usr/bin/BitchX ./usr/bin/BitchX

echo -n "Inpakken bitchx12.tgz... "
tar -zcf $DISTROOT/bitchx12.tgz *
echo gereed.
