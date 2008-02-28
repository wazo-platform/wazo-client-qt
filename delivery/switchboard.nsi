# $Revision$
# $Date$
#
# NullSoft Installation System
#

Name "XIVO Switchboard"
OutFile "switchboard-setup.exe"
InstallDir "$PROGRAMFILES\XIVO"
LicenseText "XIVO Switchboard est distribué sous licence GNU General Public License v2 avec une exception spéciale vous autorisant à le lier à OpenSSL, sous certaines conditions."
# ComponentText "(Choix des composants / sections)"
# DirText "(Choix du répertoire d'installation)"

LicenseData "GPL_V2.txt"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"

# Installation of executables
Section "Prog"
SetOutPath $INSTDIR
File "C:\MinGW\bin\mingwm10.dll"
File "C:\cygwin\home\winbuild\openssl-0.9.8g\libeay32.dll"
File "C:\cygwin\home\winbuild\openssl-0.9.8g\ssleay32.dll"
File "LICENSE-Switchboard"
File "GPL_V2.txt"
File "OpenSSL.LICENSE.txt"
File "Qt.GPL.Exception.txt"
File "..\switchboard\release\switchboard.exe"
WriteUninstaller "$INSTDIR\uninstall-switchboard.exe"
SectionEnd

# Shortcuts
Section "Shortcuts"
SetOutPath "$SMPROGRAMS\XIVO"
CreateShortCut "$SMPROGRAMS\XIVO\switchboard.lnk" "$INSTDIR\switchboard.exe"
CreateShortCut "$SMPROGRAMS\XIVO\Désinstaller switchboard.lnk" "$INSTDIR\uninstall-switchboard.exe"
CreateShortCut "$DESKTOP\switchboard.lnk" "$INSTDIR\switchboard.exe"
MessageBox MB_OK "Installation terminée."
SectionEnd

# Uninstall
Section "Uninstall"
Delete "$INSTDIR\GPL_V2.txt"
Delete "$INSTDIR\OpenSSL.LICENSE.txt"
Delete "$INSTDIR\Qt.GPL.Exception.txt"
Delete "$INSTDIR\mingwm10.dll"
Delete "$INSTDIR\libeay32.dll"
Delete "$INSTDIR\ssleay32.dll"
Delete "$INSTDIR\switchboard.exe"
Delete "$INSTDIR\uninstall-switchboard.exe"
RmDir "$INSTDIR"

Delete "$DESKTOP\switchboard.lnk"

Delete "$SMPROGRAMS\XIVO\switchboard.lnk"
Delete "$SMPROGRAMS\XIVO\Désinstaller switchboard.lnk"
RmDir "$SMPROGRAMS\XIVO"

SectionEnd
