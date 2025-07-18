if ! cd "$PKGROOT/cyrus-sasl-release"; then
        exit 1
fi

chmod +x ./usr/lib/libsasl2.so.3.0
ls ./usr/lib/sasl2/*.so.* | xargs chmod +x
ln -s libsasl2.so.3.0 ./usr/lib/libsasl2.so 2>/dev/null

echo -n "Inpakken sasl2128.tgz... "
tar -zcf $DISTROOT/sasl2128.tgz *
echo gereed.
