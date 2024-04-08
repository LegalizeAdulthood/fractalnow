; This NSI script is part of the FractalNow program.
; It will install FractalNow into a directory that the user selects,
;
; Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU Lesser General Public License as published by
; the Free Software Foundation; either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

;--------------------------------

!include "x64.nsh"
!include "MUI2.nsh"
!include "LogicLib.nsh"

; You can define PROGRAM_FILES to $PROGRAMFILES32 or $PROGRAMFILES64
; depending on whether FractalNow was built for win32 or win64.
; That way, default install directory will really be correct. 
!define PROGRAM_FILES "$PROGRAMFILES"

!define PROJECT_SIMPLE_NAME "fractalnow"
!define PROJECT_PRETTY_NAME "FractalNow"
!define GUI_SIMPLE_NAME "qfractalnow"
!define GUI_PRETTY_NAME "QFractalNow"
!define GUI_BINARY "${GUI_SIMPLE_NAME}.exe"
!define SRCDIR ".."
!define PREFIX "."
!define UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROJECT_PRETTY_NAME}"

!define INSTALLER_NAME "${PROJECT_SIMPLE_NAME}-${VERSION}.exe"

!define VERSION "0.8.0"

!include "languages\${PROJECT_SIMPLE_NAME}_en.nsh"

; The name of the installer
Name ${PROJECT_PRETTY_NAME}

; The file to write
OutFile ${INSTALLER_NAME}

; The default installation directory
InstallDir ${PROGRAM_FILES}\${PROJECT_PRETTY_NAME}

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSIS_${PROJECT_PRETTY_NAME}" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

!define MUI_WELCOMEPAGE_TITLE "$(TITLE)"
!define MUI_WELCOMEPAGE_TEXT "$(WELCOME_TEXT)"
!define MUI_FINISHPAGE_TITLE "$(TITLE)"

!insertmacro MUI_PAGE_WELCOME

!define MUI_LICENSEPAGE_CHECKBOX
!insertmacro MUI_PAGE_LICENSE "${SRCDIR}\COPYING.LESSER"

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH
  
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

VIProductVersion "${VERSION}.0"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Marc Pegon"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PROJECT_PRETTY_NAME} installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (C) 2011-2012 Marc Pegon"
VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" "${INSTALLER_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PROJECT_PRETTY_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}"

;--------------------------------

; The stuff to install
Section "${PROJECT_PRETTY_NAME}"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r "${PREFIX}\${PROJECT_SIMPLE_NAME}\*"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\NSIS_${PROJECT_PRETTY_NAME} "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "${UNINST_KEY}" "DisplayName" "${PROJECT_PRETTY_NAME}"
  WriteRegStr HKLM "${UNINST_KEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${UNINST_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr HKLM "${UNINST_KEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "${UNINST_KEY}" "Publisher" "Marc Pegon"
  WriteRegStr SHCTX "${UNINST_KEY}" "HelpLink" "http://sourceforge.net/projects/fractalnow/"
  WriteRegStr SHCTX "${UNINST_KEY}" "URLInfoAbout" "http://sourceforge.net/projects/fractalnow/"
  WriteRegStr SHCTX "${UNINST_KEY}" "URLUpdateInfo" "http://sourceforge.net/projects/fractalnow/"
  WriteRegDWORD HKLM "${UNINST_KEY}" "NoModify" 1
  WriteRegDWORD HKLM "${UNINST_KEY}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\${PROJECT_PRETTY_NAME}"
  CreateShortCut "$SMPROGRAMS\${PROJECT_PRETTY_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\${PROJECT_PRETTY_NAME}\${GUI_PRETTY_NAME}.lnk" "$INSTDIR\${GUI_BINARY}" "" "$INSTDIR\${GUI_BINARY}" 0
  CreateShortCut "$SMPROGRAMS\${PROJECT_PRETTY_NAME}\${GUI_PRETTY_NAME}.lnk" "$INSTDIR\${GUI_BINARY}" "" "$INSTDIR\${GUI_BINARY}" 0
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROJECT_PRETTY_NAME}"
  DeleteRegKey HKLM SOFTWARE\NSIS_${PROJECT_PRETTY_NAME}

  ; Remove files and uninstaller
  Delete $INSTDIR\*

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${PROJECT_PRETTY_NAME}\*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\${PROJECT_PRETTY_NAME}"
  RMDir /r "$INSTDIR"

SectionEnd

