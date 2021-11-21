REM This is to make the Windows deployment for the program.

REM Debug.
@echo on

REM Version.
set VERSION=3

REM Set variables.
set EXE_FILE=PictureGrid.exe
set BUILD_FOLDER=C:\Users\Martin Laptop\Documents\QT Projects\build-PictureGrid-Desktop_Qt_5_6_0_MinGW_32bit-Release\release
set INSTALL_FOLDER=C:\Users\Martin Laptop\Documents\QT Projects\PictureGrid-version-%VERSION%

REM Make installation folder.
mkdir "%INSTALL_FOLDER%"

REM Copy executable.
copy "%BUILD_FOLDER%\%EXE_FILE%" "%INSTALL_FOLDER%"

REM Add Qt dependencies.
call C:\Qt\5.6\mingw49_32\bin\qtenv2.bat
call windeployqt.exe "%INSTALL_FOLDER%\%EXE_FILE%"

REM Wait for user input.
cd "%INSTALL_FOLDER%"
pause
