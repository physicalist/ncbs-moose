!ifndef UNINSTALLIFEXISTS_NSH
!define UNINSTALLIFEXISTS_NSH

!define UninstallIfExists "!insertmacro UninstallIfExists"
!define un.UninstallIfExists "!insertmacro un.UninstallIfExists"

!ifdef FunctionFlag
	!undef FuncReturns
!endif
!ifdef NumParams
	!undef NumParams
!endif
!ifdef NumParamsPlusOne
	!undef NumParamsPlusOne
!endif

!define FuncReturns        ; This specifies if the function returns a value
!define NumParams "5"      ; This is the number of parameters this function takes.
!define /math NumParamsPlusOne ${NumParams} + 1

!macro UninstallIfExists name key instdir prompt message
	!ifdef FuncReturns
		Push "return"	; Reserving place for the return value
	!endif
	
	Push "${message}"
	Push "${prompt}"
	Push "${instdir}"
	Push "${key}"
	Push "${name}"
	
	Call UninstallIfExists
!macroend

!macro un.UninstallIfExists name key instdir prompt message
	!ifdef FuncReturns
		Push "return"	; Reserving place for the return value
	!endif
	
	Push "${message}"
	Push "${prompt}"
	Push "${instdir}"
	Push "${key}"
	Push "${name}"
	
	Call un.UninstallIfExists
!macroend

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

!macro UIE UN
Function ${UN}UninstallIfExists
	!ifdef FuncReturns
		Push $0
		Exch ${NumParamsPlusOne}
		Pop $0
	!endif
	
	; Remember variable values to make room for arguments
	Push $R0
	Exch ${NumParams}
	Push $R1
	Exch ${NumParams}
	Push $R2
	Exch ${NumParams}
	Push $R3
	Exch ${NumParams}
	Push $R4
	Exch ${NumParams}
	
	; Retrieve argument values
	Pop $R0	; name
	Pop $R1	; key
	Pop $R2	; instdir
	Pop $R3	; prompt
	Pop $R4	; message
	
	; Remember variable values to make room for local variables
	Push $1
	
	ReadRegStr $1 \
		HKLM \
		"Software\Microsoft\Windows\CurrentVersion\Uninstall\$R1" \
		"UninstallString"
	StrCmp $1 "" done
	
	${If} $R3 != "0"
		${If} $R4 != ""
			StrCpy $R4 "$R4$\n$\n"
		${Endif}
		
		MessageBox MB_YESNO|MB_ICONQUESTION \
			"$R4\
			$R0 has been found installed on this computer.$\n\
			Do you wish to uninstall it?" \
			IDYES uninst
		
		StrCpy $0 "0"
		Goto neverMind
	${EndIf}
	
	uninst:
	; Run the uninstaller
	ClearErrors
	${If} $R2 == ""
		ExecWait "$1"
	${Else}
		ExecWait "$1 _?=$R2" ; Do not copy the uninstaller to a temp file
	${Endif}
	StrCpy $0 "1"
	
	IfErrors no_remove_uninstaller done
	; You can either use Delete /REBOOTOK in the uninstaller or add some code
	; here to remove the uninstaller. Use a registry key to check
	; whether the user has chosen to uninstall. If you are using an uninstaller
	; components page, make sure all sections are uninstalled.
	no_remove_uninstaller:
	done:
	
	neverMind:
	
	; Restore values in locations used for local variables
	Pop $1
	
	; Restore values in locations used for arguments
	Pop $R4
	Pop $R3
	Pop $R2
	Pop $R1
	Pop $R0
	
	; Return result, and restore value
	!ifdef FuncReturns
		Exch $0
	!endif
FunctionEnd

!macroend	; UIE UN

!insertmacro UIE ""
!insertmacro UIE "un."

!endif	; UNINSTALLIFEXISTS_NSH
