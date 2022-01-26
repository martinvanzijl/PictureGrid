# Deployment

## Linux

To create the App Image file for Linux, do the following:

1. Install the libraries required for "LinuxDeployQt". On Ubuntu:

	```sudo apt install qtwebengine5-dev```

1. Install the "patchelf" program. On Ubuntu:

	```sudo apt install patchelf```

1. Download the App Image for the "appimagetool" software. You can get this from GitHub [here](https://appimage.github.io/appimagetool/). Put the image in an executable directory, and make the file executable:

	```chmod u+x appimagetool```

1. Download the code for "LinuxDeployQt" [using this link](https://github.com/probonopd/linuxdeployqt/releases/tag/continuous). For example:

	```wget https://codeload.github.com/probonopd/linuxdeployqt/tar.gz/refs/tags/continuous```

1. Open the "LinuxDeployQt" project in Qt Creator.
1. Build the debug version.
1. Put the executable you built in "~/Downloads/build-linuxdeployqt-Desktop-Debug/bin/".
1. Go to the PictureGrid code folder and run "deploy-for-linux.sh".

You should see the App Image created in the same folder.
