@echo off

SET ROOT=%cd%
SET Path=%ROOT%\miniroot;%Path%

gzip -dc %~1 | tar -C / -pxf -

DEL %~1
