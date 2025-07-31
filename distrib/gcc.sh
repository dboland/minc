if ! cd "$PKGROOT/gcc-release"; then
        exit 1
fi

ln -sf gcc ./usr/bin/cc
#ln -sf liblto_plugin.so.0.0 ./usr/libexec/gcc/i686-pe-openbsd/4.8.1/liblto_plugin.so

#mkdir -p ./sbin
#cp /mnt/c/MinGW/bin/libstdc++-6.dll ./sbin/
#cp /mnt/c/MinGW/bin/libgcc_s_dw2-1.dll ./sbin/

rm ./usr/bin/i686-openbsd-pe-*

echo -n "Compressing gcc481.tgz... "
tar -zcf $DISTROOT/gcc481.tgz *
echo done.
