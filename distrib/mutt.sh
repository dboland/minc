if ! cd "$PKGROOT/mutt-release"; then
        exit 1
fi

cp /etc/Muttrc ./etc/Muttrc

echo -n "Inpakken mutt20.tgz... "
tar -zcf $DISTROOT/mutt20.tgz *
echo gereed.

