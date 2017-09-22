SetCompressor /FINAL /SOLID lzma

!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!include MultiUser.nsh
!include "MUI2.nsh"
!define MUI_ICON "wazoicon.ico"
!define MUI_UNICON "wazoicon.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP wazo.bmp
!define MUI_HEADERIMAGE_UNBITMAP wazo.bmp
!include nsProcess.nsh

Name "Wazo Client ${XC_VERSION}"
OutFile "wazoclient-${XC_VERSION}-x86.exe"

InstallDir $PROGRAMFILES\Wazo\

; Request application privileges for Windows Vista
RequestExecutionLevel admin
ShowInstDetails show
ShowUninstDetails show

!insertmacro MULTIUSER_PAGE_INSTALLMODE
!define MUI_PAGE_HEADER_SUBTEXT $(WAZO_LICENSE_TEXT)
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

  ${nsProcess::KillProcess} "wazoclient.exe" $R0
FunctionEnd

!macro RegisterProtocolSection EXT
  Section ${EXT}
    SectionIn 1 3
    AppAssocReg::SetAppAsDefault "Wazo Client" "${EXT}" "protocol"
  SectionEnd
!macroend

!define REG_UNINST Software\Microsoft\Windows\CurrentVersion\Uninstall\wazoclient

; The stuff to install
Section "Qt libraries" QtLib ; components page
  SectionIn RO
  SetOutPath $INSTDIR

  File /r qtlibs\*
SectionEnd ; end the section

Section "Wazo client executable" WazoclientExe ; components page
  SectionIn RO
  SetOutPath $INSTDIR

  File /r wazoclient\*

  WriteRegStr HKLM "Software\Wazo\wazoclient" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "${REG_UNINST}" "DisplayName" "Wazo Client"
  WriteRegStr HKLM "${REG_UNINST}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${REG_UNINST}" "Publisher" "The Wazo Authors"
  WriteRegStr HKLM "${REG_UNINST}" "DisplayVersion" "${XC_VERSION}"
  WriteRegDWORD HKLM "${REG_UNINST}" "EstimatedSize" "102400"
  WriteRegDWORD HKLM "${REG_UNINST}" "NoModify" 1
  WriteRegDWORD HKLM "${REG_UNINST}" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"
  # Associate with tel: and callto: protocol
  WriteRegStr HKCR "tel" "" "URL:tel Protocol"
  WriteRegStr HKCR "tel" "URL Protocol" ""
  WriteRegStr HKCR "tel\shell\open\command" "" "$INSTDIR\wazoclient.exe %1"
  WriteRegStr HKCR "callto" "" "URL:callto Protocol"
  WriteRegStr HKCR "callto" "URL Protocol" ""
  WriteRegStr HKCR "callto\shell\open\command" "" "$INSTDIR\wazoclient.exe %1"

  ${If} $CheckboxShortcutDesktopState == ${BST_CHECKED}
  ${OrIf} ${Silent}
    CreateShortCut "$DESKTOP\Wazo Client.lnk" "$INSTDIR\wazoclient.exe"
  ${EndIf}

  ${If} $CheckboxShortcutStartState == ${BST_CHECKED}
  ${OrIf} ${Silent}
     CreateDirectory "$SMPROGRAMS\Wazo"
     CreateShortCut  "$SMPROGRAMS\Wazo\\$(START_WAZOCLIENT)"  "$INSTDIR\wazoclient.exe"
     CreateShortCut  "$SMPROGRAMS\Wazo\\$(REMOVE_WAZOCLIENT)" "$INSTDIR\uninstall.exe"
  ${EndIf}

    WriteRegStr HKCR "Wazo Client" "" "Wazo Click2Call Protocol"
    WriteRegStr HKCR "Wazo Client\DefaultIcon" "" "$INSTDIR\wazoclient.exe,0"
    WriteRegStr HKCR "Wazo Client\shell\open\command" "" "$INSTDIR\wazoclient.exe %1"
    WriteRegStr HKLM "Software\RegisteredApplications" "Wazo Client" "Software\Clients\Internet Call\Wazo Client\Capabilities"
    WriteRegStr HKLM "Software\Clients\Internet Call\Wazo Client\Capabilities" "ApplicationName" "Wazo Client"
    WriteRegStr HKLM "Software\Clients\Internet Call\Wazo Client\Capabilities" "ApplicationDescription" "Client for the Wazo ipbx (http://wazo.community)"
    WriteRegStr HKLM "Software\Clients\Internet Call\Wazo Client\Capabilities\UrlAssociations" "tel" "Wazo Client"
    WriteRegStr HKLM "Software\Clients\Internet Call\Wazo Client\Capabilities\UrlAssociations" "callto" "Wazo Client"
SectionEnd ; end the section

SectionGroup $(ProtocolAssociation)
    !insertmacro RegisterProtocolSection "tel"
    !insertmacro RegisterProtocolSection "callto"
SectionGroupEnd

Section "Uninstall"
  SetShellVarContext all
  ; remove any shortcut
  Delete "$DESKTOP\Wazo Client.lnk"
  Delete "$SMPROGRAMS\Wazo\*"
  RmDir "$SMPROGRAMS\Wazo"
  ; remove the uri association
  DeleteRegKey HKLM ${REG_UNINST}
  DeleteRegKey HKLM "Software\Wazo\wazoclient"
  DeleteRegKey HKCR "Wazo Client"
  DeleteRegValue HKLM "Software\RegisteredApplications" "Wazo Client"
  DeleteRegKey HKLM "Software\Clients\Internet Call\Wazo Client"

  ; remove the program dir
  RMDir /r $INSTDIR\imageformats
  RMDir /r $INSTDIR\platforms
  RMDir /r $INSTDIR\plugins
  RMDir /r $INSTDIR\sqldrivers
  RMDir /r $INSTDIR\translations
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\qt.conf
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\wazoclient.exe

  ; content present only in earlier versions
  RMDir /r $INSTDIR\Qt

  RMDir $INSTDIR

SectionEnd ; end the section

Function un.onInit
  !insertMacro MULTIUSER_UNINIT
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

LangString SuccessInstall ${LANG_ENGLISH}  "The installation was successful, do you want to start the Wazo client right now ?"
LangString SuccessInstall ${LANG_FRENCH}  "L'installation a réussi, voulez-vous lancer le Wazo client maintenant ?"

Function .onInstSuccess
  MessageBox MB_YESNO  $(SuccessInstall) /SD IDNO IDNO NoReadme
  Exec '"$WINDIR\explorer.exe" "$INSTDIR\wazoclient.exe"'
  NoReadme:
FunctionEnd

;Language strings

LangString WAZO_LICENSE_TEXT ${LANG_ENGLISH} "Wazo Client is licensed under the GNU GPLv3 with a special exception to allow the link with OpenSSL."
LangString WAZO_LICENSE_TEXT ${LANG_FRENCH}  "Wazo Client est distribué sous licence GNU GPLv3 avec une exception particulière vous autorisant à le lier à OpenSSL."

LangString DESC_QtLib ${LANG_ENGLISH} "Qt libraries (cross-platform application and UI framework)"
LangString DESC_QtLib ${LANG_FRENCH}  "Bibliothèque Qt (framework multi plateforme gérant l'IHM, le réseau, ...) "

LangString DESC_wazoclientExe ${LANG_ENGLISH} "Wazo client executable"
LangString DESC_wazoclientExe ${LANG_FRENCH}  "Exécutable Wazo client"

LangString SHORTCUT_PAGE_TITLE ${LANG_ENGLISH} "Shortcuts"
LangString SHORTCUT_PAGE_TITLE ${LANG_FRENCH}  "Raccourcis"

LangString SHORTCUT_DESKTOP ${LANG_ENGLISH} "Add a Shortcut on &Desktop"
LangString SHORTCUT_DESKTOP ${LANG_FRENCH}  "Ajouter un raccourci sur le &Bureau"

LangString SHORTCUT_STARTMENU ${LANG_ENGLISH} "Add a Shortcut in Start &Menu"
LangString SHORTCUT_STARTMENU ${LANG_FRENCH}  "Ajouter un raccourci dans le &Menu Démarrer"

LangString START_WAZOCLIENT ${LANG_ENGLISH} "Wazo Client.lnk"
LangString START_WAZOCLIENT ${LANG_FRENCH}  "Wazo Client.lnk"

LangString REMOVE_WAZOCLIENT ${LANG_ENGLISH} "Uninstall Wazo Client.lnk"
LangString REMOVE_WAZOCLIENT ${LANG_FRENCH}  "Désinstaller Wazo Client.lnk"

LangString ProtocolAssociation ${LANG_ENGLISH}  "Protocol Association"
LangString ProtocolAssociation ${LANG_FRENCH}  "Association de protocoles"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${QtLib} $(DESC_QtLib)
  !insertmacro MUI_DESCRIPTION_TEXT ${WazoclientExe} $(DESC_wazoclientExe)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

VIProductVersion "9.4.8.6"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Wazo client"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Computer Telephony Integration (CTI) client for Wazo"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "The Wazo Authors"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Wazo client installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${XC_VERSION}-${GIT_HASH}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2007-2017 The Wazo authors"
