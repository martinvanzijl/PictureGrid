#!/bin/bash

# ----------------------------------------------------------------------
# Script to make the Linux deployment.
# ----------------------------------------------------------------------

# Copy the executable.
cp ~/Documents/build-PictureGrid-Desktop-Release/PictureGrid ~/AppDirs/PictureGrid.AppDir/PictureGrid/bin/

# Make the deployment.
~/Downloads/build-linuxdeployqt-Desktop-Debug/bin/linuxdeployqt ~/AppDirs/PictureGrid.AppDir/PictureGrid/share/applications/PictureGrid.desktop -verbose=2 -appimage -ignore-glob="/home/martin/*"
