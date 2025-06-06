if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/git-release"; then
        exit 1
fi

mkdir -p etc
cp -p /etc/gitconfig ./etc/

mkdir -p ./usr/share/man
echo -n "Uitpakken man pagina's... "
xz --decompress --stdout ../git-manpages-2.31.1.tar.xz | tar -C ./usr/share/man -xf -
echo gereed.

echo -n "Inpakken git231.tgz... "
tar -zcf $DISTROOT/git231.tgz *
echo gereed.
