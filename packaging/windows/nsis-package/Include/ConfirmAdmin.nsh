!ifndef CONFIRMADMIN_NSH
!define CONFIRMADMIN_NSH

!macro ConfirmAdmin UN

Function ${UN}ConfirmAdmin
	Push $0
	
	DetailPrint "Checking if user has administrative rights."
	Call ${UN}IsUserAdmin
	Pop $0
	StrCmp $0 "true" yesAdmin
	
    # if there is not a match, print message and return
	DetailPrint "Admin status: User has administrative rights."
    MessageBox MB_OK|MB_ICONEXCLAMATION "Unable to proceed: You need administrative rights to continue."
	Abort
	
	yesAdmin:
	DetailPrint "Admin status: User does not have administrative rights."
	
	Pop $0
FunctionEnd

!macroend   ; ConfirmAdmin UN

!insertmacro ConfirmAdmin ""
!insertmacro ConfirmAdmin "un."

!endif	; CONFIRMADMIN_NSH
