# $Revision$
# $Date$
#
# NullSoft Installation System
#

!define ODPATH ..\..\openssl-0.9.8g-mingw\openssl-0.9.8g
!define REG_UNINST Software\Microsoft\Windows\CurrentVersion\Uninstall\xivoclient
!include settings.nsh
!define FULLVERSION "${XIVOVER} (svn ${SVNVER})"
Name "XIVO Client ${FULLVERSION}"
OutFile "xivoclient-setup-win32.exe"
InstallDirRegKey HKLM "Software\XIVO\xivoclient" "Install_Dir" 
LicenseText "XIVO Client est distribué sous licence GNU General Public License v2 avec une exception spéciale vous autorisant à le lier à OpenSSL, sous certaines conditions."
# ComponentText "(Choix des composants / sections)"
# DirText "(Choix du répertoire d'installation)"

Page license
LicenseData "LICENSE"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"

Page directory
DirText "Spécifier l'emplacement d'installation de XIVO Client."
InstallDir "$PROGRAMFILES\XIVO"

Page instfiles
# Installation of executables
Section "Prog"
SetOutPath $INSTDIR
File C:\MinGW\bin\mingwm10.dll
File ${ODPATH}\cryptoeay32-0.9.8.dll
File ${ODPATH}\ssleay32-0.9.8.dll
!ifdef QTBINPATH
File ${QTBINPATH}\QtCore4.dll
File ${QTBINPATH}\QtGui4.dll
File ${QTBINPATH}\QtNetwork4.dll
File ${QTBINPATH}\QtXml4.dll
File ${QTBINPATH}\QtWebKit4.dll
File ${QTBINPATH}\QtSql4.dll
!endif
!ifdef QTPLUGINSPATH
File ${QTPLUGINSPATH}\sqldrivers\qsqlite4.dll
File ${QTPLUGINSPATH}\sqldrivers\qsqlodbc4.dll
!endif
File LICENSE
File GPL_V2.txt
File OpenSSL.LICENSE.txt
File Qt.GPL.Exception.txt
File Qt.GPL.Exception.Addendum.txt
File ..\bin\xivoclient.exe
File ..\bin\xivoclientbaselib1.dll
SetOutPath $INSTDIR\plugins
File ..\bin\plugins\outlookplugin.dll
File ..\bin\plugins\videoxletplugin.dll
File ..\bin\plugins\xletnullplugin.dll
File ..\bin\plugins\xletwebplugin.dll
# Write keys in Registry in order for the applications to appear in Add/Remove Programs
WriteRegStr HKLM "Software\XIVO\xivoclient" "Install_Dir" "$INSTDIR"
WriteRegStr HKLM ${REG_UNINST} "DisplayName" "XIVO Client"
WriteRegStr HKLM ${REG_UNINST} "UninstallString" '"$INSTDIR\uninstall-xivoclient.exe"'
WriteRegDWORD HKLM ${REG_UNINST} "NoModify" 1
WriteRegDWORD HKLM ${REG_UNINST} "NoRepair" 1
WriteUninstaller "$INSTDIR\uninstall-xivoclient.exe"
SectionEnd

# Shortcuts
Section "Shortcuts"
SetOutPath "$SMPROGRAMS\XIVO"
CreateShortCut "$SMPROGRAMS\XIVO\xivoclient.lnk"  "$INSTDIR\xivoclient.exe"
CreateShortCut "$SMPROGRAMS\XIVO\Désinstaller xivoclient ${FULLVERSION}.lnk"  "$INSTDIR\uninstall-xivoclient.exe"
CreateShortCut "$DESKTOP\xivoclient.lnk"  "$INSTDIR\xivoclient.exe"
IfSilent +2
MessageBox MB_OK "Installation terminée."
SectionEnd

# Uninstall
Section "Uninstall"
Delete "$INSTDIR\GPL_V2.txt"
Delete "$INSTDIR\OpenSSL.LICENSE.txt"
Delete "$INSTDIR\Qt.GPL.Exception.txt"
Delete "$INSTDIR\Qt.GPL.Exception.Addendum.txt"
Delete "$INSTDIR\LICENSE"
Delete "$INSTDIR\mingwm10.dll"
Delete "$INSTDIR\cryptoeay32-0.9.8.dll"
Delete "$INSTDIR\ssleay32-0.9.8.dll"
Delete "$INSTDIR\xivoclient.exe"
Delete "$INSTDIR\xivoclientbaselib1.dll"
Delete "$INSTDIR\uninstall-xivoclient.exe"
!ifdef QTBINPATH
Delete "$INSTDIR\QtCore4.dll"
Delete "$INSTDIR\QtGui4.dll"
Delete "$INSTDIR\QtNetwork4.dll"
Delete "$INSTDIR\QtXml4.dll"
Delete "$INSTDIR\QtWebKit4.dll"
Delete "$INSTDIR\QtSql4.dll"
!endif
!ifdef QTPLUGINSPATH
Delete "$INSTDIR\qsqlite4.dll"
Delete "$INSTDIR\qsqlodbc4.dll"
!endif
Delete "$INSTDIR\plugins\outlookplugin.dll"
Delete "$INSTDIR\plugins\videoxletplugin.dll"
Delete "$INSTDIR\plugins\xletnullplugin.dll"
Delete "$INSTDIR\plugins\xletwebplugin.dll"
DeleteRegKey HKLM ${REG_UNINST} 
DeleteRegKey HKLM "Software\XIVO\xivoclient"
RmDir "$INSTDIR\plugins"
RmDir "$INSTDIR"

Delete "$DESKTOP\xivoclient.lnk"

Delete "$SMPROGRAMS\XIVO\xivoclient.lnk"
Delete "$SMPROGRAMS\XIVO\Désinstaller xivoclient ${FULLVERSION}.lnk"
RmDir "$SMPROGRAMS\XIVO"

SectionEnd
