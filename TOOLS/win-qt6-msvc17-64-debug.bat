@echo off

set QTVER=6
set BUILDER=msvc17
set TARGET_PLATFORM=win32
set SIZE=64
set GENERATOR="Visual Studio 17 2022"
set ENV_PATH="c:/Qt-6.3.0"
set CONFIGURATION="debug"

::----------------------------
:: DO NOT EDIT THE TEXT BELOW!
::----------------------------

set QT=qt%QTVER%
set TARGET=%TARGET_PLATFORM%-%QT%-%BUILDER%-%SIZE%-%CONFIGURATION%

mkdir BUILD
cd BUILD
mkdir %TARGET%
cd %TARGET%

cmake -DQT=%QTVER% -DTARGET_DIR=%TARGET% -DTARGET_PLATFORM=%TARGET_PLATFORM% -DENV_PATH=%ENV_PATH% -DCONFIGURATION=%CONFIGURATION% -G %GENERATOR% ../..

cmake --build . --config %CONFIGURATION%
