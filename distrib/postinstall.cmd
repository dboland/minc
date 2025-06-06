@ECHO OFF

SET ROOT=%cd%
SET Path=%ROOT%\miniroot;%ROOT%\usr\lib;%Path%

miniroot\sh -c /sbin/setup.sh
regedit /S .\etc\console.reg
regedit /S .\etc\color\%~1.reg
