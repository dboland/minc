if ! cd "$PKGROOT/nano-release"; then
        exit 1
fi

mkdir -p ./etc
cp -p /etc/nanorc ./etc/nanorc

echo -n "Inpakken nano64.tgz... "
tar -zcf $DISTROOT/nano64.tgz *
echo gereed.
