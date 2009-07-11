!ifndef REGEXTENSION_NSH
!define REGEXTENSION_NSH

!define RegisterExtension "!insertmacro RegisterExtension"
!define UnregisterExtension "!insertmacro UnregisterExtension"

!macro RegisterExtension executable extension description
	Push "${executable}"  ; "full path to my.exe"
	Push "${extension}"   ;  ".mkv"
	Push "${description}" ;  "MKV File"
	Call RegisterExtension
!macroend

; back up old value of .opt
Function RegisterExtension
	; Remember variable values to make room for arguments
	Push $R0
	Exch 3
	Push $R1
	Exch 3
	Push $R2
	Exch 3
	
	; Retrieve argument values
	Pop $R0	; Description
	Pop $R1	; Extension
	Pop $R2	; Executable
	
	; Remember variable values to make room for local variables
	Push $1
	Push $0
	
	!define Index "Line${__LINE__}"
	
	ReadRegStr $1 HKCR $R1 ""
	StrCmp $1 "" "${Index}-NoBackup"
	StrCmp $1 "OptionsFile" "${Index}-NoBackup"
	WriteRegStr HKCR $R1 "backup_val" $1
	"${Index}-NoBackup:"
	WriteRegStr HKCR $R1 "" $R0
	ReadRegStr $0 HKCR $R0 ""
	StrCmp $0 "" 0 "${Index}-Skip"
	WriteRegStr HKCR $R0 "" $R0
	WriteRegStr HKCR "$R0\shell" "" "open"
	WriteRegStr HKCR "$R0\DefaultIcon" "" "$R2,0"
	"${Index}-Skip:"
	WriteRegStr HKCR "$R0\shell\open\command" "" '$R2 "%1"'
	WriteRegStr HKCR "$R0\shell\edit" "" "Edit $R0"
	WriteRegStr HKCR "$R0\shell\edit\command" "" '$R2 "%1"'
	
	!undef Index
	
	; Restore values in locations used for local variables
	Pop $0
	Pop $1
	
	; Restore values in locations used for arguments
	Pop $R2
	Pop $R1
	Pop $R0
FunctionEnd

!macro UnregisterExtension extension description
	Push "${extension}"   ;  ".mkv"
	Push "${description}"   ;  "MKV File"
	Call un.UnregisterExtension
!macroend

Function un.UnregisterExtension
	push $R0
	exch 2
	push $R1
	exch 2
	
	pop $R1 ; description
	pop $R0 ; extension
	!define Index "Line${__LINE__}"
	ReadRegStr $1 HKCR $R0 ""
	StrCmp $1 $R1 0 "${Index}-NoOwn" ; only do this if we own it
	ReadRegStr $1 HKCR $R0 "backup_val"
	StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
	DeleteRegKey HKCR $R0
	Goto "${Index}-NoOwn"
	"${Index}-Restore:"
	WriteRegStr HKCR $R0 "" $1
	DeleteRegValue HKCR $R0 "backup_val"
	DeleteRegKey HKCR $R1 ;Delete key with association name settings
	"${Index}-NoOwn:"
	!undef Index
	
	pop $R1
	pop $R0
FunctionEnd

!endif	; REGEXTENSION_NSH
