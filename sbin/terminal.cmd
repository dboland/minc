@ECHO OFF

CD /D %~dp0..

SET MINCROOT=%cd%
SET Path=%MINCROOT%\bin;%MINCROOT%\usr\lib;%Path%
SET LANG=nl_NL
:: SET LC_ALL=nl_NL.ISO8859-1
SET LC_ALL=en_US.CP850

SUBST S: %MINCROOT%

:: input code page
:: CHCP 1252 >NUL
:: CHCP 65001 >NUL

START "MinC" %MINCROOT%\sbin\minc.exe
