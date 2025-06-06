@echo off

SET ROOT=%cd%
SET Path=%ROOT%\miniroot;%ROOT%\usr\lib;%Path%

gzip -dc %~1 | tar -C / -xf -

DEL %~1
