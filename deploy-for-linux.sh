#!/bin/bash

# ----------------------------------------------------------------------
# Script to make the Linux deployment.
# ----------------------------------------------------------------------

# Get the current directory.
CURRENT_DIR=$PWD

# Create the "AppDir" folder.
APP_DIR=~/AppDirs/PictureGrid.AppDir

#APP_DIR="$CURRENT_DIR/AppDirs/PictureGrid.AppDir"
mkdir -p $APP_DIR

# Copy the icon.
ICON_DIR="$APP_DIR/PictureGrid/share/icons/hicolor/256x256/apps"
mkdir -p $ICON_DIR

cp images/app-icon.png $ICON_DIR/PictureGrid.png

# Copy the desktop file.
DESKTOP_DIR="$APP_DIR/PictureGrid/share/applications"
mkdir -p $DESKTOP_DIR

cp linux-deploy-files/PictureGrid.desktop $DESKTOP_DIR 

# Copy the executable.
EXECUTABLE_DIR="$APP_DIR/PictureGrid/bin"
mkdir -p $EXECUTABLE_DIR

cp ../build-PictureGrid-Desktop-Release/PictureGrid $EXECUTABLE_DIR

#cp ~/Documents/build-PictureGrid-Desktop-Release/PictureGrid ~/AppDirs/PictureGrid.AppDir/PictureGrid/bin/

# Make the deployment.
~/Downloads/build-linuxdeployqt-Desktop-Debug/bin/linuxdeployqt "$DESKTOP_DIR/PictureGrid.desktop" -verbose=2 -appimage -ignore-glob="/home/martin/*"

#~/Downloads/build-linuxdeployqt-Desktop-Debug/bin/linuxdeployqt ~/AppDirs/PictureGrid.AppDir/PictureGrid/share/applications/PictureGrid.desktop -verbose=2 -appimage -ignore-glob="/home/martin/*"
