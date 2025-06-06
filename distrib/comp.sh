if ! [ -d "$DISTROOT" ]; then
        echo "$DISTROOT: No such directory"
        exit 1
elif ! cd "$PKGROOT/../minc-comp"; then
        exit 1
fi

cp -p /usr/lib/crt0.o ./usr/lib/
cp -p /usr/lib/crt1.o ./usr/lib/
cp -p /usr/lib/gcrt0.o ./usr/lib/

cp -p /usr/lib/libc.a ./usr/lib/
cp -p /usr/lib/libsocket.a ./usr/lib/

# don't overwrite base libs
rm -f ./usr/lib/libcrypto.so.27.0
rm -f ./usr/lib/libssl.so.24.1
rm -f ./usr/lib/libutil.so.12.1

echo -n "Inpakken comp61.tgz... "
tar -zcf $DISTROOT/comp61.tgz *
echo gereed.
