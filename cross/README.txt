These scripts are used to package the XiVO Client for different platforms.

If you want to package an anterior version, rename the cross directory to anything and checkout the git version you want.

*** Windows ***

You will need :
- Cygwin
- QtSDK with MinGW
- NSIS

Compile the XiVO Client by executing make in Cygwin and following the instructions.
More details at https://wiki.xivo.fr/index.php/XiVO_1.2-Skaro/CTI_XiVO_Client_Qt_Builder_Windows.

Use the xc-package-win.sh script with Cygwin or a bash-like terminal.
Run it without arguments to see what it expects.

Example of usage :
./xc-package-windows.sh .. /cygdrive/c/QtSDK/mingw/ /cygdrive/c/QtSDK/Desktop/Qt/4.7.4/mingw/

This will generate the .nsi file recipe to create the Windows installer with NSIS.
