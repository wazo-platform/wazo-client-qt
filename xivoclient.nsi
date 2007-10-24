# $Revision$
# $Date$

Name "XIVO Client"
OutFile "xivoclient_setup.exe"

InstallDir "$PROGRAMFILES\XIVO"

# Installation of executables 
Section "Prog"

SetOutPath $INSTDIR

File "C:\MinGW\bin\mingwm10.dll"
File "C:\WINDOWS\system32\libeay32.dll"
File "C:\WINDOWS\system32\ssleay32.dll"
File "C:\WINDOWS\system32\msvcr71.dll"
File "xivoclient\release\xivoclient.exe"
 
SectionEnd

# Shortcuts 
Section "Shortcuts"

SetOutPath "$SMPROGRAMS\XIVO"
# CreateShortCut "$SMPROGRAMS\XIVO\xivoclient.lnk" "$INSTDIR\xivoclient.exe"
CreateShortCut "$DESKTOP\xivoclient.lnk" "$INSTDIR\xivoclient.exe"

MessageBox MB_OK "Installation terminée"

SectionEnd


