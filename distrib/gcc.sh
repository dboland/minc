if ! cd "$PKGROOT/gcc-release"; then
        exit 1
fi

ln -s gcc ./usr/bin/cc 2>/dev/null
ln -s liblto_plugin.so.0.0 ./usr/libexec/gcc/i686-pe-openbsd/4.8.1/liblto_plugin.so 2>/dev/null

mkdir -p ./sbin
cp /mnt/c/MinGW/bin/libstdc++-6.dll ./sbin/
cp /mnt/c/MinGW/bin/libgcc_s_dw2-1.dll ./sbin/

echo -n "Inpakken gcc481.tgz... "
tar -zcf $DISTROOT/gcc481.tgz *
echo gereed.

