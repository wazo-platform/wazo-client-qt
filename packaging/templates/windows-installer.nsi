SetCompressor /FINAL /SOLID lzma

!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!include MultiUser.nsh
!include "MUI2.nsh"
!define MUI_ICON "xivoicon.ico"
!define MUI_UNICON "xivoicon_un.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP xivo.bmp
!define MUI_HEADERIMAGE_UNBITMAP xivo.bmp

Name "XiVO Client ${XC_VERSION}"
OutFile "xivoclient-${XC_VERSION}-x86.exe"

InstallDir $PROGRAMFILES\XiVO\

; Request application privileges for Windows Vista
RequestExecutionLevel admin
ShowInstDetails show
ShowUninstDetails show

!insertmacro MULTIUSER_PAGE_INSTALLMODE
!define MUI_PAGE_HEADER_SUBTEXT $(XIVO_LICENSE_TEXT)
!insertmacro MUI_PAGE_LICENSE gpl.txt

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Var Dialog
Var CheckboxShortcutDesktop
Var CheckboxShortcutStart
Var CheckboxShortcutDesktopState
Var CheckboxShortcutStartState

Page custom installMiscPage installMiscPageLeave ;Custom page

Function installMiscPage
  !insertmacro MUI_HEADER_TEXT $(SHORTCUT_PAGE_TITLE) ""
  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_CreateCheckbox} 0 10u 100% 10u $(SHORTCUT_DESKTOP)
  Pop $CheckboxShortcutDesktop

  ${NSD_CreateCheckbox} 0 25u 100% 10u $(SHORTCUT_STARTMENU)
  Pop $CheckboxShortcutStart

  ${NSD_Check} $CheckboxShortcutDesktop
  ${NSD_Check} $CheckboxShortcutStart

  nsDialogs::Show

FunctionEnd

Function installMiscPageLeave
  ${NSD_GetState} $CheckboxShortcutDesktop $CheckboxShortcutDesktopState
  ${NSD_GetState} $CheckboxShortcutStart $CheckboxShortcutStartState
FunctionEnd

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;Languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

Function .onInit
  !insertmacro MULTIUSER_INIT
  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

!macro RegisterProtocolSection EXT
  Section ${EXT}
    SectionIn 1 3
    AppAssocReg::SetAppAsDefault "XiVO Client" "${EXT}" "protocol"
  SectionEnd
!macroend

!define REG_UNINST Software\Microsoft\Windows\CurrentVersion\Uninstall\xivoclient

; The stuff to install
Section "Qt libraries" QtLib ; components page
  SectionIn RO
  SetOutPath $INSTDIR

  File /r qtlibs\*
SectionEnd ; end the section

Section "XiVO client executable" XivoclientExe ; components page
  SectionIn RO
  SetOutPath $INSTDIR

  File /r xivoclient\*

  WriteRegStr HKLM "Software\XiVO\xivoclient" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "${REG_UNINST}" "DisplayName" "XiVO Client"
  WriteRegStr HKLM "${REG_UNINST}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${REG_UNINST}" "Publisher" "Avencall"
  WriteRegStr HKLM "${REG_UNINST}" "DisplayVersion" "${XC_VERSION}"
  WriteRegDWORD HKLM "${REG_UNINST}" "EstimatedSize" "102400"
  WriteRegDWORD HKLM "${REG_UNINST}" "NoModify" 1
  WriteRegDWORD HKLM "${REG_UNINST}" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"
  # Associate with tel: and callto: protocol
  WriteRegStr HKCR "tel" "" "URL:tel Protocol"
  WriteRegStr HKCR "tel" "URL Protocol" ""
  WriteRegStr HKCR "tel\shell\open\command" "" "$INSTDIR\xivoclient.exe %1"
  WriteRegStr HKCR "callto" "" "URL:callto Protocol"
  WriteRegStr HKCR "callto" "URL Protocol" ""
  WriteRegStr HKCR "callto\shell\open\command" "" "$INSTDIR\xivoclient.exe %1"

  ${If} $CheckboxShortcutDesktopState == ${BST_CHECKED}
  ${OrIf} ${Silent}
    CreateShortCut "$DESKTOP\XiVO Client.lnk" "$INSTDIR\xivoclient.exe"
  ${EndIf}

  ${If} $CheckboxShortcutStartState == ${BST_CHECKED}
  ${OrIf} ${Silent}
     CreateDirectory "$SMPROGRAMS\XiVO"
     CreateShortCut  "$SMPROGRAMS\XiVO\\$(START_XIVOCLIENT)"  "$INSTDIR\xivoclient.exe"
     CreateShortCut  "$SMPROGRAMS\XiVO\\$(REMOVE_XIVOCLIENT)" "$INSTDIR\uninstall.exe"
  ${EndIf}

    WriteRegStr HKCR "XiVO Client" "" "XiVO Click2Call Protocol"
    WriteRegStr HKCR "XiVO Client\DefaultIcon" "" "$INSTDIR\xivoclient.exe,0"
    WriteRegStr HKCR "XiVO Client\shell\open\command" "" "$INSTDIR\xivoclient.exe %1"
    WriteRegStr HKLM "Software\RegisteredApplications" "XiVO Client" "Software\Clients\Internet Call\XiVO Client\Capabilities"
    WriteRegStr HKLM "Software\Clients\Internet Call\XiVO Client\Capabilities" "ApplicationName" "XiVO Client"
    WriteRegStr HKLM "Software\Clients\Internet Call\XiVO Client\Capabilities" "ApplicationDescription" "Client for the XiVO ipbx (http://xivo.io)"
    WriteRegStr HKLM "Software\Clients\Internet Call\XiVO Client\Capabilities\UrlAssociations" "tel" "XiVO Client"
    WriteRegStr HKLM "Software\Clients\Internet Call\XiVO Client\Capabilities\UrlAssociations" "callto" "XiVO Client"
SectionEnd ; end the section

SectionGroup $(ProtocolAssociation)
    !insertmacro RegisterProtocolSection "tel"
    !insertmacro RegisterProtocolSection "callto"
SectionGroupEnd

Section "Uninstall"
  SetShellVarContext all
  ; remove any shortcut
  Delete "$DESKTOP\XiVO Client.lnk"
  Delete "$SMPROGRAMS\XiVO\*"
  RmDir "$SMPROGRAMS\XiVO"
  ; remove the uri association
  DeleteRegKey HKLM ${REG_UNINST}
  DeleteRegKey HKLM "Software\XiVO\xivoclient"
  DeleteRegKey HKCR "XiVO Client"
  DeleteRegValue HKLM "Software\RegisteredApplications" "XiVO Client"
  DeleteRegKey HKLM "Software\Clients\Internet Call\XiVO Client"

  ; remove the program dir
  Delete $INSTDIR\uninstall.exe
  RMDir /r $INSTDIR\imageformats
  RMDir /r $INSTDIR\platforms
  RMDir /r $INSTDIR\sqldrivers
  RMDir /r $INSTDIR\plugins
  Delete $INSTDIR\qt.conf
  RMDir /r $INSTDIR\Qt
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\xivoclient.exe

  RMDir $INSTDIR

SectionEnd ; end the section

Function un.onInit
  !insertMacro MULTIUSER_UNINIT
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

LangString SuccessInstall ${LANG_ENGLISH}  "The installation was successful, do you want to start the XiVO client right now ?"
LangString SuccessInstall ${LANG_FRENCH}  "L'installation a réussi, voulez-vous lancer le XiVO client maintenant ?"

Function .onInstSuccess
  MessageBox MB_YESNO  $(SuccessInstall) /SD IDNO IDNO NoReadme
  Exec '"$WINDIR\explorer.exe" "$INSTDIR\xivoclient.exe"'
  NoReadme:
FunctionEnd

;Language strings

LangString XIVO_LICENSE_TEXT ${LANG_ENGLISH} "XiVO Client is licensed under the GNU GPLv3 with a special exception to allow the link with OpenSSL."
LangString XIVO_LICENSE_TEXT ${LANG_FRENCH}  "XiVO Client est distribué sous licence GNU GPLv3 avec une exception particulière vous autorisant à le lier à OpenSSL."

LangString DESC_QtLib ${LANG_ENGLISH} "Qt libraries (cross-platform application and UI framework)"
LangString DESC_QtLib ${LANG_FRENCH}  "Bibliothèque Qt (framework multi plateforme gérant l'IHM, le réseau, ...) "

LangString DESC_xivoclientExe ${LANG_ENGLISH} "XiVO client executable"
LangString DESC_xivoclientExe ${LANG_FRENCH}  "Exécutable XiVO client"

LangString SHORTCUT_PAGE_TITLE ${LANG_ENGLISH} "Shortcuts"
LangString SHORTCUT_PAGE_TITLE ${LANG_FRENCH}  "Raccourcis"

LangString SHORTCUT_DESKTOP ${LANG_ENGLISH} "Add a Shortcut on &Desktop"
LangString SHORTCUT_DESKTOP ${LANG_FRENCH}  "Ajouter un raccourci sur le &Bureau"

LangString SHORTCUT_STARTMENU ${LANG_ENGLISH} "Add a Shortcut in Start &Menu"
LangString SHORTCUT_STARTMENU ${LANG_FRENCH}  "Ajouter un raccourci dans le &Menu Démarrer"

LangString START_XIVOCLIENT ${LANG_ENGLISH} "XiVO Client.lnk"
LangString START_XIVOCLIENT ${LANG_FRENCH}  "XiVO Client.lnk"

LangString REMOVE_XIVOCLIENT ${LANG_ENGLISH} "Uninstall XiVO Client.lnk"
LangString REMOVE_XIVOCLIENT ${LANG_FRENCH}  "Désinstaller XiVO Client.lnk"

LangString ProtocolAssociation ${LANG_ENGLISH}  "Protocol Association"
LangString ProtocolAssociation ${LANG_FRENCH}  "Association de protocoles"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${QtLib} $(DESC_QtLib)
  !insertmacro MUI_DESCRIPTION_TEXT ${XivoclientExe} $(DESC_xivoclientExe)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

VIProductVersion "9.4.8.6"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "XiVO client"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Computer Telephony Integration (CTI) client for XiVO"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Avencall"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "XiVO client installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${XC_VERSION}-${GIT_HASH}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2007-2014 Avencall"
