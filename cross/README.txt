These scripts are used to package the XiVO Client for different platforms.

If you want to package an anterior version, rename the cross directory to anything and checkout the git version you want.

These scripts are automatically called when creating packages.

See http://documentation.xivo.fr for more info about packaging.

Developers
==========

*** Windows ***

You will need :
- Cygwin
- QtSDK with MinGW
- NSIS

Compile the XiVO Client (see http://documentation.xivo.fr).

Use the xc-package-win.sh script with Cygwin or a bash-like terminal.
Run it without arguments to see what it expects.

Example of usage :
./xc-package-windows.sh .. /cygdrive/c/QtSDK/mingw/ /cygdrive/c/QtSDK/Desktop/Qt/4.7.4/mingw/

This will generate the .nsi file recipe to create the Windows installer with NSIS.
