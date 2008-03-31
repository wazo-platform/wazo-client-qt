# $Revision$
# $Date$
#
# NullSoft Installation System
#

Name "XIVO Switchboard"
OutFile "switchboard-setup.exe"
InstallDir "$PROGRAMFILES\XIVO"
InstallDirRegKey HKLM "Software\XIVO\switchboard" "Install_Dir" 
LicenseText "XIVO Switchboard est distribué sous licence GNU General Public License v2 avec une exception spéciale vous autorisant à le lier à OpenSSL, sous certaines conditions."
# ComponentText "(Choix des composants / sections)"
# DirText "(Choix du répertoire d'installation)"

LicenseData "LICENSE"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"

# Installation of executables
Section "Prog"
SetOutPath $INSTDIR
File "C:\MinGW\bin\mingwm10.dll"
File "C:\cygwin\home\winbuild\openssl-0.9.8g\cryptoeay32-0.9.8.dll"
File "C:\cygwin\home\winbuild\openssl-0.9.8g\ssleay32-0.9.8.dll"
File "LICENSE"
File "GPL_V2.txt"
File "OpenSSL.LICENSE.txt"
File "Qt.GPL.Exception.txt"
File "Qt.GPL.Exception.Addendum.txt"
File "..\switchboard\release\switchboard.exe"
# Write keys in Registry in order for the applications to appear in Add/Remove Programs
WriteRegStr HKLM "Software\XIVO\switchboard" "Install_Dir" "$INSTDIR"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\switchboard" "DisplayName" "XIVO Switchboard"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\switchboard" "UninstallString" '"$INSTDIR\uninstall-switchboard.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\switchboard" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\switchboard" "NoRepair" 1
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
# Do not remove the common CTI files if xivoclient is installed
IfFileExists "$INSTDIR\xivoclient.exe" OnlyUninstallSB
Delete "$INSTDIR\GPL_V2.txt"
Delete "$INSTDIR\OpenSSL.LICENSE.txt"
Delete "$INSTDIR\Qt.GPL.Exception.txt"
Delete "$INSTDIR\Qt.GPL.Exception.Addendum.txt"
Delete "$INSTDIR\LICENSE"
Delete "$INSTDIR\mingwm10.dll"
Delete "$INSTDIR\cryptoeay32-0.9.8.dll"
Delete "$INSTDIR\ssleay32-0.9.8.dll"
OnlyUninstallSB:
Delete "$INSTDIR\switchboard.exe"
Delete "$INSTDIR\uninstall-switchboard.exe"
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\switchboard"
DeleteRegKey HKLM "Software\XIVO\switchboard"
RmDir "$INSTDIR"

Delete "$DESKTOP\switchboard.lnk"

Delete "$SMPROGRAMS\XIVO\switchboard.lnk"
Delete "$SMPROGRAMS\XIVO\Désinstaller switchboard.lnk"
RmDir "$SMPROGRAMS\XIVO"

SectionEnd
