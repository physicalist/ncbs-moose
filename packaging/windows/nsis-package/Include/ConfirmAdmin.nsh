!ifndef CONFIRMADMIN_FUNCTION
!define CONFIRMADMIN_FUNCTION

!macro ConfirmAdmin UN

Function ${UN}ConfirmAdmin
	Push $0
	
	Call ${UN}IsUserAdmin
	Pop $0
	StrCmp $0 "true" +4
	
    # if there is not a match, print message and return
    MessageBox MB_OK|MB_ICONEXCLAMATION "Unable to proceed: You need administrative rights to continue."
	Abort
	
	Pop $0
FunctionEnd

!macroend   ; ConfirmAdmin UN
!insertmacro ConfirmAdmin ""
!insertmacro ConfirmAdmin "un."

!endif
