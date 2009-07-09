; Choose if extras (installers for python etc.) should be
; bundled inside the installer.

;!define IncludeExtras

; Compression - Keep this at top. Can lead to errors otherwise.
SetCompressor /SOLID lzma

; Allows use of logical operations like ${If} $0 != ""
!include "LogicLib.nsh"

; This extension allows us to create file-associations in the registry.
; In other words, the user can click a *.g file, and MOOSE will execute
; the script.
!include "RegExtension.nsh"

; This extension allows us to modify environment variables. This is
; used here to include the moose binary in the execution path.
!include "EnvVarUpdate.nsh"

; Check for Admin priveleges
!include "ConfirmAdmin.nsh"

; Name of program
Name "Moose Beta 1.2.0"

; The file to write
OutFile "moose-beta-1.2.0.exe"

; The default installation directory
InstallDir $PROGRAMFILES\MOOSE

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\MOOSE" "Install_Dir"

; Request application privileges for Windows Vista and Windows 7
RequestExecutionLevel admin

LicenseText "GNU LESSER GENERAL PUBLIC LICENSE"
LicenseData "Payload\Docs\COPYING.LIB.txt"

; MUI 1.67 compatible
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING

; MUI Settings / Icons
;!define MUI_ICON "moose.ico"
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"

; MUI Settings / Header
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-uninstall-r.bmp"

; MUI Settings / Wizard
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH
; Language files
!insertmacro MUI_LANGUAGE "English"

Function .onInit
	Push $0
	Push $R0
	
	Call ConfirmAdmin
	
	ReadRegStr $R0 HKLM \
	"Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" \
	"UninstallString"
	StrCmp $R0 "" done
	
	MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
	"MOOSE is already installed. $\n$\nClick `OK` to remove the \
	previous version or `Cancel` to cancel this upgrade." \
	IDOK uninst
	Abort
	
	;Run the uninstaller
	uninst:
	ClearErrors
	ExecWait '$R0 _?=$INSTDIR' ;Do not copy the uninstaller to a temp file
	
	IfErrors no_remove_uninstaller done
	;You can either use Delete /REBOOTOK in the uninstaller or add some code
	;here to remove the uninstaller. Use a registry key to check
	;whether the user has chosen to uninstall. If you are using an uninstaller
	;components page, make sure all sections are uninstalled.
	no_remove_uninstaller:
	done:
	
	SetOutPath $TEMP
	File /oname=spltmp.bmp "beta-1.2.0.bmp"
;	File /oname=spltmp.wav "splash.wav"
	
	advsplash::show 1000 600 400 -1 $TEMP\spltmp
	
	Pop $0	; $0 has '1' if the user closed the splash screen early,
			; '0' if everything closed normally, and '-1' if some error occurred.
	
	Delete $TEMP\spltmp.bmp
;	Delete $TEMP\spltmp.wav
	
	Pop $R0
	Pop $0
FunctionEnd

!ifndef IncludeExtras
Section
	
SectionEnd
!else
Section "Prerequisites"
	Push $0
	
	StrCpy $0 "$TEMP\moose-prereq"
	CreateDirectory $0
	SetOutPath $0
	
	MessageBox MB_YESNO|MB_ICONQUESTION "Install prerequisites?" /SD IDYES IDNO endPrereq
	
	; PYTHON
	!define installer "python-2.5.2.msi"
	File "Extra\${installer}"
	ExecWait '"msiexec" /i "$0\${installer}"'
	!undef installer
	
	; NUMPY
	!define installer "numpy-1.2.0-win32-superpack-python2.5.exe"
	File "Extra\${installer}"
	ExecWait "$0\${installer}"
	!undef installer
	
	; PYQT
	!define installer "PyQt-Py2.5-gpl-4.4.3-1.exe"
	File "Extra\${installer}"
	ExecWait "$0\${installer}"
	!undef installer
	
	; PYQWT
	!define installer "PyQwt5.1.0-Python2.5-PyQt4.4.3-NumPy1.2.0-1.exe"
	File "Extra\${installer}"
	ExecWait "$0\${installer}"
	!undef installer
	
	endPrereq:
	SetOutPath $TEMP	; Needed because we cannot delete the directory otherwise.
	RMDir /r $0
	
	Pop $0
SectionEnd
!endif	; IncludeExtras

; The stuff to install
Section "moose" 
	SectionIn RO
	
	SetOutPath $INSTDIR
	File /r "Payload\*"
	
	; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\MOOSE "Install_Dir" "$INSTDIR"
	
	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" "DisplayName" "MOOSE"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
	
	; Link .g files to MOOSE
	${RegisterExtension} "$INSTDIR\launcher.bat" ".g" "MOOSE Script"
	
	; Include MOOSE binary's path in the execution path.
	; This is done by updating the PATH environment variable in the
	; registry.
	${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR"  
	
	; ExecShell "open" "$INSTDIR\Docs\Introduction.html"
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
	SetShellVarContext all
	CreateDirectory "$SMPROGRAMS\MOOSE"
	CreateShortCut "$SMPROGRAMS\MOOSE\MOOSE.lnk" "$INSTDIR\moose.exe" "" "$INSTDIR\moose.exe" 0
	CreateShortCut "$SMPROGRAMS\MOOSE\Demos.lnk" "$INSTDIR\Demos" "" "$INSTDIR\Demos" 0
	CreateShortCut "$SMPROGRAMS\MOOSE\Regression Tests.lnk" "$INSTDIR\RegressionTests" "" "$INSTDIR\RegressionTests" 0
	CreateShortCut "$SMPROGRAMS\MOOSE\Documentation.lnk" "$INSTDIR\Docs" "" "$INSTDIR\Docs" 0
	CreateShortCut "$SMPROGRAMS\MOOSE\MOOSE Website.lnk" "$INSTDIR\Docs\MOOSE Website.url" "" "$INSTDIR\Docs\MOOSE Website.url" 0
	CreateShortCut "$SMPROGRAMS\MOOSE\Report Bugs.lnk" "$INSTDIR\Docs\Report Bugs.url" "" "$INSTDIR\Docs\Report Bugs.url" 0
	CreateShortCut "$SMPROGRAMS\MOOSE\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Function un.onInit
	Call un.ConfirmAdmin
FunctionEnd

; Uninstaller
Section "Uninstall"
	SetShellVarContext all
	
	; Remove registry keys
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE"
	DeleteRegKey HKLM SOFTWARE\MOOSE
	
	; Unlink .g files with MOOSE
	${UnregisterExtension} ".g" "MOOSE Script"
	
	; Exclude MOOSE binary's path from the execution path.
	; This is done by updating the PATH environment variable in the
	; registry.
	${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"   
	
	; Remove directories used
	RMDir /r "$SMPROGRAMS\MOOSE"
	RMDir /r "$INSTDIR"
SectionEnd
