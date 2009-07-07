; moose.nsi

; It will install moose into a directory that the user selects.

;--------------------------------

; This extension allows us to create file-associations in the registry.
; In other words, the user can click a *.g file, and MOOSE will execute
; the script.
!include "RegExtension.nsh"

; This extension allows us to modify environment variables. This is
; used here to include the moose binary in the execution path.
!include "EnvVarUpdate.nsh"

; The name of the installer
Name "Moose Beta 1.1 Installer"

; The file to write
OutFile "moose-beta-1.1.0.exe"

; The default installation directory
InstallDir $PROGRAMFILES\MOOSE
;InstallDir $DOCUMENTS\MOOSE

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\MOOSE" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin


; Compression

!ifdef NSIS_LZMA_COMPRESS_WHOLE
  SetCompressor lzma
!else
  SetCompressor /SOLID lzma
!endif


;--------------------------------
; text file to open in notepad after installation
;!define notefile "Docs\RELEASE_NOTES.TXT"
 
; introductory html page with links to interesting stuff
!define introfile "Docs\Introduction.html"

; license text file
!define licensefile "Docs\COPYING.LIB.txt"
 
; icons must be Microsoft .ICO files
!define icon "moose.ico"
;-------------------------------
; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles
 
!ifdef icon
Icon "${icon}"
!endif


!ifdef licensefile
LicenseText "License"
LicenseData "${licensefile}"
!endif
!ifdef licensefile
Page license
!endif

;--------------------------------
; Show MOOSE Logo
Function .onInit
  SetOutPath $TEMP
  File /oname=spltmp.bmp "beta-1.1.0.bmp"

; optional
; File /oname=spltmp.wav "my_splash.wav"

  advsplash::show 1000 600 400 -1 $TEMP\spltmp

  Pop $0 ; $0 has '1' if the user closed the splash screen early,
         ; '0' if everything closed normally, and '-1' if some error occurred.

  Delete $TEMP\spltmp.bmp
;  Delete $TEMP\spltmp.wav
FunctionEnd
;--------------------------------

# default section start
section
 
    # call userInfo plugin to get user info.  The plugin puts the result in the stack
    userInfo::getAccountType
   
    # pop the result from the stack into $0
    pop $0
 
    # compare the result with the string "Admin" to see if the user is admin. If match, jump 3 lines down.
    strCmp $0 "Admin" +3
 
    # if there is not a match, print message and return
    messageBox MB_OK "Error! You need to be Administrator to run this installer."
    return
	
    # otherwise, confirm and return
    ;messageBox MB_OK "is admin"
 
# default section end
sectionEnd

; The stuff to install
Section "moose" 

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put files there
  File "moose.exe"
  File "moose.py"
  File "_moose.pyd"
  File "COPYING.LIB.txt"
  File "copyleft.txt"
  File "launcher.bat"
  File "launch.lnk"
  CreateDirectory "$INSTDIR\Docs"
  File /r "Docs"
  CreateDirectory "$INSTDIR\Demos"
  File /r "Demos"
  CreateDirectory "$INSTDIR\RegressionTests"
  File /r "RegressionTests"

!ifdef licensefile
  File /a "${licensefile}"
!endif
 
;!ifdef notefile
;  File /a "${notefile}"
;!endif
 
!ifdef icon
  File /a "${icon}"
!endif

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\MOOSE "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" "DisplayName" "MOOSE"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  ; Link .g files to MOOSE
  ${registerExtension} "$INSTDIR\launcher.bat" ".g" "MOOSE Script"
  
  ; Include MOOSE binary's path in the execution path.
  ; This is done by updating the PATH environment variable in the
  ; registry.
  ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR"  
  
!ifdef introfile
  ExecShell "open" "$INSTDIR\${introfile}"
!endif
 
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\MOOSE"
  CreateShortCut "$SMPROGRAMS\MOOSE\MOOSE.lnk" "$INSTDIR\moose.exe" "" "$INSTDIR\moose.exe" 0
  CreateDirectory "$SMPROGRAMS\MOOSE\Examples"
  CreateShortCut "$SMPROGRAMS\MOOSE\Examples\Demos.lnk" "$INSTDIR\Demos" "" "$INSTDIR\Demos" 0
  CreateShortCut "$SMPROGRAMS\MOOSE\Examples\Regression Tests.lnk" "$INSTDIR\RegressionTests" "" "$INSTDIR\RegressionTests" 0
  CreateShortCut "$SMPROGRAMS\MOOSE\Documentation.lnk" "$INSTDIR\Docs" "" "$INSTDIR\Docs" 0
  CreateShortCut "$SMPROGRAMS\MOOSE\MOOSE Website.lnk" "$INSTDIR\Docs\MOOSE Website.url" "" "$INSTDIR\Docs\MOOSE Website.url" 0
  CreateShortCut "$SMPROGRAMS\MOOSE\Report Bugs.lnk" "$INSTDIR\Docs\Report Bugs.url" "" "$INSTDIR\Docs\Report Bugs.url" 0
  CreateShortCut "$SMPROGRAMS\MOOSE\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  SetShellVarContext all
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MOOSE"
  DeleteRegKey HKLM SOFTWARE\MOOSE
  
  ; Unlink .g files with MOOSE
  ${unregisterExtension} ".g" "MOOSE Script"
  
  ; Exclude MOOSE binary's path from the execution path.
  ; This is done by updating the PATH environment variable in the
  ; registry.
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR"   
  
  ; Remove files and uninstaller
  Delete $INSTDIR\moose.exe
  Delete $INSTDIR\uninstall.exe
  
!ifdef licensefile
  Delete "$INSTDIR\${licensefile}"
!endif
 
!ifdef notefile
  Delete "$INSTDIR\${notefile}"
!endif
 
!ifdef icon
  Delete "$INSTDIR\${icon}"
!endif
  
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\MOOSE\*.*"

  ; Remove directories used
  RMDir /r "$SMPROGRAMS\MOOSE"
  RMDir /r "$INSTDIR"
  
SectionEnd
