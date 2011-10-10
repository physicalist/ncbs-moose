; Choose if extras (installers for python etc.) should be
; bundled inside the installer.

!define IncludeExtras

; Compression - Keep this at top. Can lead to errors otherwise.
SetCompressor /SOLID lzma

; Allows use of logical operations like ${If} $0 != ""
!include "LogicLib.nsh"

; This extension allows us to create file-associations in the registry.
; In other words, the user can click a *.g file, and MOOSE will execute
; the script.
!include "Include\RegExtension.nsh"

; This extension allows us to modify environment variables. This is
; used here to include the moose binary in the execution path.
!include "Include\EnvVarUpdate.nsh"

; Remove programs if they exist
!include "Include\UninstallIfExists.nsh"

; Find admin status: required for next header
!include "Include\isUserAdmin.nsh"

; Ensure that user has admin privileges
!include "Include\ConfirmAdmin.nsh"

; Name of program
Name "Moose Beta 1.4.0"

; The file to write
OutFile "moose-beta-1.4.0.exe"

; The default installation directory
InstallDir $PROGRAMFILES\MOOSE

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\MOOSE" "Install_Dir"

; Request application privileges for Windows Vista and Windows 7
RequestExecutionLevel admin

; LicenseText "GNU LESSER GENERAL PUBLIC LICENSE"
; LicenseData "Payload\Docs\COPYING.LIB.txt"

; MUI 1.67 compatible
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING

; MUI Settings / Icons
;!define MUI_ICON "Graphics\moose.ico"
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"

; MUI Settings / Header
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_RIGHT
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-uninstall-r.bmp"

; MUI Settings / Wizard
;!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\arrow.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"

; Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "Payload\Docs\COPYING.LIB.txt"
!insertmacro MUI_PAGE_DIRECTORY
; !insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
; !insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Function Splash
	Push $0
	
	SetOutPath $TEMP
	File /oname=spltmp.bmp "Graphics\beta-1.4.0.bmp"
	advsplash::show 1000 600 400 -1 $TEMP\spltmp
	Pop $0	; $0 has '1' if the user closed the splash screen early,
			; '0' if everything closed normally, and '-1' if some error occurred.
	Delete $TEMP\spltmp.bmp
	
	Pop $0
FunctionEnd

Function .onInit
	Call ConfirmAdmin
	
	;-----------------------------------------------------------------
	; Uninstalling existing MOOSE
	;-----------------------------------------------------------------
	
	Push $0
	Push $1
	Push $2
	
	StrCpy $1 \
		"Existing installations of MOOSE must be removed before this \
		installer can continue."
	
	/*
	Parameters:
		name:     Name of software
		key:      Name of registry key containing uninstall information
		instdir:  Installation directory for the software. Needed for NSIS
		          based installers. Can be left empty otherwise.
		prompt:   Should the user be prompted? ("0" or "1")
		message:  Message to display to user
	
	Returns:
		"0" if the user did not uninstall
		"1" if the user let uninstallation continue
	
	Retrieve returned value by popping stack:
		Pop $my_var
	*/
	
	ReadRegStr $2 \
		HKLM \
		"SOFTWARE\MOOSE" \
		"Install_Dir"
	${UninstallIfExists} "MOOSE" "MOOSE" $2 "1" $1
	
	Pop $0
	${If} $0 == "0"
		Abort
	${EndIf}
	
	Pop $1
	Pop $0
	
	;-----------------------------------------------------------------
	; Uninstallation done
	;-----------------------------------------------------------------
	
	Call Splash
FunctionEnd

!ifndef IncludeExtras
Section
	Push $1
	
	StrCpy $1 \
		"This installer will work if you have the following software installed: $\n\
			$\n\
			    - Python 2.6 (preferably not any other) $\n\
			    - Numpy 1.2 or higher $\n\
			    - PyQt 4 or higher $\n\
			    - PyQwt5 $\n\
				- PyOpenGL 3.0 $\n\
			$\n\
		You can install the above manually, or download the MOOSE installer which will $\n\
		install these for you. $\n\
		$\n\
		Click 'OK' to continue installing or 'Cancel' to abort."
	MessageBox MB_OKCANCEL|MB_ICONQUESTION $1 /SD IDOK IDOK continueAnyway
	
	Abort
	continueAnyway:
	
	Pop $1
SectionEnd
!else
Section "Prerequisites"
	Push $0
	Push $1
	
	StrCpy $0 \
		"MOOSE needs the following software to be installed: $\n\
			$\n\
			    - Python 2.6 $\n\
			    - Numpy 1.2.0 $\n\
			    - PyQt 4.4 $\n\
			    - PyQwt 5.1 $\n\
				- PyOpenGL 3.0.1 $\n\
			$\n\
		The full installation will take around 220 MB of space on your disk. $\n\
		Do you wish to install these before installing MOOSE?"
	MessageBox MB_YESNO|MB_ICONQUESTION $0 /SD IDYES IDNO endPrereq
	
	; Temp directory
	StrCpy $1 "$TEMP\moose-prereq"
	CreateDirectory $1
	SetOutPath $1
	
	; Extract, execute and delete immediately (to reduce working space requirement).
	; PYTHON
	!define installer "python-2.6.6.msi"
	File "Extra\${installer}"
	ExecWait '"msiexec" /i "$1\${installer}"'
	Delete "$1\${installer}"
	!undef installer
	
	; NUMPY
	!define installer "numpy-1.3.0-win32-superpack-python2.6.exe"
	File "Extra\${installer}"
	ExecWait "$1\${installer}"
	Delete "$1\${installer}"
	Delete "$TEMP\numpy*.exe"	; Numpy installer creates a temp file but does not delete it.
	!undef installer
	
	; PYQT
	!define installer "PyQt-Py2.6-gpl-4.5.4-1.exe"
	File "Extra\${installer}"
	ExecWait "$1\${installer}"
	Delete "$1\${installer}"
	!undef installer
	
	; PYQWT
	!define installer "PyQwt5.2.0-Python2.6-PyQt4.5.4-NumPy1.3.0-1.exe"
	File "Extra\${installer}"
	ExecWait "$1\${installer}"
	Delete "$1\${installer}"
	!undef installer
	
	; PyOpenGL
	!define installer "PyOpenGL-3.0.1.win32.exe"
	File "Extra\${installer}"
	ExecWait "$1\${installer}"
	Delete "$1\${installer}"
	!undef installer
	
	SetOutPath $TEMP	; Needed because we cannot delete the directory otherwise.
	RMDir /r $1
	
	endPrereq:
	
	Pop $1
	Pop $0
SectionEnd
!endif	; IncludeExtras

; The stuff to install
Section "moose" 
	SectionIn RO
	
	Push $0
	
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
	
	Pop $0
SectionEnd

Function .onInstSuccess
	ExecShell "open" "$INSTDIR\Docs\Introduction.html"
FunctionEnd

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
	Push $0
	Push $1
	Push $2
	Push $3
	
	; Find original installation location from registry
	ReadRegStr \
		$0 \
		HKLM \
		"Software\MOOSE" \
		"Install_Dir"
	
!ifdef IncludeExtras
	;-----------------------------------------------------------------
	; Uninstalling dependencies (Python, etc.)
	;-----------------------------------------------------------------
	
	/*
	Parameters:
		name:     Name of software
		key:      Name of registry key containing uninstall information
		instdir:  Installation directory for the software. Needed for NSIS
		          based installers. Can be left empty otherwise.
		prompt:   Should the user be prompted? ("0" or "1")
		message:  Message to display to user
	
	Returns:
		"0" if the user did not uninstall
		"1" if the user let uninstallation continue
	
	Retrieve returned value by popping stack:
		Pop $my_var
	*/
	
	StrCpy $1 \
		"PyOpenGL was installed along with MOOSE during the MOOSE installation."
	StrCpy $2 ""
	${un.UninstallIfExists} "PyOpenGL-3.0.1" "PyOpenGL-py2.6" $2 "1" $1
	Pop $1
	
	StrCpy $1 \
		"PyQwt was installed along with MOOSE during the MOOSE installation."
	ReadRegStr $2 \
		HKLM \
		"SOFTWARE\PyQwt5" \
		""
	${un.UninstallIfExists} "PyQwt 5.1" "PyQwt5" $2 "1" $1
	Pop $1
	
	StrCpy $1 \
		"PyQt was installed along with MOOSE during the MOOSE installation."
	ReadRegStr $2 \
		HKLM \
		"SOFTWARE\PyQt4\Py2.5\InstallPath" \
		""
	${un.UninstallIfExists} "PyQt 4.4" "PyQt GPL v4.5.4 for Python v2.6" $2 "1" $1
	; This directory was actually created by the PyQwt (not PyQt) installer and not
	; cleaned up properly, so deleting it here:
	RMDir /r "$2\PyQt4"
	; This directory also gets left over. This is because the actual installation
	; directory for PyQt is:
	;	C:\Python2.6\Lib\site-packages\PyQt4
	; but we pass it C:\Python2.6, as read from the registry. Here we delete this
	; dir, but another possiblity is to append the extra part to the inst-dir
	; parameter passed to the uninstallation function above.
	RMDir /r "$2\Lib\site-packages\PyQt4"
	Pop $1
	
	StrCpy $1 \
		"Numpy was installed along with MOOSE during the MOOSE installation."
	StrCpy $2 ""
	${un.UninstallIfExists} "Numpy 1.2.0" "numpy-py2.6" $2 "1" $1
	Pop $1
	
	StrCpy $1 \
		"Python was installed along with MOOSE during the MOOSE installation."
	StrCpy $2 ""
	${un.UninstallIfExists} "Python 2.6" "{6151cf20-0bd8-4023-a4a0-6a86dcfe58e5}" $2 "1" $1
	Pop $1	
!endif	; IncludeExtras
	
	;-----------------------------------------------------------------
	; Uninstalling MOOSE
	;-----------------------------------------------------------------
	; Remove registry keys
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE"
	DeleteRegKey HKLM SOFTWARE\MOOSE
	
	; Unlink .g files with MOOSE
	${UnregisterExtension} ".g" "MOOSE Script"
	
	; Exclude MOOSE binary's path from the execution path.
	; This is done by updating the PATH environment variable in the
	; registry.
	${un.EnvVarUpdate} $3 "PATH" "R" "HKLM" "$INSTDIR"   
	
	; Remove directories used
	RMDir /r "$SMPROGRAMS\MOOSE"
	
	; The installation directory can be deleted like this:
	
	; RMDir /r "$INSTDIR"
	
	; ..but this method is risky. If the user moves the uninstaller to
	; some other location for some reason, then that directory will get deleted.
	; Instead we use the original installation directory as found out from the
	; registry:
	
	RMDir /r "$0"
	
	Pop $3
	Pop $2
	Pop $1
	Pop $0
SectionEnd
